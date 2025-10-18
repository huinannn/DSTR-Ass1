#include <iostream>
#include <string>
#include <cstdlib>   // for system()
#include <limits>
#include <algorithm>

using namespace std;

// Function to get compile/run commands based on role, structure, algorithm
void getCompileRunCommands(const string &role, const string &structure, const string &algorithm,
                           string &compileCmd, string &runCmd) {
#ifdef _WIN32
    string exeExt = ".exe";
#else
    string exeExt = "";
#endif

    if (role == "hr") {
        if (structure == "array") {
            if (algorithm == "1") {
                cout << "\nRunning Array Insertion Sort and Binary Search (HR)\n";
                compileCmd = "g++ ./Array_Jing/Array_HR/InsertionBinary_HR.cpp -o HR" + exeExt;
            } else {
                cout << "\nRunning Array Merge Sort and Jump Search (HR)\n";
                compileCmd = "g++ ./Array_Xin/HR/MergeJump_HR.cpp -o HR" + exeExt;
            }
            runCmd = "HR" + exeExt;
        } else { // linked list
            if (algorithm == "1") {
                cout << "\nRunning Linked List Insertion Sort and Linear Search (HR)\n";
                compileCmd = "g++ ./linked_list/hr/LinearInsertion_HR.cpp -o HR" + exeExt;
            } else {
                cout << "\nRunning Linked List Merge Sort and Optimized Linear Search (HR)\n";
                compileCmd = "g++ ./linked_list/hr/OptimizedMerge_HR.cpp -o HR" + exeExt;
            }
            runCmd = "HR" + exeExt;
        }
    } else { // job seeker
        if (structure == "array") {
            if (algorithm == "1") {
                cout << "\nRunning Array Insertion Sort and Binary Search (Job Seeker)\n";
                compileCmd = "g++ ./Array_Jing/Array_jobseeker/InsertionBinary_JobSeeker.cpp -o JobSeeker" + exeExt;
            } else {
                cout << "\nRunning Array Merge Sort and Jump Search (Job Seeker)\n";
                compileCmd = "g++ ./Array_Xin/Job_Seeker/MergeJump_JobSeeker.cpp -o JobSeeker" + exeExt;
            }
            runCmd = "JobSeeker" + exeExt;
        } else { // linked list
            if (algorithm == "1") {
                cout << "\nRunning Linked List Insertion Sort and Linear Search (Job Seeker)\n";
                compileCmd = "g++ ./linked_list/job_seeker/LinearInsertion_JobSeeker.cpp -o JobSeeker" + exeExt;
            } else {
                cout << "\nRunning Linked List Merge Sort and Optimized Linear Search (Job Seeker)\n";
                compileCmd = "g++ ./linked_list/job_seeker/OptimizedMerge_JobSeeker.cpp -o JobSeeker" + exeExt;
            }
            runCmd = "JobSeeker" + exeExt;
        }
    }

#ifndef _WIN32
    runCmd = "./" + runCmd; // Unix-style prefix
#endif
}

int main() {
    while (true) {
        cout << "=========================================\n";
        cout << "      Welcome to Job Matching System\n";
        cout << "=========================================\n\n";

        string role, structure, algorithm;

        // Step 1: Choose Role
        while (true) {
            cout << "Choose your role:\n";
            cout << "1. HR\n2. Job Seeker\n";
            cout << "Enter your choice (HR/Job Seeker or 1/2): ";
            getline(cin, role);
            transform(role.begin(), role.end(), role.begin(), ::tolower);

            if (role == "hr" || role == "1") {
                role = "hr";
                break;
            } else if (role == "job seeker" || role == "2") {
                role = "job seeker";
                break;
            } else {
                cout << "Invalid role. Try again.\n\n";
            }
        }

        // Step 2: Choose Data Structure
        while (true) {
            cout << "\nChoose Data Structure:\n";
            cout << "1. Array\n2. Linked List\n";
            cout << "Enter your choice (array/linked list or 1/2): ";
            getline(cin, structure);
            transform(structure.begin(), structure.end(), structure.begin(), ::tolower);

            if (structure == "array" || structure == "1") {
                structure = "array";
                break;
            } else if (structure == "linked list" || structure == "link list" || structure == "2") {
                structure = "linked list";
                break;
            } else {
                cout << "Invalid data structure. Try again.\n\n";
            }
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
            cout << "Enter your choice (1/2): ";
            getline(cin, algorithm);

            if (algorithm == "1" || algorithm == "2") break;
            cout << "Invalid input. Try again.\n";
        }

        cout << "\n-----------------------------------------\n";

        string compileCmd, runCmd;
        getCompileRunCommands(role, structure, algorithm, compileCmd, runCmd);

        cout << "\nCompiling selected module...\n";
        int compileResult = system(compileCmd.c_str());
        if (compileResult != 0) {
            cout << "Compilation failed. Check file paths and syntax.\n";
            continue;
        }

        cout << "\nExecuting module...\n";
        int runResult = system(runCmd.c_str());
        if (runResult != 0) {
            cout << "Error running the selected module.\n";
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
                cout << "Invalid choice. Try again.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else break;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (menuChoice == 2) {
            cout << "\nExiting program. Goodbye!\n";
            break;
        }

        cout << "\n";
    }

    return 0;
}
