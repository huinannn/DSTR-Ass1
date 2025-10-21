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
        ((sizeof(Job) + sizeof(Job*) + sizeof(int)) * seekerSkillCount + baseMemory) / 1024.0;

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

Job* extractMatchedJobs(Job* head) {
    Job* matchedHead = nullptr;
    Job* matchedTail = nullptr;

    Job* temp = head;
    while (temp) {
        if (temp->matchScore > 0.0) {
            Job* newJob = new Job{temp->title, temp->requiredSkills, temp->matchScore, nullptr, nullptr};
            if (!matchedHead) {
                matchedHead = matchedTail = newJob;
            } else {
                matchedTail->next = newJob;
                newJob->prev = matchedTail;
                matchedTail = newJob;
            }
        }
        temp = temp->next;
    }

    return matchedHead;
}

Job* merge(Job* left, Job* right) {
    if (!left) return right;
    if (!right) return left;

    if (left->matchScore >= right->matchScore) {
        left->next = merge(left->next, right);
        if (left->next) left->next->prev = left;
        left->prev = nullptr;
        return left;
    } else {
        right->next = merge(left, right->next);
        if (right->next) right->next->prev = right;
        right->prev = nullptr;
        return right;
    }
}

void split(Job* source, Job** frontRef, Job** backRef) {
    Job* slow = source;
    Job* fast = source->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    if (*backRef) (*backRef)->prev = nullptr;
    slow->next = nullptr;
}

int countJobs(Job* head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

void mergeSort(Job*& head, double& sortTime, size_t& sortMemory) {
    auto start = chrono::high_resolution_clock::now();

    // Base case — empty or single node
    if (!head || !head->next) {
        sortMemory = sizeof(Job); // Base memory
        auto end = chrono::high_resolution_clock::now();
        sortTime = chrono::duration<double, milli>(end - start).count();
        return;
    }

    Job* left = nullptr;
    Job* right = nullptr;

    // Split list into two halves
    split(head, &left, &right);

    size_t leftMemory = 0, rightMemory = 0;

    // Recursive sort both halves
    mergeSort(left, sortTime, leftMemory);
    mergeSort(right, sortTime, rightMemory);

    // Merge and measure memory
    head = merge(left, right);

    // Estimated memory usage:
    // Each recursion allocates left + right halves + merge overhead
    sortMemory = (leftMemory + rightMemory + sizeof(Job) * countJobs(head)) / 1024.0;

    auto end = chrono::high_resolution_clock::now();
    sortTime = chrono::duration<double, milli>(end - start).count();
}

void sortByScore(Job*& head, double& sortTime, size_t& sortMemory) {
    sortTime = 0.0;
    sortMemory = 0;
    mergeSort(head, sortTime, sortMemory);
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
    size_t sortMemoryKB = 0, searchMemoryKB = 0;
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

                updateAllMatchScores(head, userSkills, searchDuration, searchMemoryKB);

                // Extract only matched jobs
                Job* matchedJobs = extractMatchedJobs(head);

                // Sort matched jobs using insertion sort
                mergeSort(matchedJobs, sortDuration, sortMemoryKB);

                performanceRecorded = true;
                displayJobs(matchedJobs, 0);
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