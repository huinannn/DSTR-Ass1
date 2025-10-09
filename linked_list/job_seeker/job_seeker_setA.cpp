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
    #include <fstream>
#endif
using namespace std;
using namespace std::chrono;

struct SkillList {
    string skills[100];
    int size = 0;

    void add(const string& skill) {
        // Avoid duplicates
        for (int i = 0; i < size; ++i) {
            if (skills[i] == skill) return;
        }
        skills[size++] = skill;
    }

    bool contains(const string& skill) const {
        for (int i = 0; i < size; ++i) {
            if (skills[i] == skill) return true;
        }
        return false;
    }
};

struct Job {
    string title;
    SkillList requiredSkills;
    double matchScore;
    Job* prev = nullptr;
    Job* next = nullptr;
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

void loadJobsFromCSV(Job*& head, const string& filename) {
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
            while (!skill.empty() && (skill.front() == ' ')) skill.erase(skill.begin());
            skills.add(toLowerCase(skill));
        }

        insertAtTail(head, title, skills);
    }

    file.close();
}

SkillList insertSkills() {
    SkillList skills;
    string skill;

    cout << "Enter your skills (type 'done' to finish):\n";
    cin.ignore();

    while (true) {
        cout << "> ";
        getline(cin, skill);

        string lowerSkill = toLowerCase(skill);
        if (lowerSkill == "done")
            break;

        skill.erase(0, skill.find_first_not_of(" \t"));
        skill.erase(skill.find_last_not_of(" \t") + 1);

        if (!skill.empty())
            skills.add(toLowerCase(skill));
    }

    return skills;
}

void updateAllMatchScores(Job* head, SkillList userSkills) {
    Job* temp = head;
    while (temp) {
        int matched = 0;
        for (int i = 0; i < temp->requiredSkills.size; ++i) {
            if (userSkills.contains(temp->requiredSkills.skills[i]))
                matched++;
        }
        temp->matchScore = ((double)matched / temp->requiredSkills.size) * 100.0;
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
    while (temp != nullptr) {
        if (temp->matchScore >= minScore)
            cout << temp->title << " - " << temp->matchScore << "% match\n";
        temp = temp->next;
    }
    cout.unsetf(ios::fixed);
    cout.precision(6); 
    cout << "\n" << endl;
}

void menu(Job*& head) {
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

        switch (choice)
        {
        case 1: {
            userSkills = insertSkills();

            // Measure search/matching time & memory
            auto matchStart = high_resolution_clock::now();
            updateAllMatchScores(head, userSkills);
            auto matchEnd = high_resolution_clock::now();
            double matchDuration = duration<double, milli>(matchEnd - matchStart).count();
            double matchMemory = getMemoryUsageKB() / 1024.0; // Convert to MB

            // Measure sorting time + memory
            auto sortStart = high_resolution_clock::now();
            sortByScore(head);
            auto sortEnd = high_resolution_clock::now();
            double sortDuration = duration<double, milli>(sortEnd - sortStart).count();
            double sortMemory = getMemoryUsageKB() / 1024.0; // Convert to MB

            // Display jobs
            displayJobs(head, 0);

            // Show focused performance report
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

    loadJobsFromCSV(head, "../../job_description/mergejob.csv");
    menu(head);
    return 0;
}
