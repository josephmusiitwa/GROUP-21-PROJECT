#include "meanshift/Dataset.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

namespace meanshift {

Dataset::Dataset() {}
bool Dataset::loadFromCSV(const string& filename, bool hasHeader) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    string line;
    if (hasHeader) {
        getline(file, line); // Skip header
    }
   
    data_.clear();
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Point p;
        while (getline(ss, token, ',')) {
            try {
                p.push_back(stod(token));
            } catch (const exception& e) {
                // In case of non-numeric data, we can ignore the point or handle it.
                // For simplicity, we assume all columns are features.
            }
        }
        if (!p.empty()) {
            data_.push_back(p);
        }
    }
    return true;
}