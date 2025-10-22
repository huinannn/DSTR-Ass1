#ifndef MERGEJUMP_COMMON_HPP
#define MERGEJUMP_COMMON_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <windows.h>
#include <psapi.h>
#include <functional>
#pragma comment(lib, "psapi.lib")
using namespace std;
using namespace chrono;

// ====================== DynamicArray Template ======================
template<typename T>
class DynamicArray {
private:
    T* data;
    int capacity;
    int size;

    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < size; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    DynamicArray() : data(nullptr), capacity(0), size(0) {}
    DynamicArray(const DynamicArray& other) : data(nullptr), capacity(0), size(0) {
        if (other.size > 0) {
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (int i = 0; i < size; i++) data[i] = other.data[i];
        }
    }
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
                for (int i = 0; i < size; i++) data[i] = other.data[i];
            }
        }
        return *this;
    }
    ~DynamicArray() { delete[] data; }

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

// ====================== Structs ======================
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

// ====================== Common Functions ======================
string toLower(const string &str);
string trim(const string &s);
DynamicArray<string> splitSkills(const string &line);
bool jumpSearch(const DynamicArray<string> &arr, const string &target);
DynamicArray<Candidate> readCandidates(const string &filename);
DynamicArray<Job> readJobs(const string &filename);

#endif
