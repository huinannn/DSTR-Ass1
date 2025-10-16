#include "InsertionBinary_JobSeeker.hpp"

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
void runJobSeekerSystem() {
    cout << "==============================================" << endl;
    cout << "        JOB SEEKER MATCHING SYSTEM" << endl;
    cout << "==============================================" << endl;

    JobMatcher jm;
    jm.loadJobs("job_description/mergejob.csv");

    int choice;
    do {
        cout << "\n----------------------------------------------" << endl;
        jm.inputSeekerSkills();
        jm.matchSkillsWeighted();
        jm.sortJobsByWeightedScore(); // ✅ Insertion Sort
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

}

int main() {
    runJobSeekerSystem();
    return 0;
}