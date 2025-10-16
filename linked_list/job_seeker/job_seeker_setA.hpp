#ifndef JOB_SEEKER_SETA_HPP
#define JOB_SEEKER_SETA_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
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

    Job(const string& t, const SkillList& skills = {}, double score = 0.0);
};

string toLowerCase(const string& str);

void insertAtTail(Job*& head, string title, SkillList skills);
void loadJobsFromCSV(Job*& head, const string& filename, SkillList& allValidSkills);
SkillList insertSkills(const SkillList& allValidSkills);
void updateAllMatchScores(Job* head, const SkillList& userSkills);
void sortByScore(Job*& head);
void displayJobs(Job* head, double minScore);
void menu(Job*& head, const SkillList& allValidSkills, size_t baselineMemory);

#endif