#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
using namespace std;

// Clean text: remove extra spaces and unwanted punctuation
string cleanWord(string word) {
    string cleaned;
    for (char c : word) {
        if (isalnum(c) || c == ' ' || c == '-' || c == '+')
            cleaned += c;
    }
    while (!cleaned.empty() && isspace(cleaned.front()))
        cleaned.erase(cleaned.begin());
    while (!cleaned.empty() && isspace(cleaned.back()))
        cleaned.pop_back();
    return cleaned;
}

int main() {
    ifstream file("resume.csv");
    if (!file.is_open()) {
        cout << "Failed to open resume.csv!" << endl;
        return 1;
    }

    vector<vector<string>> allSkills;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Remove quotes if exist
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        // Find "skilled in"
        size_t pos = line.find("skilled in");
        if (pos == string::npos) continue;

        // Skip "skilled in"
        string skillsPart = line.substr(pos + 10);

        // Stop before the first period
        size_t endPos = skillsPart.find('.');
        if (endPos != string::npos)
            skillsPart = skillsPart.substr(0, endPos);

        stringstream ss(skillsPart);
        string skill;
        vector<string> skills;

        while (getline(ss, skill, ',')) {
            skill = cleanWord(skill);
            if (!skill.empty() && isupper(skill[0]))  // keep only capitalized words
                skills.push_back(skill);
        }

        // Sort alphabetically
        sort(skills.begin(), skills.end());
        allSkills.push_back(skills);
    }

    file.close();

    // Remove duplicates (based on skill combinations)
    vector<vector<string>> uniqueSkills;
    set<string> seenSets;

    for (auto &skills : allSkills) {
        string combined;
        for (size_t i = 0; i < skills.size(); ++i) {
            combined += skills[i];
            if (i != skills.size() - 1) combined += ", ";
        }

        if (seenSets.find(combined) == seenSets.end()) {
            seenSets.insert(combined);
            uniqueSkills.push_back(skills);
        }
    }

    // Write to candidates.csv
    ofstream output("candidates.csv");
    if (!output.is_open()) {
        cout << "Failed to create candidates.csv!" << endl;
        return 1;
    }

    for (size_t idx = 0; idx < uniqueSkills.size(); ++idx) {
        auto &skills = uniqueSkills[idx];
        output << "Candidate " << setw(2) << setfill('0') << (idx + 1) << ",\"";
        for (size_t i = 0; i < skills.size(); ++i) {
            output << skills[i];
            if (i != skills.size() - 1)
                output << ", ";
        }
        output << "\"";
        if (idx != uniqueSkills.size() - 1)
            output << "\n";
    }

    output.close();

    cout << "✅ Resume data cleaning completed successfully!" << endl;
    cout << "Output saved to candidates.csv." << endl;

    return 0;
}