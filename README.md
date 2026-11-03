# Mean Shift Clustering Library (C++)

**Group 21**· Machine Learning 13: **Mean Shift Clustering in C++**

## 1. Overview

This is a reusable C++ library that implements **Mean Shift clustering**. Clustering means grouping similar data points together without being told the groups in advance. Unlike k-means, Mean Shift does not need you to say how many clusters you want. It finds that number from the data.

You give the library a set of points (for example, rows from a CSV file). It gives back the cluster centres, which points belong to which cluster, and a simple score for how tight the clusters are.

## 2. Objectives

- Implement Mean Shift clustering using C++ classes.
- Keep a clear public interface (headers in `include/`, code in `src/`).
- Load CSV data and scale it (Z-score or Min-Max).
- Test the library with automated tests, including bad input.
- Show how to use it with small example programs.
- Build from a clean machine with CMake.
- Work as a team using Git and GitHub.

## 3. Main features

| Feature | Function |
| :--- | :--- |
| Load a CSV file (with or without a header) | `Dataset::loadFromCSV` |
| Z-score scaling (mean 0, standard deviation 1) | `Dataset::standardize` |
| Min-Max scaling (values between 0 and 1) | `Dataset::normalize` |
| Distance between two points | `MeanShift::euclideanDistance` |
| Gaussian kernel weight | `MeanShift::gaussianKernel` |
| Automatic bandwidth estimate | `MeanShift::estimateBandwidth` |
| One mean shift step | `MeanShift::shiftPoint` |
| Run the whole clustering | `MeanShift::fit` |
| Read the clusters and labels | `getClusters`, `getLabels` |
| Cluster quality score | `MeanShift::evaluate` |
| Any numeric type (`short`, `int`, `long`, `unsigned`, `int8_t`, `float`, `double`, `long double`, ...) | `BasicDataset<T>`, `BasicMeanShift<T, R, K>` |

## 4. Project structure

```
GROUP-21-PROJECT/
├── README.md
├── CMakeLists.txt        builds the library, examples and tests
├── LICENSE
├── .gitignore
├── include/meanshift/    public headers: Dataset.hpp, MeanShift.hpp
├── src/                  explicit instantiations for int, float, double
├── tests/                unit tests (test_meanshift.cpp)
├── examples/             example1.cpp, example2.cpp, example3.cpp
├── data/input/           sample CSV files
├── data/output/          clustering results are written here
└── reports/              weekly reports, individual reports, final report
```

## 5. Requirements

- Git
- CMake 3.20 or newer
- A C++ compiler that supports C++23 (we checked with GCC 13)

There are no other dependencies. The library uses only the C++ standard library.

## 6. How to build

```
git clone https://github.com/josephmusiitwa/GROUP-21-PROJECT.git
cd GROUP-21-PROJECT
cmake -S . -B build
cmake --build build
```

This creates the library `libmeanshift.a` (or `meanshift.lib` on Windows), the three example programs and the test program inside the `build` folder.

## 7. How to use the library

```cpp
#include "meanshift/Dataset.hpp"
#include "meanshift/MeanShift.hpp"
#include <iostream>

int main() {
    meanshift::Dataset dataset;
    if (!dataset.loadFromCSV("data/input/my_data.csv")) {   // false if the file cannot be opened
        return 1;
    }
    // dataset.standardize();   // optional: Z-score scaling
    // dataset.normalize();     // optional: Min-Max scaling

    meanshift::MeanShift ms(2.0, 1e-3, 300);   // bandwidth, epsilon, max iterations
    ms.fit(dataset.getData());

    for (const auto& cluster : ms.getClusters()) {
        std::cout << "Cluster with " << cluster.pointIndices.size() << " points\n";
    }
    std::cout << "Score: " << ms.evaluate(dataset.getData()) << "\n";
}
```

**What the settings mean:**

- **bandwidth**: how far a point "looks" for neighbours. Small values give many small clusters, large values give a few big ones. If you pass `-1` (the default), the library estimates it.
- **epsilon**: a point stops moving when its step is smaller than this.
- **max iterations**: the most steps a point may take.

**After `fit()`:**

- `getClusters()` gives a list of `Cluster`, each with a `centroid` (the centre) and `pointIndices` (which rows belong to it).
- `getLabels()` gives one cluster number per input row.
- `evaluate()` gives the average distance from each point to its centre. Lower means tighter clusters.

