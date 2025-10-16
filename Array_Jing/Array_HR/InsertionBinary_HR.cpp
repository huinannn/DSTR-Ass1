#include "InsertionBinary_HR.hpp"

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

Candidate::Candidate() {
    skillCount = matchedSkills = matchedWeight = 0;
    percentage = 0.0;
}

JobHR::JobHR() {
    skillCount = 0;
}

HRSystem::HRSystem() {
    jobCount = candCount = 0;
}

void HRSystem::loadJobs(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line) && jobCount < MAX_JOBS) {
        if (line.empty()) continue;
        stringstream ss(line);
        string jobName, skillsLine;
        getline(ss, jobName, ',');
        getline(ss, skillsLine);

        jobs[jobCount].name = trim(jobName);

        stringstream skillStream(skillsLine);
        string skill;
        while (getline(skillStream, skill, ',') && jobs[jobCount].skillCount < MAX_SKILLS)
            jobs[jobCount].skills[jobs[jobCount].skillCount++] = toLower(trim(skill));

        jobCount++;
    }
    file.close();
}

void HRSystem::loadCandidates(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line) && candCount < MAX_CANDIDATES) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, skillsLine;
        getline(ss, name, ',');
        getline(ss, skillsLine);

        candidates[candCount].name = trim(name);

        stringstream skillStream(skillsLine);
        string skill;
        while (getline(skillStream, skill, ',') && candidates[candCount].skillCount < MAX_SKILLS)
            candidates[candCount].skills[candidates[candCount].skillCount++] = toLower(trim(skill));

        candCount++;
    }
    file.close();
}

void HRSystem::displayJobs() {
    cout << "Available Jobs:\n";
    for (int i = 0; i < jobCount; i++)
        cout << " " << i + 1 << ". " << jobs[i].name << endl;
}

int HRSystem::binarySearch(string target) {
    int low = 0, high = jobCount - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        string jobLower = toLower(jobs[mid].name);
        if (jobLower == target) return mid;
        else if (jobLower < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void HRSystem::insertionSort() {
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

void HRSystem::searchAndMatch() {
    while (true) {
        cout << "\n==================================\n";
        cout << "===== HR Job Matching System =====\n";
        cout << "==================================\n";
        displayJobs();

        cout << "\nEnter job name to search: ";
        string jobInput;
        getline(cin, jobInput);
        jobInput = toLower(trim(jobInput));

        int jobIndex = binarySearch(jobInput);
        if (jobIndex == -1) {
            cout << "Job not found.\n";
            continue;
        }

        cout << "\nJob Found: " << jobs[jobIndex].name << endl;
        cout << "Required Skills:\n";
        for (int i = 0; i < jobs[jobIndex].skillCount; i++)
            cout << " " << i + 1 << ". " << jobs[jobIndex].skills[i] << endl;

        int selectedCount;
        cout << "\nEnter number of skills to use for matching: ";
        while (true) {
            cin >> selectedCount;
            if (cin.fail() || selectedCount < 1 || selectedCount > 20) {
                cout << "❌ Invalid number. Please enter between 1 and 20: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }

        int selectedIdx[MAX_SKILLS];
        int weights[MAX_SKILLS];
        fill(begin(selectedIdx), end(selectedIdx), -1);

        for (int i = 0; i < selectedCount; i++) {
            int skillNum;
            bool duplicate;
            do {
                duplicate = false;
                cout << "Enter skill number #" << i + 1 << ": ";
                cin >> skillNum;

                if (cin.fail() || skillNum < 1 || skillNum > jobs[jobIndex].skillCount || skillNum > 20) {
                    cout << "❌ Invalid skill number (must be 1–20 and within job skills). Try again.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    duplicate = true;
                } else {
                    for (int j = 0; j < i; j++) {
                        if (selectedIdx[j] == skillNum - 1) {
                            cout << "⚠️ Duplicate skill selected. Choose a different one.\n";
                            duplicate = true;
                            break;
                        }
                    }
                }
            } while (duplicate);
            selectedIdx[i] = skillNum - 1;
        }

        cout << "\nEnter the weight (1-10) for each selected skill:\n";
        for (int i = 0; i < selectedCount; i++) {
            int weight;
            while (true) {
                cout << "Weight for \"" << jobs[jobIndex].skills[selectedIdx[i]] << "\": ";
                cin >> weight;
                if (cin.fail() || weight < 1 || weight > 10) {
                    cout << "❌ Invalid weight. Please enter between 1 and 10.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else break;
            }
            weights[i] = weight;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Matching logic
        for (int i = 0; i < candCount; i++) {
            candidates[i].matchedSkills = 0;
            candidates[i].matchedWeight = 0;

            for (int j = 0; j < selectedCount; j++) {
                string skill = jobs[jobIndex].skills[selectedIdx[j]];
                for (int k = 0; k < candidates[i].skillCount; k++) {
                    if (skill == candidates[i].skills[k]) {
                        candidates[i].matchedSkills++;
                        candidates[i].matchedWeight += weights[j];
                    }
                }
            }

            candidates[i].percentage =
                (double)candidates[i].matchedSkills / selectedCount * 100.0;
        }

        insertionSort();
        displayTop5();

        int choice;
        while (true) {
            cout << "\n=============================\n";
            cout << "Action:\n";
            cout << "1. Find another match\n";
            cout << "2. Exit\n";
            cout << "=============================\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "❌ Invalid input. Please enter 1 or 2.\n";
                continue;
            }

            if (choice == 1) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break; // Continue main while loop
            } else if (choice == 2) {
                cout << "Exiting system...\n";
                return;
            } else {
                cout << "❌ Invalid choice. Please enter 1 or 2.\n";
            }
        }
    }
}

void HRSystem::displayTop5() {
    cout << "\n===== Top 5 Matching Candidates =====\n";
    cout << left << setw(20) << "Candidate"
         << setw(20) << "Matched Skills"
         << setw(20) << "Matched Weight"
         << setw(15) << "Score (%)" << endl;
    cout << "---------------------------------------------------------------\n";
    for (int i = 0; i < min(candCount, 5); i++) {
        cout << left << setw(20) << candidates[i].name
             << setw(20) << candidates[i].matchedSkills
             << setw(20) << candidates[i].matchedWeight
             << fixed << setprecision(1) << candidates[i].percentage << "%" << endl;
    }
}

void runHRSystem() {
    HRSystem hr;
    hr.loadJobs("job_description/mergejob.csv");
    hr.loadCandidates("resume/candidates.csv");
    hr.searchAndMatch();
}

int main() {
    runHRSystem();
    return 0;
}
