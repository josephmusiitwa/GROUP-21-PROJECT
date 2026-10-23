#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

void testEuclideanDistance() {
meanshift::Point p1 = {0.0, 0.0};
meanshift::Point p2 = {3.0, 4.0};
double dist = meanshift::MeanShift::euclideanDistance(p1, p2);
if (dist != 5.0) throw runtime_error("testEuclideanDistance failed");
cout << "testEuclideanDistance passed." << endl;
}

void testMeanShiftClustering() {
meanshift::MeanShift ms(2.0, 1e-3, 100);
meanshift::Data data = {
{0.0, 0.0}, {0.1, 0.1}, {0.2, 0.2},
{10.0, 10.0}, {10.1, 10.1}, {10.2, 10.2}
};
ms.fit(data);
auto clusters = ms.getClusters();
if (clusters.size() != 2) throw runtime_error("testMeanShiftClustering failed");
cout << "testMeanShiftClustering passed." << endl;
}
void testGaussianKernel() {
    meanshift::MeanShift ms(2.0, 1e-3, 100);
    double w0 = ms.gaussianKernel(0.0);
    double w2 = ms.gaussianKernel(2.0);
    if (abs(w0 - 1.0) > 1e-6 || abs(w2 - exp(-0.5)) > 1e-6) {
        throw runtime_error("testGaussianKernel failed");
    }
    cout << "testGaussianKernel passed." << endl;
}

int main() {
testEuclideanDistance();
testMeanShiftClustering();
testGaussianKernel();

cout << "All tests passed!" << endl;
return 0;
}
