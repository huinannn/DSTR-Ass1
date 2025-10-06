#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
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

int main() {
    // Open to job_description.csv
    ifstream file("job_description.csv");
    if (!file.is_open()) {
        cout << "Failed to open job_description.csv!" << endl;
        return 1;
    }

    // Sort alphabetically
    map<string, set<string>> jobSkills;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Remove quotes if they exist
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        stringstream ss(line);
        string word1, word2;
        ss >> word1 >> word2;

        // Keep original capitalization
        string jobTitle = cleanWord(word1 + " " + word2);

        // Find "with experience in"
        size_t pos = line.find("with experience in");
        if (pos == string::npos) continue;

        // Skip "with experience in"
        string skillsPart = line.substr(pos + 18);

        // Stop before the first period (.)
        size_t endPos = skillsPart.find('.');
        if (endPos != string::npos)
            skillsPart = skillsPart.substr(0, endPos);

        stringstream skillsStream(skillsPart);
        string skill;
        while (getline(skillsStream, skill, ',')) {
            skill = cleanWord(skill);
            if (skill.size() > 1)
                jobSkills[jobTitle].insert(skill);
        }
    }

    file.close();

    // Write to mergejob.csv
    ofstream output("mergejob.csv");
    if (!output.is_open()) {
        cout << "Failed to create mergejob.csv!" << endl;
        return 1;
    }

    int jobCount = 0;
    int totalJobs = jobSkills.size();
    for (auto it = jobSkills.begin(); it != jobSkills.end(); ++it) {
        ++jobCount;
        output << it->first << ",\"";
        int count = 0;
        int total = it->second.size();
        for (auto s = it->second.begin(); s != it->second.end(); ++s) {
            ++count;
            output << *s;
            if (count != total) output << ", ";
        }
        output << "\"";
        if (jobCount != totalJobs) output << "\n";
    }

    output.close();

    cout << "✅ Job Description data cleaning completed successfully!" << endl;
    cout << "Output saved to mergejob.csv." << endl;

    return 0;
}