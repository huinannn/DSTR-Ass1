#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits> // for input validation
using namespace std;

const int MAX_JOBS = 50;
const int MAX_SKILLS = 20;
const int MAX_CANDIDATES = 500;

// Utility functions
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---------- Class Definitions ----------

class Candidate {
public:
    string name;
    string skills[MAX_SKILLS];
    int skillCount;
    int matched;
    double percentage;

    Candidate() {
        name = "";
        skillCount = 0;
        matched = 0;
        percentage = 0.0;
    }
};

class Job {
public:
    string name;
    string skills[MAX_SKILLS];
    int skillCount;

    Job() {
        name = "";
        skillCount = 0;
    }
};

class HRSystem {
private:
    Job jobs[MAX_JOBS];
    Candidate candidates[MAX_CANDIDATES];
    int jobCount;
    int candCount;

    // Binary Search
    int binarySearch(string target) {
        int low = 0, high = jobCount - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            string jobLower = toLower(jobs[mid].name);
            if (jobLower == target)
                return mid;
            else if (jobLower < target)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    // Insertion Sort (descending)
    void insertionSort() {
        for (int i = 1; i < candCount; i++) {
            Candidate key = candidates[i];
            int j = i - 1;
            while (j >= 0 && candidates[j].percentage < key.percentage) {
                candidates[j + 1] = candidates[j];
                j--;
            }
            candidates[j + 1] = key;
        }
    }

public:
    HRSystem() {
        jobCount = 0;
        candCount = 0;
    }

    void loadJobs(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Error: Cannot open " << filename << endl;
            exit(1);
        }

        string line;
        while (getline(file, line) && jobCount < MAX_JOBS) {
            if (line.empty()) continue; // ⚠️ skip empty lines
            stringstream ss(line);
            string jobName, skillsLine;
            getline(ss, jobName, ',');
            getline(ss, skillsLine);

            if (!skillsLine.empty() && skillsLine.front() == '"') skillsLine.erase(0, 1);
            if (!skillsLine.empty() && skillsLine.back() == '"') skillsLine.pop_back();

            jobs[jobCount].name = trim(jobName);
            if (jobs[jobCount].name.empty()) continue; // ⚠️ skip invalid job lines

            stringstream skillStream(skillsLine);
            string skill;
            int sCount = 0;
            while (getline(skillStream, skill, ',') && sCount < MAX_SKILLS) {
                jobs[jobCount].skills[sCount++] = trim(skill);
            }
            jobs[jobCount].skillCount = sCount;
            jobCount++;
        }
        file.close();

        if (jobCount == 0) {
            cerr << "❌ Error: No job data loaded from file." << endl;
            exit(1);
        }

        // Sort jobs alphabetically
        for (int i = 0; i < jobCount - 1; i++) {
            for (int j = i + 1; j < jobCount; j++) {
                if (toLower(jobs[i].name) > toLower(jobs[j].name)) {
                    swap(jobs[i], jobs[j]);
                }
            }
        }
    }

    void loadCandidates(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Error: Cannot open " << filename << endl;
            exit(1);
        }

        string line;
        while (getline(file, line) && candCount < MAX_CANDIDATES) {
            if (line.empty()) continue;
            stringstream ss(line);
            string name, skillsLine;
            getline(ss, name, ',');
            getline(ss, skillsLine);

            if (!skillsLine.empty() && skillsLine.front() == '"') skillsLine.erase(0, 1);
            if (!skillsLine.empty() && skillsLine.back() == '"') skillsLine.pop_back();

            candidates[candCount].name = trim(name);
            if (candidates[candCount].name.empty()) continue; // ⚠️ skip invalid lines

            stringstream skillStream(skillsLine);
            string skill;
            int count = 0;
            while (getline(skillStream, skill, ',') && count < MAX_SKILLS) {
                candidates[candCount].skills[count++] = toLower(trim(skill));
            }
            candidates[candCount].skillCount = count;
            candCount++;
        }
        file.close();

