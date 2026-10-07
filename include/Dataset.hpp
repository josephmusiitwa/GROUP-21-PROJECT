#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <string>

using namespace std;

namespace meanshift {

using Point = vector<double>;
using Data = vector<Point>;

class Dataset {
public:
    Dataset();

    // Dataset loading from CSV
    bool loadFromCSV(const string& filename, bool hasHeader = true);

    // Get the raw data
    const Data& getData() const;
}
}