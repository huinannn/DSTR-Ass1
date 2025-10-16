#include <iostream>
#include <string>
#include <cstdlib> // for system()
#include <limits>

using namespace std;

int main() {
    while (true) {
        cout << "=========================================\n";
        cout << "      Welcome to Job Matching System\n";
        cout << "=========================================\n\n";

        string role, structure, algorithm;

        // Step 1: Choose Role
        while (true) {
            cout << "Choose your role:\n";
            cout << "1. HR\n";
            cout << "2. Job Seeker\n";
            cout << "Enter your choice (HR/Job Seeker): ";
            getline(cin, role);
            for (auto &c : role) c = tolower(c);

            if (role == "hr" || role == "job seeker") break;
            cout << "❌ Invalid role. Try again.\n\n";
        }

        // Step 2: Choose Data Structure
        while (true) {
            cout << "\nChoose Data Structure:\n";
            cout << "1. Array\n";
            cout << "2. Linked List\n";
            cout << "Enter your choice (array/linked list): ";
            getline(cin, structure);
            for (auto &c : structure) c = tolower(c);

            if (structure == "array" || structure == "linked list" || structure == "link list") break;
            cout << "❌ Invalid data structure. Try again.\n\n";
        }

        // Step 3: Choose Algorithm
        while (true) {
            if (structure == "array") {
                cout << "\nChoose Algorithm Pair:\n";
                cout << "1. Binary Search and Insertion Sort\n";
                cout << "2. Jump Search and Merge Sort\n";
            } else {
                cout << "\nChoose Algorithm Pair:\n";
                cout << "1. Linear Search and Insertion Sort\n";
                cout << "2. Optimized Linear Search and Merge Sort\n";
            }
            cout << "Enter your choice: ";
            getline(cin, algorithm);
            for (auto &c : algorithm) c = tolower(c);

            if (!algorithm.empty()) break;
            cout << "❌ Invalid input. Try again.\n";
        }

        cout << "\n-----------------------------------------\n";

        string compileCmd, runCmd;

        // ROLE: HR
        if (role == "hr") {
            if (structure == "array") {
                if (algorithm.find("binary") != string::npos && algorithm.find("insertion") != string::npos) {
#ifdef _WIN32
                    compileCmd = "g++ ./Array_Jing/Array_HR/InsertionBinary_HR.cpp -o HR.exe";
                    runCmd = "HR.exe";
#else
                    compileCmd = "g++ ./Array_Jing/Array_HR/InsertionBinary_HR.cpp -o HR";
                    runCmd = "./HR";
#endif
                } else {
#ifdef _WIN32
                    compileCmd = "g++ ./Array_Xin/HR/MergeJump_HR.cpp -o HR.exe";
                    runCmd = "HR.exe";
#else
                    compileCmd = "g++ ./Array_Xin/HR/MergeJump_HR.cpp -o HR";
                    runCmd = "./HR";
#endif
                }
            } else { // linked list
#ifdef _WIN32
                compileCmd = "g++ ./Linked_list/Linked_HR/OptimizedMerge_HR.cpp -o HR.exe";
                runCmd = "HR.exe";
#else
                compileCmd = "g++ ./Linked_list/Linked_HR/OptimizedMerge_HR.cpp -o HR";
                runCmd = "./HR";
#endif
            }
        }

        // ROLE: Job Seeker
        else {
            if (structure == "array") {
                if (algorithm.find("binary") != string::npos && algorithm.find("insertion") != string::npos) {
#ifdef _WIN32
                    compileCmd = "g++ ./Array_Jing/Array_jobseeker/InsertionBinary_JobSeeker.cpp -o JobSeeker.exe";
                    runCmd = "JobSeeker.exe";
#else
                    compileCmd = "g++ ./Array_Jing/Array_jobseeker/InsertionBinary_JobSeeker.cpp -o JobSeeker";
                    runCmd = "./JobSeeker";
#endif
                } else {
#ifdef _WIN32
                    compileCmd = "g++ ./Array_Xin/Job_Seeker/MergeJump_JobSeeker.cpp -o JobSeeker.exe";
                    runCmd = "JobSeeker.exe";
#else
                    compileCmd = "g++ ./Array_Xin/Job_Seeker/MergeJump_JobSeeker.cpp -o JobSeeker";
                    runCmd = "./JobSeeker";
#endif
                }
            } else { // linked list
#ifdef _WIN32
                compileCmd = "g++ ./Linked_list/Linked_jobseeker/OptimizedMerge_JobSeeker.cpp -o JobSeeker.exe";
                runCmd = "JobSeeker.exe";
#else
                compileCmd = "g++ ./Linked_list/Linked_jobseeker/OptimizedMerge_JobSeeker.cpp -o JobSeeker";
                runCmd = "./JobSeeker";
#endif
            }
        }

        cout << "Compiling selected module...\n";
        int compileResult = system(compileCmd.c_str());
        if (compileResult != 0) {
            cout << "❌ Compilation failed. Check file paths and syntax.\n";
            continue;
        }

        cout << "Executing module...\n";
        int runResult = system(runCmd.c_str());
        if (runResult != 0) {
            cout << "⚠️ Error running the selected module.\n";
        }

        // After execution: loop menu
        int menuChoice;
        while (true) {
            cout << "\nDo you want to:\n";
            cout << "1. Continue running the system\n";
            cout << "2. Terminate the system\n";
            cout << "Enter 1 or 2: ";
            cin >> menuChoice;
            if (cin.fail() || (menuChoice != 1 && menuChoice != 2)) {
                cout << "❌ Invalid choice. Try again.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (menuChoice == 2) {
            cout << "Exiting program. Goodbye!\n";
            break;
        }

        cout << "\n";
    }

    return 0;
}
