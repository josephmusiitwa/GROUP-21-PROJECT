#include "meanshift/Dataset.hpp"
#include <iostream>
#include <fstream>
#include <sstream> // used to split each CSV line into individual tokens
#include <cmath>

using namespace std;

namespace meanshift {
// reading a CSV file into data_
Dataset::Dataset() {}
bool Dataset::loadFromCSV(const string& filename, bool hasHeader) {
ifstream file(filename); // open the file for reading
if (!file.is_open()) {
cerr << "Error: Cannot open file " << filename << endl;
return false;
}

string line; // string to store each line read from the file
if (hasHeader) {
getline(file, line); // read the fist line and throw it away
}

data_.clear(); // remove previously loaded data
while (getline(file, line)) {
if (line.empty()) continue; // skip blank lines

stringstream ss(line); //allow line to be split at commas
string token;
Point p;
  // splitting the line at every comma
while (getline(ss, token, ',')) {
try {
p.push_back(stod(token)); // string to double conversion
} catch (const exception& e) {
}
}
if (!p.empty()) {
data_.push_back(p); //populating data_
}
}
return true; //success
}
// providing read only access to the stored dataset 
const Data& Dataset::getData() const {
return data_; 
}

void Dataset::standardize() {
if (data_.empty()) return; //check if dataset is empty

size_t numFeatures = data_[0].size(); // get the number of features in each point
Point means(numFeatures, 0.0); // store the mean of each feature
Point stdDevs(numFeatures, 0.0); // store the standard deviation of each feature
  
// calculate sum of values for each feature
for (const auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
means[i] += point[i];
}
}
// calculate mean of each feature
for (size_t i = 0; i < numFeatures; ++i) {
means[i] /= data_.size();
}
// calculate the sum of squared differences from the mean
for (const auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
stdDevs[i] += pow(point[i] - means[i], 2);
}
} 
// calculate the standard deviation of each feature
for (size_t i = 0; i < numFeatures; ++i) {
stdDevs[i] = sqrt(stdDevs[i] / data_.size());
}
// standardise each feature of every point
for (auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
if (stdDevs[i] != 0) {
point[i] = (point[i] - means[i]) / stdDevs[i];
}
}
}
}

void Dataset::normalize() {
if (data_.empty()) return;

size_t numFeatures = data_[0].size();
Point mins = data_[0]; // initialised minimum value
Point maxs = data_[0]; // initialised maximum value
// iterate through the dataset and identify the minimum and maximum values
for (const auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
if (point[i] < mins[i]) mins[i] = point[i];
if (point[i] > maxs[i]) maxs[i] = point[i];
}
}
// normalise every value
for (auto& point : data_) {
for (size_t i = 0; i < numFeatures; ++i) {
if (maxs[i] - mins[i] != 0) {
point[i] = (point[i] - mins[i]) / (maxs[i] - mins[i]);
}
}
}
}

} // namespace meanshift
