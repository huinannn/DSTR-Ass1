#ifndef OPTIMIZEDMERGE_JOBSEEKER_HPP
#define OPTIMIZEDMERGE_JOBSEEKER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
using namespace std;
using namespace std::chrono;

struct SkillList {
    string skills[100];
    double weights[100];
    int size = 0;

    void add(const string& skill, double weight = 0.0);
    bool contains(const string& skill) const;
    double getWeight(const string& skill) const;
};

struct Job {
    string title;
    SkillList requiredSkills;
    double matchScore;
    Job* prev;
    Job* next;
};

string toLowerCase(const string& str);

void insertAtTail(Job*& head, string title, SkillList skills);
void loadJobsFromCSV(Job*& head, const string& filename, SkillList& allValidSkills);
SkillList insertSkills(const SkillList& allValidSkills);
void updateAllMatchScores(Job* head, const SkillList& userSkills);
Job* extractMatchedJobs(Job* head);

void split(Job* source, Job** frontRef, Job** backRef);
Job* merge(Job* first, Job* second);
int countJobs(Job* head);
Job* mergeSort(Job* head);
void sortByScore(Job*& head);

void displayJobs(Job* head, double minScore);
void menu(Job*& head, const SkillList& allValidSkills, size_t baselineMemory);

#endif