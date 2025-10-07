#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <string>
#include <iomanip> 
#include <algorithm> 
using namespace std;

struct Job {
    string title;
    unordered_set<string> requiredSkills;
    double matchScore;
    Job* prev = nullptr;
    Job* next = nullptr;
};

string toLowerCase(const string& str) {
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(),
              [](unsigned char c){ return tolower(c); });
    return lower;
}

void insertAtTail(Job*& head, string title, unordered_set<string> skills) {
    // Create new Job node
    Job* newJob = new Job{title, skills, 0, nullptr, nullptr};
    // If empty list, add first job
    if (!head) {
        head = newJob;
        return;
    }
    // If list not empty, start at head, move to last node using temp->next
    Job* temp = head;
    while (temp->next) temp = temp->next;
    // Point last node to new Job
    temp->next = newJob;
    // Point new Job back to old last node
    // If JobA exist & add JobB now:
    // JobA.next -> JobB
    // JobB.prev -> JobA
    // JobB.next -> nullptr
    newJob->prev = temp;
}

void loadJobsFromCSV(Job*& head, const string& filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string title, skillsString;
        unordered_set<string> skills;

        // Split title and skill
        getline(ss, title, ',');
        getline(ss, skillsString);

        // Remove quotes from skillsString if present
        if (!skillsString.empty() && skillsString.front() == '"')
            skillsString = skillsString.substr(1, skillsString.size() - 2);
        
        // Split by comma to get individual skills
        stringstream skillsStream(skillsString);
        string skill;
        while (getline(skillsStream, skill, ',')) {
            // trim whitespace
            while (!skill.empty() && (skill.front() == ' ')) skill.erase(skill.begin());
            skills.insert(toLowerCase(skill));
        }

        // Insert job into list
        insertAtTail(head, title, skills);
    }

    file.close();
}

unordered_set<string> insertSkills() {
    unordered_set<string> skills;
    string skill;


    cout << "Enter your skills (type 'done' to finish):\n";
    cin.ignore();

    while (true) {
        cout << "> ";
        // Read whole line, include spaces ' '
        getline(cin, skill);

        string lowerSkill = toLowerCase(skill);
        if (lowerSkill == "done")
            break;
        skill.erase(0, skill.find_first_not_of(" \t"));
        skill.erase(skill.find_last_not_of(" \t") + 1);

        if (!skill.empty())
            skills.insert(toLowerCase(skill));
    }

    return skills;
}

void updateAllMatchScores(Job* head, unordered_set<string> skills) {
    Job* temp = head;
    while(temp) {
        int matched = 0;
        for (string js : temp->requiredSkills)
            for (string us: skills)
                if (js == us) matched++;
            temp->matchScore = ((double) matched / temp->requiredSkills.size()) * 100.0;
            temp = temp->next;
    }
}   

void sortByScore(Job*& head) {
    if (!head) return;
    Job* sorted = nullptr;
    Job* current = head;
    while (current) {
        Job* next = current->next;
        current->prev = current->next = nullptr;
        // Insert into sorted list
        if (!sorted || current->matchScore > sorted->matchScore) {
            current->next = sorted;
            if (sorted) sorted->prev = current;
            sorted = current;
        } else {
            Job* temp = sorted;
            while (temp->next && temp->next->matchScore > current->matchScore)
                temp = temp->next;
            current->next = temp->next;
            if (temp->next) temp->next->prev = current;
            temp->next = current;
            current->prev = temp;
        }
        
        current = next;
    }

    head = sorted;
}

void displayJobs(Job* head, double minScore) {
    cout << "\n====== Matching Jobs ======\n";
    Job* temp = head;

    cout << fixed << setprecision(2);

    while (temp != nullptr) {
        if (temp->matchScore >= minScore)
            cout << temp->title << " - " << temp->matchScore << "% match\n";
        temp = temp->next;
    }
    cout.unsetf(ios::fixed);
    cout.precision(6); 
    cout << "\n" << endl;
}

void deleteJob(Job*& head, string title) {
    Job* temp = head;
    while (temp != nullptr && temp->title != title)
        temp = temp->next;
    if (temp == nullptr) return;

    if (temp->prev != nullptr)
        temp->prev->next = temp->next;
    else
        head = temp->next;

    if (temp->next != nullptr)
        temp->next->prev = temp->prev;
    
    delete temp;
}

void menu(Job*& head) {
    int choice;
    unordered_set<string> userSkills;

    do {
        cout << "==================================" << endl;
        cout << "            Job Seeker            " << endl;
        cout << "==================================" << endl;
        cout << "1. Insert Skills" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            userSkills = insertSkills();
            updateAllMatchScores(head, userSkills);
            sortByScore(head);
            displayJobs(head, 0);
            break;
        case 2:
            cout << "Exiting program, have a nice day!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }

    } while (choice != 2);
}

int main() {
    Job* head = nullptr;

    loadJobsFromCSV(head, "mergejob.csv");
    menu(head);
    return 0;
}