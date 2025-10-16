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

    size_t memoryUsed = 0;
    while (temp) {
        double matchedWeight = 0.0;
        int skillCount = temp->requiredSkills.size;

        double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

        for (int i = 0; i < skillCount; ++i) {
            const string& jobSkill = temp->requiredSkills.skills[i];
            double weight = skillCount - i;

            // Each call to contains() is a linear search
            memoryUsed += sizeof(string) + sizeof(double);
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
    searchMemory = memoryUsed;
}

// Merge Sort for Doubly Linked List
Job* split(Job* head) {
    Job* fast = head, *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    Job* temp = slow->next;
    slow->next = nullptr;
    if (temp) temp->prev = nullptr;
    return temp;
}

Job* merge(Job* first, Job* second) {
    if (!first) return second;
    if (!second) return first;

    if (first->matchScore >= second->matchScore) {
        first->next = merge(first->next, second);
        if (first->next) first->next->prev = first;
        first->prev = nullptr;
        return first;
    } else {
        second->next = merge(first, second->next);
        if (second->next) second->next->prev = second;
        second->prev = nullptr;
        return second;
    }
}

Job* mergeSort(Job* head) {
    if (!head || !head->next) return head;
    Job* second = split(head);

    head = mergeSort(head);
    second = mergeSort(second);

    return merge(head, second);
}

void sortByScore(Job*& head) {
    head = mergeSort(head);
}

void displayJobs(Job* head, double minScore) {
    cout << "\n====== Matching Jobs ======\n";
    Job* temp = head;

    cout << fixed << setprecision(2);
    while (temp != nullptr) {
        if (temp->matchScore >= minScore)
            cout << temp->title << " - " << temp->matchScore << "% match\n";
        temp = temp->next;
    }
    cout << "\n";
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
                sortByScore(head);
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
                    cout << "Skill Matching Time: " << matchDuration << " ms\n";
                    cout << "Linear Search Time: " << searchDuration << " ms\n";
                    cout << "Insertion Sort Time: " << sortDuration << " ms\n";
                    cout << "Linear Search Memory: " << (searchMemoryKB / 1024.0) << " KB\n";
                    cout << "Insertion Sort Memory: " << (sortMemoryKB / 1024.0) << " KB\n";
                    cout << "Approx. Base Memory Used: " << (baseMemory / 1024.0) << " KB\n\n";
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
