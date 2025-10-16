#include "InsertionBinary_JobSeeker.hpp"

#include <chrono>
using namespace std::chrono;

// ---------- Constructor ----------
JobMatcher::JobMatcher() {
    jobCount = 0;
    seekerSkillCount = 0;
}

// ---------- Trim Whitespace ----------
string JobMatcher::trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// ---------- Convert String to Lowercase ----------
string JobMatcher::toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---------- ✅ Binary Search (for Job Title) ----------
int JobMatcher::binarySearchJob(const string &title) {
    int left = 0, right = jobCount - 1;
    string searchTitle = toLower(title);
    while (left <= right) {
        int mid = left + (right - left) / 2;
        string midTitle = toLower(jobJSs[mid].title);
        if (midTitle == searchTitle)
            return mid;
        else if (midTitle < searchTitle)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// ---------- Load Jobs from CSV ----------
void JobMatcher::loadJobs(const string &filename) {
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
        getline(ss, title, ',');
        getline(ss, dummy, '"');
        getline(ss, skillLine, '"');

        jobJSs[jobCount].title = trim(title);
        if (jobJSs[jobCount].title.empty()) continue;

        stringstream skillStream(skillLine);
        string skill;
        while (getline(skillStream, skill, ',') && jobJSs[jobCount].skillCount < 20) {
            string trimmedSkill = trim(skill);
            if (!trimmedSkill.empty())
                jobJSs[jobCount].skills[jobJSs[jobCount].skillCount++] = trimmedSkill;
        }

        if (jobJSs[jobCount].skillCount > 0)
            jobCount++;
    }
    file.close();
}

// ---------- Input Seeker Skills ----------
void JobMatcher::inputSeekerSkills() {
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

// ---------- Weighted Skill Matching ----------
void JobMatcher::matchSkillsWeighted() {
    for (int i = 0; i < jobCount; i++) {
        jobJSs[i].matched = 0;
        jobJSs[i].weightedScore = 0;

        for (int j = 0; j < jobJSs[i].skillCount; j++) {
            int weight = jobJSs[i].skillCount - j;
            for (int s = 0; s < seekerSkillCount; s++) {
                if (toLower(seekerSkills[s]) == toLower(jobJSs[i].skills[j])) {
                    jobJSs[i].matched++;
                    jobJSs[i].weightedScore += weight;
                    break;
                }
            }
        }

        int maxWeight = jobJSs[i].skillCount * (jobJSs[i].skillCount + 1) / 2;
        jobJSs[i].percentage = (double)jobJSs[i].weightedScore / maxWeight * 100.0;
    }
}

// ---------- ✅ Insertion Sort (Descending by Weighted Score) ----------
void JobMatcher::sortJobsByWeightedScore() {
    for (int i = 1; i < jobCount; i++) {
        JobJS key = jobJSs[i];
        int j = i - 1;
        while (j >= 0 && jobJSs[j].weightedScore < key.weightedScore) {
            jobJSs[j + 1] = jobJSs[j];
            j--;
        }
        jobJSs[j + 1] = key;
    }
}

// ---------- Display Top 3 Matches ----------
void JobMatcher::displayTopMatches() {
    cout << "\nTop 3 Best-Matching Jobs (Weighted Scoring):\n";
    cout << left << setw(20) << "Job Title"
         << setw(15) << "Matched"
         << setw(15) << "Weight"
         << setw(15) << "Percentage" << endl;
    cout << string(65, '-') << endl;

    int limit = (jobCount < 3) ? jobCount : 3;
    int validCount = 0;
    for (int i = 0; i < limit; i++) {
        if (jobJSs[i].weightedScore > 0) {
            cout << left << setw(20) << jobJSs[i].title
                 << setw(15) << jobJSs[i].matched
                 << setw(15) << jobJSs[i].weightedScore
                 << fixed << setprecision(2) << jobJSs[i].percentage << "%" << endl;
            validCount++;
        }
    }

    if (validCount == 0) {
        cout << "⚠️ No matching jobs found.\n";
    }
}


// ---------- Main Program ----------
// ---------- Main Program ----------
void runJobSeekerSystem() {
    cout << "==============================================" << endl;
    cout << "        JOB SEEKER MATCHING SYSTEM" << endl;
    cout << "==============================================" << endl;

    JobMatcher jm;
    jm.loadJobs("job_description/mergejob.csv");

    bool running = true;
    auto systemStart = chrono::high_resolution_clock::now();

    while (running) {
        jm.inputSeekerSkills();

        auto start = chrono::high_resolution_clock::now();
        jm.matchSkillsWeighted();
        jm.sortJobsByWeightedScore();
        auto end = chrono::high_resolution_clock::now();

        jm.displayTopMatches();

        double totalTime = chrono::duration<double, milli>(end - start).count();

        // 🔁 Inner loop for menu navigation
        bool backToSkill = false;
        while (true) {
            int choice;
            cout << "\n----------------------------------------------" << endl;
            cout << "Action:\n"
                 << "1. Continue Finding Job\n"
                 << "2. Performance Summary\n"
                 << "3. Exit System\n"
                 << "Enter your choice: ";

            while (!(cin >> choice) || (choice < 1 || choice > 3)) {
                cout << "⚠️ Invalid input! Enter 1, 2, or 3: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore();

            if (choice == 1) {
                backToSkill = true;
                break; // go back to skill input
            } 
            else if (choice == 2) {
                size_t memoryUsed =
                    sizeof(jm) +
                    jm.getJobCount() * sizeof(JobJS) +
                    jm.getSeekerSkillCount() * sizeof(string);

                cout << "\n=============================\n";
                cout << "Performance Summary\n";
                cout << "=============================\n";
                cout << "Time for last job match: " << fixed << setprecision(3) << totalTime << " ms\n";
                cout << "Approx. Memory Used: " << (memoryUsed / 1024.0) << " KB\n";
                // 👇 stay in the same menu after showing summary
            } 
            else if (choice == 3) {
                running = false;
                backToSkill = false;
                cout << "\n==============================================" << endl;
                cout << "        Thank you for using the system!" << endl;
                cout << "==============================================" << endl;
                break;
            }
        }

        if (!running) break; // exit entire program
        if (!backToSkill) continue; // stay in current menu if summary shown
    }

    auto systemEnd = chrono::high_resolution_clock::now();
    double totalSystemTime = chrono::duration<double, milli>(systemEnd - systemStart).count();

    cout << "Total session runtime: " << fixed << setprecision(3) << totalSystemTime << " ms\n";
}




int main() {
    runJobSeekerSystem();
    return 0;
}