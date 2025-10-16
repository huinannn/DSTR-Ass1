#ifndef OPTIMIZEDMERGE_HR_HPP
#define OPTIMIZEDMERGE_HR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;

struct SkillNode {
    string skill;
    SkillNode* next;
};

struct Candidate {
    string name;
    SkillNode* skills;
    int matchedSkillCount;
    int weightedScore;
    double score;
    Candidate* next;
};

struct JobRole {
    string roleName;
    SkillNode* skills;
    JobRole* next;
    JobRole* prev;
};

struct MatchResult {
    Candidate* sortedCandidates;
    double searchTimeMS;
    double sortTimeMS;
    size_t searchMemoryKB;
    size_t sortMemoryKB;
};

class Utils {
public:
    static string trim(const string& str);
    static string toLower(const string& str);
    static string normalizeSkill(const string& s);
    static SkillNode* addSkill(SkillNode* head, const string& skill);
    static int countSkills(SkillNode* head);
    static bool skillExists(SkillNode* head, const string& target);
    static SkillNode* buildSkillList(const string& input, SkillNode* jobRoleSkills);
    static void sortSkills(SkillNode*& head);
};

class FileLoader {
public:
    static SkillNode* parseSkills(const string & skillsStr);
    static JobRole* loadJobs(const string& filename);
    static Candidate* loadCandidates(const string& filename);
};

class OptimizedLinearSearch {
public:
    static JobRole* findRole(JobRole* head, const string& searchRole);
    static bool hasSkill(Candidate* c, const string& skill);
};

class MergeSort {
public:
    static Candidate* sortCandidates(Candidate* head);
private:
    static void splitList(Candidate* source, Candidate** front, Candidate** back);
    static Candidate* merge(Candidate* a, Candidate* b);
};

class InputUtils {
public:
    static string getInput(const string& prompt, const string& valid1 = "", const string& valid2 = "", const string& valid3 = "");
    static JobRole* getValidJobRole(JobRole* jobs);
    static string getSkillsInput(JobRole* role);
};

class Matcher {
public:
    static MatchResult matchCandidates(JobRole* role, Candidate* candidates, SkillNode* searchSkills);
};

#endif