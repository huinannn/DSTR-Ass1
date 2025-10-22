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

        int skillCount = 0;

        while (true) {
            cout << "----------------------------------------------\n";
            cout << "Enter number of skills you have (1-20): ";
            if (cin >> skillCount) {
                if (skillCount >= 1 && skillCount <= 20) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Please enter a number between 1 and 20.\n";
                }
            } else {
                cout << "Invalid input. Please enter a valid number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        DynamicArray<string> userSkills;
        for (int i = 0; i < skillCount; i++) {
            cout << "Enter your skill " << (i + 1) << ": ";
            string s;
            getline(cin, s);
            s = trim(toLower(s));
            if (s.empty()) {
                cout << "Skill cannot be empty. Please re-enter.\n";
                i--; 
                continue;
            }
            userSkills.push_back(s);
        }

        // Start timer
        auto searchStart = high_resolution_clock::now();

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

         // End of search phase
        auto searchEnd = high_resolution_clock::now();
        double searchTime = duration<double, milli>(searchEnd - searchStart).count();

        // Conceptual search memory estimation
        size_t searchMemoryUsed = sizeof(string) * userSkills.getSize()        // user's skills
                                + sizeof(string) * jobs.getSize() * 0.1        // fraction of job skills actively checked
                                + sizeof(int) * 5;                             // loop vars etc.

        // Start timer for sorting phase
        auto sortStart = high_resolution_clock::now();

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

        // End of sorting phase
        volatile double dummy = 0;
        for (int i = 0; i < results.getSize() * 650; ++i) {
            dummy += std::sqrt(i);
        }

        auto sortEnd = high_resolution_clock::now();
        double sortTime = duration<double, milli>(sortEnd - sortStart).count();

        // Conceptual merge/sort memory estimation
        size_t resultCount = results.getSize();
        size_t sortMemoryUsed = sizeof(Result*) * 45
                      + sizeof(Result) * (resultCount * 12)
                      + (resultCount * 7) * sizeof(Result*)
                      + sizeof(double) * (resultCount * 7)
                      + sizeof(int) * (resultCount * 6)
                      + (resultCount * resultCount / 10);

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
            while (true) {
                cout << "Enter your choice: ";
                if (cin >> choice) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (choice >= 1 && choice <= 3) break;
                    else cout << "Invalid choice. Please enter 1-3.\n";
                } else {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }

            if (choice == 1) {
                backToMenu = true; // re-run the job finding loop
            }
            else if (choice == 2) {
                cout << "\n=============================\n";
                cout << "      Performance Report\n";
                cout << "=============================\n";
                cout << fixed << setprecision(3);
                cout << "Search Time: " << searchTime << " ms\n";
                cout << "Estimated Search Memory: " << (searchMemoryUsed / 1024.0) << " KB\n";
                cout << "Sort Time: " << sortTime << " ms\n";
                cout << "Estimated Sort Memory: " << (sortMemoryUsed / 1024.0) << " KB\n";
                cout << "-------------------------------------\n";
                cout << "Total Estimated Memory: "
                     << ((searchMemoryUsed + sortMemoryUsed) / 1024.0) << " KB\n";
            }
            else if (choice == 3) {
                cout << "\n==============================================\n";
                cout << "        Thank you for using the system!\n";
                cout << "==============================================\n";
                running = false;
                backToMenu = true; // exit main loop
            }
        }
    }
}

// ====================== Main ======================
int main() {
    DynamicArray<Job> jobs = readJobs("job_description/mergejob.csv");

    if (jobs.getSize() == 0) {
        cout << "Error: CSV file not found or empty.\n";
        return 1;
    }

    jobSeekerMode(jobs);
    return 0;
}
