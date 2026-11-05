# Group 21: Mean Shift Clustering Algorithm Presentation (`meanshift`)

**Course Project:** Machine Learning 13 — Design and Implementation of the Mean Shift Clustering Algorithm in C++  

---

## Summary & Quick Navigation

Use this summary section to jump directly to any part of the presentation:

* **[1. Introduction](#1-introduction)** — Core concepts, problem statement, non-parametric mode-seeking workflow, and complete mathematical formulation.
  * [$\rightarrow$ 1.1 Definitions](#11-definitions)
  * [$\rightarrow$ 1.2 What It Does](#12-what-it-does)
  * [$\rightarrow$ 1.3 How It Does Its Function (Mathematical Formulae)](#13-how-it-does-its-function-mathematical-formulae)
* **[2. What We Did (Implementation in C++)](#2-what-we-did-implementation-in-c)** — Architecture, OOP design, header/source implementation details, and automated logic verification.
  * [$\rightarrow$ ① Dataset (`Dataset.hpp` & `Dataset.cpp`): What It Does & How](#21-1-dataset-hpp--cpp-what-it-does--how)
  * [$\rightarrow$ ② MeanShift (`MeanShift.hpp` & `meanshift.cpp`): What It Does & How](#22-2-meanshift-hpp--cpp-what-it-does--how)
  * [$\rightarrow$ ③ Tests Implemented to Test Function Logic (In All Files)](#23-3-tests-implemented-to-test-function-logic-in-all-files)
* **[3. Member Contributions](#3-member-contributions)** — Individual module ownership, code files, unit tests, and Git attribution commands.
  * [$\rightarrow$ 3.1 Team Contributions Matrix](#31-team-contributions-matrix)
  * [$\rightarrow$ 3.2 Live Git Attribution Commands](#32-live-git-attribution-commands)
* **[4. Demonstration of Codes (Running Examples)](#4-demonstration-of-codes-running-examples)** — Live terminal commands for building, testing, and running the 3 example applications.
  * [$\rightarrow$ 4.1 Step 1: Configure & Build (`CMakeLists.txt`)](#41-step-1-configure--build-the-library-and-executables)
  * [$\rightarrow$ 4.2 Step 2: Execute Automated Unit Tests (`CTest`)](#42-step-2-execute-the-10-automated-unit-tests-ctest)
  * [$\rightarrow$ 4.3 Step 3: Run the 3 Example Programs (`example1`, `example2`, `example3`)](#43-step-3-run-the-3-practical-example-programs)
* **[5. Conclusion](#5-conclusion)** — Summary of technical achievements and takeaways.

### Direct Repository File Links (Click to Open in VS Code)
* **Build & Configuration:** [`CMakeLists.txt`](./CMakeLists.txt) | [`examples/CMakeLists.txt`](./examples/CMakeLists.txt) | [`tests/CMakeLists.txt`](./tests/CMakeLists.txt) | [`.gitignore`](./.gitignore) | [`README.md`](./README.md)
* **Public Headers (`include/meanshift/`):** [`Dataset.hpp`](./include/meanshift/Dataset.hpp) | [`MeanShift.hpp`](./include/meanshift/MeanShift.hpp)
* **Core Implementations (`src/`):** [`Dataset.cpp`](./src/Dataset.cpp) | [`meanshift.cpp`](./src/meanshift.cpp)
* **Example Programs (`examples/`):** [`example1.cpp` (CLI CSV Workflow)](./examples/example1.cpp) | [`example2.cpp` (In-Memory Demo)](./examples/example2.cpp) | [`example3.cpp` (Interactive Tool)](./examples/example3.cpp)
* **Automated Test Suite (`tests/`):** [`test_meanshift.cpp` (10 Unit Tests)](./tests/test_meanshift.cpp)
* **Sample Datasets (`data/input/`):** [`my_data.csv`](./data/input/my_data.csv) | [`students.csv`](./data/input/students.csv) | [`biology_data.csv`](./data/input/biology_data.csv) | [`finance.csv`](./data/input/finance.csv) | [`text_data.csv`](./data/input/text_data.csv)
* **Project Reports (`reports/`):** [`finalreport.md`](./reports/finalreport.md) | [`week-01-report.md`](./reports/week-01-report.md) | [`week-2-report.md`](./reports/week-2-report.md) | [`week-3-report.md`](./reports/week-3-report.md) | [`week-4-report.md`](./reports/week-4-report.md)

---

## 1. Introduction

### 1.1 Definitions
* **Clustering:** An unsupervised machine learning task that partitions unlabeled data points into natural groups (*clusters*) based on feature similarity.
* **Mean Shift Algorithm:** A **non-parametric, mode-seeking clustering algorithm** that locates local maxima (*modes*) of a data distribution's underlying probability density function without requiring the number of clusters $K$ in advance.
* **Kernel Density Estimation (KDE) & Gaussian Kernel:** A mathematical technique that estimates a smooth continuous density surface over discrete data points using a radial weighting function (such as the **Gaussian kernel**).
* **Bandwidth ($h$ / `bandwidth_`):** The key scale parameter (neighborhood radius) controlling the smoothness of the kernel density estimate and the radius within which nearby points influence one another.
* **Mode / Centroid:** A stationary peak of local density toward which nearby points converge; represented in our library by the [`Cluster`](./include/meanshift/MeanShift.hpp) struct (`centroid` coordinates and `pointIndices`).
* **Convergence Tolerance ($\epsilon$ / `epsilon_`):** The minimum step displacement threshold below which a shifting point is considered stationary.

### 1.2 What It Does
Unlike parametric clustering methods such as K-Means—which require the user to specify the number of clusters $K$ in advance and assume spherical cluster shapes—**Mean Shift** discovers clusters directly from the data geometry:
1. Each data point is treated as a particle ("walker") on an underlying kernel density surface controlled by a neighborhood radius (**bandwidth** $h$).
2. At each step, every active point computes the **Gaussian kernel-weighted mean** of all dataset points around it and shifts toward the region of highest local density.
3. Points stop moving once their step displacement drops below a convergence tolerance $\epsilon$ (`epsilon_`) or reaches `maxIterations_`.
4. Converged points landing within `bandwidth / 2.0` of one another are merged into a single [`Cluster`](./include/meanshift/MeanShift.hpp) centroid, automatically discovering the number of clusters $K$ and assigning each point an integer label.

Our objective was to engineer this algorithm as a **reusable, standard C++ static library (`libmeanshift.a`)** (configured via [`CMakeLists.txt`](./CMakeLists.txt)) with clean header/implementation separation ([`Dataset.hpp`](./include/meanshift/Dataset.hpp), [`MeanShift.hpp`](./include/meanshift/MeanShift.hpp), [`Dataset.cpp`](./src/Dataset.cpp), [`meanshift.cpp`](./src/meanshift.cpp)), feature preprocessing, fault-tolerant CSV I/O, automated unit tests ([`test_meanshift.cpp`](./tests/test_meanshift.cpp)), and practical example applications ([`example1.cpp`](./examples/example1.cpp), [`example2.cpp`](./examples/example2.cpp), [`example3.cpp`](./examples/example3.cpp)).

### 1.3 How It Does Its Function (Mathematical Formulae)
Every stage of the library's pipeline is governed by explicit mathematical formulations implemented in [`src/Dataset.cpp`](./src/Dataset.cpp) and [`src/meanshift.cpp`](./src/meanshift.cpp):

1. **Z-Score Standardization (`Dataset::standardize()` in [`src/Dataset.cpp`](./src/Dataset.cpp), Lines 48–80):**
   Rescales each feature column $j$ across $N$ points to zero mean ($\mu_j = 0$) and unit standard deviation ($\sigma_j = 1$):
   $$\mu_j = \frac{1}{N}\sum_{i=0}^{N-1} x_{ij}, \qquad \sigma_j = \sqrt{\frac{1}{N}\sum_{i=0}^{N-1}(x_{ij} - \mu_j)^2}, \qquad z_{ij} = \frac{x_{ij} - \mu_j}{\sigma_j}$$

2. **Min-Max Normalization (`Dataset::normalize()` in [`src/Dataset.cpp`](./src/Dataset.cpp), Lines 82–103):**
   Linearly maps each feature column $j$ into $[0.0, 1.0]$:
   $$x'_{ij} = \frac{x_{ij} - \min(x_j)}{\max(x_j) - \min(x_j)}$$

3. **Euclidean Distance (`MeanShift::euclideanDistance()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 12–18):**
   Measures the straight-line distance between two $D$-dimensional points $a, b \in \mathbb{R}^D$:
   $$d(a, b) = \|a - b\|_2 = \sqrt{\sum_{i=0}^{D-1} (a_i - b_i)^2}$$

4. **Radial Gaussian Kernel (`MeanShift::gaussianKernel()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 20–22):**
   Assigns an exponentially decaying weight based on distance $d$ and bandwidth $h$:
   $$K(d) = \exp\left(-\frac{d^2}{2h^2}\right)$$

5. **Data-Driven Bandwidth Estimation (`MeanShift::estimateBandwidth()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 24–39):**
   Estimates $h$ as half the mean pairwise Euclidean distance across $M = \min(500, N)$ sampled points:
   $$h = 0.5 \times \bar{d} = 0.5 \times \frac{2}{M(M-1)} \sum_{0 \le i < j < M} \|x_i - x_j\|_2$$

6. **Kernel-Weighted Mean Shift Update (`MeanShift::shiftPoint()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 41–63):**
   Updates a query point $p$ toward the local center of mass of all dataset points $\{x_0, \dots, x_{N-1}\}$:
   $$m(p) = \frac{\sum_{j=0}^{N-1} K(\|p - x_j\|_2) \, x_j}{\sum_{j=0}^{N-1} K(\|p - x_j\|_2)}$$

7. **Stopping Criterion & Post-Convergence Mode Merging (`MeanShift::fit()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 65–124):**
   $$\|p_i^{(t+1)} - p_i^{(t)}\|_2 < \epsilon \quad \text{(point freezes)}, \qquad \|p_i^* - c_k\|_2 < \frac{h}{2} \quad \text{(merged into cluster } k\text{)}$$

8. **Intra-Cluster Cohesion Evaluation Metric (`MeanShift::evaluate()` in [`src/meanshift.cpp`](./src/meanshift.cpp), Lines 134–146):**
   Computes the average Euclidean distance from each original point $x_i$ to its assigned cluster centroid $c_k$:
   $$\text{Cohesion}(X, \mathcal{C}) = \frac{1}{N} \sum_{k=0}^{K-1} \sum_{i \in C_k.\text{pointIndices}} \|x_i - c_k\|_2$$

[⬆ Back to Summary](#summary--quick-navigation)

---

## 2. What We Did (Implementation in C++)

### Repository & Build Overview
* **Build Files:** [`CMakeLists.txt`](./CMakeLists.txt) | [`examples/CMakeLists.txt`](./examples/CMakeLists.txt) | [`tests/CMakeLists.txt`](./tests/CMakeLists.txt)
* **Architecture & OOP Highlights:**
  *(Diagrams in [`reports/images/`](./reports/images/): [`encapsulation_classes.jpeg`](./reports/images/encapsulation_classes.jpeg), [`encapsulation_namespace.jpeg`](./reports/images/encapsulation_namespace.jpeg), [`inheritance_stdlib.jpeg`](./reports/images/inheritance_stdlib.jpeg), [`polymorphism_overview.jpeg`](./reports/images/polymorphism_overview.jpeg))*
  * **Static Library Packaging:** [`CMakeLists.txt`](./CMakeLists.txt) compiles [`src/Dataset.cpp`](./src/Dataset.cpp) and [`src/meanshift.cpp`](./src/meanshift.cpp) into `libmeanshift.a` using C++23 (`cxx_std_23`) and exposes [`include/`](./include/) via `target_include_directories(meanshift PUBLIC ...)`.
  * **Encapsulation & Zero-Copy Accessors:** Internal state (`data_`, `bandwidth_`, `epsilon_`, `maxIterations_`, `clusters_`, `labels_`) is strictly `private` and read via `const` reference getters (`getData()`, `getClusters()`, `getLabels()`).
  * **Low Coupling (Composition over Inheritance):** [`MeanShift::fit(const Data& data)`](./include/meanshift/MeanShift.hpp) accepts the `Data` (`vector<vector<double>>`) alias rather than `Dataset`, supporting both CSV workflows ([`example1.cpp`](./examples/example1.cpp), [`example3.cpp`](./examples/example3.cpp)) and direct in-memory vectors ([`example2.cpp`](./examples/example2.cpp)).

```text
GROUP-21-PROJECT/
├── CMakeLists.txt                     # Root build config: static library (meanshift), install rules, subdirs
├── include/meanshift/
│   ├── Dataset.hpp                    # Public interface: Point, Data aliases & Dataset class
│   └── MeanShift.hpp                  # Public interface: Cluster struct & MeanShift class
├── src/
│   ├── Dataset.cpp                    # Implementation: CSV ingestion, standardize(), normalize()
│   └── meanshift.cpp                  # Implementation: KDE math, estimateBandwidth(), shiftPoint(), fit(), evaluate()
├── examples/                          # example1.cpp, example2.cpp, example3.cpp + CMakeLists.txt
├── tests/                             # test_meanshift.cpp (10 unit tests) + CMakeLists.txt
├── data/                              # input/ (5 CSV datasets) & output/ (exported results)
└── reports/                           # Weekly reports, finalreport.md, OOP images, 9 Individual reports
```

---

### 2.1 (1) Dataset (`HPP` & `CPP`): What It Does & How
* **Header File:** [`include/meanshift/Dataset.hpp`](./include/meanshift/Dataset.hpp) (Lines 1–33)
* **Implementation File:** [`src/Dataset.cpp`](./src/Dataset.cpp) (Lines 1–105)

#### What `Dataset` Does
The [`Dataset`](./include/meanshift/Dataset.hpp) module handles **data ingestion from CSV files**, **in-memory storage** of $D$-dimensional numeric points, and **feature scaling** (Z-score standardization and Min-Max normalization) so features with large numerical ranges do not dominate isotropic Euclidean distance calculations.

#### How `Dataset` Works
1. **Type Aliases & Class Interface ([`include/meanshift/Dataset.hpp`](./include/meanshift/Dataset.hpp)):**
   ```cpp
   namespace meanshift {

   using Point = vector<double>;
   using Data = vector<Point>;

   class Dataset {
   public:
       Dataset();
       bool loadFromCSV(const string& filename, bool hasHeader = true);
       const Data& getData() const;
       void standardize(); // Z-score normalization (mean = 0, std = 1)
       void normalize();   // Min-Max scaling to [0, 1]
   private:
       Data data_;
   };
   } // namespace meanshift
   ```
2. **Fault-Tolerant CSV Ingestion — `Dataset::loadFromCSV()` ([`src/Dataset.cpp`](./src/Dataset.cpp), Lines 12–42):**
   * Checks `if (!file.is_open())`, printing an error to `std::cerr` and returning `false` on missing files.
   * Skips the header row when `hasHeader == true`, splits each row by `','` via `std::stringstream`, and converts tokens using `std::stod(token)` inside a `try-catch` block:
     ```cpp
     while (getline(ss, token, ',')) {
         try {
             p.push_back(stod(token));
         } catch (const exception& e) {
         }
     }
     if (!p.empty()) {
         data_.push_back(p);
     }
     ```
   * **Mixed-Type Resilience:** In [`data/input/students.csv`](./data/input/students.csv), rows contain `StudentName,Math_Score,Science_Score` (e.g., `John Smith,85,92`). `stod("John Smith")` throws `std::invalid_argument`, which is caught and ignored, automatically extracting `{85.0, 92.0}`.
3. **Z-Score Standardization — `Dataset::standardize()` ([`src/Dataset.cpp`](./src/Dataset.cpp), Lines 48–80):**
   * Computes column means and population standard deviations in two passes, then rescales each feature to $\mu_j = 0, \sigma_j = 1$.
   * Guards against empty datasets (`if (data_.empty()) return;`) and zero-variance columns (`if (stdDevs[i] != 0)`).
4. **Min-Max Normalization — `Dataset::normalize()` ([`src/Dataset.cpp`](./src/Dataset.cpp), Lines 82–103):**
   * Initializes `Point mins = data_[0]; Point maxs = data_[0];` to find column extrema and linearly maps each feature into $[0.0, 1.0]$, guarding against constant columns with `if (maxs[i] - mins[i] != 0)`.

---

### 2.2 (2) MeanShift (`HPP` & `CPP`): What It Does & How
* **Header File:** [`include/meanshift/MeanShift.hpp`](./include/meanshift/MeanShift.hpp) (Lines 1–47)
* **Implementation File:** [`src/meanshift.cpp`](./src/meanshift.cpp) (Lines 1–148)

#### What `MeanShift` Does
The [`MeanShift`](./include/meanshift/MeanShift.hpp) module executes the **non-parametric mode-seeking clustering algorithm**: it automatically estimates the kernel bandwidth $h$ (if not supplied), iteratively shifts every point up the Gaussian density gradient until convergence, merges nearby converged points into [`Cluster`](./include/meanshift/MeanShift.hpp) centroids, assigns cluster labels, and evaluates intra-cluster cohesion.

#### How `MeanShift` Works
1. **Public Structures & Class Interface ([`include/meanshift/MeanShift.hpp`](./include/meanshift/MeanShift.hpp), Lines 12–45):**
   * Defines `struct Cluster { Point centroid; vector<int> pointIndices; };`.
   * Provides constructor `MeanShift(double bandwidth = -1.0, double epsilon = 1e-3, int maxIterations = 300);`, core methods `fit()`, `evaluate()`, and `const` accessors `getClusters()` and `getLabels()`.
2. **Distance & Kernel Helpers ([`src/meanshift.cpp`](./src/meanshift.cpp), Lines 12–22):**
   * `euclideanDistance(const Point& a, const Point& b)`: `static` helper computing $\|a - b\|_2$.
   * `gaussianKernel(double distance) const`: computes $\exp(-0.5 \cdot d^2 / h^2)$ using private `bandwidth_`.
3. **Subsampled Bandwidth Estimation — `estimateBandwidth()` ([`src/meanshift.cpp`](./src/meanshift.cpp), Lines 24–39):**
   * Triggered automatically in `fit()` when `bandwidth_ <= 0`.
   * Caps sample size at `min<size_t>(500, data.size())` (at most $124,750$ pairs instead of $O(N^2)$), averages pairwise distances, and sets `bandwidth_ = 0.5 * avgDist` (with a `1.0` fallback).
4. **Single-Point Kernel Shift — `shiftPoint()` ([`src/meanshift.cpp`](./src/meanshift.cpp), Lines 41–63):**
   * Accumulates `numerator[i] += other_p[i] * weight` and `denominator += weight` across all dataset points, returning the weighted mean coordinate $m(p)$.
5. **Two-Stage Mode-Seeking & Merging — `fit()` ([`src/meanshift.cpp`](./src/meanshift.cpp), Lines 65–124):**
   * **Stage 1 (Iterative Shifting, Lines 65–96):** Copies `data` into local `shiftedPoints` (preserving caller data). Uses a per-point freeze mask `vector<bool> stopMoving(data.size(), false)` so points whose step size drops below `epsilon_` stop re-evaluating `shiftPoint()`, and exits early when `allStopped == true`.
   * **Stage 2 (Mode Merging & Labeling, Lines 98–124):** Groups converged points within `bandwidth_ / 2.0` of an existing `cluster.centroid` into that cluster; otherwise creates a new `Cluster`, populating both `clusters_` and `labels_`.
6. **Cohesion Evaluation — `evaluate()` ([`src/meanshift.cpp`](./src/meanshift.cpp), Lines 134–146):**
   * Computes the mean Euclidean distance from each original point `data[idx]` to its assigned `cluster.centroid` in $O(N \cdot D)$ time.

---

### 2.3 (3) Tests Implemented to Test Function Logic (In All Files)
* **Test Suite File:** [`tests/test_meanshift.cpp`](./tests/test_meanshift.cpp) (Lines 1–171)
* **Test Build Config:** [`tests/CMakeLists.txt`](./tests/CMakeLists.txt)
* **Files Tested:** [`include/meanshift/Dataset.hpp`](./include/meanshift/Dataset.hpp), [`src/Dataset.cpp`](./src/Dataset.cpp), [`include/meanshift/MeanShift.hpp`](./include/meanshift/MeanShift.hpp), [`src/meanshift.cpp`](./src/meanshift.cpp)

We implemented **10 automated unit tests** in [`tests/test_meanshift.cpp`](./tests/test_meanshift.cpp) registered with CMake's `CTest` runner to verify the mathematical and functional logic across every class and method in both `Dataset` and `MeanShift`:

| # | Test Function in [`test_meanshift.cpp`](./tests/test_meanshift.cpp) | Lines | Target File & Function Logic Tested | What It Verifies |
| :---: | :--- | :---: | :--- | :--- |
| 1 | `testEuclideanDistance()` | 10–16 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::euclideanDistance()` | Verifies the 3-4-5 right triangle distance between `{0.0, 0.0}` and `{3.0, 4.0}` equals exactly `5.0`. |
| 2 | `testMeanShiftClustering()` | 18–28 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::fit()` & `getClusters()` | Verifies 6 points in 2 well-separated groups (`{0,0}..{0.2,0.2}` and `{10,10}..{10.2,10.2}`) converge into `2` clusters. |
| 3 | `testShiftPoint()` | 30–41 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::shiftPoint()` & `gaussianKernel()` | Verifies a midpoint `{1.0, 1.0}` equidistant between `{0.0, 0.0}` and `{2.0, 2.0}` experiences balanced kernel pull and remains at `{1.0, 1.0}`. |
| 4 | `testEstimateBandwidth()` | 42–52 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::estimateBandwidth()` | Verifies default `MeanShift ms;` (`bandwidth = -1.0`) automatically computes a positive bandwidth and discovers `2` clusters. |
| 5 | `testGetLabels()` | 54–69 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::getLabels()` & Stage 2 of `fit()` | Verifies 6 labels are returned where points `0..2` share one cluster ID, `3..5` share another, and `labels[0] != labels[3]`. |
| 6 | `testEvaluate()` | 70–80 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `MeanShift::evaluate()` | Verifies intra-cluster cohesion distance satisfies `0.0 < score <= 0.5` on compact clusters. |
| 7 | `testEmptyData()` | 81–89 | [`src/meanshift.cpp`](./src/meanshift.cpp) $\rightarrow$ `fit()`, `getClusters()`, `getLabels()`, `evaluate()` | Edge case: verifies passing empty `Data` (`{}`) leaves `clusters` and `labels` empty and returns `evaluate() == 0.0` safely. |
| 8 | `testLoadFromCSV()` | 90–110 | [`src/Dataset.cpp`](./src/Dataset.cpp) $\rightarrow$ `Dataset::loadFromCSV()` & `getData()` | Verifies loading a valid temporary CSV (`2` rows) succeeds and `"non_existent_file.csv"` returns `false` without crashing. |
| 9 | `testStandardize()` | 112–132 | [`src/Dataset.cpp`](./src/Dataset.cpp) $\rightarrow$ `Dataset::standardize()` | Verifies `{1.0, 10.0}` and `{3.0, 30.0}` ($10\times$ scale gap) rescale to exact Z-scores `{-1.0, +1.0}`. |
| 10 | `testNormalize()` | 133–154 | [`src/Dataset.cpp`](./src/Dataset.cpp) $\rightarrow$ `Dataset::normalize()` | Verifies `{10, 50}`, `{20, 100}`, `{30, 150}` rescale to exact Min-Max values `{0.0, 0.5, 1.0}` across all features. |

[⬆ Back to Summary](#summary--quick-navigation)

---

## 3. Member Contributions

### 3.1 Team Contributions Matrix
> **Tip for Assessors:** Click any code file link or Individual Report link below to inspect the exact code and documentation written by each member in VS Code.

| # | Team Member | Primary Role & Module Ownership | Code Files, Methods & Tests to Highlight | Individual Report |
| :---: | :--- | :--- | :--- | :--- |
| 1 | **Kizza Johnson Ssozi** | `Dataset` Class Architecture, CSV Ingestion & Statistical Moments | [`Dataset.hpp`](./include/meanshift/Dataset.hpp), [`src/Dataset.cpp`](./src/Dataset.cpp) (`Dataset()`, `loadFromCSV`, `getData`, mean/stdDev passes in `standardize`), `testEstimateBandwidth()`, `testGetLabels()`, [`biology_data.csv`](./data/input/biology_data.csv) | [`kizza.md`](./reports/Individual%20reports/kizza.md) |
| 2 | **Atuzarirwe Happy** | Feature Scaling & Preprocessing Algorithms | [`src/Dataset.cpp`](./src/Dataset.cpp) (Z-score scaling in `standardize` Lines 73–80, full `normalize` Lines 82–103), `testLoadFromCSV()`, `testStandardize()`, [`students.csv`](./data/input/students.csv) | [`Happy.md`](./reports/Individual%20reports/Happy.md) |
| 3 | **Atugonza Simon** | `Cluster` Struct, `MeanShift` Interface, Distance/Kernel Math & CLI App | [`MeanShift.hpp`](./include/meanshift/MeanShift.hpp), [`src/meanshift.cpp`](./src/meanshift.cpp) (`MeanShift` ctor, `euclideanDistance`, `gaussianKernel`), [`examples/example1.cpp`](./examples/example1.cpp), [`text_data.csv`](./data/input/text_data.csv) | [`simon_atugonza.md`](./reports/Individual%20reports/simon_atugonza.md) |
| 4 | **Sserunkuma Jonathan G.** | Automatic Bandwidth Estimation & Kernel Shift Update | [`src/meanshift.cpp`](./src/meanshift.cpp) (`estimateBandwidth` Lines 24–39, `shiftPoint` Lines 41–63), `testShiftPoint()`, co-developed [`examples/example2.cpp`](./examples/example2.cpp) | [`Jonathan.md`](./reports/Individual%20reports/Jonathan.md) |
| 5 | **Muhindo Nazarene Kamabu** | Iterative Mode-Seeking Engine & Convergence Control (`fit` Stage 1) | [`src/meanshift.cpp`](./src/meanshift.cpp) (`fit` Stage 1 Lines 65–96: `stopMoving` optimization, `epsilon_` convergence, `allStopped` early exit), [`finance.csv`](./data/input/finance.csv) | [`nazarene.md`](./reports/Individual%20reports/nazarene.md) |
| 6 | **Muzaki Desire Wadelo.F** | Post-Convergence Mode Merging & Label Assignment (`fit` Stage 2) | [`src/meanshift.cpp`](./src/meanshift.cpp) (`fit` Stage 2 Lines 98–124: mode merging within `bandwidth_ / 2.0`, populating `clusters_` & `labels_`), `testNormalize()`, [`examples/example3.cpp`](./examples/example3.cpp) | [`desire_muzaki.md`](./reports/Individual%20reports/desire_muzaki.md) |
| 7 | **Clive** | Output Accessors, Cohesion Evaluation Metric & Interactive Tool | [`src/meanshift.cpp`](./src/meanshift.cpp) (`getClusters`, `getLabels`, `evaluate` Lines 126–146), `testEvaluate()`, `testEmptyData()`, [`examples/example3.cpp`](./examples/example3.cpp) | [`Clive.md`](./reports/Individual%20reports/Clive.md) |
| 8 | **Mark Jordan** | In-Memory Demonstration, Default Dataset & OOP Architecture Diagrams | [`examples/example2.cpp`](./examples/example2.cpp), [`data/input/my_data.csv`](./data/input/my_data.csv), OOP diagrams in [`reports/images/`](./reports/images/), PR reviews & merges | [`Mark_Jordan.md`](./reports/Individual%20reports/Mark_Jordan.md) |
| 9 | **Musiitwa Joseph** | CMake Build System, Repository Integration & Automated Unit Test Suite | [`CMakeLists.txt`](./CMakeLists.txt), [`examples/CMakeLists.txt`](./examples/CMakeLists.txt), [`tests/CMakeLists.txt`](./tests/CMakeLists.txt), [`.gitignore`](./.gitignore), [`tests/test_meanshift.cpp`](./tests/test_meanshift.cpp) (10-test suite alignment) | [`Musiitwa_Joseph.md`](./reports/Individual%20reports/Musiitwa_Joseph.md) |

### 3.2 Live Git Attribution Commands
```bash
# 1. View visual commit history, branch structure, and Pull Request merges
git log --oneline --graph --decorate -n 25

# 2. View commit counts per team contributor
git shortlog -sn --all

# 3. Inspect line-by-line authorship on core implementation files
git blame src/Dataset.cpp
git blame src/meanshift.cpp
```

[⬆ Back to Summary](#summary--quick-navigation)

---

## 4. Demonstration of Codes (Running Examples)

Run these commands live in the VS Code integrated terminal (`Ctrl + ~`) to demonstrate compilation, unit test execution, and running examples on real datasets.

### 4.1 Step 1: Configure & Build the Library and Executables
* **Build Files:** [`CMakeLists.txt`](./CMakeLists.txt) | [`examples/CMakeLists.txt`](./examples/CMakeLists.txt) | [`tests/CMakeLists.txt`](./tests/CMakeLists.txt)
```bash
cmake -B build
cmake --build build
```
* **What to point out:** CMake compiles `libmeanshift.a` first, then links the 3 example binaries (`example1`, `example2`, `example3`) and the test runner (`test_meanshift`).

### 4.2 Step 2: Execute the 10 Automated Unit Tests (`CTest`)
* **Test File:** [`tests/test_meanshift.cpp`](./tests/test_meanshift.cpp)
```bash
ctest --test-dir build --output-on-failure --verbose
```

### 4.3 Step 3: Run the 3 Practical Example Programs

#### Demo A: Command-Line CSV Clustering & Export ([`examples/example1.cpp`](./examples/example1.cpp))
1. **Default Run on [`data/input/my_data.csv`](./data/input/my_data.csv) (30 points in 3 groups):**
   ```bash
   ./build/examples/example1 data/input/my_data.csv data/output/results.csv
   ```
   * **Expected Output:** Discovers **3 clusters** (10 points each) around centroids `(2.00108, 2.0977)`, `(7.92302, 8.01874)`, and `(2.07534, 7.94359)` with cohesion score **`0.259014`**, and writes labeled rows (`Feature1,Feature2,Cluster_ID`) to `data/output/results.csv`.
2. **Mixed Text & Numeric CSV Run on [`data/input/students.csv`](./data/input/students.csv):**
   ```bash
   ./build/examples/example1 data/input/students.csv data/output/students_results.csv
   ```
   * **What to highlight:** `loadFromCSV()` automatically ignores the `StudentName` text column, clusters the 12 students by `(Math_Score, Science_Score)` into **4 performance groups**, and exports the labeled results.
3. **Empty File Edge Case on [`data/input/text_data.csv`](./data/input/text_data.csv):**
   ```bash
   ./build/examples/example1 data/input/text_data.csv data/output/empty_results.csv
   ```
   * **What to highlight:** Gracefully reports `Number of clusters found: 0` and `Clustering evaluation score: 0` without crashing.

#### Demo B: In-Memory Synthetic Data Clustering ([`examples/example2.cpp`](./examples/example2.cpp))
```bash
./build/examples/example2
```
* **What to highlight:** Demonstrates low coupling—`MeanShift` clusters a `meanshift::Data` vector defined directly in C++ memory (9 points in 3 groups) without needing `Dataset` or CSV files, finding **3 clusters** with cohesion score **`0.622962`**.

#### Demo C: Interactive User Console Tool ([`examples/example3.cpp`](./examples/example3.cpp))
```bash
./build/examples/example3
```
* When prompted in the terminal, enter:
  * Input path: `data/input/biology_data.csv` (or `data/input/finance.csv`)
  * Output path: `data/output/bio_results.csv`

[⬆ Back to Summary](#summary--quick-navigation)

---

## 5. Conclusion

1. **Non-Parametric Mode Discovery:** By implementing Gaussian Kernel Density Estimation and iterative gradient ascent in C++, our `meanshift` library automatically identifies cluster count $K$ and arbitrary cluster shapes without prior assumptions.
2. **Clean Modular C++ Engineering:** Strict separation between [`Dataset`](./include/meanshift/Dataset.hpp) ([`src/Dataset.cpp`](./src/Dataset.cpp)) for fault-tolerant CSV ingestion and feature scaling and [`MeanShift`](./include/meanshift/MeanShift.hpp) ([`src/meanshift.cpp`](./src/meanshift.cpp)) for mode-seeking clustering ensures high cohesion, low coupling, and reusable static library packaging (`libmeanshift.a`).
3. **Verified Correctness & Practical Usability:** All mathematical and edge-case behaviors are validated across 10 automated unit tests in [`tests/test_meanshift.cpp`](./tests/test_meanshift.cpp) and demonstrated through 3 end-to-end applications ([`example1.cpp`](./examples/example1.cpp), [`example2.cpp`](./examples/example2.cpp), [`example3.cpp`](./examples/example3.cpp)).

[⬆ Back to Summary](#summary--quick-navigation)
