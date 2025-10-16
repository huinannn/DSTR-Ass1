#ifndef INSERTIONBINARY_HR_HPP
#define INSERTIONBINARY_HR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <chrono> // For performance timing
using namespace std;

// =======================
//  CONSTANT DEFINITIONS
// =======================
const int MAX_JOBS = 50;
const int MAX_SKILLS = 20;
const int MAX_CANDIDATES = 500;

// =======================
//  UTILITY FUNCTIONS
// =======================
string trim(const string &s);
string toLower(string str);

// =======================
//  STRUCT DEFINITIONS
// =======================
struct Candidate {
    string name;
    string skills[MAX_SKILLS];
    int skillCount;
    int matchedSkills;
    int matchedWeight;
    double percentage;
    Candidate();
};

struct JobHR {
    string name;
    string skills[MAX_SKILLS];
    int skillCount;
    JobHR();
};

// =======================
//  CLASS: HRSystem
// =======================
class HRSystem {
private:
    JobHR jobs[MAX_JOBS];
    Candidate candidates[MAX_CANDIDATES];
    int jobCount;
    int candCount;

public:
    HRSystem();

    // File loading
    void loadJobs(const string &filename);
    void loadCandidates(const string &filename);

    // Display and Search
    void displayJobs();
    int binarySearch(string target); // Binary Search for job name

    // Sorting
    void insertionSort(); // Insertion Sort by percentage

    // Matching and Results
    void searchAndMatch();
    void displayTop5();
};

// =======================
//  MAIN EXECUTION FUNCTION
// =======================
void runHRSystem();

#endif
