#ifndef INSERTIONBINARY_HR_HPP
#define INSERTIONBINARY_HR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

// ---------- Constants ----------
const int MAX_JOBS = 50;
const int MAX_SKILLS = 20;
const int MAX_CANDIDATES = 500;

// ---------- Candidate ----------
struct Candidate {
    string name;
    string skills[MAX_SKILLS];
    int skillCount;
    int matchedSkills;
    int matchedWeight;
    double percentage;

    Candidate();
};

// ---------- Job ----------
struct JobHR {
    string name;
    string skills[MAX_SKILLS];
    int skillCount;

    JobHR();
};

// ---------- HR System ----------
class HRSystem {
private:
    JobHR jobs[MAX_JOBS];
    Candidate candidates[MAX_CANDIDATES];
    int jobCount;
    int candCount;

public:
    // Constructor
    HRSystem();

    // Utility
    string trim(const string &s);
    string toLower(string str);

    // File handling
    void loadJobs(const string &filename);
    void loadCandidates(const string &filename);

    // Display
    void displayJobs();
    void displayTop5(const vector<Candidate> &sortedList);

    // Core algorithms
    int binarySearchTimed(const string &target, double &binaryTime, size_t &binaryMemory);

    // Sort only the matched list (vector) and measure time/memory for that sort
    void insertionSortTimed(vector<Candidate> &matchList, double &insertionTime, size_t &sortMemory);

    // Matching flow
    void searchAndMatch();

    // Helpers (optional)
    size_t calculateBaseMemory() const;
    size_t calculateMatchingMemory(int selectedCount) const;

    // Getters
    int getJobCount() const { return jobCount; }
    int getCandidateCount() const { return candCount; }
};

// Runner
void runHRSystem();

#endif // INSERTIONBINARY_HR_HPP
