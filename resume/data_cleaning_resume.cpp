#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

// Convert string to lowercase
string toLowerStr(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Check if 'text' contains 'word' ignoring case
bool containsIgnoreCase(const string& text, const string& word) {
    return toLowerStr(text).find(toLowerStr(word)) != string::npos;
}

// Trim leading and trailing spaces
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

// Custom container structures
const int MAX_SKILLS = 30;
const int MAX_CANDIDATES = 1000;

// Represents one candidate
struct Candidate {
    string skills[MAX_SKILLS];
    int skillCount = 0;

    // Add skill if not duplicate
    void addSkill(const string& skill) {
        if (skill.empty()) return;
        for (int i = 0; i < skillCount; ++i)
            if (skills[i] == skill)
                return; // skip duplicates
        if (skillCount < MAX_SKILLS)
            skills[skillCount++] = skill;
    }

    // Combine skills into one string (for duplicate checking)
    string combinedSkills() const {
        string combined;
        for (int i = 0; i < skillCount; ++i) {
            combined += trim(skills[i]);
            if (i != skillCount - 1)
                combined += ", ";
        }
        return combined;
    }

    // Sort skills alphabetically
    void sortSkills() {
        for (int i = 0; i < skillCount - 1; ++i)
            for (int j = i + 1; j < skillCount; ++j)
                if (skills[i] > skills[j])
                    swap(skills[i], skills[j]);
    }
};

// Custom container that holds all candidates
struct CandidateContainer {
    Candidate list[MAX_CANDIDATES];
    int count = 0;

    // Add a new candidate if not duplicate
    void addCandidate(const Candidate& c) {
        string combined = c.combinedSkills();
        for (int i = 0; i < count; ++i) {
            if (list[i].combinedSkills() == combined)
                return; // duplicate, skip
        }
        if (count < MAX_CANDIDATES)
            list[count++] = c;
    }

    // Save to CSV
    void saveToCSV(const string& filename) {
        ofstream output(filename);
        if (!output.is_open()) {
            cout << "Failed to create " << filename << "!" << endl;
            return;
        }

        for (int i = 0; i < count; ++i) {
            output << "Candidate " << setw(2) << setfill('0') << (i + 1) << ",\"";
            for (int j = 0; j < list[i].skillCount; ++j) {
                output << list[i].skills[j];
                if (j != list[i].skillCount - 1)
                    output << ", ";
            }
            output << "\"";
            if (i != count - 1) output << "\n";
        }

        output.close();
    }
};

int main() {
    ifstream file("resume.csv");
    if (!file.is_open()) {
        cout << "Failed to open resume.csv!" << endl;
        return 1;
    }

    string validSkills[MAX_SKILLS] = {
        "Data Cleaning", "Excel", "Power BI", "Reporting", "SQL", "Tableau",
        "Deep Learning", "Machine Learning", "NLP", "Pandas", "Python", "Statistics",
        "Cloud", "Computer Vision", "Keras", "MLOps", "PyTorch", "TensorFlow",
        "Agile", "Product Roadmap", "Scrum", "Stakeholder Management", "User Stories",
        "Docker", "Git", "Java", "REST APIs", "Spring Boot", "System Design"
    };

    CandidateContainer container;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;
        if (container.count >= MAX_CANDIDATES) break;

        // Remove quotes
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

        // Create a new candidate
        Candidate c;

        for (int i = 0; i < MAX_SKILLS; ++i) {
            if (containsIgnoreCase(skillsPart, validSkills[i]))
                c.addSkill(validSkills[i]);
        }

        if (c.skillCount == 0) {
            continue;
        }

        c.sortSkills();
        container.addCandidate(c);
    }

    file.close();

    container.saveToCSV("candidates.csv");

    cout << "Resume data cleaning completed successfully!" << endl;
    cout << "Output saved to candidates.csv." << endl;
    
    return 0;
}