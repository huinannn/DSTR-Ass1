#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
using namespace std;

// Convert string to lowercase
string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Check if 'text' contains 'word' ignoring case
bool containsIgnoreCase(const string& text, const string& word) {
    string t = toLowerStr(text);
    string w = toLowerStr(word);
    return t.find(w) != string::npos;
}

int main() {
    ifstream file("resume.csv");
    if (!file.is_open()) {
        cout << "Failed to open resume.csv!" << endl;
        return 1;
    }

    // List of valid skills
    vector<string> validSkills = {
        "Data Cleaning", "Excel", "Power BI", "Reporting", "SQL", "Tableau",
        "Deep Learning", "Machine Learning", "NLP", "Pandas", "Python", "Statistics",
        "Cloud", "Computer Vision", "Keras", "MLOps", "PyTorch", "TensorFlow",
        "Agile", "Product Roadmap", "Scrum", "Stakeholder Management", "User Stories",
        "Docker", "Git", "Java", "REST APIs", "Spring Boot", "System Design"
    };

    vector<vector<string>> allCandidates;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Remove quotes if they exist
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

        vector<string> foundSkills;
        for (auto& skill : validSkills) {
            if (containsIgnoreCase(skillsPart, skill))
                foundSkills.push_back(skill);
        }

        // Sort alphabetically
        sort(foundSkills.begin(), foundSkills.end());
        if (!foundSkills.empty())
            allCandidates.push_back(foundSkills);
    }

    file.close();

    // Remove duplicates
    set<string> seen;
    vector<vector<string>> uniqueSkills;
    for (auto& skills : allCandidates) {
        string combined;
        for (size_t i = 0; i < skills.size(); ++i) {
            combined += skills[i];
            if (i != skills.size() - 1) combined += ", ";
        }
        if (seen.insert(combined).second)
            uniqueSkills.push_back(skills);
    }

    // Write to candidates.csv
    ofstream output("candidates.csv");
    if (!output.is_open()) {
        cout << "Failed to create candidates.csv!" << endl;
        return 1;
    }

    for (size_t i = 0; i < uniqueSkills.size(); ++i) {
        output << "Candidate " << setw(2) << setfill('0') << (i + 1) << ",\"";
        for (size_t j = 0; j < uniqueSkills[i].size(); ++j) {
            output << uniqueSkills[i][j];
            if (j != uniqueSkills[i].size() - 1)
                output << ", ";
        }
        output << "\"";
        if (i != uniqueSkills.size() - 1)
            output << "\n";
    }

    output.close();
    cout << "✅ Resume data cleaning completed successfully!" << endl;
    cout << "Output saved to candidates.csv." << endl;
    return 0;
}