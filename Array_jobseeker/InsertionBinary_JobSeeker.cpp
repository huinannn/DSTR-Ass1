#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

struct Job {
    string title;
    string skills[20];   // maximum 20 skills per job
    int skillCount = 0;  // actual number of skills
    int matched = 0;
    int weightedScore = 0;
    double percentage = 0.0;
};

class JobMatcher {
private:
    Job jobs[50];        // maximum 50 jobs
    int jobCount = 0;
    string seekerSkills[20];
    int seekerSkillCount = 0;

    string trim(const string &s) {
        size_t start = s.find_first_not_of(" \t");
        size_t end = s.find_last_not_of(" \t");
        return (start == string::npos) ? "" : s.substr(start, end - start + 1);
    }

    string toLower(string str) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    // Binary search to find job index by title
    int binarySearchJob(const string &title) {
        int left = 0, right = jobCount - 1;
        string searchTitle = toLower(title);
        while (left <= right) {
            int mid = left + (right - left) / 2;
            string midTitle = toLower(jobs[mid].title);
            if (midTitle == searchTitle) return mid;
            else if (midTitle < searchTitle) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

public:
    void loadJobs(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Error: Cannot open file '" << filename << "'.\n";
            exit(1);
        }

        string line;
        while (getline(file, line) && jobCount < 50) {
            if (line.empty()) continue;

            stringstream ss(line);
            string title, dummy, skillLine;
            getline(ss, title, ',');       // job title
            getline(ss, dummy, '"');       // skip quote
            getline(ss, skillLine, '"');   // skills list

            jobs[jobCount].title = trim(title);
            if (jobs[jobCount].title.empty()) continue;

            stringstream skillStream(skillLine);
            string skill;
            while (getline(skillStream, skill, ',') && jobs[jobCount].skillCount < 20) {
                string trimmedSkill = trim(skill);
                if (!trimmedSkill.empty())
                    jobs[jobCount].skills[jobs[jobCount].skillCount++] = trimmedSkill;
            }

            if (jobs[jobCount].skillCount > 0)
                jobCount++;
        }
        file.close();

        // Sort jobs alphabetically for binary search
        sort(jobs, jobs + jobCount, [](const Job &a, const Job &b) {
            return a.title < b.title;
        });

        if (jobCount == 0) {
            cerr << "❌ Error: No valid jobs found.\n";
            exit(1);
        }
    }

    void inputSeekerSkills() {
        seekerSkillCount = 0;
        cout << "Enter number of skills you have: ";
        while (!(cin >> seekerSkillCount) || seekerSkillCount <= 0 || seekerSkillCount > 20) {
            cout << "⚠️ Invalid input! Enter a number between 1 and 20: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();

        for (int i = 0; i < seekerSkillCount; i++) {
            do {
                cout << "Enter your skill " << i + 1 << ": ";
                getline(cin, seekerSkills[i]);
                seekerSkills[i] = trim(seekerSkills[i]);
                if (seekerSkills[i].empty()) {
                    cout << "⚠️ Skill cannot be empty.\n";
                }
            } while (seekerSkills[i].empty());
        }
    }

    void matchSkillsWeighted() {
        for (int i = 0; i < jobCount; i++) {
            jobs[i].matched = 0;
            jobs[i].weightedScore = 0;

            // Assign descending weights: first skill highest weight
            for (int j = 0; j < jobs[i].skillCount; j++) {
                int weight = jobs[i].skillCount - j;
                for (int s = 0; s < seekerSkillCount; s++) {
                    if (toLower(seekerSkills[s]) == toLower(jobs[i].skills[j])) {
                        jobs[i].matched++;
                        jobs[i].weightedScore += weight;
                        break;
                    }
                }
            }

            int maxWeight = jobs[i].skillCount * (jobs[i].skillCount + 1) / 2;
            jobs[i].percentage = (double)jobs[i].weightedScore / maxWeight * 100.0;
        }
    }

    // Insertion sort by weightedScore descending
    void sortJobsByWeightedScore() {
        for (int i = 1; i < jobCount; i++) {
            Job key = jobs[i];
            int j = i - 1;
            while (j >= 0 && jobs[j].weightedScore < key.weightedScore) {
                jobs[j + 1] = jobs[j];
                j--;
            }
            jobs[j + 1] = key;
        }
    }

    void displayTopMatches() {
        cout << "\nTop 3 Best-Matching Jobs (Weighted Scoring):\n";
        cout << left << setw(20) << "Job Title"
             << setw(15) << "Matched"
             << setw(15) << "Weight"
             << setw(15) << "Percentage" << endl;
        cout << string(65, '-') << endl;

        int limit = (jobCount < 3) ? jobCount : 3;
        int validCount = 0;
        for (int i = 0; i < limit; i++) {
            if (jobs[i].weightedScore > 0) {
                cout << left << setw(20) << jobs[i].title
                     << setw(15) << jobs[i].matched
                     << setw(15) << jobs[i].weightedScore
                     << fixed << setprecision(2) << jobs[i].percentage << "%" << endl;
                validCount++;
            }
        }

        if (validCount == 0) {
            cout << "⚠️ No matching jobs found.\n";
        }
    }
};

int main() {
    cout << "==============================================" << endl;
    cout << "        JOB SEEKER MATCHING SYSTEM" << endl;
    cout << "==============================================" << endl;

    JobMatcher jm;
    jm.loadJobs("../job_description/mergejob.csv");  // Adjust path if needed

    int choice;
    do {
        cout << "\n----------------------------------------------" << endl;
        jm.inputSeekerSkills();
        jm.matchSkillsWeighted();
        jm.sortJobsByWeightedScore();
        jm.displayTopMatches();

        cout << "\n----------------------------------------------" << endl;
        cout << "Action:\n1. Continue Finding Job\n2. Exit\nEnter your choice: ";
        while (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "⚠️ Invalid input! Enter 1 or 2: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
    } while (choice == 1);

    cout << "\n==============================================" << endl;
    cout << "        Thank you for using the system!" << endl;
    cout << "==============================================" << endl;
    return 0;
}
