#include "OptimizedMerge_JobSeeker.hpp"

void SkillList::add(const string& skill, double weight) {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill) return;
    skills[size] = skill;
    weights[size] = weight;
    size++;
}

bool SkillList::contains(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill) return true;
    return false;
}

double SkillList::getWeight(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill) return weights[i];
    return 0.0;
}

string toLowerCase(const string& str) {
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(),
              [](unsigned char c){ return tolower(c); });
    return lower;
}

void insertAtTail(Job*& head, string title, SkillList skills) {
    Job* newJob = new Job{title, skills, 0, nullptr, nullptr};
    if (!head) {
        head = newJob;
        return;
    }
    Job* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newJob;
    newJob->prev = temp;
}

void loadJobsFromCSV(Job*& head, const string& filename, SkillList& allValidSkills) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string title, skillsString;
        SkillList skills;

        getline(ss, title, ',');
        getline(ss, skillsString);

        if (!skillsString.empty() && skillsString.front() == '"')
            skillsString = skillsString.substr(1, skillsString.size() - 2);

        stringstream skillsStream(skillsString);
        string skill;
        while (getline(skillsStream, skill, ',')) {
            while (!skill.empty() && skill.front() == ' ') skill.erase(skill.begin());
            string lowerSkill = toLowerCase(skill);
            skills.add(lowerSkill);
            allValidSkills.add(lowerSkill); // ✅ add to global valid skills too
        }

        insertAtTail(head, title, skills);
    }

    file.close();
}


SkillList insertSkills(const SkillList& allValidSkills) {
    SkillList userSkills;
    string skill;
    double weightCounter = 1.0;

    cout << "Enter your skills (type 'done' to finish):\n";
    cin.ignore();

    while (true) {
        cout << "> ";
        getline(cin, skill);
        string lowerSkill = toLowerCase(skill);
        if (lowerSkill == "done")
            break;

        // trim
        lowerSkill.erase(0, lowerSkill.find_first_not_of(" \t"));
        lowerSkill.erase(lowerSkill.find_last_not_of(" \t") + 1);

        // Only add skills that exist in the dataset
        if (allValidSkills.contains(lowerSkill)) {
            userSkills.add(lowerSkill, weightCounter);
            weightCounter++;
        }
    }

    return userSkills;
}

// Optimized Linear Search for skill matching
void updateAllMatchScores(Job* head, const SkillList& userSkills, 
                          double& searchTime, size_t& searchMemory) {
    Job* temp = head;

    auto searchStart = chrono::high_resolution_clock::now();

    size_t seekerSkillCount = userSkills.size;
    size_t baseMemory = sizeof(SkillList) + sizeof(double) * 2 + sizeof(int);

    // ✅ Optimized search memory model (using Job instead of Node)
    size_t optimizedSearchMemory =
        (sizeof(Job) + sizeof(Job*) + sizeof(int)) * seekerSkillCount + baseMemory;

    searchMemory = optimizedSearchMemory;

    while (temp) {
        double matchedWeight = 0.0;
        int skillCount = temp->requiredSkills.size;
        double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

        for (int i = 0; i < skillCount; ++i) {
            const string& jobSkill = temp->requiredSkills.skills[i];
            double weight = skillCount - i;

            // ✅ Optimized linear search check
            if (userSkills.contains(jobSkill)) {
                matchedWeight += weight;
            }
        }

        if (maxWeight > 0)
            temp->matchScore = (matchedWeight / maxWeight) * 100.0;
        else
            temp->matchScore = 0.0;

        temp = temp->next;
    }

    auto searchEnd = chrono::high_resolution_clock::now();
    searchTime = chrono::duration<double, milli>(searchEnd - searchStart).count();
}

