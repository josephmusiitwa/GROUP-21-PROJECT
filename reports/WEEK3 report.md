# Week 3 Progress Report

**Group 21 · Mean Shift Clustering Library (C++)**

## Completed
- **Dataset Implementation (`src/Dataset.cpp`):**
  - Completed `loadFromCSV()` and `getData()`, plus column mean and standard deviation calculations for `standardize()` *(Kizza)*.
  - Completed the feature scaling step in `standardize()` (Z-score scaling to mean `0`, standard deviation `1`) and implemented `normalize()` (Min-Max scaling to `[0, 1]`) *(Happy)*.
- **MeanShift Implementation (`src/meanshift.cpp`):**
  - Implemented the first stage of `fit()`, including automatic bandwidth estimation when `bandwidth_ <= 0` and iterative point shifting until convergence (`dist < epsilon_`) or `maxIterations_` is reached *(Nazarene)*.
  - Implemented the cluster formation stage of `fit()`, grouping converged points within `bandwidth_ / 2.0` and populating `clusters_` and `labels_` *(Desire)*.
  - Implemented `getClusters()`, `getLabels()`, and `evaluate()` (intra-cluster cohesion score) *(Clive)*.
- **Examples & Testing (`examples/`, `tests/`):**
  - Added `examples/example1.cpp` to load external CSV data, run `MeanShift`, print cluster summaries, and save labeled output to `data/output/results.csv` *(Simon)*.
  - Set up `examples/` and added unit tests in `tests/test_meanshift.cpp` (`testEuclideanDistance` and `testMeanShiftClustering`) *(Joseph)*.
  - Implemented `examples/example2.cpp` with sample data setup, `fit()` execution, and output of cluster count, centroids, and evaluation score *(Mark & Jonathan)*.
- **Repository Structure & Code Cleanup (`include/meanshift/`, `src/`, `examples/`):**
  - Moved `Dataset.hpp` and `MeanShift.hpp` into `include/meanshift/`, removed tracked compiler cache files (`src/gcm.cache/`) and `reports/.gitkeep`, and cleaned up unnecessary comments across headers, source files, and examples *(Joseph)*.

---

## In Progress
- Expanding `tests/test_meanshift.cpp` to test `Dataset` methods (`loadFromCSV`, `getData`, `standardize`, `normalize`), edge cases, and invalid inputs for `MeanShift`.
- Adding `data/input/my_data.csv` (required by `example1.cpp`) along with additional sample datasets and example programs.
- Writing the full `README.md` documentation and drafting the final project report.

---

## Challenges/Blockers
- **Missing Input Data File:** `examples/example1.cpp` exits with an error by default because `data/input/my_data.csv` has not been added to `data/input/` yet.
- **Limited Test Coverage:** Current tests only verify two normal-use cases and do not yet cover `Dataset` operations, edge cases, or invalid inputs.

---

## Next Week
- Add unit tests for `Dataset` (`loadFromCSV`, `standardize`, `normalize`) and edge/invalid inputs for `MeanShift` (such as empty datasets).
- Add `data/input/my_data.csv` and the remaining example programs and sample datasets.
- Complete all sections of `README.md`, prepare the Week 4 and final reports, and test building and running the library on a clean machine before the final demonstration.

---

## Encapsulation, Inheritance, and Polymorphism

### 1. Encapsulation
Encapsulation is implemented by bundling internal data (`private`) with the methods that operate on it (`public`) inside the `meanshift` namespace, preventing external code from corrupting object state:
- **`Dataset` (`include/meanshift/Dataset.hpp`, `src/Dataset.cpp`):**
  - **Private State:** `Data data_` (a 2D vector `vector<vector<double>>`) is declared `private` so external code cannot directly modify or invalidate loaded points.
  - **Public Interface:** Data can only be populated or transformed through `loadFromCSV()`, `standardize()`, and `normalize()`. Read access is provided through `const Data& getData() const`, which returns a `const` reference to avoid copying while preventing external modification.
- **`MeanShift` (`include/meanshift/MeanShift.hpp`, `src/meanshift.cpp`):**
  - **Private State:** Hyperparameters (`bandwidth_`, `epsilon_`, `maxIterations_`) and clustering outputs (`clusters_`, `labels_`) are `private`, ensuring results always match the parameters used during `fit()`.
  - **Public Interface:** The constructor initializes settings, `fit()` updates internal cluster state, and `const` member functions (`getClusters()`, `getLabels()`, `evaluate()`, `gaussianKernel()`, `shiftPoint()`) inspect or compute values without altering internal members.
