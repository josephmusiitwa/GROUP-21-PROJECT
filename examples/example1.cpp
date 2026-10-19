#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    string csvFilePath = "data/input/my_data.csv";
    string outFilePath = "data/output/results.csv";
    
    if (argc > 1) {
        csvFilePath = argv[1];
    }
    if (argc > 2) {
        outFilePath = argv[2];
    }

    meanshift::Dataset dataset;
    
    cout << "Loading data from external CSV file: " << csvFilePath << endl;
    
    if (!dataset.loadFromCSV(csvFilePath, true)) {
        cerr << "Error: Failed to load external data from " << csvFilePath << endl;
        cerr << "Please ensure the file exists and is in CSV format." << endl;
        return 1;
    }
    
    cout << "Successfully loaded external data." << endl;
    cout << "Running MeanShift clustering on the external dataset..." << endl;
    
    meanshift::MeanShift ms(2.0, 1e-3, 300);
    ms.fit(dataset.getData());
    
    auto clusters = ms.getClusters();
    cout << "\nNumber of clusters found: " << clusters.size() << endl;
    
    int i = 0;
    for (const auto& c : clusters) {
        cout << "Cluster " << i++ << " centroid: (";
        for (size_t d = 0; d < c.centroid.size(); ++d) {
            cout << c.centroid[d] << (d == c.centroid.size() - 1 ? "" : ", ");
        }
        cout << ") with " << c.pointIndices.size() << " points." << endl;
    }
    
    cout << "Clustering evaluation score: " << ms.evaluate(dataset.getData()) << endl;
    
    cout << "\nWriting results to: " << outFilePath << " ..." << endl;
    
    ofstream outFile(outFilePath);
    if (outFile.is_open()) {
        outFile << "Feature1,Feature2,Cluster_ID\n";
        
        const auto& realData = dataset.getData();
        const auto& labels = ms.getLabels();
        
        for (size_t k = 0; k < realData.size(); ++k) {
            for (size_t d = 0; d < realData[k].size(); ++d) {
                outFile << realData[k][d] << ",";
            }
            outFile << labels[k] << "\n";
        }
        outFile.close();
        cout << "Successfully saved!" << endl;
    } else {
        cerr << "Warning: Could not open output file for writing!" << endl;
    }

    return 0;
}

