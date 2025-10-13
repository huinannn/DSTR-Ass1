#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace chrono;

// Custom dynamic array template
template<typename T>
class DynamicArray {
private:
    T* data;
    int capacity;
    int size;

    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    DynamicArray() : data(nullptr), capacity(0), size(0) {}
    
    // Copy constructor
    DynamicArray(const DynamicArray& other) : data(nullptr), capacity(0), size(0) {
        if (other.size > 0) {
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
    }
    
    // Assignment operator
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            data = nullptr;
            capacity = 0;
            size = 0;
            
            if (other.size > 0) {
                capacity = other.capacity;
                size = other.size;
                data = new T[capacity];
                for (int i = 0; i < size; i++) {
                    data[i] = other.data[i];
                }
            }
        }
        return *this;
    }
    
    ~DynamicArray() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (size >= capacity) resize();
        data[size++] = value;
    }

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }
    
    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    
    void clear() {
        delete[] data;
        data = nullptr;
        capacity = 0;
        size = 0;
    }
};

struct Candidate {
    string name;
    DynamicArray<string> skills;
};

struct Job {
    string title;
    DynamicArray<string> skills;
};

struct ScorePair {
    string name;
    double score;
};

// Convert string to lowercase
string toLower(const string &str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Trim spaces
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Split comma-separated string
DynamicArray<string> splitSkills(const string &line) {
    DynamicArray<string> skills;
    stringstream ss(line);
    string skill;

    while (getline(ss, skill, ',')) {
        skill = trim(skill);
        if (!skill.empty()) skills.push_back(toLower(skill));
    }

    // Bubble sort for simplicity
    for (int i = 0; i < skills.getSize() - 1; i++) {
        for (int j = 0; j < skills.getSize() - i - 1; j++) {
            if (skills[j] > skills[j + 1]) {
                string temp = skills[j];
                skills[j] = skills[j + 1];
                skills[j + 1] = temp;
            }
        }
    }

    return skills;
}

// Jump search algorithm
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

// Merge sort by score (descending)
void merge(DynamicArray<ScorePair> &arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    ScorePair* L = new ScorePair[n1];
    ScorePair* R = new ScorePair[n2];
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    delete[] L;
    delete[] R;
}

void mergeSort(DynamicArray<ScorePair> &arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Read candidates.csv
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
        Candidate c;
        c.name = name;
        c.skills = splitSkills(skillsStr);
        candidates.push_back(c);
    }
    return candidates;
}

// Read mergejob.csv
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
        Job j;
        j.title = title;
        j.skills = splitSkills(skillsStr);
        jobs.push_back(j);
    }
    return jobs;
}

