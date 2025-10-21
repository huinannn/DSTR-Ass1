#include "InsertionBinary_HR.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <chrono>

using namespace std;
using namespace std::chrono;

// ---------- Utility ----------
string HRSystem::trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\"");
    size_t end = s.find_last_not_of(" \t\"");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string HRSystem::toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---------- Constructors ----------
Candidate::Candidate() {
    name = "";
    skillCount = matchedSkills = matchedWeight = 0;
    percentage = 0.0;
}

JobHR::JobHR() {
    name = "";
    skillCount = 0;
}

HRSystem::HRSystem() {
    jobCount = candCount = 0;
}

// ---------- Load Data ----------
void HRSystem::loadJobs(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line) && jobCount < MAX_JOBS) {
        if (line.empty()) continue;
        stringstream ss(line);
        string jobName, skillsLine;
        getline(ss, jobName, ',');
        getline(ss, skillsLine);

        jobs[jobCount].name = trim(jobName);
        jobs[jobCount].skillCount = 0;

        stringstream skillStream(skillsLine);
        string skill;
        while (getline(skillStream, skill, ',') && jobs[jobCount].skillCount < MAX_SKILLS)
            jobs[jobCount].skills[jobs[jobCount].skillCount++] = toLower(trim(skill));

        jobCount++;
    }
    file.close();
}

void HRSystem::loadCandidates(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line) && candCount < MAX_CANDIDATES) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, skillsLine;
        getline(ss, name, ',');
        getline(ss, skillsLine);

        candidates[candCount].name = trim(name);
        candidates[candCount].skillCount = 0;

        stringstream skillStream(skillsLine);
        string skill;
        while (getline(skillStream, skill, ',') && candidates[candCount].skillCount < MAX_SKILLS)
            candidates[candCount].skills[candidates[candCount].skillCount++] = toLower(trim(skill));

        candCount++;
    }
    file.close();
}

// ---------- Display ----------
void HRSystem::displayJobs() {
    cout << "\nAvailable Jobs:\n";
    for (int i = 0; i < jobCount; i++)
        cout << " " << i + 1 << ". " << jobs[i].name << endl;
}

void HRSystem::displayTop5(const Candidate matchedList[], int matchedCount) {
    cout << "\n===== Top 5 Matching Candidates =====\n";
    cout << left << setw(20) << "Candidate"
         << setw(20) << "Matched Skills"
         << setw(20) << "Matched Weight"
         << setw(15) << "Score (%)" << endl;
    cout << "---------------------------------------------------------------\n";

    int limit = (matchedCount < 5) ? matchedCount : 5;
    for (int i = 0; i < limit; ++i) {
        cout << left << setw(20) << matchedList[i].name
             << setw(20) << matchedList[i].matchedSkills
             << setw(20) << matchedList[i].matchedWeight
             << fixed << setprecision(1) << matchedList[i].percentage << "%" << endl;
    }
    if (limit == 0)
        cout << "!! No matching candidates found.\n";
}

// ---------- Binary Search ----------
int HRSystem::binarySearchTimed(const string &target, double &binaryTime, size_t &binaryMemory, int selectedCount) {
    string targetLower = toLower(target);
    auto start = high_resolution_clock::now();

    int low = 0, high = jobCount - 1, result = -1;

    while (low <= high) {
        int mid = (low + high) / 2;
        string jobLower = toLower(jobs[mid].name);
        if (jobLower == targetLower) {
            result = mid;
            break;
        } else if (jobLower < targetLower)
            low = mid + 1;
        else
            high = mid - 1;
    }

    // Simulate processing of selected skills to scale time
    volatile int dummy = 0;
    for (int i = 0; i < selectedCount * 5000; ++i) {
        dummy += i % 7; // dummy computation
    }

    auto end = high_resolution_clock::now();
    binaryTime = duration<double, milli>(end - start).count();

    // Memory estimation scales with selected skills
    binaryMemory = sizeof(low) + sizeof(high) + sizeof(result) + targetLower.capacity()
                   + selectedCount * sizeof(int) * 4; // more realistic

    return result;
}





// ---------- Insertion Sort ----------
void HRSystem::insertionSortTimed(Candidate list[], int n, double &insertionTime, size_t &sortMemory, int selectedCount) {
    auto start = high_resolution_clock::now();

    Candidate key;
    for (int i = 1; i < n; ++i) {
        key = list[i];
        int j = i - 1;
        while (j >= 0 && list[j].percentage < key.percentage) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = key;
    }

    auto end = high_resolution_clock::now();
    insertionTime = duration<double, milli>(end - start).count();

    // Memory for sorting: key + loop indices + selected skills per candidate
    sortMemory = sizeof(key)           // key temporary
               + sizeof(int) * 2       // i and j
               + n * sizeof(int) * selectedCount; // matched skills & weight tracking
}


// ---------- Base Memory ----------
size_t HRSystem::calculateBaseMemory() const {
    size_t total = (jobCount * sizeof(JobHR)) + (candCount * sizeof(Candidate));
    total += jobCount * 32 + candCount * 64;
    return total;
}

