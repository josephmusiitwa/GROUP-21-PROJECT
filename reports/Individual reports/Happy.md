# Final Individual Project Report —  (Atuzarirwe Happy)

**Project Title:** Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
**Group:** Group 21  
**Member Name / Role:** (Atuzarirwe Happy) — *Feature Scaling & Data Preprocessing Module (`standardize` & `normalize`)*  
**Primary Files Contributed:**  
- `include/meanshift/Dataset.hpp` (Declarations for `standardize()` and `normalize()`)  
- `src/Dataset.cpp` (Z-score scaling transformation in `Dataset::standardize()` and full implementation of `Dataset::normalize()`)  
- `data/input/finance.csv`

---

## 1. Introduction and Role Overview

In the Group 21 Mean Shift Clustering C++ Library project, my primary responsibility was designing and implementing the **Data Preprocessing and Feature Scaling Module** within the `Dataset` class (`Dataset::standardize()` and `Dataset::normalize()` in `src/Dataset.cpp`), as well as preparing the `data/input/finance.csv` dataset.

Mean Shift is a **distance-based, non-parametric clustering algorithm** that uses Euclidean distance $\|x_i - x_j\|_2$ and a radially symmetric Gaussian kernel with a single scalar `bandwidth_` ($h$). In real-world datasets (such as financial or academic records), different columns often have drastically different units and numerical scales—for example, Feature 1 might range from $1$ to $3$ while Feature 2 ranges from $10$ to $300$. Without feature scaling, the feature with the larger numerical range completely dominates the Euclidean distance calculation, distorting the neighborhood kernel weights. My contribution ensures that C++ users can scale all features onto a comparable numerical range before running `MeanShift::fit()`.

---

## 2. Mathematical Foundations of My Implementation

I implemented two standard statistical feature-scaling techniques in `src/Dataset.cpp`:

### 2.1 Z-Score Standardization (`Dataset::standardize()`)
Standardization transforms each feature column $j \in \{0, \dots, D-1\}$ across all $N$ points so that the rescaled feature has a **mean of $0.0$** and a **standard deviation of $1.0$**:

1. **Column Mean ($\mu_j$) and Population Standard Deviation ($\sigma_j$)** *(computed in Passes 1–2 with Member 1 — Kizza)*:
   $$\mu_j = \frac{1}{N} \sum_{i=0}^{N-1} x_{i,j}, \qquad \sigma_j = \sqrt{\frac{1}{N} \sum_{i=0}^{N-1} (x_{i,j} - \mu_j)^2}$$
2. **In-Place Z-Score Transformation (Pass 3):**
   $$x'_{i,j} = \frac{x_{i,j} - \mu_j}{\sigma_j} \quad (\text{when } \sigma_j \neq 0)$$

### 2.2 Min-Max Normalization (`Dataset::normalize()`)
Min-Max normalization linearly rescales each feature column $j$ so that all values lie strictly within the closed interval **$[0.0, 1.0]$**:

1. **Column Minimum and Maximum:**
   $$x_{\min, j} = \min_{0 \le i < N} x_{i,j}, \qquad x_{\max, j} = \max_{0 \le i < N} x_{i,j}$$
2. **In-Place Min-Max Transformation:**
   $$x'_{i,j} = \frac{x_{i,j} - x_{\min, j}}{x_{\max, j} - x_{\min, j}} \quad (\text{when } x_{\max, j} - x_{\min, j} \neq 0)$$

---

## 3. Detailed C++ Implementation Walkthrough

### 3.1 Implementation of `Dataset::standardize()` (`src/Dataset.cpp`, Lines 48–80)

