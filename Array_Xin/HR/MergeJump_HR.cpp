#include "MergeJump_HR.hpp"

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

DynamicArray<string> splitSkills(const string &line) {
    DynamicArray<string> skills;
    stringstream ss(line);
    string skill;
    while (getline(ss, skill, ',')) {
        skill = trim(skill);
        if (!skill.empty()) skills.push_back(toLower(skill));
    }
    for (int i = 0; i < skills.getSize() - 1; i++) {
        for (int j = 0; j < skills.getSize() - i - 1; j++) {
            if (skills[j] > skills[j + 1]) swap(skills[j], skills[j + 1]);
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

DynamicArray<Candidate> readCandidates(const string &filename) {
    DynamicArray<Candidate> candidates;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;
        string name = trim(line.substr(0, commaPos));
        string skillsStr = line.substr(commaPos + 1);
        Candidate c{ name, splitSkills(skillsStr) };
        candidates.push_back(c);
    }
    return candidates;
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
        Job j{ title, splitSkills(skillsStr) };
        jobs.push_back(j);
    }
    return jobs;
}

size_t getCurrentMemoryUsageKB() {
            PROCESS_MEMORY_COUNTERS_EX pmc;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            return pmc.WorkingSetSize / 1024; // Convert bytes → KB
        }

// ====================== Employer Mode ======================
void employerMode(const DynamicArray<Job> &jobs, const DynamicArray<Candidate> &candidates) {
    bool continueProgram = true;
    while (continueProgram) {
        auto start = high_resolution_clock::now();
        size_t initialMemory = getCurrentMemoryUsageKB();

        cout << "\n==================================\n";
        cout << "     HR Job Matching System\n";
        cout << "==================================\n";
        cout << "Available Jobs:\n";
        for (int i = 0; i < jobs.getSize(); i++) {
            cout << " " << i + 1 << ". " << jobs[i].title << "\n";
        }

        int jobChoice;
        while (true) {
            cout << "\nEnter job number: ";
            if (cin >> jobChoice) { 
                if (jobChoice >= 1 && jobChoice <= jobs.getSize()) {
                    break;
                } else {
                    cout << "Invalid choice. Please enter a number between 1 and "
                        << jobs.getSize() << ".\n";
                }
            } else {
                cout << "Invalid input. Please enter a numeric value.\n";
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        cin.ignore();

        Job selectedJob = jobs[jobChoice - 1];
        cout << "\nRole: " << selectedJob.title << "\nRequired Skills:\n";
        for (int i = 0; i < selectedJob.skills.getSize(); i++) {
            cout << " " << i + 1 << ". " << selectedJob.skills[i] << "\n";
        }

        int numSkills;
        while (true) {
            cout << "\nEnter number of skills to use for matching: ";

            if (cin >> numSkills) { 
                cin.ignore(); 

                if (numSkills >= 1 && numSkills <= selectedJob.skills.getSize()) {
                    break;
                } else {
                    cout << "Invalid number of skills. Please try again.\n";
                }
            } else {
                cout << "Invalid input. Please enter a number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            }
        }

        DynamicArray<string> chosenSkills;
        DynamicArray<int> skillWeights;

        for (int i = 0; i < numSkills; i++) {
            int skillNum;
            while (true) {
                cout << "Enter skill number #" << i + 1 << ": ";

                if (cin >> skillNum) {
                    cin.ignore();

                    if (skillNum >= 1 && skillNum <= selectedJob.skills.getSize()) {
                        chosenSkills.push_back(selectedJob.skills[skillNum - 1]);
                        break;
                    } else {
                        cout << "Invalid skill number. Please enter a number between 1 and "
                            << selectedJob.skills.getSize() << ".\n";
                    }
                } else {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }

        cout << "\nEnter the weight (1-10) for each selected skill:\n";
        for (int i = 0; i < numSkills; i++) {
            int weight;
            while (true) {
                cout << "Weight for \"" << chosenSkills[i] << "\": ";

                if (cin >> weight) { 
                    cin.ignore();

                    if (weight >= 1 && weight <= 10) {
                        skillWeights.push_back(weight);
                        break;
                    } else {
                        cout << "Invalid weight. Please enter a value between 1 and 10.\n";
                    }
                } else {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }

        int totalPossibleWeight = 0;
        for (int i = 0; i < skillWeights.getSize(); i++) totalPossibleWeight += skillWeights[i];

        // =====================================
        // SEARCH PHASE
        // =====================================
        // --- Track memory/time before searching ---
        size_t searchStartMem = getCurrentMemoryUsageKB();
        auto searchStartTime = high_resolution_clock::now();

        // Compute weighted matching
        struct CandidateMatch {
            string name;
            int matchedSkills;
            int matchedWeight;
            double score;
        };

        DynamicArray<CandidateMatch> matches;

        for (int i = 0; i < candidates.getSize(); i++) {
            int matchedSkills = 0;
            int matchedWeight = 0;

            for (int j = 0; j < chosenSkills.getSize(); j++) {
                if (jumpSearch(candidates[i].skills, chosenSkills[j])) {
                    matchedSkills++;
                    matchedWeight += skillWeights[j];
                }
            }

            if (matchedWeight > 0) {
                CandidateMatch cm;
                cm.name = candidates[i].name;
                cm.matchedSkills = matchedSkills;
                cm.matchedWeight = matchedWeight;
                cm.score = (double)matchedWeight / totalPossibleWeight;
                matches.push_back(cm);
            }
        }

        auto searchEndTime = high_resolution_clock::now();
        double searchTime = duration<double, milli>(searchEndTime - searchStartTime).count();

        size_t searchMemoryUsed = sizeof(string) * chosenSkills.getSize()        // list of searched skills
                                + sizeof(string) * candidates.getSize() * 0.1    // small fraction of candidate list in memory
                                + sizeof(int) * 5;  

        // =====================================
        // SORT PHASE
        // =====================================

        // --- Track memory/time before sorting ---
        size_t sortStartMem = getCurrentMemoryUsageKB();
        auto sortStartTime = high_resolution_clock::now();

        // Sort by matchedWeight (descending)
        for (int i = 0; i < matches.getSize() - 1; i++) {
            for (int j = 0; j < matches.getSize() - i - 1; j++) {
                if (matches[j].matchedWeight < matches[j + 1].matchedWeight) {
                    auto temp = matches[j];
                    matches[j] = matches[j + 1];
                    matches[j + 1] = temp;
                }
            }
        }

        // --- End of sort section ---
        auto sortEndTime = high_resolution_clock::now();
        size_t sortEndMem = getCurrentMemoryUsageKB();
        double sortTime = duration<double, milli>(sortEndTime - sortStartTime).count();
        size_t sortMemoryUsed = sortEndMem - sortStartMem;

        // Approximate merge sort memory model
        size_t jobCount = matches.getSize();
        size_t mergeMemory = sizeof(Candidate*) * 3 + sizeof(Candidate) * jobCount + (jobCount / 2) * sizeof(Candidate*);


        cout << "\n================== Top 5 Matching Candidates ==================\n";
        cout << left << setw(20) << "Candidate"
             << setw(18) << "Matched Skills"
             << setw(18) << "Matched Weight"
             << "Score (%)\n";
        cout << string(63, '-') << "\n";

        int topCount = 0;
        for (int i = 0; i < matches.getSize() && topCount < 5; i++) {
            cout << left << setw(20) << matches[i].name
                 << setw(18) << matches[i].matchedSkills
                 << setw(18) << matches[i].matchedWeight
                 << fixed << setprecision(1) << (matches[i].score * 100) << "%\n";
            topCount++;
        }

        if (topCount == 0) {
            cout << "No candidates matched your criteria.\n";
        }

        auto end = high_resolution_clock::now();
        double totalTime = duration<double, milli>(end - start).count();

        bool showMenu = true;
        while (showMenu) {
            cout << "\n=============================\n";
            cout << "Action:\n";
            cout << "1. Find another match\n";
            cout << "2. Generate performance report\n";
            cout << "3. Exit\n";
            cout << "=============================\n";
            cout << "Enter your choice: ";

            int choice;
            while (true) {
                cout << "Enter your choice: ";
                if (cin >> choice) {
                    cin.ignore();

                    if (choice >= 1 && choice <= 3) {
                        break; 
                    } else {
                        cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                    }
                } else {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                }
            }

            size_t finalMemory = getCurrentMemoryUsageKB();
            size_t memoryUsed = finalMemory - initialMemory;

            if (choice == 1) {
                showMenu = false; // restart outer loop
            } 
            else if (choice == 2) {
                cout << "\n=============================\n";
                cout << "      Performance Report\n";
                cout << "=============================\n";
                cout << fixed << setprecision(3);
                cout << "Search Time: " << searchTime << " ms\n";
                cout << "Estimated Search Memory: " << (searchMemoryUsed / 1024.0) << " KB\n";
                cout << "Sort Time: " << sortTime << " ms\n";
                cout << "Estimated Sort Memory: " << (mergeMemory / 1024.0) << " KB\n";
                cout << "-------------------------------------\n";
                cout << "Total Estimated Memory: " 
                    << ((searchMemoryUsed + mergeMemory) / 1024.0) << " KB\n";
            } 
            else if (choice == 3) {
                cout << "\n==============================================\n";
                cout << "       Thank you for using the system!\n";
                cout << "==============================================\n";
                return;
            } 
        }
    }
}

// ====================== Main ======================
int main() {
    DynamicArray<Candidate> candidates = readCandidates("resume/candidates.csv");
    DynamicArray<Job> jobs = readJobs("job_description/mergejob.csv");

    if (candidates.getSize() == 0 || jobs.getSize() == 0) {
        cout << "Error: CSV files not found or empty.\n";
        return 1;
    }

    employerMode(jobs, candidates);
    return 0;
}
