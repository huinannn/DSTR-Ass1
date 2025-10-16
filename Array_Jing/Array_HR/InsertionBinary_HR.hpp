#ifndef INSERTIONBINARY_HR_HPP
#define INSERTIONBINARY_HR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

const int MAX_JOBS = 50;
const int MAX_SKILLS = 20;
const int MAX_CANDIDATES = 500;

string trim(const string &s);
string toLower(string str);

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

class HRSystem {
private:
    JobHR jobs[MAX_JOBS];
    Candidate candidates[MAX_CANDIDATES];
    int jobCount;
    int candCount;

public:
    HRSystem();
    void loadJobs(const string &filename);
    void loadCandidates(const string &filename);
    void displayJobs();
    void insertionSort(); // labeled (Insertion Sort)
    int binarySearch(string target); // labeled (Binary Search)
    void searchAndMatch();
    void displayTop5();
};

#endif
