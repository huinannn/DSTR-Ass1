#include "LinearInsertion_HR.hpp"

// Utils
string Utils::trim(const string& str) {
    size_t start = str.find_first_not_of(" ");
    size_t end = str.find_last_not_of(" ");
    if (start == string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

string Utils::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return tolower(c);
    });
    return result;
}

string Utils::normalizeSkill(const string& s) {
    string res;
    for (char c : s) {
        if (!isspace(c)) res += tolower(c);
    }
    return res;
}

SkillNode* Utils::addSkill(SkillNode* head, const string& skill) {
    for (SkillNode* temp = head; temp; temp = temp->next) {
        if (Utils::toLower(temp->skill) == Utils::toLower(skill)) {
            return head;
        }
    }
    SkillNode* newNode = new SkillNode{ skill, nullptr };
    if (!head) {
        return newNode;
    }
    SkillNode* temp = head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = newNode;
    return head;
}

int Utils::countSkills(SkillNode* head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

bool Utils::skillExists(SkillNode* head, const string& target) {
    string t = toLower(target);
    while (head) {
        if (toLower(head->skill) == t) {
            return true;
        }
        head = head->next;
    }
    return false;
}

SkillNode* Utils::buildSkillList(const string& input, SkillNode* jobRoleSkills) {
    stringstream ss(input);
    string token;
    SkillNode* head = nullptr;

    while (getline(ss, token, ',')) {
        string trimmed = Utils::trim(token);
        if (trimmed.empty()) {
            continue;
        }

        for (SkillNode* js = jobRoleSkills; js; js = js->next) {
            if (Utils::normalizeSkill(js->skill) == Utils::normalizeSkill(trimmed)) {
                head = addSkill(head, js->skill);
                break;
            }
        }
    }
    return head;
}

void Utils::sortSkills(SkillNode*& head) {
    if (!head || !head->next) return;
    for (SkillNode* i = head; i && i->next; i = i->next) {
        for (SkillNode* j = i->next; j; j = j->next) {
            if (Utils::toLower(i->skill) > Utils::toLower(j->skill)) {
                swap(i->skill, j->skill);
            }
        }
    }
}

// File Loader
SkillNode* FileLoader::parseSkills(const string & skillsStr) {
    SkillNode* skillHead = nullptr;
    string skill;
    stringstream skillStream(skillsStr);
    while (getline(skillStream, skill, ',')) {
        if (!skill.empty() && skill[0] == ' ')
            skill.erase(0, 1);
        skillHead = Utils::addSkill(skillHead, skill);
    }
    return skillHead;
}

JobRole* FileLoader::loadJobs(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open " << filename << "!" << endl;
        return nullptr;
    }
    JobRole* head = nullptr;
    JobRole* tail = nullptr;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string role, skillsStr;
        getline(ss, role, ',');
        getline(ss, skillsStr, '"');
        getline(ss, skillsStr, '"');
        SkillNode* skillHead = parseSkills(skillsStr);
        JobRole* node = new JobRole{role, skillHead, nullptr, tail};
        if (!head) {
            head = tail = node;
        } else { 
            tail->next = node; 
            node->prev = tail; 
            tail = node; 
        }
    }
    file.close();
    return head;
}

Candidate* FileLoader::loadCandidates(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open " << filename << "!" << endl;
        return nullptr;
    }
    Candidate* head = nullptr;
    Candidate* tail = nullptr;
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string name, skillsStr;
        getline(ss, name, ',');
        getline(ss, skillsStr, '"');
        getline(ss, skillsStr, '"');
        SkillNode* skillHead = parseSkills(skillsStr);
        Candidate* node = new Candidate{name, skillHead, 0, 0, 0.0, nullptr};
        if (!head) {
            head = tail = node;
        } else { 
            tail->next = node;
            tail = node; 
        }
    }
    file.close();
    return head;
}

