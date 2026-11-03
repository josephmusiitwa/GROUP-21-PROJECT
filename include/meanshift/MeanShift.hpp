#ifndef MEANSHIFT_HPP
#define MEANSHIFT_HPP
// for point and data
#include "meanshift/Dataset.hpp"
#include <vector>
//for size_t
#include <cstddef>

using namespace std;

namespace meanshift {

struct Cluster {
//the centre (hilltop) of this cluster
Point centroid;
//which rows of the data belong to it
vector<size_t> pointIndices;
};

class MeanShift {
public:
MeanShift(double bandwidth = -1.0, double epsilon = 1e-3, int maxIterations = 300);

static double euclideanDistance(const Point& a, const Point& b);
double gaussianKernel(double distance) const;

// Automatically estimates bandwidth when bandwidth <= 0
void estimateBandwidth(const Data& data);

void fit(const Data& data);
Point shiftPoint(const Point& p, const Data& data) const;

const vector<Cluster>& getClusters() const;
const vector<int>& getLabels() const;

// Mean intra-cluster distance to centroid (lower indicates tighter cohesion)
double evaluate(const Data& data) const;

private:
double bandwidth_;
double epsilon_;
int maxIterations_;

vector<Cluster> clusters_;
vector<int> labels_;
};

} // namespace meanshift

#endif // MEANSHIFT_HPP
