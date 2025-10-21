#ifndef INSERTIONBINARY_HR_HPP
#define INSERTIONBINARY_HR_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
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
    HRSystem();

    // Utility
    string trim(const string &s);
    string toLower(string str);

    // File handling
    void loadJobs(const string &filename);
    void loadCandidates(const string &filename);

    // Display
    void displayJobs();
    void displayTop5(const Candidate matchedList[], int matchedCount);

    // Core algorithms
    int binarySearchTimed(const string &target, double &binaryTime, size_t &binaryMemory, int selectedCount = 0);
    void insertionSortTimed(Candidate list[], int n, double &insertionTime, size_t &sortMemory, int selectedCount);

    // Base memory
    size_t calculateBaseMemory() const;

    // Main process
    void searchAndMatch();

    int getJobCount() const { return jobCount; }
    int getCandidateCount() const { return candCount; }
};

// Runner
void runHRSystem();

#endif // INSERTIONBINARY_HR_HPP
