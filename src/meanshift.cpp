#include "meanshift/MeanShift.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

namespace meanshift {

MeanShift::MeanShift(double bandwidth, double epsilon, int maxIterations)
    : bandwidth_(bandwidth), epsilon_(epsilon), maxIterations_(maxIterations) {}

double MeanShift::euclideanDistance(const Point& a, const Point& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(sum);
}

double MeanShift::gaussianKernel(double distance) const {
    return exp(-0.5 * (distance * distance) / (bandwidth_ * bandwidth_));
}

void MeanShift::estimateBandwidth(const Data& data) {
    if (data.empty()) return;
    
    // Simple heuristic: average distance between all pairs (or a subset)
    // To avoid O(N^2) on large datasets, we use a subset.
    size_t numSamples = min<size_t>(500, data.size());
    double totalDist = 0;
    int count = 0;
    for (size_t i = 0; i < numSamples; ++i) {
        for (size_t j = i + 1; j < numSamples; ++j) {
            totalDist += euclideanDistance(data[i], data[j]);
            count++;
        }
    }
    bandwidth_ = (count > 0) ? (totalDist / count) * 0.5 : 1.0;
    if (bandwidth_ <= 0) bandwidth_ = 1.0;
}
