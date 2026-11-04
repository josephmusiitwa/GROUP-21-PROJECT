// Include the Dataset class declaration and MeanShift clustering class
#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
// Include libraries for input and output, vectors, strings and formatting
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main() {
  // Create a Dataset object
  // This object can be used to manage dataset-related operations
meanshift::Dataset dataset;
  
// Display messages to the user
cout << "Creating dummy dataset for example..." << endl;
cout << "Running MeanShift clustering..." << endl;
  
   // Create a MeanShift object with the following parameters:
   // 2.0   = bandwidth, which controls how far points influence each other
   // 1e-3  = epsilon (0.001), the convergence tolerance
   // 300   = maximum number of iterations
meanshift::MeanShift ms(2.0, 1e-3, 300);

  // Create a sample dataset containing 9 two-dimensional points
  // Each point is represented by {x, y}
  // The points are arranged into three groups in different regions
meanshift::Data data = {
{1.0, 1.0}, {1.5, 2.0}, {2.0, 1.0}, // First group of points near (1.5, 1.3)
{8.0, 8.0}, {8.5, 9.0}, {9.0, 8.0}, // Second group of points near (8.5, 8.3)
{1.0, 8.0}, {1.5, 9.0}, {2.0, 8.0}   // Third group of points near (1.5, 8.3)
};
   // Perform MeanShift clustering on the dataset
  // The algorithm shifts points toward high-density regions
  // and groups points that converge to the same region
ms.fit(data);
 // Retrieve the clusters identified by the algorithm
auto clusters = ms.getClusters(); 
// Display the total number of clusters found
cout << "Number of clusters found: " << clusters.size() << endl;

// Initialize a counter for numbering the clusters
  int i = 0;
for (const auto& c : clusters) {   // Loop through each cluster found by MeanShift
// Display the cluster number and its centroid
  cout << "Cluster " << i++ << " centroid: (";
   // Loop through each coordinate of the centroid
  for (size_t d = 0; d < c.centroid.size(); ++d) {
    // Print the coordinate
    // Add a comma between coordinates, but not after the last one
cout << c.centroid[d]
     << (d == c.centroid.size() - 1 ? "" : ", ");
}
  // Display the number of points belonging to this cluster
cout << ") with " << c.pointIndices.size() 
                  << " points." << endl;
}

 // Evaluate the clustering using the evaluation function
// This score measures how closely points are grouped
// according to the implementation of evaluate() 
cout << "Clustering evaluation score (cohesion): " << ms.evaluate(data) << endl;

return 0;
}
