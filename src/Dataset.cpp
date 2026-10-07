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
}