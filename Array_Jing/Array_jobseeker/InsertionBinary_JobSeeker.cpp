#include "InsertionBinary_JobSeeker.hpp"
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <limits>
using namespace std::chrono;

// ---------- Constructor ----------
JobMatcher::JobMatcher() {
    jobCount = 0;
    seekerSkillCount = 0;
}

// ---------- Trim Whitespace ----------
string JobMatcher::trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// ---------- Convert String to Lowercase ----------
string JobMatcher::toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ---------- Binary Search (for Job Title) ----------
int JobMatcher::binarySearchJob(const string &title) {
    int left = 0, right = jobCount - 1;
    string searchTitle = toLower(title); // one-time lowercase
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (jobJSs[mid].title == searchTitle)
            return mid;
        else if (jobJSs[mid].title < searchTitle)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// ---------- Binary Search Helper (for Skills) ----------
bool JobMatcher::binarySearchSkill(string arr[], int n, string target) {
    int left = 0, right = n - 1;
    target = toLower(target);

    while (left <= right) {
        int mid = left + (right - left) / 2;
        string midVal = toLower(arr[mid]);

        if (midVal == target)
            return true;
        else if (midVal < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return false;
}

// ---------- Insertion Sort (Seeker Skills) ----------
void JobMatcher::insertionSortSkills() {
    int indices[20];
    for (int i = 0; i < seekerSkillCount; i++) indices[i] = i;

    for (int i = 1; i < seekerSkillCount; i++) {
        int keyIndex = indices[i];
        int j = i - 1;
        while (j >= 0 && toLower(seekerSkills[indices[j]]) > toLower(seekerSkills[keyIndex])) {
            indices[j + 1] = indices[j];
            j--;
        }
        indices[j + 1] = keyIndex;
    }

    string sorted[20];
    for (int i = 0; i < seekerSkillCount; i++)
        sorted[i] = seekerSkills[indices[i]];

    for (int i = 0; i < seekerSkillCount; i++)
        seekerSkills[i] = sorted[i];
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

        jobJSs[jobCount].title = toLower(trim(title));
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

    // Sort jobs by title alphabetically for binary search
    for (int i = 1; i < jobCount; ++i) {
        JobJS key = jobJSs[i];
        int j = i - 1;
        while (j >= 0 && jobJSs[j].title > key.title) {
            jobJSs[j + 1] = jobJSs[j];
            j--;
        }
        jobJSs[j + 1] = key;
    }
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

// ---------- Getter for individual seeker skill ----------
string JobMatcher::getSeekerSkillAt(int index) const {
    if (index < 0 || index >= seekerSkillCount) return "";
    return seekerSkills[index];
}

// ---------- Weighted Skill Matching ----------
void JobMatcher::matchSkillsWeighted() {
    insertionSortSkills();

    for (int i = 0; i < jobCount; i++) {
        jobJSs[i].matched = 0;
        jobJSs[i].weightedScore = 0;

        for (int j = 0; j < jobJSs[i].skillCount; j++) {
            int weight = jobJSs[i].skillCount - j;
            if (binarySearchSkill(seekerSkills, seekerSkillCount, jobJSs[i].skills[j])) {
                jobJSs[i].matched++;
                jobJSs[i].weightedScore += weight;
            }
        }

        int maxWeight = jobJSs[i].skillCount * (jobJSs[i].skillCount + 1) / 2;
        jobJSs[i].percentage = (maxWeight == 0) ? 0.0 : (double)jobJSs[i].weightedScore / maxWeight * 100.0;
    }
}

// ---------- Getter for a job (copy) ----------
JobJS JobMatcher::getJobAt(int index) const {
    if (index < 0 || index >= jobCount) return JobJS();
    return jobJSs[index];
}

// ---------- Static: Sort jobs by weighted score using indices ----------
void JobMatcher::sortJobsByWeightedScoreArray(JobJS arr[], int count) {
    for (int i = 1; i < count; i++) {
        JobJS key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].weightedScore < key.weightedScore) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---------- Static: Display Top Matches ----------
void JobMatcher::displayTopMatchesArray(JobJS arr[], int count) {
    cout << "\nTop 3 Best-Matching Jobs (Weighted Scoring):\n";
    cout << left << setw(30) << "Job Title"
         << setw(12) << "Matched"
         << setw(12) << "Weight"
         << setw(12) << "Percentage" << endl;
    cout << string(70, '-') << endl;

    int limit = (count < 3) ? count : 3;
    int validCount = 0;
    for (int i = 0; i < limit; i++) {
        if (arr[i].weightedScore > 0) {
            cout << left << setw(30) << arr[i].title
                 << setw(12) << arr[i].matched
                 << setw(12) << arr[i].weightedScore
                 << fixed << setprecision(2) << arr[i].percentage << "%" << endl;
            validCount++;
        }
    }

    if (validCount == 0) cout << "⚠️ No matching jobs found.\n";
}

// ---------- Main Runner ----------
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

        auto startMatch = chrono::high_resolution_clock::now();
        jm.matchSkillsWeighted();
        auto endMatch = chrono::high_resolution_clock::now();

        // Use indices to reduce memory for matched jobs
        int matchedIndices[50];
        int matchedJobCount = 0;
        for (int i = 0; i < jm.getJobCount(); i++) {
            if (jm.getJobAt(i).weightedScore > 0)
                matchedIndices[matchedJobCount++] = i;
        }

        // Sort matched jobs by weighted score
        JobJS sortedMatches[50];
        for (int i = 0; i < matchedJobCount; i++)
            sortedMatches[i] = jm.getJobAt(matchedIndices[i]);

        auto startSort = chrono::high_resolution_clock::now();
        JobMatcher::sortJobsByWeightedScoreArray(sortedMatches, matchedJobCount);
        auto endSort = chrono::high_resolution_clock::now();

        JobMatcher::displayTopMatchesArray(sortedMatches, matchedJobCount);

        double matchTime = chrono::duration<double, milli>(endMatch - startMatch).count();
        double sortTime = chrono::duration<double, milli>(endSort - startSort).count();

        bool backToSkill = false;
        while (true) {
            int choice;
            cout << "\n----------------------------------------------" << endl;
            cout << "Action:\n1. Continue Finding Job\n2. Performance Summary\n3. Exit System\nEnter your choice: ";

            while (!(cin >> choice) || (choice < 1 || choice > 3)) {
                cout << "⚠️ Invalid input! Enter 1, 2, or 3: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore();

            if (choice == 1) {
                backToSkill = true;
                break;
            } else if (choice == 2) {
                size_t skillMem = jm.getSeekerSkillCount() * sizeof(string);
                size_t jobMem = matchedJobCount * sizeof(JobJS);
                size_t baseMem = sizeof(jm);

                cout << "\n=============================\n";
                cout << "Performance Summary\n";
                cout << "=============================\n";
                cout << "Skill Matching (Binary) Time : " << fixed << setprecision(3) << matchTime << " ms\n";
                cout << "Skill Matching Memory        : " << fixed << setprecision(3) << (skillMem / 1024.0) << " KB\n";
                cout << "Insertion Sort Time          : " << fixed << setprecision(3) << sortTime << " ms\n";
                cout << "Insertion Sort Memory        : " << fixed << setprecision(3) << (jobMem / 1024.0) << " KB\n";
                // cout << "Approx. Base Memory (JobMatcher object) : " << fixed << setprecision(3) << (baseMem / 1024.0) << " KB\n";
            } else if (choice == 3) {
                running = false;
                backToSkill = false;
                cout << "\n==============================================" << endl;
                cout << "        Thank you for using the system!" << endl;
                cout << "==============================================" << endl;
                break;
            }
        }

        if (!running) break;
        if (!backToSkill) continue;
    }

    auto systemEnd = chrono::high_resolution_clock::now();
    double totalSystemTime = chrono::duration<double, milli>(systemEnd - systemStart).count();
    cout << "Total session runtime: " << fixed << setprecision(3) << totalSystemTime << " ms\n";
}

int main() {
    runJobSeekerSystem();
    return 0;
}
