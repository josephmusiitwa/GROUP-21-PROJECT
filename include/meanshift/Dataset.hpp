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

    bool loadFromCSV(const string& filename, bool hasHeader = true);
    const Data& getData() const;

    // Z-score normalization
    void standardize();

    // Min-Max scaling to [0, 1]
    void normalize();

private:
    Data data_;
};

} // namespace meanshift

#endif // DATASET_HPP

