#include "MergeJump_JobSeeker.hpp"

// ====================== Common Function Implementations ======================
string toLower(const string &str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

DynamicArray<string> splitSkills(const string &line, bool doSort = true) {
    DynamicArray<string> skills;
    stringstream ss(line);
    string skill;

    while (getline(ss, skill, ',')) {
        skill = trim(skill);
        if (!skill.empty()) skills.push_back(toLower(skill));
    }
    
    if (doSort && skills.getSize() > 1) {
        // Bubble sort for simplicity
        for (int i = 0; i < skills.getSize() - 1; i++) {
            for (int j = 0; j < skills.getSize() - i - 1; j++) {
                if (skills[j] > skills[j + 1]) {
                    string temp = skills[j];
                    skills[j] = skills[j + 1];
                    skills[j + 1] = temp;
                }
            }
        }
    }
    return skills;
}

bool jumpSearch(const DynamicArray<string> &arr, const string &target) {
    int n = arr.getSize();
    if (n == 0) return false;
    int step = sqrt(n);
    int prev = 0;
    while (prev < n && arr[min(step, n) - 1] < target) {
        prev = step;
        step += sqrt(n);
        if (prev >= n) return false;
    }
    for (int i = prev; i < min(step, n); i++) {
        if (arr[i] == target) return true;
    }
    return false;
}

DynamicArray<Job> readJobs(const string &filename) {
    DynamicArray<Job> jobs;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;

        string title = trim(line.substr(0, commaPos));
        string skillsStr = line.substr(commaPos + 1);
        Job j;
        j.title = title;
        j.skills = splitSkills(skillsStr, false); 
        jobs.push_back(j);
    }
    return jobs;
}

// ====================== Job Seeker Mode ======================
void jobSeekerMode(const DynamicArray<Job> &jobs) {
    bool running = true;

    while (running) {
        cout << "\n==============================================\n";
        cout << "        JOB SEEKER MATCHING SYSTEM\n";
        cout << "==============================================\n\n";

        cout << "----------------------------------------------\n";
        cout << "Enter number of skills you have: ";
        int skillCount;
        cin >> skillCount;
        cin.ignore(); // clear newline

        DynamicArray<string> userSkills;
        for (int i = 0; i < skillCount; i++) {
            cout << "Enter your skill " << (i + 1) << ": ";
            string s;
            getline(cin, s);
            userSkills.push_back(trim(toLower(s))); // normalize input
        }

        // Start timer
        auto start = high_resolution_clock::now();

        struct Result {
            string title;
            int matchedSkills;
            int matchedWeight;
            double percentage;
        };

        DynamicArray<Result> results;

        // Calculate weighted scores
        for (int i = 0; i < jobs.getSize(); i++) {
            const Job &job = jobs[i];
            int totalSkills = job.skills.getSize();
            int totalPossibleWeight = 0;
            for (int w = totalSkills; w >= 1; w--)
                totalPossibleWeight += w;

            int matchedSkills = 0;
            int matchedWeight = 0;

            for (int j = 0; j < totalSkills; j++) {
                int weight = totalSkills - j;
                string skill = trim(toLower(job.skills[j]));

                for (int k = 0; k < userSkills.getSize(); k++) {
                    if (toLower(userSkills[k]) == skill) {
                        matchedSkills++;
                        matchedWeight += weight;
                        break;
                    }
                }
            }

            double percentage = (double)matchedWeight / totalPossibleWeight * 100.0;

            // Only store results with non-zero match
            if (matchedSkills > 0) {
                Result r{job.title, matchedSkills, matchedWeight, percentage};
                results.push_back(r);
            }
        }

        // Stop timer
        auto end = high_resolution_clock::now();
        double totalTime = duration<double, milli>(end - start).count();

        // Memory estimate
        size_t memoryUsed = sizeof(jobs) + sizeof(results)
            + userSkills.getCapacity() * sizeof(string)
            + results.getCapacity() * sizeof(Result);

        // Sort by weight (descending)
        for (int i = 0; i < results.getSize() - 1; i++) {
            for (int j = 0; j < results.getSize() - i - 1; j++) {
                if ((results[j].matchedWeight < results[j + 1].matchedWeight) ||
                    (results[j].matchedWeight == results[j + 1].matchedWeight &&
                     results[j].percentage < results[j + 1].percentage)) {
                    Result temp = results[j];
                    results[j] = results[j + 1];
                    results[j + 1] = temp;
                }
            }
        }

        // Display top 3 jobs
        if (results.getSize() > 0) {
            cout << "\nTop 3 Best-Matching Jobs:\n";
            cout << left << setw(20) << "Job Title"
                 << setw(15) << "Matched"
                 << setw(15) << "Weight"
                 << setw(15) << "Percentage\n";
            cout << string(65, '-') << "\n";

            int topN = min(3, results.getSize());
            for (int i = 0; i < topN; i++) {
                cout << left << setw(20) << results[i].title
                     << setw(15) << results[i].matchedSkills
                     << setw(15) << results[i].matchedWeight
                     << fixed << setprecision(2)
                     << results[i].percentage << "%\n";
            }
        } else {
            cout << "\nNo matching jobs found.\n";
        }

        // Action menu
        bool backToMenu = false;
        while (!backToMenu) {
            cout << "\n----------------------------------------------\n";
            cout << "Action:\n";
            cout << "1. Continue Finding Job\n";
            cout << "2. Generate Performance Report\n";
            cout << "3. Exit\n";
            cout << "Enter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                backToMenu = true; // re-run the job finding loop
            }
            else if (choice == 2) {
                cout << "\n=============================\n";
                cout << "      Performance Report\n";
                cout << "=============================\n";
                cout << "Total Time Taken: " << fixed << setprecision(3)
                     << totalTime << " ms\n";
                cout << "Approx. Memory Used: " << (memoryUsed / 1024.0)
                     << " KB\n";
                // stay in the same action menu
            }
            else if (choice == 3) {
                cout << "\n==============================================\n";
                cout << "        Thank you for using the system!\n";
                cout << "==============================================\n";
                running = false;
                backToMenu = true; // exit main loop
            }
            else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }
}

// ====================== Main ======================
int main() {
    DynamicArray<Job> jobs = readJobs("../../job_description/mergejob.csv");

    if (jobs.getSize() == 0) {
        cout << "Error: CSV file not found or empty.\n";
        return 1;
    }

    jobSeekerMode(jobs);
    return 0;
}
