#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
string csvFilePath = "data/input/my_data.csv";  // Set the default path for the input CSV file
string outFilePath = "data/output/results.csv";  // Set the default path for the output CSV file

   // If the user provides a first command-line argument,
  // use it as the input CSV file path
if (argc > 1) {
csvFilePath = argv[1];
}
  // If the user provides a second command-line argument,
 // use it as the output CSV file path
if (argc > 2) {
outFilePath = argv[2];
}

meanshift::Dataset dataset; // Create a Dataset object to load and store the data
// Display the path of the CSV file being loaded
cout << "Loading data from external CSV file: " << csvFilePath << endl;

   // Load the CSV file into the dataset
  // The second argument, true, indicates that the CSV
  // file contains a header row
if (!dataset.loadFromCSV(csvFilePath, true)) {
  // Display an error message if loading fails
cerr << "Error: Failed to load external data from " << csvFilePath << endl;
cerr << "Please ensure the file exists and is in CSV format." << endl; // Inform the user to check the file
return 1; // Stop the program and return an error code
}

  // Display a message confirming successful data loading
cout << "Successfully loaded external data." << endl;
  // Inform the user that clustering is starting
cout << "Running MeanShift clustering on the external dataset..." << endl;

  // Create a MeanShift object with the following parameters:
  // 2.0  = bandwidth, which controls the influence of nearby points
   // 1e-3 = epsilon (0.001), the convergence tolerance
   // 300  = maximum number of iterations
meanshift::MeanShift ms(2.0, 1e-3, 300);
  // Perform MeanShift clustering on the loaded dataset
  // getData() returns the actual data points from the Dataset object
ms.fit(dataset.getData());

  // Retrieve the clusters identified by the algorithm
auto clusters = ms.getClusters();
  // Display the total number of clusters found
cout << "\nNumber of clusters found: " << clusters.size() << endl;

int i = 0;  // Initialize a counter for displaying cluster numbers
for (const auto& c : clusters) { // Loop through each cluster found by MeanShift
cout << "Cluster " << i++ << " centroid: (";  // Display the cluster number and its centroid
 // Display the cluster number and its centroid
  for (size_t d = 0; d < c.centroid.size(); ++d) {
 // Print the centroid coordinate
 // Add a comma between coordinates, except after the last one
    cout << c.centroid[d] << (d == c.centroid.size() - 1 ? "" : ", ");
}
  // Display the number of data points in the cluster
cout << ") with " << c.pointIndices.size() << " points." << endl;
}

   // Evaluate the quality of the clustering
  // The evaluation function calculates the score using the dataset
cout << "Clustering evaluation score: " << ms.evaluate(dataset.getData()) << endl;

   // Display the path where the results will be saved
cout << "\nWriting results to: " << outFilePath << " ..." << endl;

  // Open the output CSV file for writing
ofstream outFile(outFilePath);
 // Check whether the output file was successfully opened
  if (outFile.is_open()) {
     // Write the column headings into the output CSV file
     // Feature1 and Feature2 represent the data coordinates
    // Cluster_ID represents the cluster assigned to each point
  outFile << "Feature1,Feature2,Cluster_ID\n";

    // Get the original data points from the dataset
const auto& realData = dataset.getData();
  // Get the cluster labels assigned to each data point
const auto& labels = ms.getLabels();

    // Get the cluster labels assigned to each data point
for (size_t k = 0; k < realData.size(); ++k) {
 // Loop through all features or coordinates of the point
for (size_t d = 0; d < realData[k].size(); ++d) {
 // Write each feature value followed by a comma
outFile << realData[k][d] << ",";
}
  // Write the cluster label for the current data point
  // Move to the next row after writing the label
outFile << labels[k] << "\n";
}
    // Close the output file after writing all results
outFile.close();
     // Display a message confirming successful saving
cout << "Successfully saved!" << endl;
} else {
    // Display a warning if the output file could not be opened
cerr << "Warning: Could not open output file for writing!" << endl;
}

  // Return 0 to indicate successful program execution
return 0;
}