// Merge Sort for Doubly Linked List
void split(Job* source, Job** frontRef, Job** backRef) {
    Job* slow = source;
    Job* fast = source->next;

    // Move 'fast' twice as fast as 'slow'
    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' is before the midpoint
    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

Job* merge(Job* a, Job* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    Job* result = nullptr;

    // Sort in descending order (higher matchScore first)
    if (a->matchScore >= b->matchScore) {
        result = a;
        result->next = merge(a->next, b);
    } else {
        result = b;
        result->next = merge(a, b->next);
    }

    return result;
}

void mergeSort(Job** headRef, double& sortTime, size_t& sortMemory) {
    Job* head = *headRef;
    if (head == nullptr || head->next == nullptr)
        return;

    auto sortStart = high_resolution_clock::now();

    // Split into two halves
    Job* a;
    Job* b;
    split(head, &a, &b);

    // Recursively sort both halves
    mergeSort(&a, sortTime, sortMemory);
    mergeSort(&b, sortTime, sortMemory);

    // Merge the sorted halves
    *headRef = merge(a, b);

    // ✅ Linked List Merge Sort memory model
    int jobCount = 0;
    Job* temp = *headRef;
    while (temp) {
        jobCount++;
        temp = temp->next;
    }

    // Formula from your image:
    sortMemory = (sizeof(Job) * jobCount) + (sizeof(Job*) * 4);

    auto sortEnd = high_resolution_clock::now();
    sortTime = duration<double, milli>(sortEnd - sortStart).count();
}

void sortByScore(Job*& head, double& sortTime, size_t& sortMemory) {
    mergeSort(&head, sortTime, sortMemory);
}

void displayJobs(Job* head, double minScore) {
    cout << "\nTop 3 Best-Matching Jobs (Weighted Scoring):\n";
    cout << left << setw(25) << "Job Title"
         << setw(10) << "Matched"
         << setw(10) << "Weight"
         << setw(12) << "Percentage" << endl;
    cout << string(55, '-') << endl;

    Job* temp = head;
    int count = 0;

    cout << fixed << setprecision(2);
    while (temp && count < 3) {  // Show top 3 only
        if (temp->matchScore >= minScore) {
            // Example placeholders: assume matched skills and total weights calculated earlier
            int matched = 0;
            double totalWeight = 0.0;

            int skillCount = temp->requiredSkills.size;
            double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

            for (int i = 0; i < skillCount; ++i) {
                if (temp->requiredSkills.skills[i] != "") matched++;
            }

            totalWeight = skillCount; // Example: using skill count as total weight

            ostringstream perc;
            perc << fixed << setprecision(2) << temp->matchScore << "%";

            cout << left << setw(25) << temp->title
                 << setw(10) << matched
                 << setw(10) << totalWeight
                 << setw(12) << perc.str() << endl;

            count++;
        }
        temp = temp->next;
    }

    if (count == 0)
        cout << "No jobs matched your skills.\n";

    cout.unsetf(ios::fixed);
    cout.precision(6);
    cout << endl;
}

void menu(Job*& head, const SkillList& allValidSkills) {
    int choice;
    SkillList userSkills;

    // Variables to store performance data
    double matchDuration = 0.0, sortDuration = 0.0, searchDuration = 0.0;
    size_t baseMemory = 0, matchMemoryKB = 0, sortMemoryKB = 0, searchMemoryKB = 0;
    bool performanceRecorded = false;

    do {
        cout << "====================================" << endl;
        cout << "              Job Seeker            " << endl;
        cout << "Optimized Linear Search & Merge Sort" << endl;
        cout << "====================================" << endl;
        cout << "1. Insert Skills" << endl;
        cout << "2. Performance Report" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                userSkills = insertSkills(allValidSkills);

                auto matchStart = chrono::high_resolution_clock::now();
                updateAllMatchScores(head, userSkills, searchDuration, searchMemoryKB);
                auto matchEnd = chrono::high_resolution_clock::now();
                matchDuration = chrono::duration<double, milli>(matchEnd - matchStart).count();

                auto sortStart = chrono::high_resolution_clock::now();
                sortByScore(head, sortDuration, sortMemoryKB);
                auto sortEnd = chrono::high_resolution_clock::now();

                matchDuration = chrono::duration<double, milli>(matchEnd - matchStart).count();
                sortDuration = chrono::duration<double, milli>(sortEnd - sortStart).count();

                // ✅ Manual memory estimation
                baseMemory = sizeof(SkillList) * 2 + sizeof(Job) * 100 + sizeof(string) * 200;
                matchMemoryKB = sizeof(SkillList) + sizeof(string) * userSkills.size;
                sortMemoryKB = sizeof(Job) * 100 + sizeof(int) * 2;

                performanceRecorded = true;
                displayJobs(head, 0);
                break;
            }

            case 2: {
                if (performanceRecorded) {
                    cout << fixed << setprecision(4);
                    cout << "\n=============================\n";
                    cout << "Performance Summary\n";
                    cout << "=============================\n";
                    cout << "Optimized Linear Search Time: " << searchDuration << " ms\n";
                    cout << "Merge Sort Time: " << sortDuration << " ms\n";
                    cout << "Optimized Linear Search Memory: " << (searchMemoryKB / 1024.0) << " KB\n";
                    cout << "Merge Sort Memory: " << (sortMemoryKB / 1024.0) << " KB\n\n";
                    cout.unsetf(ios::fixed);
                } else {
                    cout << "\nNo performance data available yet. Please run 'Insert Skills' first.\n\n";
                }
                break;
            }

            case 3:
                cout << "Exiting program, have a nice day!" << endl;
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }

    } while (choice != 3);
}

int main() {
    Job* head = nullptr;
    SkillList allValidSkills;

    loadJobsFromCSV(head, "job_description/mergejob.csv", allValidSkills);
    menu(head, allValidSkills);
    return 0;
}
