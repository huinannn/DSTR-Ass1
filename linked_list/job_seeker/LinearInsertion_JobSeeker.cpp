#include "LinearInsertion_JobSeeker.hpp"

void SkillList::add(const string& skill, double weight) {
    skills[size] = skill;
    weights[size] = weight;
    size++;
}

bool SkillList::contains(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill)
            return true;
    return false;
}

double SkillList::getWeight(const string& skill) const {
    for (int i = 0; i < size; ++i)
        if (skills[i] == skill)
            return weights[i];
    return 0.0;
}

Job::Job(const string& t, const SkillList& skills, double score)
    : title(t), requiredSkills(skills), matchScore(score), prev(nullptr), next(nullptr) {}

string toLowerCase(const string& str) {
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(),
              [](unsigned char c){ return tolower(c); });
    return lower;
}

void insertAtTail(Job*& head, string title, SkillList skills) {
    Job* newJob = new Job(title, skills);
    if (!head) {
        head = newJob;
        return;
    }

    Job* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newJob;
    newJob->prev = temp;
}

void loadJobsFromCSV(Job*& head, const string& filename, SkillList& allValidSkills) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: could not open " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        string title, skillsString;
        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;

        title = line.substr(0, commaPos);
        skillsString = line.substr(commaPos + 1);

        if (!skillsString.empty() && skillsString.front() == '"')
            skillsString = skillsString.substr(1, skillsString.size() - 2);

        SkillList skills;
        stringstream skillsStream(skillsString);
        string skill;

        while (getline(skillsStream, skill, ',')) {
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (skill.empty()) continue;

            skill = toLowerCase(skill);
            skills.add(skill);

            if (!allValidSkills.contains(skill))
                allValidSkills.add(skill);
        }

        insertAtTail(head, title, skills);
    }

    file.close();
}

SkillList insertSkills(const SkillList& allValidSkills) {
    SkillList userSkills;
    string skill;
    double weightCounter = 1.0;

    cout << "Enter your skills (type 'done' to finish):\n";
    cin.ignore();

    while (true) {
        cout << "> ";
        getline(cin, skill);

        string lowerSkill = toLowerCase(skill);
        if (lowerSkill == "done") break;

        lowerSkill.erase(0, lowerSkill.find_first_not_of(" \t"));
        lowerSkill.erase(lowerSkill.find_last_not_of(" \t") + 1);

        if (allValidSkills.contains(lowerSkill)) {
            userSkills.add(lowerSkill, weightCounter);
            weightCounter++;
        }
    }

    return userSkills;
}

void updateAllMatchScores(Job* head, const SkillList& userSkills) {
    Job* temp = head;

    while (temp) {
        double matchedWeight = 0.0;
        int skillCount = temp->requiredSkills.size;
        double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

        for (int i = 0; i < skillCount; ++i) {
            const string& jobSkill = temp->requiredSkills.skills[i];
            double weight = skillCount - i;
            if (userSkills.contains(jobSkill)) {
                matchedWeight += weight;
            }
        }

        if (maxWeight > 0)
            temp->matchScore = (matchedWeight / maxWeight) * 100.0;
        else
            temp->matchScore = 0.0;

        temp = temp->next;
    }
}

void sortByScore(Job*& head) {
    if (!head || !head->next) return;

    Job* sorted = nullptr;
    Job* current = head;

    while (current) {
        Job* nextNode = current->next;
        current->prev = current->next = nullptr;

        // ✅ Only consider jobs with a matchScore > 0
        if (current->matchScore > 0.0) {
            if (!sorted || current->matchScore > sorted->matchScore) {
                current->next = sorted;
                if (sorted)
                    sorted->prev = current;
                sorted = current;
            } else {
                Job* temp = sorted;
                while (temp->next && temp->next->matchScore > current->matchScore)
                    temp = temp->next;

                current->next = temp->next;
                if (temp->next)
                    temp->next->prev = current;
                temp->next = current;
                current->prev = temp;
            }
        }

        current = nextNode;
    }

    head = sorted;
}