// Employer mode
void employerMode(const DynamicArray<Job> &jobs, const DynamicArray<Candidate> &candidates) {
    auto start = high_resolution_clock::now();
    
    cout << "Enter desired role (e.g., Data Scientist): ";
    string role;
    getline(cin, role);

    DynamicArray<string> jobSkills;
    for (int i = 0; i < jobs.getSize(); i++) {
        if (toLower(jobs[i].title) == toLower(role)) {
            jobSkills = jobs[i].skills;
            break;
        }
    }

    if (jobSkills.getSize() == 0) {
        cout << "Role not found.\n";
        return;
    }

    cout << "\nSkills required for this role:\n";
    for (int i = 0; i < jobSkills.getSize(); i++) {
        cout << "- " << jobSkills[i] << "\n";
    }

    cout << "\nEnter desired skills (comma separated): ";
    string skillsInput;
    getline(cin, skillsInput);
    DynamicArray<string> desiredSkills = splitSkills(skillsInput);

    DynamicArray<ScorePair> scores;
    for (int i = 0; i < candidates.getSize(); i++) {
        int matched = 0;
        for (int j = 0; j < desiredSkills.getSize(); j++) {
            if (jumpSearch(candidates[i].skills, desiredSkills[j])) matched++;
        }

        double score = 0.0;
        if (matched == desiredSkills.getSize() && candidates[i].skills.getSize() == desiredSkills.getSize()) {
            score = 1.0;
        } else if (matched > 0) {
            score = (double)matched / (desiredSkills.getSize() + candidates[i].skills.getSize() - matched);
        }

        ScorePair sp;
        sp.name = candidates[i].name;
        sp.score = score;
        scores.push_back(sp);
    }

    mergeSort(scores, 0, scores.getSize() - 1);

    int totalIdealCandidates = 0;
    for (int i = 0; i < scores.getSize(); i++) {
        if (scores[i].score > 0) totalIdealCandidates++;
    }

    cout << "Total number of ideal candidates: " << totalIdealCandidates << endl;
    cout << "Top 5 Candidates:\n";
    for (int i = 0; i < min(5, scores.getSize()); i++) {
        cout << i + 1 << ". " << scores[i].name << " (Score: " << scores[i].score * 100 << "%)\n";
    }

    auto end = high_resolution_clock::now();
    double totalTime = duration<double, milli>(end - start).count();

    size_t memoryUsed = sizeof(jobs) + sizeof(candidates) + sizeof(scores)
                      + jobSkills.getCapacity() * sizeof(string)
                      + desiredSkills.getCapacity() * sizeof(string)
                      + scores.getCapacity() * sizeof(ScorePair);

    cout << "\n=============================\n";
    cout << "Performance Summary\n";
    cout << "=============================\n";
    cout << "Total Time Taken: " << fixed << setprecision(3) << totalTime << " ms\n";
    cout << "Approx. Memory Used: " << (memoryUsed / 1024.0) << " KB\n";
}

// Job seeker mode
void jobSeekerMode(const DynamicArray<Job> &jobs) {
    auto start = high_resolution_clock::now();

    cout << "Enter your skills (comma separated): ";
    string skillsInput;
    getline(cin, skillsInput);
    DynamicArray<string> userSkills = splitSkills(skillsInput);

    DynamicArray<ScorePair> scores;
    for (int i = 0; i < jobs.getSize(); i++) {
        int matched = 0;
        for (int j = 0; j < jobs[i].skills.getSize(); j++) {
            if (jumpSearch(userSkills, jobs[i].skills[j])) matched++;
        }
        double score = (double)matched / jobs[i].skills.getSize();
        
        ScorePair sp;
        sp.name = jobs[i].title;
        sp.score = score;
        scores.push_back(sp);
    }

    mergeSort(scores, 0, scores.getSize() - 1);

    cout << "\nBest matching roles:\n";
    for (int i = 0; i < min(5, scores.getSize()); i++) {
        cout << i + 1 << ". " << scores[i].name << " (Match: " << scores[i].score * 100 << "%)\n";
    }

    auto end = high_resolution_clock::now();
    double totalTime = duration<double, milli>(end - start).count();

    size_t memoryUsed = sizeof(jobs) + sizeof(scores)
                      + userSkills.getCapacity() * sizeof(string)
                      + scores.getCapacity() * sizeof(ScorePair);

    cout << "\n=============================\n";
    cout << "Performance Summary\n";
    cout << "=============================\n";
    cout << "Total Time Taken: " << fixed << setprecision(3) << totalTime << " ms\n";
    cout << "Approx. Memory Used: " << (memoryUsed / 1024.0) << " KB\n";
}

int main() {
    DynamicArray<Candidate> candidates = readCandidates("candidates.csv");
    DynamicArray<Job> jobs = readJobs("mergejob.csv");

    if (candidates.getSize() == 0 || jobs.getSize() == 0) {
        cout << "Error: CSV files not found or empty.\n";
        return 1;
    }

    cout << "Are you an Employer or Job Seeker? (E/J): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'E' || choice == 'e') employerMode(jobs, candidates);
    else if (choice == 'J' || choice == 'j') jobSeekerMode(jobs);
    else cout << "Invalid input.\n";

    return 0;
}