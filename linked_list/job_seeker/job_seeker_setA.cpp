#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
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

struct Job {
    string title;
    unordered_set<string> requiredSkills;
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
    return pmc.WorkingSetSize / 1024; // in KB
#else
    long rss = 0L;
    std::ifstream statm("/proc/self/statm");
    long dummy;
    statm >> dummy >> rss;
    statm.close();
    return rss * (sysconf(_SC_PAGESIZE) / 1024);
#endif
}

void insertAtTail(Job*& head, string title, unordered_set<string> skills) {
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
        unordered_set<string> skills;

        getline(ss, title, ',');
        getline(ss, skillsString);

        if (!skillsString.empty() && skillsString.front() == '"')
            skillsString = skillsString.substr(1, skillsString.size() - 2);
        
        stringstream skillsStream(skillsString);
        string skill;
        while (getline(skillsStream, skill, ',')) {
            while (!skill.empty() && (skill.front() == ' ')) skill.erase(skill.begin());
            skills.insert(toLowerCase(skill));
        }

        insertAtTail(head, title, skills);
    }

    file.close();
}

unordered_set<string> insertSkills() {
    unordered_set<string> skills;
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
            skills.insert(toLowerCase(skill));
    }

    return skills;
}

void updateAllMatchScores(Job* head, unordered_set<string> skills) {
    Job* temp = head;
    while (temp) {
        int matched = 0;
        for (string js : temp->requiredSkills)
            for (string us: skills)
                if (js == us) matched++;
        temp->matchScore = ((double) matched / temp->requiredSkills.size()) * 100.0;
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
    unordered_set<string> userSkills;

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

            // Measure search/matching time + memory
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
            cout << "Total Time: " << matchDuration + sortDuration << " ms\n";
            cout << "Total Memory: " << matchMemory + sortMemory << " MB\n\n";
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