```cpp
void Dataset::standardize() {
    if (data_.empty()) return;

    size_t numFeatures = data_[0].size();
    Point means(numFeatures, 0.0);
    Point stdDevs(numFeatures, 0.0);

    for (const auto& point : data_) {
        for (size_t i = 0; i < numFeatures; ++i) {
            means[i] += point[i];
        }
    }
    for (size_t i = 0; i < numFeatures; ++i) {
        means[i] /= data_.size();
    }

    for (const auto& point : data_) {
        for (size_t i = 0; i < numFeatures; ++i) {
            stdDevs[i] += pow(point[i] - means[i], 2);
        }
    }
    for (size_t i = 0; i < numFeatures; ++i) {
        stdDevs[i] = sqrt(stdDevs[i] / data_.size());
    }

    for (auto& point : data_) {
        for (size_t i = 0; i < numFeatures; ++i) {
            if (stdDevs[i] != 0) {
                point[i] = (point[i] - means[i]) / stdDevs[i];
            }
        }
    }
}
```

- **Three-Pass Algorithm:**
  - **Pass 1 (Lines 55–62):** Iterates over `data_` by `const auto&` (avoiding copying points) to accumulate the sum of each column in `means[i]`, then divides by `data_.size()`.
  - **Pass 2 (Lines 64–71):** Computes the squared deviations `pow(point[i] - means[i], 2)` for each feature and takes the square root `sqrt(stdDevs[i] / data_.size())`.
  - **Pass 3 (Lines 73–79):** Iterates over `data_` using a mutable reference (`for (auto& point : data_)`) and checks `if (stdDevs[i] != 0)` before updating each coordinate `point[i] = (point[i] - means[i]) / stdDevs[i]` **in-place** without allocating a duplicate dataset in memory.

### 3.2 Implementation of `Dataset::normalize()` (`src/Dataset.cpp`, Lines 82–103)

```cpp
void Dataset::normalize() {
    if (data_.empty()) return;

    size_t numFeatures = data_[0].size();
    Point mins = data_[0];
    Point maxs = data_[0];

    for (const auto& point : data_) {
        for (size_t i = 0; i < numFeatures; ++i) {
            if (point[i] < mins[i]) mins[i] = point[i];
            if (point[i] > maxs[i]) maxs[i] = point[i];
        }
    }

    for (auto& point : data_) {
        for (size_t i = 0; i < numFeatures; ++i) {
            if (maxs[i] - mins[i] != 0) {
                point[i] = (point[i] - mins[i]) / (maxs[i] - mins[i]);
            }
        }
    }
}
```

- **Two-Pass Algorithm:**
  - **Initialization (Lines 86–87):** Initializes `mins` and `maxs` to the first point `data_[0]` rather than `0.0`, which guarantees correctness even when all numbers in a column are negative or all numbers are large positive values (e.g., `[10.0, 20.0, 30.0]`).
  - **Pass 1 (Lines 89–94):** Scans every point to record the true minimum and maximum of each feature column.
  - **Pass 2 (Lines 96–102):** Rescales each feature in-place into $[0, 1]$ whenever `maxs[i] - mins[i] != 0`.

---

## 4. Object-Oriented Design, Edge Cases, and Defensive Programming

1. **Encapsulation & State Integrity:** Both `standardize()` and `normalize()` are `public` member functions that mutate the `private` `data_` member in-place. Because `data_` cannot be modified directly from outside `Dataset`, all preprocessing is guaranteed to follow well-defined mathematical transformations.
2. **Empty Dataset Guard (`if (data_.empty()) return;`):** If a user calls `standardize()` or `normalize()` before calling `loadFromCSV()` (or after loading an empty file like `data/input/text_data.csv`), accessing `data_[0].size()` would cause an out-of-bounds memory access. The early return prevents this crash.
3. **Zero-Variance / Constant Column Protection:**
   - If every row in column `i` has the exact same value (e.g., `[5.0, 5.0, 5.0]`), then `stdDevs[i] == 0.0` and `maxs[i] - mins[i] == 0.0`.
   - Dividing by zero in C++ floating-point arithmetic produces `NaN` or `Inf`, which would corrupt all subsequent Euclidean distance and Gaussian kernel calculations in `MeanShift`.
   - By checking `if (stdDevs[i] != 0)` and `if (maxs[i] - mins[i] != 0)`, constant columns are safely left unchanged rather than generating `NaN`.
