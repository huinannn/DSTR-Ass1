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
    JobJS jobJSs[50];
    int jobCount;
    string seekerSkills[20];
    int seekerSkillCount;

    // Utility
    string trim(const string &s);
    string toLower(string str);

public:
    // Constructor
    JobMatcher();

    // Core functions
    void loadJobs(const string &filename);
    void inputSeekerSkills();
    void matchSkillsWeighted();
    void insertionSortSkills();

    // Searching
    int binarySearchJob(const string &title);
    bool binarySearchSkill(string arr[], int n, string target);

    // Getters
    int getJobCount() const { return jobCount; }
    int getSeekerSkillCount() const { return seekerSkillCount; }
    JobJS getJobAt(int index) const; // return a copy of job at index
    string getSeekerSkillAt(int index) const; // <-- ADD THIS

    // Static helpers operating on arrays of JobJS (used for matched jobs)
    static void sortJobsByWeightedScoreArray(JobJS arr[], int count);
    static void displayTopMatchesArray(JobJS arr[], int count);
};

#endif // INSERTIONBINARY_JOBSEEKER_HPP