void displayJobs(Job* head, double minScore) {
    cout << "\nTop 3 Best-Matching Jobs (Weighted Scoring):\n";
    cout << left << setw(25) << "Job Title"
         << setw(10) << "Matched"
         << setw(10) << "Weight"
         << setw(12) << "Percentage" << endl;
    cout << string(55, '-') << endl;

    Job* temp = head;
    int count = 0;

    cout << fixed << setprecision(2);
    while (temp && count < 3) {
        if (temp->matchScore >= minScore) {
            int skillCount = temp->requiredSkills.size;
            double maxWeight = (skillCount * (skillCount + 1)) / 2.0;

            ostringstream perc;
            perc << fixed << setprecision(2) << temp->matchScore << "%";

            cout << left << setw(25) << temp->title
                 << setw(10) << skillCount
                 << setw(10) << maxWeight
                 << setw(12) << perc.str() << endl;

            count++;
        }
        temp = temp->next;
    }

    if (count == 0)
        cout << "No jobs matched your skills.\n";

    cout.unsetf(ios::fixed);
    cout.precision(6);
    cout << endl;
}

int countJobs(Job* head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

void menu(Job*& head, const SkillList& allValidSkills) {
    int choice;
    SkillList userSkills;

    double searchDuration = 0.0, sortDuration = 0.0;
    size_t searchMemoryKB = 0, sortMemoryKB = 0;
    bool performanceRecorded = false;

    do {
        cout << "====================================" << endl;
        cout << "              Job Seeker            " << endl;
        cout << "   Insertion Sort + Weighted Match  " << endl;
        cout << "====================================" << endl;
        cout << "1. Insert Skills" << endl;
        cout << "2. Performance Report" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                userSkills = insertSkills(allValidSkills);

                // ✅ Measure Linear Search Time & Memory
                auto searchStart = chrono::high_resolution_clock::now();
                updateAllMatchScores(head, userSkills);
                auto searchEnd = chrono::high_resolution_clock::now();
                searchDuration = chrono::duration<double, milli>(searchEnd - searchStart).count();
                searchMemoryKB = (sizeof(SkillList) + sizeof(string) * userSkills.size) /  1024.0;

                // ✅ Measure Insertion Sort Time & Memory
                auto sortStart = chrono::high_resolution_clock::now();
                sortByScore(head);
                auto sortEnd = chrono::high_resolution_clock::now();
                sortDuration = chrono::duration<double, milli>(sortEnd - sortStart).count();
                int jobCount = countJobs(head);
                sortMemoryKB = (sizeof(Job) * jobCount) / 1024.0;

                performanceRecorded = true;
                displayJobs(head, 0);
                break;
            }

            case 2: {
                if (performanceRecorded) {
                    cout << fixed << setprecision(4);
                    cout << "\n=============================\n";
                    cout << "Performance Summary\n";
                    cout << "=============================\n";
                    cout << "Linear Search Time: " << searchDuration << " ms\n";
                    cout << "Insertion Sort Time: " << sortDuration << " ms\n";
                    cout << "Linear Search Memory: " << (searchMemoryKB / 1024.0) << " KB\n";
                    cout << "Insertion Sort Memory: " << (sortMemoryKB / 1024.0) << " KB\n\n";
                    cout.unsetf(ios::fixed);
                } else {
                    cout << "\nNo performance data available yet. Please run 'Insert Skills' first.\n\n";
                }
                break;
            }

            case 3:
                cout << "Exiting program, have a nice day!" << endl;
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }

    } while (choice != 3);
}

int main() {
    Job* head = nullptr;
    SkillList allValidSkills;

    loadJobsFromCSV(head, "job_description/mergejob.csv", allValidSkills);
    menu(head, allValidSkills);
    return 0;
}