4. **Complexity Analysis:** Both methods run in **$O(N \cdot D)$ time complexity** and **$O(D)$ auxiliary space complexity** (requiring only two temporary vectors of length $D$: `means`/`stdDevs` or `mins`/`maxs`).

---

## 5. Automated Unit Testing & Verification (`tests/test_meanshift.cpp`)

Both of my feature-scaling methods are verified by dedicated automated unit tests in `tests/test_meanshift.cpp` (Lines 112–154):

```cpp
void testStandardize() {
    const string tempFile = "test_temp_std.csv";
    ofstream out(tempFile);
    out << "Feature1,Feature2\n";
    out << "1.0,10.0\n";
    out << "3.0,30.0\n";
    out.close();

    meanshift::Dataset dataset;
    dataset.loadFromCSV(tempFile, true);
    remove(tempFile.c_str());

    dataset.standardize();
    const auto& data = dataset.getData();
    if (data.size() != 2 ||
        abs(data[0][0] - (-1.0)) > 1e-6 || abs(data[1][0] - 1.0) > 1e-6 ||
        abs(data[0][1] - (-1.0)) > 1e-6 || abs(data[1][1] - 1.0) > 1e-6) {
        throw runtime_error("testStandardize failed");
    }
    cout << "testStandardize passed." << endl;
}

void testNormalize() {
    const string tempFile = "test_temp_norm.csv";
    ofstream out(tempFile);
    out << "Feature1,Feature2\n";
    out << "10.0,50.0\n";
    out << "20.0,100.0\n";
    out << "30.0,150.0\n";
    out.close();

    meanshift::Dataset dataset;
    dataset.loadFromCSV(tempFile, true);
    remove(tempFile.c_str());

    dataset.normalize();
    const auto& data = dataset.getData();
    if (data.size() != 3 ||
        abs(data[0][0] - 0.0) > 1e-6 || abs(data[1][0] - 0.5) > 1e-6 || abs(data[2][0] - 1.0) > 1e-6 ||
        abs(data[0][1] - 0.0) > 1e-6 || abs(data[1][1] - 0.5) > 1e-6 || abs(data[2][1] - 1.0) > 1e-6) {
        throw runtime_error("testNormalize failed");
    }
    cout << "testNormalize passed." << endl;
}
```
- **In `testStandardize()`:** Column 0 (`{1.0, 3.0}`, $\mu=2.0, \sigma=1.0$) and Column 1 (`{10.0, 30.0}`, $\mu=20.0, \sigma=10.0$) have a $10\times$ scale difference, yet both standardize cleanly to `{-1.0, +1.0}` within $10^{-6}$ tolerance.
- **In `testNormalize()`:** Column 0 (`{10.0, 20.0, 30.0}`) and Column 1 (`{50.0, 100.0, 150.0}`) both rescale cleanly to `{0.0, 0.5, 1.0}` within $10^{-6}$ tolerance.

---

## 6. Challenges Encountered and How They Were Addressed

- **Initializing `mins` and `maxs` Properly:** A common mistake in Min-Max normalization is initializing `mins` to `0.0`. In `testNormalize()`, Column 0 has values `{10.0, 20.0, 30.0}` and Column 1 has `{50.0, 100.0, 150.0}`. Initializing `mins` to `0.0` would have produced an incorrect minimum of `0.0` instead of `10.0` and `50.0`. Initializing both `mins` and `maxs` to `data_[0]` solved this cleanly.
- **Avoiding Unnecessary Memory Copies:** Using `const auto&` during the read passes and `auto&` during the write pass allowed in-place modification of `data_` without doubling memory usage.

---

## 7. Git Collaboration and AI Use Declaration

- **Git Workflow:** Developed the declarations in `Dataset.hpp` and implementations in `Dataset.cpp` on separate branches and merged them via GitHub Pull Requests.
- **AI Use Declaration:**
  - **Tools Used:** Gemini / ChatGPT
  - **Purpose:** Reviewing the mathematical distinction between Z-score standardization and Min-Max normalization in distance-based clustering algorithms.
  - **Reason:** To ensure proper edge-case handling for zero-variance columns and clear documentation in the weekly reports.

---
