#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main() {
    meanshift::Dataset dataset;
    
    // Create some dummy data for the example if no CSV is provided
    cout << "Creating dummy dataset for example..." << endl;
    // Assuming we could load: dataset.loadFromCSV("data/input/data.csv", true);
    
    cout << "Running MeanShift clustering..." << endl;
    meanshift::MeanShift ms(2.0, 1e-3, 300);
    
    // Dummy manual data
    // In practice use dataset.getData()
    meanshift::Data data = {
        {1.0, 1.0}, {1.5, 2.0}, {2.0, 1.0},
        {8.0, 8.0}, {8.5, 9.0}, {9.0, 8.0},
        {1.0, 8.0}, {1.5, 9.0}, {2.0, 8.0}
    };
    