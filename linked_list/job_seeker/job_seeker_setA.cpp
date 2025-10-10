#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <unistd.h>
#endif
using namespace std;
using namespace std::chrono;

struct SkillList {
    string skills[100];
    double weights[100];
    int size = 0;

    void add(const string& skill, double weight = 0.0) {
        skills[size] = skill;
        weights[size] = weight;
        size++;
    }

    bool contains(const string& skill) const {
        for (int i = 0; i < size; ++i)
            if (skills[i] == skill)
                return true;
        return false;
    }

    double getWeight(const string& skill) const {
        for (int i = 0; i < size; ++i)
            if (skills[i] == skill)
                return weights[i];
        return 0.0;
    }
};

struct Job {
    string title;
    SkillList requiredSkills;
    double matchScore;
    Job* prev = nullptr;
    Job* next = nullptr;

    Job(const string& t, const SkillList& skills = {}, double score = 0.0)
        : title(t), requiredSkills(skills), matchScore(score) {}
};

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
    return pmc.WorkingSetSize / 1024; // KB
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

void updateAllMatchScores(Job* head, SkillList userSkills) {
    Job* temp = head;
    while (temp) {
        double matchedWeight = 0.0;
        double totalUserWeight = (userSkills.size * (userSkills.size + 1)) / 2.0;

        for (int i = 0; i < temp->requiredSkills.size; ++i) {
            const string& reqSkill = temp->requiredSkills.skills[i];

            if (userSkills.contains(reqSkill)) {
                matchedWeight += userSkills.getWeight(reqSkill);
            }
        }

        if (totalUserWeight > 0)
            temp->matchScore = (matchedWeight / totalUserWeight) * 100.0;
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

void menu(Job*& head, const SkillList& allValidSkills) {
    int choice;
    SkillList userSkills;

    do {
        cout << "==================================" << endl;
        cout << "            Job Seeker            " << endl;
        cout << "==================================" << endl;
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
                double matchDuration = duration<double, milli>(matchEnd - matchStart).count();
                double matchMemory = getMemoryUsageKB() / 1024.0; // Convert to MB

                // Measure sorting time & memory
                auto sortStart = high_resolution_clock::now();
                sortByScore(head);
                auto sortEnd = high_resolution_clock::now();
                double sortDuration = duration<double, milli>(sortEnd - sortStart).count();
                double sortMemory = getMemoryUsageKB() / 1024.0; // Convert to MB

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

    loadJobsFromCSV(head, "../../job_description/mergejob.csv", allValidSkills);
    menu(head, allValidSkills);
    return 0;
}
