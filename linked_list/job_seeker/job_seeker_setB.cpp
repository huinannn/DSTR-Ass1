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
            while (!skill.empty() && skill.front() == ' ') skill.erase(skill.begin());
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

// Optimized Linear Search for skill matching
void updateAllMatchScores(Job* head, const unordered_set<string>& userSkills) {
    Job* temp = head;
    while (temp) {
        int matched = 0;
        for (const auto& js : temp->requiredSkills) {
            if (userSkills.find(js) != userSkills.end()) { // O(1) lookup
                matched++;
            }
        }
        temp->matchScore = ((double)matched / temp->requiredSkills.size()) * 100.0;
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

        switch (choice) {
        case 1: {
            userSkills = insertSkills();

            // Measure optimized search/matching
            auto matchStart = high_resolution_clock::now();
            updateAllMatchScores(head, userSkills);
            auto matchEnd = high_resolution_clock::now();
            double matchDuration = duration<double, milli>(matchEnd - matchStart).count();
            double matchMemory = getMemoryUsageKB() / 1024.0; // MB

            // Measure merge sort
            auto sortStart = high_resolution_clock::now();
            sortByScore(head);
            auto sortEnd = high_resolution_clock::now();
            double sortDuration = duration<double, milli>(sortEnd - sortStart).count();
            double sortMemory = getMemoryUsageKB() / 1024.0; // MB

            displayJobs(head, 0);

            cout << fixed << setprecision(3);
            cout << "\n=== Performance Report ===\n";
            cout << "Optimized Match Time: " << matchDuration << " ms\n";
            cout << "Match Memory: " << matchMemory << " MB\n";
            cout << "Merge Sort Time: " << sortDuration << " ms\n";
            cout << "Sort Memory: " << sortMemory << " MB\n\n";
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
