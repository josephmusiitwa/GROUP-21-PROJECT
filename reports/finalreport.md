# Final Report

**Group 21 · Mean Shift Clustering Library in C++ (`meanshift`)**
Topic: Machine Learning 13 — Design and Implementation of the Mean Shift Clustering Algorithm in C++
Repository: https://github.com/josephmusiitwa/GROUP-21-PROJECT
Submission date: 3rd November, 2026

---

## 1. The problem

Clustering means putting similar data points into the same group without being told the groups in advance. Many clustering methods (for example k-means) need you to say how many clusters you want before you start. Mean Shift does not. It finds the number of clusters from the data itself.

Our task was to build this algorithm as a **reusable C++ library**, not a one-off program, so that any C++ programmer can add it to their project, give it numbers, and get clusters back.

## 2. Objectives

1. Implement Mean Shift clustering in standard C++ using classes.
2. Provide a clear public interface (headers in `include/`, code in `src/`).
3. Load data from CSV files and scale it so that big-number columns do not dominate small-number columns.
4. Test the library with automated tests, including bad inputs.
5. Show how to use it with small example programs.
6. Build a project that compiles from a clean machine using CMake.
7. Work as a team of 9 using Git and GitHub (branches and pull requests).

## 3. How Mean Shift works (plain language)

1. Every data point is treated as a "walker".
2. Each walker looks at the points around it. Closer points count more, farther points count less. The weights come from a Gaussian (bell-curve) kernel, and the **bandwidth** controls how far "around" reaches.
3. The walker moves to the weighted average position of those points. This move is the **mean shift step**.
4. Steps 2–3 repeat. A walker stops when its move is smaller than a small number (`epsilon`), or after `maxIterations` rounds.
5. Walkers that end up in nearly the same place (closer than `bandwidth / 2`) are put in one cluster. That place is the cluster **centroid**.
6. Every original point gets the number of the cluster its walker ended in.

Small bandwidth gives many small clusters. Large bandwidth gives a few big ones. If the user does not give a bandwidth, the library estimates one from the data.


## 4. Library design

### 4.1 Structure

```
GROUP-21-PROJECT/
├── CMakeLists.txt            builds the static library, examples and tests
├── include/meanshift/
│   ├── Dataset.hpp           Dataset class: load and scale data
│   └── MeanShift.hpp         MeanShift class and Cluster struct
├── src/
│   ├── Dataset.cpp
│   └── meanshift.cpp
├── tests/test_meanshift.cpp  10 unit tests, run with ctest
├── examples/                 example1, example2, example3
├── data/input/               5 sample CSV files
├── data/output/              results written here
└── reports/                  weekly reports, individual reports, this report
```

All code is inside `namespace meanshift`. Two type names are used everywhere: `Point` (a `vector<double>`) and `Data` (a `vector<Point>`).

### 4.2 The classes 

**Class 1: `Dataset`**

`meanshift::Dataset` (`Dataset.hpp`, `Dataset.cpp`). 
To keep reading and preparing data in one place, separate from the clustering. 
It holds one member: `data_`, all the points read from the CSV file. 
Functions:`loadFromCSV()` fills it. `standardize()` and `normalize()` rescale it. `getData()` lets others read it. 
`data_` is private so nothing outside the class can change the points by accident. The functions are public because they are how users work with the class. `getData()` returns a `const` reference, so callers can read but not change the data. 
Usage: create a `Dataset`, call `loadFromCSV()`, optionally scale, then pass `getData()` to `MeanShift`. 
