#ifndef DATASET_HPP //header guard start
#define DATASET_HPP //header guard definition

#include <vector>
#include <string>

using namespace std;
//custom namespace 'meanshift'
namespace meanshift {

using Point = vector<double>; //Point to represent a data point with multiple numerical features
using Data = vector<Point>;  // Data to represnt multiple data points

class Dataset {
public:
Dataset(); //constructor

//function to load data from a CSV file
bool loadFromCSV(const string& filename, bool hasHeader = true);

//function to access the dataset without modifying it
const Data& getData() const;

void standardize(); // Z-score normalization

void normalize(); // Min-Max scaling to [0, 1]

private:
Data data_; //stores the dataset
};

} // namespace meanshift

#endif // DATASET_HPP