- **`Cluster` (`include/meanshift/MeanShift.hpp`):**
  - Defined as a plain data `struct` with `public` members (`centroid` and `pointIndices`), safely exposed to callers as a read-only reference via `const vector<Cluster>& getClusters() const`.

### 2. Inheritance
- **Library Classes (`Dataset`, `MeanShift`, `Cluster`):**
  - The library uses **composition and modular separation** rather than class inheritance (`Base -> Derived`). `Dataset` handles file I/O and feature scaling, while `MeanShift` handles clustering on `const Data&`. Because neither class has derived subclasses, no `protected` members are needed.
- **Standard Library Inheritance Used:**
  - **Stream Hierarchy:** `Dataset::loadFromCSV()` uses `std::ifstream` and `std::stringstream` (both derived from `std::istream`) with `std::getline()`.
  - **Exception Hierarchy:** `Dataset::loadFromCSV()` catches `const std::exception&` to handle derived exceptions (`std::invalid_argument`, `std::out_of_range`) thrown by `std::stod()`.

### 3. Polymorphism
- **Compile-Time Overloading via Default Arguments:**
  - `MeanShift(double bandwidth = -1.0, double epsilon = 1e-3, int maxIterations = 300)` allows instantiating `MeanShift` with 0, 1, 2, or 3 arguments (e.g., `MeanShift ms;` for automatic bandwidth estimation vs. `MeanShift ms(2.0, 1e-3, 300);`).
  - `loadFromCSV(const string& filename, bool hasHeader = true)` can be called with or without explicitly passing `hasHeader`.
- **Compile-Time Parametric Polymorphism (Templates):**
  - Uses C++ Standard Library templates (`std::vector<double>`, `std::vector<Point>`, `std::vector<Cluster>`, `std::vector<int>`, `std::vector<size_t>`, and `std::min<size_t>`) to work generically across data containers.
- **Runtime Polymorphism:**
  - Used via base-class references in the C++ standard library (`const std::exception&` in `try/catch` and `std::istream&` in `std::getline()`). Custom `virtual` functions are not used since `Dataset` and `MeanShift` are standalone concrete classes without derived types.

---

## Class Layouts

*(Hand-drawn paper class layout diagrams are uploaded alongside this report.)*

```text
+-------------------------------------------------------------------+
|                        namespace meanshift                        |
|   using Point = std::vector<double>;                              |
|   using Data  = std::vector<Point>;                               |
+-------------------------------------------------------------------+

+-------------------------------------------------------------------+
|                          class Dataset                            |
+-------------------------------------------------------------------+
| private:                                                          |
|   - data_ : Data                                                  |
+-------------------------------------------------------------------+
| protected:                                                        |
|   (none)                                                          |
+-------------------------------------------------------------------+
| public:                                                           |
|   + Dataset()                                                     |
|   + loadFromCSV(filename : const string&, hasHeader : bool = true)|
|       : bool                                                      |
|   + getData() const : const Data&                                 |
|   + standardize() : void                                          |
|   + normalize() : void                                            |
+-------------------------------------------------------------------+
                                  |
                                  | passes const Data& (via getData())
                                  v
+-------------------------------------------------------------------+
|                         class MeanShift                           |
+-------------------------------------------------------------------+
| private:                                                          |
|   - bandwidth_     : double                                       |
|   - epsilon_       : double                                       |
|   - maxIterations_ : int                                          |
|   - clusters_      : vector<Cluster>                              |
|   - labels_        : vector<int>                                  |
+-------------------------------------------------------------------+
| protected:                                                        |
|   (none)                                                          |
+-------------------------------------------------------------------+
| public:                                                           |
|   + MeanShift(bandwidth : double = -1.0,                          |
|               epsilon : double = 1e-3,                            |
|               maxIterations : int = 300)                          |
|   + static euclideanDistance(a : const Point&, b : const Point&)  |
|       : double                                                    |
|   + gaussianKernel(distance : double) const : double              |
|   + estimateBandwidth(data : const Data&) : void                  |
|   + fit(data : const Data&) : void                                |
|   + shiftPoint(p : const Point&, data : const Data&) const        |
|       : Point                                                     |
|   + getClusters() const : const vector<Cluster>&                  |
|   + getLabels() const : const vector<int>&                        |
|   + evaluate(data : const Data&) const : double                   |
+-------------------------------------------------------------------+
                                  |
                                  | contains (composition: vector<Cluster>)
                                  v
+-------------------------------------------------------------------+
|                          struct Cluster                           |
+-------------------------------------------------------------------+
| public:                                                           |
|   + centroid     : Point                                          |
|   + pointIndices : vector<size_t>                                 |
+-------------------------------------------------------------------+
```

---

## AI Use
- **Tool:** Gemini and Claude
