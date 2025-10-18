// InsertionBinary_HR.cpp
#include "InsertionBinary_HR.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <vector>

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
    skillCount = matchedSkills = matchedWeight = 0;
    percentage = 0.0;
}

JobHR::JobHR() {
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
    cout << "Available Jobs:\n";
    for (int i = 0; i < jobCount; i++)
        cout << " " << i + 1 << ". " << jobs[i].name << endl;
}

void HRSystem::displayTop5(const vector<Candidate> &sortedList) {
    cout << "\n===== Top 5 Matching Candidates =====\n";
    cout << left << setw(20) << "Candidate"
         << setw(20) << "Matched Skills"
         << setw(20) << "Matched Weight"
         << setw(15) << "Score (%)" << endl;
    cout << "---------------------------------------------------------------\n";
    int limit = min((int)sortedList.size(), 5);
    for (int i = 0; i < limit; ++i) {
        cout << left << setw(20) << sortedList[i].name
             << setw(20) << sortedList[i].matchedSkills
             << setw(20) << sortedList[i].matchedWeight
             << fixed << setprecision(1) << sortedList[i].percentage << "%" << endl;
    }
    if (limit == 0) {
        cout << "!! No matching candidates found.\n";
    }
}

// ---------- Binary Search ----------
int HRSystem::binarySearchTimed(const string &target, double &binaryTime, size_t &binaryMemory) {
    auto start = high_resolution_clock::now();

    int low = 0, high = jobCount - 1;
    int result = -1;
    string targetLower = toLower(target);

    while (low <= high) {
        int mid = (low + high) / 2;
        string jobLower = toLower(jobs[mid].name);
        if (jobLower == targetLower) {
            result = mid;
            break;
        } else if (jobLower < targetLower) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    auto end = high_resolution_clock::now();
    binaryTime = duration<double, milli>(end - start).count();

    // approximate memory used by the search variables
    binaryMemory = sizeof(int) * 3 + sizeof(string) * 2;
    return result;
}

// ---------- Insertion Sort (on vector of matched candidates) ----------
void HRSystem::insertionSortTimed(vector<Candidate> &matchList, double &insertionTime, size_t &sortMemory) {
    auto start = high_resolution_clock::now();

    int n = (int)matchList.size();
    for (int i = 1; i < n; i++) {
        Candidate key = matchList[i];
        int j = i - 1;
        while (j >= 0 && matchList[j].percentage < key.percentage) { // descending by percentage
            matchList[j + 1] = matchList[j];
            j--;
        }
        matchList[j + 1] = key;
    }

    auto end = high_resolution_clock::now();
    insertionTime = duration<double, milli>(end - start).count();

    // memory used for sorting the temporary matched list
    sortMemory = sizeof(Candidate) * n + sizeof(Candidate) + sizeof(int) * 2;
}

// ---------- Memory helpers ----------
size_t HRSystem::calculateBaseMemory() const {
    return sizeof(*this) + sizeof(JobHR) * jobCount + sizeof(Candidate) * candCount;
}

size_t HRSystem::calculateMatchingMemory(int selectedCount) const {
    // approximate memory cost for matching procedure (variables + strings)
    return sizeof(int) * 3 + sizeof(string) * (2 + selectedCount) + calculateBaseMemory();
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

        double binaryTime = 0.0, insertionTime = 0.0, matchingTime = 0.0;
        size_t binaryMemory = 0, sortMemory = 0;

        int jobIndex = binarySearchTimed(jobInput, binaryTime, binaryMemory);
        if (jobIndex == -1) {
            cout << "X Job not found.\n";
            continue;
        }

        cout << "\nJob Found: " << jobs[jobIndex].name << endl;
        cout << "Required Skills:\n";
        for (int i = 0; i < jobs[jobIndex].skillCount; i++) {
            string skill = trim(jobs[jobIndex].skills[i]);
            if (!skill.empty() && skill.front() == '"') skill.erase(skill.begin());
            if (!skill.empty() && skill.back() == '"') skill.pop_back();
            jobs[jobIndex].skills[i] = trim(skill);
            cout << " " << i + 1 << ". " << jobs[jobIndex].skills[i] << endl;
        }

        int selectedCount;
        cout << "\nEnter number of skills to use for matching: ";
        while (true) {
            cin >> selectedCount;
            if (cin.fail() || selectedCount < 1 || selectedCount > jobs[jobIndex].skillCount) {
                cout << "X Invalid number. Enter 1-" << jobs[jobIndex].skillCount << ": ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }

        binaryMemory = calculateMatchingMemory(selectedCount);

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
                    cout << "❌ Invalid skill number. Try again.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    duplicate = true;
                } else {
                    for (int j = 0; j < i; j++) {
                        if (selectedIdx[j] == skillNum - 1) {
                            cout << "!! Duplicate skill selected. Choose different.\n";
                            duplicate = true;
                            break;
                        }
                    }
                }
            } while (duplicate);
            selectedIdx[i] = skillNum - 1;
        }

        cout << "\nEnter weight (1-10) for each selected skill:\n";
        for (int i = 0; i < selectedCount; i++) {
            int weight;
            while (true) {
                cout << "Weight for \"" << jobs[jobIndex].skills[selectedIdx[i]] << "\": ";
                cin >> weight;
                if (cin.fail() || weight < 1 || weight > 10) {
                    cout << "X Invalid weight. Enter 1-10.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
            weights[i] = weight;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Build temporary vector of candidates (only first candCount elements)
        vector<Candidate> matchList;
        matchList.reserve(candCount);
        for (int i = 0; i < candCount; ++i) matchList.push_back(candidates[i]);

        // Matching process: compute matchedSkills, matchedWeight, percentage
        auto matchStart = high_resolution_clock::now();
        for (auto &cand : matchList) {
            cand.matchedSkills = 0;
            cand.matchedWeight = 0;
            for (int j = 0; j < selectedCount; j++) {
                string skill = jobs[jobIndex].skills[selectedIdx[j]];
                for (int k = 0; k < cand.skillCount; k++) {
                    if (skill == cand.skills[k]) {
                        cand.matchedSkills++;
                        cand.matchedWeight += weights[j];
                    }
                }
            }
            cand.percentage = (selectedCount == 0) ? 0.0 : (double)cand.matchedSkills / selectedCount * 100.0;
        }
        auto matchEnd = high_resolution_clock::now();
        matchingTime = duration<double, milli>(matchEnd - matchStart).count();

        // Remove candidates with no matches (so sorting only considers matched ones)
        vector<Candidate> onlyMatched;
        onlyMatched.reserve(matchList.size());
        for (const auto &c : matchList) {
            if (c.matchedSkills > 0) onlyMatched.push_back(c);
        }

        // Sort only the matched candidates and measure sort time/memory
        insertionSortTimed(onlyMatched, insertionTime, sortMemory);

        // Display top 5 from onlyMatched
        displayTop5(onlyMatched);

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
                cout << "Matching Process Time     : " << fixed << setprecision(3) << matchingTime << " ms\n";
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
            } else cout << "X Invalid choice. Enter 1-3.\n";
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
