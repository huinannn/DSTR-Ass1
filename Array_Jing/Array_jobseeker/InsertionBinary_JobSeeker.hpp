#ifndef INSERTIONBINARY_JOBSEEKER_HPP
#define INSERTIONBINARY_JOBSEEKER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

// ---------- Struct for Job ----------
struct JobJS {
    string title;
    string skills[20];
    int skillCount = 0;
    int matched = 0;
    int weightedScore = 0;
    double percentage = 0.0;
};

// ---------- Job Matching System (Class) ----------
class JobMatcher {
private:
    JobJS jobJSs[50];               // List of jobs
    int jobCount;
    string seekerSkills[20];
    int seekerSkillCount;

    string trim(const string &s);
    string toLower(string str);
    int binarySearchJob(const string &title);

public:
    JobMatcher();

    void loadJobs(const string &filename);
    void inputSeekerSkills();
    void matchSkillsWeighted();
    void sortJobsByWeightedScore();
    void displayTopMatches();

    // ✅ Correct getter methods
    int getJobCount() const { return jobCount; }
    int getSeekerSkillCount() const { return seekerSkillCount; }
};

#endif // INSERTIONBINARY_JOBSEEKER_HPP