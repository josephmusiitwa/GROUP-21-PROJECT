#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main() {
    meanshift::Dataset dataset;
    
    cout << "Creating dummy dataset for example..." << endl;
    cout << "Running MeanShift clustering..." << endl;
    meanshift::MeanShift ms(2.0, 1e-3, 300);
    
    meanshift::Data data = {
        {1.0, 1.0}, {1.5, 2.0}, {2.0, 1.0},
        {8.0, 8.0}, {8.5, 9.0}, {9.0, 8.0},
        {1.0, 8.0}, {1.5, 9.0}, {2.0, 8.0}
    };
    ms.fit(data);
    
    auto clusters = ms.getClusters();
    cout << "Number of clusters found: " << clusters.size() << endl;
    
    int i = 0;
    for (const auto& c : clusters) {
        cout << "Cluster " << i++ << " centroid: (";
        for (size_t d = 0; d < c.centroid.size(); ++d) {
            cout << c.centroid[d] << (d == c.centroid.size() - 1 ? "" : ", ");
        }
        cout << ") with " << c.pointIndices.size() << " points." << endl;
    }

    cout << "Clustering evaluation score (cohesion): " << ms.evaluate(data) << endl;
    
    return 0;
}