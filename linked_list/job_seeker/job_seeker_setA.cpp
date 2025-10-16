#include "Job_seeker_setA.hpp"

void SkillList::add(const string& skill, double weight) {
    skills[size] = skill;
    weights[size] = weight;
    size++;
}

bool SkillList::contains(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill)
            return true;
    return false;
}

double SkillList::getWeight(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill)
            return weights[i];
    return 0.0;
}

Job::Job(const string& t, const SkillList& skills, double score)
    : title(t), requiredSkills(skills), matchScore(score), prev(nullptr), next(nullptr) {}

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
    ifstream statm("/proc/self/statm");
    long dummy;
    statm >> dummy >> rss;
    statm.close();
    return rss * (sysconf(_SC_PAGESIZE) / 1024);
#endif
}

void insertAtTail(Job*& head, string title, SkillList skills) {
    Job* newJob = new Job(title, skills);
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
    if (!file.is_open()) {
        cerr << "Error: could not open " << filename << endl;
        return;
    }

    string line;
    int jobCount = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        string title, skillsString;
        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;

        title = line.substr(0, commaPos);
        skillsString = line.substr(commaPos + 1);


        // Remove surrounding quotes if present
        if (!skillsString.empty() && skillsString.front() == '"')
            skillsString = skillsString.substr(1, skillsString.size() - 2);

        SkillList skills;
        stringstream skillsStream(skillsString);
        string skill;

        while (getline(skillsStream, skill, ',')) {
            // trim
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (skill.empty()) continue;

            skill = toLowerCase(skill);
            skills.add(skill);

            if (!allValidSkills.contains(skill))
                allValidSkills.add(skill);
        }

        insertAtTail(head, title, skills);
        jobCount++;
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
        if (lowerSkill == "done") break;

        lowerSkill.erase(0, lowerSkill.find_first_not_of(" \t"));
        lowerSkill.erase(lowerSkill.find_last_not_of(" \t") + 1);

        if (allValidSkills.contains(lowerSkill)) {
            userSkills.add(lowerSkill, weightCounter);
            weightCounter++;
        }
    }

    return userSkills;
}

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


void sortByScore(Job*& head) {
    if (!head) return;

    Job* sorted = nullptr;
    Job* current = head;

    while (current) {
        Job* next = current->next;
        current->prev = current->next = nullptr;

        if (!sorted || current->matchScore > sorted->matchScore) {
            current->next = sorted;
            if (sorted) sorted->prev = current;
            sorted = current;
        } else {
            Job* temp = sorted;
            while (temp->next && temp->next->matchScore > current->matchScore)
                temp = temp->next;
            current->next = temp->next;
            if (temp->next) temp->next->prev = current;
            temp->next = current;
            current->prev = temp;
        }

        current = next;
    }

    head = sorted;
}

void displayJobs(Job* head, double minScore) {
    cout << "\n====== Matching Jobs ======\n";
    Job* temp = head;

    cout << fixed << setprecision(2);
    while (temp) {
        if (temp->matchScore >= minScore)
            cout << temp->title << " - " << temp->matchScore << "% match\n";
        temp = temp->next;
    }

    cout.unsetf(ios::fixed);
    cout.precision(6);
    cout << endl;
}

void menu(Job*& head, const SkillList& allValidSkills, size_t baselineMemory) {
    int choice;
    SkillList userSkills;

    // Variables to store performance data
    double matchDuration = 0.0, matchMemory = 0.0;
    double sortDuration = 0.0, sortMemory = 0.0;
    bool performanceRecorded = false;

    do {
        cout << "==================================" << endl;
        cout << "            Job Seeker            " << endl;
        cout << "  Linear Search & Insertion Sort  " << endl;
        cout << "==================================" << endl;
        cout << "1. Insert Skills" << endl;
        cout << "2. Performance Report" << endl;
        cout << "3. Exit" << endl;
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

                matchDuration = duration<double, milli>(matchEnd - matchStart).count();
                matchMemory = static_cast<double>(matchEndMem) / 1024.0; // Convert to MB

                // Measure sorting time & memory
                auto sortStart = high_resolution_clock::now();
                sortByScore(head);
                auto sortEnd = high_resolution_clock::now();
                size_t sortEndMem = getMemoryUsageKB() - baselineMemory;

                sortDuration = duration<double, milli>(sortEnd - sortStart).count();
                sortMemory = static_cast<double>(sortEndMem) / 1024.0; // Convert to MB

                performanceRecorded = true;

                // Display matching jobs
                displayJobs(head, 0);
                break;
            }

            case 3:
                cout << "Exiting program, have a nice day!" << endl;
                break;

            case 2:
                if (performanceRecorded) {
                    cout << fixed << setprecision(3);
                    cout << "\n=== Performance Report ===\n";
                    cout << "Search / Match Time: " << matchDuration << " ms\n";
                    cout << "Search / Match Memory: " << matchMemory << " MB\n";
                    cout << "Sort Time: " << sortDuration << " ms\n";
                    cout << "Sort Memory: " << sortMemory << " MB\n\n";
                    cout.unsetf(ios::fixed);
                } else {
                    cout << "\nNo performance data available yet. Please run 'Insert Skills' first.\n\n";
                }
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
