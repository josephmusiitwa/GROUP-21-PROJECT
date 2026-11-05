// Include the MeanShift class declaration,  mathematical functions such as sqrt() and exp() and algorithms such as min()
#include "meanshift/MeanShift.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

// All Mean Shift code is placed inside the meanshift namespace
namespace meanshift {

// Constructor: initializes the Mean Shift parameters
MeanShift::MeanShift(double bandwidth, double epsilon, int maxIterations)
: bandwidth_(bandwidth), epsilon_(epsilon), maxIterations_(maxIterations) {}

// Calculates the Euclidean distance between two points
double MeanShift::euclideanDistance(const Point& a, const Point& b) {
double sum = 0.0;
// Calculate the squared difference for each dimension
for (size_t i = 0; i < a.size(); ++i) {
sum += (a[i] - b[i]) * (a[i] - b[i]);   
}
 // Take the square root to obtain the Euclidean distance
return sqrt(sum);
}

// Calculates the Gaussian kernel weight for a given distance
double MeanShift::gaussianKernel(double distance) const {
// Points closer to the center receive larger weights
// Points farther away receive smaller weights
return exp(-0.5 * (distance * distance) / (bandwidth_ * bandwidth_));
}

// Estimates the bandwidth automatically from the dataset
void MeanShift::estimateBandwidth(const Data& data) {
// If the dataset is empty, there is nothing to estimate
if (data.empty()) return;

// use at most 500 rows/points to reduce computation time
size_t numSamples = min<size_t>(500, data.size());  
double totalDist = 0;
int count = 0;
// Calculate distances between all pairs of sampled points
for (size_t i = 0; i < numSamples; ++i) {
for (size_t j = i + 1; j < numSamples; ++j) {
// Add the distance between the two points
totalDist += euclideanDistance(data[i], data[j]);
//Count the number of distance calculations
count++;
}
}

// Set bandwidth to half the average pairwise distance
// If there are not enough points, use 1.0 as the default
bandwidth_ = (count > 0) ? (totalDist / count) * 0.5 : 1.0;
// Prevent bandwidth from being zero or negative
 if (bandwidth_ <= 0) bandwidth_ = 1.0;
}

// Moves a point toward a nearby high-density region
Point MeanShift::shiftPoint(const Point& p, const Data& data) const {
// Start with the original point
 Point shiftedP = p;
 // Stores the weighted sum of coordinates
 Point numerator(p.size(), 0.0);
// Stores the sum of all weights
 double denominator = 0.0;

 // Examine every point in the dataset
for (const auto& other_p : data) {
 // Calculate the distance from the current point
 double dist = euclideanDistance(p, other_p);
// Calculate the Gaussian weight based on distance
 double weight = gaussianKernel(dist);

 // Add each coordinate multiplied by its weight
for (size_t i = 0; i < p.size(); ++i) {
numerator[i] += other_p[i] * weight;
}
 // Add the weight to the total weight
denominator += weight;
}

 // Calculate the weighted mean if the total weight is positive
if (denominator > 0) {
for (size_t i = 0; i < p.size(); ++i) {
// New coordinate = weighted sum / total weight
 shiftedP[i] = numerator[i] / denominator;
}
}
 // Return the new position of the point
return shiftedP;
}

// Main function that performs Mean Shift clustering
void MeanShift::fit(const Data& data) {
// exit if the dataset contains no points
 if (data.empty()) return;

 // Estimate bandwidth if it has not been set properly
if (bandwidth_ <= 0) {
estimateBandwidth(data);
}
 
 // creates a working copy of all points
vector<Point> shiftedPoints = data;
 // Track which points have stopped moving. vector initialised to false
 vector<bool> stopMoving(data.size(), false);

 // oterative loop that evaluates up to maxIterations
for (int iter = 0; iter < maxIterations_; ++iter) {
  //initializes 
int maxDistMoved = 0;
bool allStopped = true;
//loops through all points
for (size_t i = 0; i < shiftedPoints.size(); ++i) {
  // If one has settled: skip (saves time)
if (stopMoving[i]) continue;
//function moves point up one step
Point newPoint = shiftPoint(shiftedPoints[i], data);
  //straight line distance moved
double dist = euclideanDistance(shiftedPoints[i], newPoint);
//tiny step: it has arrived
if (dist < epsilon_) {
stopMoving[i] = true;
} else {
  //still moving: need another round
allStopped = false;
}
shiftedPoints[i] = newPoint;
}
//everyone settled: stop early
if (allStopped) {
break;
}
}

// Merge converged points within bandwidth / 2 into clusters
clusters_.clear();/wipes out any left over clusters from previous calls/
  //-1 = "not assigned"
  /resizes the labels array to match the total number of input points/
labels_.assign(data.size(), -1);
/iterate through shifted points and initializes keeping track of both the current cordinates and its origin/
for (size_t i = 0; i < shiftedPoints.size(); ++i) {
int clusterIdx = 0;
bool found = false;
  
//is there already a claster close by?
for (auto& cluster : clusters_) {
if (euclideanDistance(shiftedPoints[i], cluster.centroid) < (bandwidth_ / 2.0)) {
  //join the near by cluster
cluster.pointIndices.push_back(i);
labels_[i] = clusterIdx;
found = true;
break;
}
clusterIdx++;
}
  
// start a new cluster if nearby cluster is not found
if (!found) {
  /creates a fresh container object to hold point indices of the new cluster/
Cluster newCluster;
/sets the centroid member of newcluster to converged coordinate vector/
newCluster.centroid = shiftedPoints[i];
/pushes the current points original O based indexi to the vector/ 
newCluster.pointIndices.push_back(i);
/appends newcluster to the classmaster vector container/
clusters_.push_back(newCluster);
  //index of the cluster just added
  /computes cluster-size and assigns that value to labels-i/
labels_[i] = clusters_.size() - 1;
}
}
}

const vector<Cluster>& MeanShift::getClusters() const {
return clusters_;
}

const vector<int>& MeanShift::getLabels() const {
return labels_;
}

double MeanShift::evaluate(const Data& data) const {
if (clusters_.empty() || data.empty()) return 0.0;

double totalVariance = 0.0;
for (const auto& cluster : clusters_) {
double clusterVar = 0.0;
for (size_t idx : cluster.pointIndices) {
clusterVar += euclideanDistance(data[idx], cluster.centroid);
}
totalVariance += clusterVar;
}
return totalVariance / data.size();
}

} // namespace meanshift
