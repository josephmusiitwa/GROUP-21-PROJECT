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

**Class 2: `MeanShift`**

`meanshift::MeanShift` (`MeanShift.hpp`, `meanshift.cpp`). 
To keep the algorithm and its settings together. The user sets the settings once, calls `fit()`, then reads the results. 
It holds Settings: `bandwidth_`, `epsilon_`, `maxIterations_`. Results: `clusters_`, `labels_`. 
The constructor sets the settings. `estimateBandwidth()` picks a bandwidth. `gaussianKernel()` weights a neighbour. `shiftPoint()` does one mean shift step. `fit()` runs the whole algorithm. `getClusters()`, `getLabels()` and `evaluate()` read the results. 
Settings and results are private, so the results always match the settings that produced them. The small maths functions are public so they can be tested one by one. `euclideanDistance()` is `static` because it needs only two points and no object data. 
Usage: construct, `fit(data)`, read results. `MeanShift` needs only the `Data` type, not `Dataset`, so it can cluster data from any source. 

**Struct: `Cluster`** 

This is a plain struct with two public members, `centroid` (a `Point`) and `pointIndices` (which rows belong to it). It is a simple bundle of result data with no behaviour, so a struct is enough. Callers get it through a `const` reference.

### 4.3 Encapsulation, inheritance and polymorphism

- **Encapsulation:** all data members of `Dataset` and `MeanShift` are private. Outside code can only use the public functions. Read access returns `const` references.
- **Inheritance:** our own classes do not inherit from each other. `Dataset` and `MeanShift` do different jobs, and one is not a kind of the other, so we use composition (the program uses both side by side). Inheritance does appear in the standard library parts we rely on: `ifstream` and `stringstream` derive from `istream`, and `std::stod` throws `invalid_argument` and `out_of_range`, which derive from `std::exception`.
- **Polymorphism:** we use default arguments (`MeanShift ms;` or `MeanShift ms(2.0, 1e-3, 300);`, and `loadFromCSV(file)` or `loadFromCSV(file, false)`), templates (`vector<Point>`, `vector<Cluster>`), and base-class references from the standard library (`catch (const exception&)`, `istream&` in `getline`). We did not write our own `virtual` functions, because there are no derived classes.

### 4.4 Important implementation decisions

1. **Separate `Dataset` and `MeanShift`:** Loading files and clustering are different jobs. This made each one easier to write, test and give to different people. 
2. **Gaussian kernel:** Smooth weights, no sharp cut-off, one parameter (bandwidth). 
3. **Bandwidth guessed as half the average distance between points, using at most the first 500 points:** Avoids comparing every pair on large data, which would be very slow. 
4. **Per-point stop flag (`stopMoving`):** Once a point has settled it is no longer recomputed. This saves a lot of time because every step compares a point with all others. 
5. **`fit()` works on a copy of the data:** The caller's data is `const` and is never changed. 
6. **Merge converged points that are within `bandwidth / 2`:** Floating-point numbers are almost never exactly equal, so we compare with a distance, not with `==`. 
7. **`evaluate()` returns the mean distance from each point to its centroid:** It is quick to compute and easy to explain. Lower means tighter clusters. 
8. **`loadFromCSV()` skips the header, empty lines and values that are not numbers:** Lets us use files with name or category columns (for example `students.csv`) without extra code. 
9. **Static library `libmeanshift.a` with `install()` rules:** Other CMake projects can use it as `MeanShift::meanshift`. 