        if (candCount == 0) {
            cerr << "❌ Error: No candidate data loaded from file." << endl;
            exit(1);
        }
    }

    void displayJobs() {
        cout << "\n==================================\n";
        cout << "===== HR Job Matching System =====\n";
        cout << "==================================\n";
        cout << "Available Jobs:\n";
        for (int i = 0; i < jobCount; i++) {
            cout << " " << i + 1 << ". " << jobs[i].name << endl;
        }
    }

    void searchAndMatch() {
        cout << "\nEnter job name to search: ";
        string jobInput;
        getline(cin, jobInput);
        jobInput = toLower(trim(jobInput));

        if (jobInput.empty()) {
            cout << "⚠️ Invalid input. Please enter a job name.\n";
            return;
        }

        int jobIndex = binarySearch(jobInput);
        if (jobIndex == -1) {
            cout << "\n❌ Job not found.\n";
            return;
        }

        cout << "\n✅ Job Found: " << jobs[jobIndex].name << endl;
        cout << "Required Skills:\n";
        for (int i = 0; i < jobs[jobIndex].skillCount; i++) {
            cout << " " << i + 1 << ". " << jobs[jobIndex].skills[i] << endl;
        }

        int numSkills;
        cout << "\nEnter number of skills to use for matching: ";
        while (!(cin >> numSkills) || numSkills <= 0 || numSkills > jobs[jobIndex].skillCount) {
            cout << "⚠️ Invalid number of skills. Please try again: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        int chosenIndexes[MAX_SKILLS];
        cout << "Enter the skill numbers separated by spaces: ";
        for (int i = 0; i < numSkills; i++) {
            int skillNum;
            cin >> skillNum;
            if (skillNum < 1 || skillNum > jobs[jobIndex].skillCount) {
                cout << "⚠️ Invalid skill number. Defaulting to first skill.\n";
                chosenIndexes[i] = 0;
            } else {
                chosenIndexes[i] = skillNum - 1;
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Match candidates
        for (int i = 0; i < candCount; i++) {
            int matched = 0;
            for (int j = 0; j < numSkills; j++) {
                string selectedSkill = toLower(trim(jobs[jobIndex].skills[chosenIndexes[j]]));
                for (int k = 0; k < candidates[i].skillCount; k++) {
                    if (selectedSkill == candidates[i].skills[k]) {
                        matched++;
                        break;
                    }
                }
            }
            candidates[i].matched = matched;
            candidates[i].percentage = ((double)matched / numSkills) * 100.0;
        }

        insertionSort();
        displayTop5();
    }

    void displayTop5() {
        cout << "\n===== Top 5 Matching Candidates =====\n";
        cout << left << setw(20) << "Candidate"
             << setw(20) << "Matched Skills"
             << setw(15) << "Score (%)" << endl;
        cout << "-------------------------------------------------\n";

        int displayCount = (candCount < 5) ? candCount : 5;
        for (int i = 0; i < displayCount; i++) {
            cout << left << setw(20) << candidates[i].name
                 << setw(20) << candidates[i].matched
                 << fixed << setprecision(1) << candidates[i].percentage << "%" << endl;
        }
    }
};

// ---------- Main ----------
int main() {
    HRSystem hr;
    hr.loadJobs("../job_description/mergejob.csv");
    hr.loadCandidates("../resume/candidates.csv");

    hr.displayJobs();
    hr.searchAndMatch();

    int choice;
    do {
        cout << "\n=============================\n";
        cout << "Action:\n";
        cout << "1. Find another match\n";
        cout << "2. Exit\n";
        cout << "=============================\n";
        cout << "Enter your choice: ";

        while (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "⚠️ Invalid input. Please enter 1 or 2: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                hr.displayJobs();
                hr.searchAndMatch();
                break;
            case 2:
                cout << "\n==============================================" << endl;
                cout << "        Thank you for using the system!" << endl;
                cout << "==============================================" << endl;
                break;
        }
    } while (choice != 2);

    return 0;
}
