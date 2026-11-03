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

