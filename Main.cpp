#include <iostream>
#include <string>
#include <cstdlib> // for system()

using namespace std;

int main() {
    cout << "=========================================\n";
    cout << "      Welcome to Job Matching System\n";
    cout << "=========================================\n\n";

    string role, structure, algorithm;

    // Step 1: Choose Role
    cout << "Choose your role:\n";
    cout << "1. HR\n";
    cout << "2. Job Seeker\n";
    cout << "Enter your choice (HR/Job Seeker): ";
    getline(cin, role);

    for (auto &c : role) c = tolower(c);
    if (role != "hr" && role != "job seeker") {
        cout << "❌ Invalid role. Exiting...\n";
        return 0;
    }

    // Step 2: Choose Data Structure
    cout << "\nChoose Data Structure:\n";
    cout << "1. Array\n";
    cout << "2. Linked List\n";
    cout << "Enter your choice (array/linked list): ";
    getline(cin, structure);
    for (auto &c : structure) c = tolower(c);

    if (structure != "array" && structure != "linked list" && structure != "link list") {
        cout << "❌ Invalid data structure. Exiting...\n";
        return 0;
    }

    // Step 3: Choose Algorithm
    if (structure == "array") {
        cout << "\nChoose Algorithm Pair:\n";
        cout << "1. Binary Search and Insertion Sort\n";
        cout << "2. Jump Search and Merge Sort\n";
        cout << "Enter your choice: ";
        getline(cin, algorithm);
    } else {
        cout << "\nChoose Algorithm Pair:\n";
        cout << "1. Linear Search and Insertion Sort\n";
        cout << "2. Optimized Linear Search and Merge Sort\n";
        cout << "Enter your choice: ";
        getline(cin, algorithm);
    }

    for (auto &c : algorithm) c = tolower(c);

    cout << "\n-----------------------------------------\n";
    string command;

    // ROLE: HR
    if (role == "hr") {
        if (structure == "array") {
            if (algorithm.find("binary") != string::npos && algorithm.find("insertion") != string::npos) {
                cout << "Running: InsertionBinary_HR.cpp (Binary Search + Insertion Sort)\n";
                command = "g++ ./Array_Jing/Array_HR/InsertionBinary_HR.cpp -o HR && ./HR";
            } else if (algorithm.find("jump") != string::npos && algorithm.find("merge") != string::npos) {
                cout << "Running: MergeJump_HR.cpp (Jump Search + Merge Sort)\n";
                command = "g++ ./Array_Xin/HR/MergeJump_HR.cpp -o HR && ./HR";
            } else {
                cout << "❌ Invalid algorithm combination.\n";
                return 0;
            }
        } else {
            if (algorithm.find("linear") != string::npos && algorithm.find("insertion") != string::npos) {
                cout << "Running: LinearInsertion_HR.cpp (Linear Search + Insertion Sort)\n";
                command = "g++ ./Linked_Jing/Linked_HR/LinearInsertion_HR.cpp -o HR && ./HR";
            } else if (algorithm.find("optimized") != string::npos && algorithm.find("merge") != string::npos) {
                cout << "Running: OptimizedMerge_HR.cpp (Optimized Linear Search + Merge Sort)\n";
                command = "g++ ./Linked_Jing/Linked_HR/OptimizedMerge_HR.cpp -o HR && ./HR";
            } else {
                cout << "❌ Invalid algorithm combination.\n";
                return 0;
            }
        }
    }

    // ROLE: Job Seeker
    else if (role == "job seeker") {
        if (structure == "array") {
            if (algorithm.find("binary") != string::npos && algorithm.find("insertion") != string::npos) {
                cout << "Running: InsertionBinary_JobSeeker.cpp (Binary Search + Insertion Sort)\n";
                command = "g++ ./Array_Jing/Array_jobseeker/InsertionBinary_JobSeeker.cpp -o JobSeeker && ./JobSeeker";
            } else if (algorithm.find("jump") != string::npos && algorithm.find("merge") != string::npos) {
                cout << "Running: MergeJump_JobSeeker.cpp (Jump Search + Merge Sort)\n";
                command = "g++ ./Array_Xin/Job_Seeker/MergeJump_JobSeeker.cpp -o JobSeeker && ./JobSeeker";
            } else {
                cout << "❌ Invalid algorithm combination.\n";
                return 0;
            }
        } else {
            if (algorithm.find("linear") != string::npos && algorithm.find("insertion") != string::npos) {
                cout << "Running: LinearInsertion_JobSeeker.cpp (Linear Search + Insertion Sort)\n";
                command = "g++ ./Linked_Jing/Linked_jobseeker/LinearInsertion_JobSeeker.cpp -o JobSeeker && ./JobSeeker";
            } else if (algorithm.find("optimized") != string::npos && algorithm.find("merge") != string::npos) {
                cout << "Running: OptimizedMerge_JobSeeker.cpp (Optimized Linear Search + Merge Sort)\n";
                command = "g++ ./Linked_Jing/Linked_jobseeker/OptimizedMerge_JobSeeker.cpp -o JobSeeker && ./JobSeeker";
            } else {
                cout << "❌ Invalid algorithm combination.\n";
                return 0;
            }
        }
    }

    cout << "-----------------------------------------\n";
    cout << "Compiling and executing selected algorithm...\n\n";

    int result = system(command.c_str());
    if (result != 0)
        cout << "⚠️ Error executing the selected algorithm.\n";

    cout << "\n-----------------------------------------\n";
    cout << "✅ System execution completed.\n";
    return 0;
}
