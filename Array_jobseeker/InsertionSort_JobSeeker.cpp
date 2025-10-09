#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits> // for numeric_limits
using namespace std;

struct Job {
    string title;
    string skills[20];  // maximum 20 skills per job
    int skillCount = 0;
    int matched = 0;
    double percentage = 0.0;
};

class JobMatcher {
private:
    Job jobs[50]; // maximum 50 jobs
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

public:
    // Load jobs from CSV file
    void loadJobs(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "❌ Error: Cannot open file '" << filename << "'. Please check the path.\n";
            exit(1);
        }

        string line;
        while (getline(file, line) && jobCount < 50) {
            if (line.empty()) continue; // skip blank lines

            stringstream ss(line);
            string title, dummy, skillLine;

            getline(ss, title, ',');     // job title
            getline(ss, dummy, '"');     // skip quote
            getline(ss, skillLine, '"'); // get skills

            jobs[jobCount].title = trim(title);
            if (jobs[jobCount].title.empty()) continue; // skip invalid job entries

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

        if (jobCount == 0) {
            cerr << "❌ Error: No valid jobs found in file.\n";
            exit(1);
        }
    }

    // Input job seeker's skills with validation
    void inputSeekerSkills() {
        seekerSkillCount = 0;
        cout << "Enter number of skills you have: ";
        while (!(cin >> seekerSkillCount) || seekerSkillCount <= 0 || seekerSkillCount > 20) {
            cout << "⚠️ Invalid input! Please enter a number between 1 and 20: ";
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
                    cout << "⚠️ Skill cannot be empty. Please try again.\n";
                }
            } while (seekerSkills[i].empty());
        }
    }

    // Match skills
    void matchSkills() {
        for (int i = 0; i < jobCount; i++) {
            jobs[i].matched = 0;
            for (int s = 0; s < seekerSkillCount; s++) {
                for (int j = 0; j < jobs[i].skillCount; j++) {
                    if (toLower(seekerSkills[s]) == toLower(jobs[i].skills[j])) {
                        jobs[i].matched++;
                        break;
                    }
                }
            }
            jobs[i].percentage = (double)jobs[i].matched / jobs[i].skillCount * 100.0;
        }
    }

    // Sort jobs by percentage (descending) using Insertion Sort
    void sortJobs() {
        for (int i = 1; i < jobCount; i++) {
            Job key = jobs[i];
            int j = i - 1;
            // Sort in descending order
            while (j >= 0 && jobs[j].percentage < key.percentage) {
                jobs[j + 1] = jobs[j];
                j--;
            }
            jobs[j + 1] = key;
        }
    }

    // Display top 3 matches
    void displayTopMatches() {
        cout << "\nTop 3 Best-Matching Jobs:\n";
        cout << left << setw(20) << "Job Title"
             << setw(15) << "Matched Skills"
             << setw(15) << "Percentage" << endl;
        cout << string(50, '-') << endl;

        int limit = (jobCount < 3) ? jobCount : 3;
        int validCount = 0;

        for (int i = 0; i < limit; i++) {
            if (jobs[i].percentage > 0) {
                cout << left << setw(20) << jobs[i].title
                     << setw(15) << jobs[i].matched
                     << fixed << setprecision(2) << jobs[i].percentage << "%" << endl;
                validCount++;
            }
        }

        if (validCount == 0) {
            cout << "⚠️ No matching jobs found based on your skills.\n";
        }
    }
};

int main() {
    cout << "==============================================" << endl;
    cout << "        JOB SEEKER MATCHING SYSTEM" << endl;
    cout << "==============================================" << endl;
    cout << "Find your best job matches based on your skills." << endl;

    JobMatcher jm;
    jm.loadJobs("../job_description/mergejob.csv");  // Adjust path if needed

    int choice;
    do {
        cout << "\n----------------------------------------------" << endl;
        jm.inputSeekerSkills();
        jm.matchSkills();
        jm.sortJobs();
        jm.displayTopMatches();

        cout << "\n----------------------------------------------" << endl;
        cout << "Action:\n";
        cout << "1. Continue Finding Job" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter your choice: ";

        while (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cout << "⚠️ Invalid input! Please enter 1 or 2: ";
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
