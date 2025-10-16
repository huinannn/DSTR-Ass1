#include "job_seeker_setB.hpp"

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

size_t getMemoryUsageKB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize / 1024;
#else
    long rss = 0L;
    std::ifstream statm("/proc/self/statm");
    long dummy;
    statm >> dummy >> rss;
    statm.close();
    return rss * (sysconf(_SC_PAGESIZE) / 1024);
#endif
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
void updateAllMatchScores(Job* head, const SkillList& userSkills) {
    Job* temp = head;
    while (temp) {
        double matchedWeight = 0.0;
        int skillCount = temp->requiredSkills.size;

        // Calculate maximum possible weight for this job
        double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

        // Assign descending weights to job skills (like 5, 4, 3, 2, 1)
        for (int i = 0; i < skillCount; ++i) {
            const string& jobSkill = temp->requiredSkills.skills[i];
            double weight = skillCount - i; // descending weight

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

void menu(Job*& head, const SkillList& allValidSkills, size_t baselineMemory) {
    int choice;
    SkillList userSkills;

    do {
        cout << "====================================" << endl;
        cout << "              Job Seeker            " << endl;
        cout << "Optimized Linear Search & Merge Sort" << endl;
        cout << "====================================" << endl;
        cout << "1. Insert Skills" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                userSkills = insertSkills(allValidSkills);

                // Measure search/matching time & memory
                auto matchStart = high_resolution_clock::now();

                updateAllMatchScores(head, userSkills);

                auto matchEnd = high_resolution_clock::now();
                size_t matchEndMem = getMemoryUsageKB() - baselineMemory;

                double matchDuration = duration<double, milli>(matchEnd - matchStart).count();
                double matchMemory = static_cast<double>(matchEndMem) / 1024.0; // Convert to MB

                // Measure sorting time & memory
                auto sortStart = high_resolution_clock::now();

                sortByScore(head);

                auto sortEnd = high_resolution_clock::now();
                size_t sortEndMem = getMemoryUsageKB() - baselineMemory;

                double sortDuration = duration<double, milli>(sortEnd - sortStart).count();
                double sortMemory = static_cast<double>(sortEndMem) / 1024; // Convert to MB

                // Display jobs
                displayJobs(head, 0);

                // Performance report
                cout << fixed << setprecision(3);
                cout << "\n=== Performance Report ===\n";
                cout << "Search / Match Time: " << matchDuration << " ms\n";
                cout << "Search / Match Memory: " << matchMemory << " MB\n";
                cout << "Sort Time: " << sortDuration << " ms\n";
                cout << "Sort Memory: " << sortMemory << " MB\n\n";
                cout.unsetf(ios::fixed);
                break;
            }
            case 2:
                cout << "Exiting program, have a nice day!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 2);
}

int main() {
    Job* head = nullptr;
    SkillList allValidSkills;
    size_t baselineMemory = getMemoryUsageKB();

    loadJobsFromCSV(head, "../../job_description/mergejob.csv", allValidSkills);
    menu(head, allValidSkills, baselineMemory);
    return 0;
}