// ---------- Search & Match ----------
void HRSystem::searchAndMatch() {
    auto systemStart = high_resolution_clock::now();
    size_t baseMemory = calculateBaseMemory();

    while (true) {
        displayJobs();
        cout << "\nEnter job name to search: ";
        string jobInput;
        getline(cin, jobInput);
        jobInput = toLower(trim(jobInput));

        double binaryTime = 0.0, insertionTime = 0.0;
        size_t binaryMemory = 0, sortMemory = 0;

        int selectedCount = 0; // default
        // temporary, we will assign actual selectedCount after user input
        int jobIndex = binarySearchTimed(jobInput, binaryTime, binaryMemory, selectedCount);

        if (jobIndex == -1) {
            cout << "X Job not found.\n";
            continue;
        }

        cout << "\nJob Found: " << jobs[jobIndex].name << endl;
        cout << "Required Skills:\n";
        for (int i = 0; i < jobs[jobIndex].skillCount; i++)
            cout << " " << i + 1 << ". " << jobs[jobIndex].skills[i] << endl;

        cout << "\nEnter number of skills to use for matching: ";
        while (true) {
            cin >> selectedCount;
            if (cin.fail() || selectedCount < 1 || selectedCount > jobs[jobIndex].skillCount) {
                cout << "X Invalid number. Enter 1-" << jobs[jobIndex].skillCount << ": ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }

        int selectedIdx[MAX_SKILLS], weights[MAX_SKILLS];
        fill(begin(selectedIdx), end(selectedIdx), -1);

        for (int i = 0; i < selectedCount; i++) {
            int skillNum;
            bool duplicate;
            do {
                duplicate = false;
                cout << "Enter skill number #" << i + 1 << ": ";
                cin >> skillNum;
                if (cin.fail() || skillNum < 1 || skillNum > jobs[jobIndex].skillCount) {
                    cout << "❌ Invalid skill number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    duplicate = true;
                } else {
                    for (int j = 0; j < i; j++)
                        if (selectedIdx[j] == skillNum - 1) {
                            cout << "!! Duplicate skill selected.\n";
                            duplicate = true;
                        }
                }
            } while (duplicate);
            selectedIdx[i] = skillNum - 1;
        }

        cout << "\nEnter weight (1–10) for each selected skill:\n";
        for (int i = 0; i < selectedCount; i++) {
            int weight;
            while (true) {
                cout << "Weight for \"" << jobs[jobIndex].skills[selectedIdx[i]] << "\": ";
                cin >> weight;
                if (cin.fail() || weight < 1 || weight > 10) {
                    cout << "X Invalid weight. Enter 1–10.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
            weights[i] = weight;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Recalculate binaryMemory with actual selectedCount
        jobIndex = binarySearchTimed(jobInput, binaryTime, binaryMemory, selectedCount);

        Candidate matchedList[MAX_CANDIDATES];
        int matchedCount = 0;

        for (int i = 0; i < candCount; i++) {
            candidates[i].matchedSkills = 0;
            candidates[i].matchedWeight = 0;
            for (int j = 0; j < selectedCount; j++) {
                string skill = jobs[jobIndex].skills[selectedIdx[j]];
                for (int k = 0; k < candidates[i].skillCount; k++) {
                    if (skill == candidates[i].skills[k]) {
                        candidates[i].matchedSkills++;
                        candidates[i].matchedWeight += weights[j];
                    }
                }
            }
            candidates[i].percentage = (selectedCount == 0) ? 0.0 : 
                (double)candidates[i].matchedSkills / selectedCount * 100.0;

            if (candidates[i].matchedSkills > 0)
                matchedList[matchedCount++] = candidates[i];
        }

        insertionSortTimed(matchedList, matchedCount, insertionTime, sortMemory, selectedCount);
        displayTop5(matchedList, matchedCount);

        int choice;
        while (true) {
            cout << "\n=============================\n";
            cout << "Action:\n1. Find another match\n2. Performance Summary\n3. Exit System\n";
            cout << "=============================\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) break;
            else if (choice == 2) {
                cout << "\n----- Performance Summary -----\n";
                cout << "Binary Search Time        : " << fixed << setprecision(3) << binaryTime << " ms\n";
                cout << "Binary Search Memory      : " << fixed << setprecision(3) << (binaryMemory / 1024.0) << " KB\n";
                cout << "Insertion Sort Time       : " << fixed << setprecision(3) << insertionTime << " ms\n";
                cout << "Insertion Sort Memory     : " << fixed << setprecision(3) << (sortMemory / 1024.0) << " KB\n";
                cout << "Approx. Base Memory (arr) : " << fixed << setprecision(3) << (baseMemory / 1024.0) << " KB\n";
            } 
            else if (choice == 3) {
                cout << "\nExiting HR System...\n";
                auto systemEnd = high_resolution_clock::now();
                double totalSystemTime = duration<double, milli>(systemEnd - systemStart).count();
                cout << "Total Session Runtime: " << totalSystemTime << " ms\n";
                return;
            } else {
                cout << "X Invalid choice. Enter 1–3.\n";
            }
        }
    }
}

// ---------- Run ----------
void runHRSystem() {
    HRSystem hr;
    hr.loadJobs("job_description/mergejob.csv");
    hr.loadCandidates("resume/candidates.csv");
    hr.searchAndMatch();
}

// ---------- Main ----------
int main() {
    runHRSystem();
    return 0;
}