// Linear Search
JobRole* LinearSearch::findRole(JobRole* head, const string& searchRole) {
    string target = Utils::toLower(searchRole);
    JobRole* temp = head;
    while (temp) {
        if (Utils::toLower(temp->roleName) == target) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

// Insertion Sort
Candidate* InsertionSort::sortCandidates(Candidate* head) {
    if (!head || !head->next) {
        return head;
    }
    Candidate* sorted = nullptr;
    while (head) {
        Candidate* current = head;
        head = head->next;
        current->next = nullptr;
        if (!sorted || current->score > sorted->score ||
            (current->score == sorted->score && current->weightedScore > sorted->weightedScore)) {
            current->next = sorted;
            sorted = current;
        } else {
            Candidate* temp = sorted;
            while (temp->next && 
                   (temp->next->score > current->score ||
                    (temp->next->score == current->score && temp->next->weightedScore >= current->weightedScore))) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
    }
    return sorted;
}

// Input Utils
string InputUtils::getInput(const string& prompt, const string& valid1, const string& valid2, const string& valid3) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        input = Utils::trim(input);
        if (input.empty()) {
            cout << "Input cannot be empty!" << endl << endl;
            continue;
        }
        if (!valid1.empty()) {
            bool valid = (input == valid1) || (!valid2.empty() && input == valid2) || (!valid3.empty() && input == valid3);
            if (!valid) {
                cout << "Invalid input! Enter " << valid1;
                if (!valid2.empty()) cout << " or " << valid2;
                if (!valid3.empty()) cout << " or " << valid3;
                cout << " only." << endl << endl;
                continue;
            }
        }
        break;
    }
    return input;
}

JobRole* InputUtils::getValidJobRole(JobRole* jobs) {
    cout << endl << "=============== AVAILABLE JOBS ================" << endl;
    int index = 1;
    for (JobRole* temp = jobs; temp; temp = temp->next)
        cout << " " << index++ << ". " << temp->roleName << endl;
    cout << endl;
    JobRole* role = nullptr;
    while (!role) {
        string searchRole = InputUtils::getInput("Enter the job role to search: ");
        role = LinearSearch::findRole(jobs, searchRole);
        if (!role) {
            cout << "Role not found! Please try again." << endl << endl;
        }
    }
    return role;
}

string InputUtils::getSkillsInput(JobRole* role) {
    cout << endl << "============== ROLE INFORMATION ===============" << endl;
    cout << "Job Role: " << role->roleName << endl;
    cout << "Required Skills: " << endl;
    int idx = 1;
    for (SkillNode* s = role->skills; s; s = s->next)
        cout << " " << idx++ << ". " << s->skill << endl;
    cout << endl;
    string input;
    while (true) {
        input = InputUtils::getInput("Enter skills to match (separated by comma): ");
        stringstream ss(input);
        string skill;
        bool allInvalid = true;
        while (getline(ss, skill, ',')) {
            skill = Utils::trim(skill);
            if (Utils::skillExists(role->skills, skill)) {
                allInvalid = false;
                break;
            }
        }
        if (allInvalid) {
            cout << "No such skill for this role! Please try again." << endl << endl;
            continue;
        }
        break;
    }
    return input;
}

// Matcher
MatchResult Matcher::matchCandidates(JobRole* role, Candidate* candidates, SkillNode* searchSkills) {
    int totalSkills = Utils::countSkills(searchSkills);
    int* weights = new int[totalSkills];
    int idx = 0, totalWeight = 0;

    for (SkillNode* s = searchSkills; s; s = s->next, idx++) {
        int weight = 0;
        while (true) {
            cout << "Weight for \"" << s->skill << "\": ";
            string input; getline(cin, input);
            input = Utils::trim(input);
            stringstream ss(input);
            if (input.empty()) {
                cout << "Input cannot be empty!" << endl;
                continue;
            }
            if (ss >> weight && !(ss >> ws) && weight >= 1 && weight <= 10)
                break;
            cout << "Invalid input! Enter 1-10." << endl;
        }
        weights[idx] = weight;
        totalWeight += weight;
    }
    
    auto startSearch = chrono::high_resolution_clock::now();
    int candidateCount = 0;
    int candidateSkillCount = 0;
    int roleSkillCount = 0;

    for (SkillNode* s = role->skills; s; s = s->next)
        roleSkillCount++;

    for (Candidate* c = candidates; c; c = c->next) {
        c->matchedSkillCount = 0;
        c->weightedScore = 0;
        c->score = 0.0;
        idx = 0;

        for (SkillNode* s = searchSkills; s; s = s->next, idx++) {
            for (SkillNode* cSkill = c->skills; cSkill; cSkill = cSkill->next) {
                if (Utils::toLower(s->skill) == Utils::toLower(cSkill->skill)) {
                    c->matchedSkillCount++;
                    c->weightedScore += weights[idx];
                    break;
                }
            }
        }
        c->score = (totalWeight > 0) ? (double(c->weightedScore) / totalWeight) * 100.0 : 0.0;

        candidateCount++;
        for (SkillNode* cSkill = c->skills; cSkill; cSkill = cSkill->next)
            candidateSkillCount++;
    }
    auto endSearch = chrono::high_resolution_clock::now();
    double linearTime = chrono::duration<double, std::milli>(endSearch - startSearch).count();

    auto startSort = chrono::high_resolution_clock::now();
    Candidate* sorted = InsertionSort::sortCandidates(candidates);
    auto endSort = chrono::high_resolution_clock::now();
    double insertionTime = chrono::duration<double, std::milli>(endSort - startSort).count();

    size_t linearMemory = sizeof(int) * 3 + sizeof(string) * 1;
    size_t insertionMemory = sizeof(Candidate) * candidateCount + sizeof(int) * 2; 

    delete[] weights;
    return { sorted, linearTime, insertionTime, linearMemory, insertionMemory };
}

void linear_insertion() {
    cout << "===============================================" << endl;
    cout << "            HR Job Matching System" << endl;
    cout << "        Linear Search + Insertion Sort" << endl;
    cout << "===============================================" << endl;

    bool exitProgram = false;
    bool showMainMenu = true;

    while (!exitProgram) {
        if (showMainMenu) {
            cout << endl << "[ MENU ]" << endl;
            cout << "1. Search Skills & Match Candidates" << endl;
            cout << "2. Exit" << endl << endl;
            string choice = InputUtils::getInput("Enter your choice (1-2): ", "1", "2");

            if (choice == "2") {
                exitProgram = true;
                break;
            }
        }

        JobRole* jobs = FileLoader::loadJobs("job_description/mergejob.csv");
        Candidate* candidates = FileLoader::loadCandidates("resume/candidates.csv");
        JobRole* role = InputUtils::getValidJobRole(jobs);
        string skillInput = InputUtils::getSkillsInput(role);
        SkillNode* searchSkills = Utils::buildSkillList(skillInput, role->skills);

        auto result = Matcher::matchCandidates(role, candidates, searchSkills);
        cout << endl << "======================= JOB MATCHING =======================" << endl;
        Utils::sortSkills(searchSkills);
        cout << "Skills: ";
        for (SkillNode* s = searchSkills; s; s = s->next) {
            cout << s->skill;
            if (s->next) cout << ", ";
        }
        cout << endl;

        int totalMatch = 0;
        for (Candidate* c = result.sortedCandidates; c; c = c->next)
            if (c->matchedSkillCount > 0) totalMatch++;
        cout << "Total Matching Candidates: " << totalMatch << endl << endl;
        cout << "Top 5 candidates:" << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << left << setw(16) << "Candidates" << setw(17) << "Matched Skills" << setw(18) << "Weighted Score" << "Score (%)" << endl;
        cout << "------------------------------------------------------------" << endl;

        int count = 0;
        for (Candidate* c = result.sortedCandidates; c && count < 5; c = c->next, count++) {
            if (c->matchedSkillCount > 0) {
                cout << left << setw(22) << c->name << setw(17) << c->matchedSkillCount << setw(13) << c->weightedScore << fixed << setprecision(2) << c->score << endl;
            }
        }
        while (!exitProgram) {
            cout << endl << "[ ACTION ]" << endl;
            cout << "1. Performance Summary" << endl;
            cout << "2. Continue Search Skills & Match Candidates" << endl;
            cout << "3. Exit" << endl << endl;
            string postChoice = InputUtils::getInput("Enter your choice (1-3): ", "1", "2", "3");

            if (postChoice == "1") {
                cout << endl << "============ PERFORMANCE SUMMARY ==============" << endl;
                cout << "Linear Search Time    : " << fixed << setprecision(3) << result.searchTimeMS << " ms" << endl;
                cout << "Linear Search Memory  : " << fixed << setprecision(3) << (result.searchMemoryKB / 1024.0) << " KB" << endl; 
                cout << "Insertion Sort Time   : " << fixed << setprecision(3) << result.sortTimeMS << " ms" << endl;
                cout << "Insertion Sort Memory : " << fixed << setprecision(3) << (result.sortMemoryKB / 1024.0) << " KB" << endl;
            } else if (postChoice == "2") {
                showMainMenu = false;
                break; 
            } else if (postChoice == "3") {
                exitProgram = true;
            }
        }
    }
    cout << endl << "Thank you for using the Job Matching System. Goodbye!" << endl;
}

int main() {
    linear_insertion();
    return 0;
}