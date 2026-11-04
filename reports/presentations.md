# Group 21: Mean Shift Clustering Library in C++ (`meanshift`)

**Course Project:** Machine Learning 13 — Design and Implementation of the Mean Shift Clustering Algorithm in C++  
**Repository:** `GROUP-21-PROJECT` | **Language Standard:** C++23 | **Build System:** CMake & CTest

> **How to Use This Guide in VS Code:**  
> Open this file in VS Code and press `Ctrl+Shift+V` (or click **Open Preview to the Side**) to render the interactive documentation, mathematical formulas, and clickable hyperlinks. Click any file link below to jump directly to the source code in the VS Code editor.

---

## Quick Navigation Hub

### Presentation Sections
1. [1. Project Overview & Repository Structure](#1-project-overview--repository-structure)
2. [2. Design Decisions & OOP Principles](#2-design-decisions--oop-principles)
3. [3. Actual Code & Implementation Walkthrough](#3-actual-code--implementation-walkthrough)
   - [3.1 Module A: Data Ingestion & Feature Scaling (`Dataset`)](#31-module-a-data-ingestion--feature-scaling-dataset)
   - [3.2 Module B: Mean Shift Clustering Engine (`MeanShift` & `Cluster`)](#32-module-b-mean-shift-clustering-engine-meanshift--cluster)
4. [4. Individual Team Contributions](#4-individual-team-contributions)
5. [5. Practical Demonstrations & Automated Testing Playbook](#5-practical-demonstrations--automated-testing-playbook)
6. [6. Live Coding Walkthroughs](#6-live-coding-walkthroughs)

### Direct Repository File Links (Click to Open in VS Code)
* **Build & Configuration:** [`CMakeLists.txt`](./CMakeLists.txt) | [`examples/CMakeLists.txt`](./examples/CMakeLists.txt) | [`tests/CMakeLists.txt`](./tests/CMakeLists.txt) | [`.gitignore`](./.gitignore) | [`README.md`](./README.md)
* **Public Headers (`include/meanshift/`):** [`Dataset.hpp`](./include/meanshift/Dataset.hpp) | [`MeanShift.hpp`](./include/meanshift/MeanShift.hpp)
* **Core Implementations (`src/`):** [`Dataset.cpp`](./src/Dataset.cpp) | [`meanshift.cpp`](./src/meanshift.cpp)
* **Example Programs (`examples/`):** [`example1.cpp` (CLI CSV Workflow)](./examples/example1.cpp) | [`example2.cpp` (In-Memory Demo)](./examples/example2.cpp) | [`example3.cpp` (Interactive Tool)](./examples/example3.cpp)
* **Automated Test Suite (`tests/`):** [`test_meanshift.cpp` (10 Unit Tests)](./tests/test_meanshift.cpp)
* **Sample Datasets (`data/input/`):** [`my_data.csv`](./data/input/my_data.csv) | [`students.csv`](./data/input/students.csv) | [`biology_data.csv`](./data/input/biology_data.csv) | [`finance.csv`](./data/input/finance.csv) | [`text_data.csv`](./data/input/text_data.csv)
* **Project Reports (`reports/`):** [`finalreport.md`](./reports/finalreport.md) | [`week-01-report.md`](./reports/week-01-report.md) | [`week-2-report.md`](./reports/week-2-report.md) | [`week-3-report.md`](./reports/week-3-report.md) | [`week-4-report.md`](./reports/week-4-report.md)

---

## 1. Project Overview & Repository Structure

### 1.1 Problem Statement & Algorithmic Overview
Unlike parametric clustering methods such as K-Means—which require the user to specify the number of clusters $K$ in advance and assume spherical cluster shapes—**Mean Shift** is a **non-parametric, mode-seeking clustering algorithm**.
1. Each data point $x_i \in \mathbb{R}^D$ is treated as a particle ("walker") on an underlying kernel density surface controlled by a neighborhood radius (**bandwidth** $h$).
2. At each step, every active point computes the **Gaussian kernel-weighted mean** of all dataset points around it and shifts toward the region of highest local density.
3. Points stops moving once their step displacement drops below a convergence tolerance $\epsilon$ (`epsilon_`) or reaches `maxIterations_`.
4. Converged points landing within `bandwidth / 2.0` of one another are merged into a single `Cluster` centroid, automatically discovering the number of clusters $K$ from the data geometry.

Our objective was to engineer this algorithm as a **reusable, standard C++ static library (`libmeanshift.a`)** with clean header/implementation separation, feature preprocessing, fault-tolerant CSV I/O, automated unit tests, and example applications.

### 1.2 Repository Structure
```text
GROUP-21-PROJECT/
├── CMakeLists.txt                     # Root build config: static library (meanshift), install rules, subdirs
├── README.md                          # Project overview and usage documentation
├── Presentation.md                    # Technical presentation guide (this document)
├── LICENSE                            # Project license
├── .gitignore                         # Ignores build artifacts, binaries, and temporary files
├── include/
│   └── meanshift/
│       ├── Dataset.hpp                # Public interface: Point, Data aliases & Dataset class
│       └── MeanShift.hpp              # Public interface: Cluster struct & MeanShift class
├── src/
│   ├── Dataset.cpp                    # Implementation: CSV ingestion, standardize(), normalize()
│   └── meanshift.cpp                  # Implementation: KDE math, estimateBandwidth(), shiftPoint(), fit(), evaluate()
├── examples/
│   ├── CMakeLists.txt                 # Builds example1, example2, example3 linked against meanshift
│   ├── example1.cpp                   # CLI workflow: loads CSV, clusters, exports labeled CSV
│   ├── example2.cpp                   # In-memory workflow: clusters 9 synthetic 2D points directly
│   └── example3.cpp                   # Interactive CLI tool: prompts user for input/output CSV paths
├── tests/
│   ├── CMakeLists.txt                 # Registers test_meanshift executable with CTest
│   └── test_meanshift.cpp             # 10 automated unit tests (normal, edge, and invalid input cases)
├── data/
│   ├── input/                         # 5 test datasets: my_data.csv, students.csv, biology_data.csv, finance.csv, text_data.csv
│   └── output/                        # Destination folder for exported clustering results (results.csv)
└── reports/
    ├── finalreport.md                 # Complete project report
    ├── week-01-report.md ... week-4-report.md
    ├── images/                        # Architectural & OOP diagrams
    └── Individual reports/            # 9 detailed individual technical reports
```

### 1.3 Build System Walkthrough (`CMakeLists.txt`)
* **Inspect Files:** [`CMakeLists.txt`](./CMakeLists.txt) | [`examples/CMakeLists.txt`](./examples/CMakeLists.txt) | [`tests/CMakeLists.txt`](./tests/CMakeLists.txt)
* **Technical Highlights:**
  * **Static Library Target (Lines 9–12):** Compiles [`src/Dataset.cpp`](./src/Dataset.cpp) and [`src/meanshift.cpp`](./src/meanshift.cpp) into `libmeanshift.a` (`add_library(meanshift STATIC ...)`), decoupling library compilation from consumer executables.
  * **Target-Scoped Include Propagation (Lines 14–18):** Uses `target_include_directories(meanshift PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)` so any target linking `meanshift` automatically resolves `#include "meanshift/Dataset.hpp"` and `#include "meanshift/MeanShift.hpp"`.
  * **Modern C++ Standard & Package Export (Lines 20–38):** Enforces `cxx_std_23` and exports `MeanShift::meanshift` via `install(EXPORT MeanShiftTargets ...)` for external CMake projects.
  * **Downstream Executables & CTest Integration (Lines 41–44):** Links `example1`, `example2`, `example3`, and `test_meanshift` via `target_link_libraries(<target> PRIVATE meanshift)`.

[⬆ Back to Top](#quick-navigation-hub)


## 2. Design Decisions & OOP Principles

### 2.1 Core Engineering & Algorithmic Design Decisions

| Design Decision | Implementation Location | Technical Justification |
| :--- | :--- | :--- |
| **1. Strict Header / Source Separation** | [`include/meanshift/`](./include/meanshift/) $\rightarrow$ [`src/`](./src/) | Decouples the public API contract from mathematical implementation details, enabling clean static library packaging (`libmeanshift.a`). |
| **2. Decoupling `Dataset` from `MeanShift`** | [`Dataset.hpp`](./include/meanshift/Dataset.hpp) & [`MeanShift.hpp`](./include/meanshift/MeanShift.hpp) | `MeanShift::fit(const Data& data)` operates on the `Data` (`vector<vector<double>>`) type rather than `Dataset`. This allows clustering both CSV files ([`example1.cpp`](./examples/example1.cpp)) and in-memory vectors ([`example2.cpp`](./examples/example2.cpp)). |
| **3. Feature Scaling Pipeline (`standardize` & `normalize`)** | [`src/Dataset.cpp`](./src/Dataset.cpp) (Lines 48–103) | Mean Shift uses isotropic Euclidean distance inside a single scalar bandwidth $h$. Feature scaling prevents large-magnitude columns from dominating small-magnitude columns. |
| **4. Subsampled Bandwidth Estimation ($M \le 500$)** | [`src/meanshift.cpp`](./src/meanshift.cpp) (Lines 24–39) | Computing all pairwise distances costs $O(N^2)$. Capping the sample at `min<size_t>(500, data.size())` bounds estimation to at most $124,750$ pairs, running in milliseconds on large datasets. |
| **5. Per-Point Freeze Mask (`stopMoving`)** | [`src/meanshift.cpp`](./src/meanshift.cpp) (Lines 73–91) | Each `shiftPoint()` call costs $O(N \cdot D)$. Freezing points as soon as their step size drops below `epsilon_` (`stopMoving[i] = true`) skips redundant kernel evaluations in later iterations. |
| **6. Non-Mutating `fit(const Data&)` Execution** | [`src/meanshift.cpp`](./src/meanshift.cpp) (Line 72) | Copies input coordinates into a local working vector `vector<Point> shiftedPoints = data;` so the caller's original dataset is never mutated. |
| **7. Radius-Based Mode Merging ($h / 2$)** | [`src/meanshift.cpp`](./src/meanshift.cpp) (Lines 98–124) | Converged floating-point trajectories around the same density peak differ by up to $\sim \epsilon$. Grouping points within `bandwidth_ / 2.0` merges numerical jitter without collapsing distinct peaks. |
| **8. Fault-Tolerant Mixed-Type CSV Parser** | [`src/Dataset.cpp`](./src/Dataset.cpp) (Lines 12–42) | Skips optional headers, ignores blank lines, and wraps `std::stod(token)` in `try-catch` so mixed text/numeric datasets (like [`students.csv`](./data/input/students.csv)) automatically extract numeric columns without crashing. |
| **9. $O(N \cdot D)$ Intra-Cluster Cohesion Metric** | [`src/meanshift.cpp`](./src/meanshift.cpp) (Lines 134–146) | Computes the mean Euclidean distance from each original point `data[idx]` to its assigned `cluster.centroid`, avoiding $O(N^2)$ pairwise Silhouette overhead. 

### 2.2 Object-Oriented Programming (OOP) Principles
*(Visual architecture diagrams available in [`reports/images/`](./reports/images/): [`encapsulation_classes.jpeg`](./reports/images/encapsulation_classes.jpeg), [`encapsulation_namespace.jpeg`](./reports/images/encapsulation_namespace.jpeg), [`inheritance_stdlib.jpeg`](./reports/images/inheritance_stdlib.jpeg), [`polymorphism_overview.jpeg`](./reports/images/polymorphism_overview.jpeg))*

1. **Encapsulation & Data Hiding:**
   * All internal state is `private`: `data_` in [`Dataset`](./include/meanshift/Dataset.hpp) (Line 28), and `bandwidth_`, `epsilon_`, `maxIterations_`, `clusters_`, and `labels_` in [`MeanShift`](./include/meanshift/MeanShift.hpp) (Lines 36–42).
   * Read access is exposed strictly through `const` member functions returning `const` references (`const Data& getData() const`, `const vector<Cluster>& getClusters() const`, `const vector<int>& getLabels() const`), achieving **zero-copy read efficiency** while preventing external state corruption.
   * [`Cluster`](./include/meanshift/MeanShift.hpp) (Lines 12–15) is a passive data `struct` (`centroid` and `pointIndices`) safely exposed via `const vector<Cluster>&`.
2. **Composition & Standard Library Inheritance:**
   * Our domain classes (`Dataset` and `MeanShift`) have a *"uses-a"* workflow relationship rather than an *"is-a"* hierarchy, so we favor **composition over inheritance**.
   * **Standard Library Inheritance:** [`Dataset::loadFromCSV()`](./src/Dataset.cpp) leverages the C++ stream hierarchy (`std::ifstream` and `std::stringstream` inheriting from `std::istream`) and exception hierarchy (`std::invalid_argument` and `std::out_of_range` inheriting from `std::exception`).
3. **Compile-Time & Runtime Polymorphism:**
   * **Function Overloading via Default Arguments:** `MeanShift(double bandwidth = -1.0, double epsilon = 1e-3, int maxIterations = 300)` can be instantiated with 0, 1, 2, or 3 arguments; `loadFromCSV(const string& filename, bool hasHeader = true)` works with 1 or 2 arguments.
   * **Parametric Polymorphism (Templates):** Uses C++ STL templates (`std::vector<double>`, `std::vector<Point>`, `std::vector<Cluster>`, `std::min<size_t>`) to support arbitrary $D$-dimensional datasets.
   * **Runtime Polymorphism:** Uses base-class references in `std::getline(std::istream&, ...)` and `catch (const std::exception& e)`.

[⬆ Back to Top](#quick-navigation-hub)
