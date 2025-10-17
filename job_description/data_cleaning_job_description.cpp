#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

// Clean up text: remove punctuation and trim spaces
string cleanWord(string word) {
    string cleaned;
    for (char c : word) {
        if (isalnum(c) || c == ' ' || c == '-' || c == '+')
            cleaned += c;
    }
    while (!cleaned.empty() && isspace(cleaned.front())) cleaned.erase(cleaned.begin());
    while (!cleaned.empty() && isspace(cleaned.back())) cleaned.pop_back();
    return cleaned;
}

// Structure to store one job and its skills
struct Job {
    string title;
    string skills[100];
    int skillCount = 0;
};

// Custom container to hold all jobs
struct JobContainer {
    Job jobs[100];
    int jobCount = 0;
    // Add a job title if not exists
    int addJob(const string& title) {
        for (int i = 0; i < jobCount; ++i) {
            if (jobs[i].title == title)
                return i; // already exists
        }
        jobs[jobCount].title = title;
        jobCount++;
        return jobCount - 1;
    }

    // Add a skill to a specific job (ignore duplicates)
    void addSkill(int jobIndex, const string& skill) {
        if (skill.empty()) return;
        for (int i = 0; i < jobs[jobIndex].skillCount; ++i) {
            if (jobs[jobIndex].skills[i] == skill)
                return; // duplicate
        }
        jobs[jobIndex].skills[jobs[jobIndex].skillCount++] = skill;
    }

    // Sort jobs alphabetically
    void sortJobs() {
        for (int i = 0; i < jobCount - 1; ++i) {
            for (int j = i + 1; j < jobCount; ++j) {
                if (jobs[i].title > jobs[j].title) {
                    swap(jobs[i], jobs[j]);
                }
            }
        }
    }

    // Sort skills for each job alphabetically
    void sortSkills() {
        for (int i = 0; i < jobCount; ++i) {
            for (int j = 0; j < jobs[i].skillCount - 1; ++j) {
                for (int k = j + 1; k < jobs[i].skillCount; ++k) {
                    if (jobs[i].skills[j] > jobs[i].skills[k]) {
                        swap(jobs[i].skills[j], jobs[i].skills[k]);
                    }
                }
            }
        }
    }

    // Save all jobs and skills to CSV
    void saveToCSV(const string& filename) {
        ofstream output(filename);
        if (!output.is_open()) {
            cout << "Failed to create " << filename << "!" << endl;
            return;
        }

        for (int i = 0; i < jobCount; ++i) {
            output << jobs[i].title << ",\"";
            for (int j = 0; j < jobs[i].skillCount; ++j) {
                output << jobs[i].skills[j];
                if (j != jobs[i].skillCount - 1)
                    output << ", ";
            }
            output << "\"";
            if (i != jobCount - 1) {
                output << endl;
            }
        }
        output.close();
    }
};

int main() {
    // Open to job_description.csv
    ifstream file("job_description.csv");
    if (!file.is_open()) {
        cout << "Failed to open job_description.csv!" << endl;
        return 1;
    }

    JobContainer jc;
    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        // Remove quotes
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        stringstream ss(line);
        string word1, word2;
        ss >> word1 >> word2;
        // Keep original capitalization
        string jobTitle = cleanWord(word1 + " " + word2);

        // Find "with experience in"
        size_t pos = line.find("with experience in");
        if (pos == string::npos) {
            continue;
        }

        // Skip "with experience in"
        string skillsPart = line.substr(pos + 18);
        // Stop before the first period
        size_t endPos = skillsPart.find('.');
        if (endPos != string::npos)
            skillsPart = skillsPart.substr(0, endPos);

        int jobIndex = jc.addJob(jobTitle);

        stringstream skillsStream(skillsPart);
        string skill;
        while (getline(skillsStream, skill, ',')) {
            skill = cleanWord(skill);
            jc.addSkill(jobIndex, skill);
        }
    }

    file.close();

    jc.sortJobs();
    jc.sortSkills();
    jc.saveToCSV("mergejob.csv");

    cout << "Job Description data cleaning completed successfully!" << endl;
    cout << "Output saved to mergejob.csv." << endl;

    return 0;
}