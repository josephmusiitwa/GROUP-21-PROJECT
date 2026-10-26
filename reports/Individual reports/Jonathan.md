# Final Individual Project Report — SSERUNKUMA JONATHAN G.
**Project Title:** Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
**Group:** Group 21  
**Member Name / Role:** SSERUNKUMA JONATHAN G. — *Automatic Bandwidth Estimation & Local Mean Shift Vector Update (`estimateBandwidth` & `shiftPoint`)*  
**Primary Files Contributed:**  
- `src/meanshift.cpp` (`MeanShift::estimateBandwidth()`, `MeanShift::shiftPoint()`)  
- `examples/example2.cpp` (In-memory synthetic 2D clustering demonstration)

---

## 1. Introduction and Role Overview

In the Group 21 Mean Shift Clustering C++ Library project, i implemented the two core **Kernel Density Estimation (KDE) mechanics** of the Mean Shift algorithm in `src/meanshift.cpp`:
1. **`MeanShift::estimateBandwidth(const Data& data)`:** A data-driven heuristic that automatically computes a suitable kernel radius (`bandwidth_`) when the user does not specify one (`bandwidth <= 0`).
2. **`MeanShift::shiftPoint(const Point& p, const Data& data) const`:** The fundamental mathematical update rule of the Mean Shift algorithm, which computes the kernel-weighted center of mass around a point $p$ and shifts $p$ toward the local maximum of data density.

Additionally, I co-developed `examples/example2.cpp` with ** MARK JORDAN ** to demonstrate running `MeanShift` directly on an in-memory C++ `meanshift::Data` dataset with three distinct clusters.

---

## 2. Mathematical Foundations of My Implementation

### 2.1 Data-Driven Bandwidth Estimation (`estimateBandwidth`)
Unlike K-Means—which requires the user to guess the number of clusters $K$ in advance—Mean Shift automatically discovers the number of clusters, governed only by the kernel radius $h$ (`bandwidth_`). However, if $h$ is unknown for a new dataset, it can be estimated from the average pairwise Euclidean distance among a sample of $M = \min(500, N)$ points:

$$\bar{d} = \frac{2}{M(M - 1)} \sum_{0 \le i < j < M} \|x_i - x_j\|_2, \qquad h = 0.5 \times \bar{d}$$

Scaling the average pairwise distance by $0.5$ sets the search radius large enough to capture local neighborhood structure without spanning across separate clusters.

### 2.2 The Mean Shift Vector Update (`shiftPoint`)
Given a current point position $p \in \mathbb{R}^D$ and the dataset $X = \{x_0, x_1, \dots, x_{N-1}\}$, the updated position $m(p)$ is the **Gaussian kernel-weighted average** of all points in $X$:

$$m(p) = \frac{\sum_{j=0}^{N-1} K(\|p - x_j\|_2) \cdot x_j}{\sum_{j=0}^{N-1} K(\|p - x_j\|_2)}$$

where $K(d) = \exp\left(-\frac{d^2}{2h^2}\right)$ is the Gaussian kernel weight returned by `gaussianKernel(dist)`. Points $x_j$ very close to $p$ have weights near $1.0$ and pull $p$ strongly toward their coordinates; points far from $p$ have weights near $0.0$ and exert negligible pull. Thus, moving $p \leftarrow m(p)$ performs **gradient ascent** on the underlying kernel density surface.

---

## 3. Detailed C++ Implementation Walkthrough

### 3.1 Implementation of `MeanShift::estimateBandwidth()` (`src/meanshift.cpp`, Lines 24–39)

```cpp
void MeanShift::estimateBandwidth(const Data& data) {
    if (data.empty()) return;

    // Sample up to 500 points to avoid O(N^2) pairwise distance cost on large datasets
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
```

**Key Engineering & Algorithmic Decisions:**
- **Subsampling Optimization (`min<size_t>(500, data.size())`):** Computing all pairwise distances across $N$ points requires $\frac{N(N-1)}{2}$ evaluations ($O(N^2)$). For $N = 50,000$, that would be $1.25 \times 10^9$ distance computations just to initialize `bandwidth_`. By capping `numSamples` at $500$ using the C++ STL template `std::min<size_t>`, the loop performs at most $\frac{500 \times 499}{2} = 124,750$ distance checks—completing in a few milliseconds regardless of dataset size.
- **Upper-Triangular Pair Loop (`j = i + 1`):** Because Euclidean distance is symmetric ($d(x_i, x_j) = d(x_j, x_i)$) and $d(x_i, x_i) = 0$, starting the inner loop at `j = i + 1` avoids redundant self-distances and cuts the work in half.
- **Degenerate Dataset Safeguards:**
  - If the dataset has only 1 point (`data.size() == 1`), the inner loop does not execute (`count == 0`). The ternary expression `(count > 0) ? (totalDist / count) * 0.5 : 1.0` prevents division by zero (`0.0 / 0`) and defaults `bandwidth_` to `1.0`.
  - If all sampled points are identical (`totalDist == 0`), `if (bandwidth_ <= 0) bandwidth_ = 1.0;` ensures `bandwidth_` remains strictly positive so `gaussianKernel()` never divides by zero.

### 3.2 Implementation of `MeanShift::shiftPoint()` (`src/meanshift.cpp`, Lines 41–63)

```cpp
Point MeanShift::shiftPoint(const Point& p, const Data& data) const {
    Point shiftedP = p;
    Point numerator(p.size(), 0.0);
    double denominator = 0.0;

    for (const auto& other_p : data) {
        double dist = euclideanDistance(p, other_p);
        double weight = gaussianKernel(dist);

        for (size_t i = 0; i < p.size(); ++i) {
            numerator[i] += other_p[i] * weight;
        }
        denominator += weight;
    }

    if (denominator > 0) {
        for (size_t i = 0; i < p.size(); ++i) {
            shiftedP[i] = numerator[i] / denominator;
        }
    }

    return shiftedP;
}
```

**Step-by-Step Execution:**
1. Initializes `shiftedP = p` (fallback in case `denominator == 0`), a $D$-dimensional accumulator vector `numerator(p.size(), 0.0)`, and scalar `denominator = 0.0`.
2. Loops over every reference point `other_p` in `data` by `const auto&` (avoiding vector copies).
3. Computes the distance `dist = euclideanDistance(p, other_p)` and weight `weight = gaussianKernel(dist)`.
4. Accumulates `other_p[i] * weight` into each coordinate `i` of `numerator` and adds `weight` to `denominator`.
5. Checks `if (denominator > 0)` to guard against underflow before dividing each coordinate `numerator[i] / denominator`.

---

## 4. Object-Oriented Programming (OOP) Concepts Applied

- **Encapsulation & `const`-Correctness:** `shiftPoint()` is marked `const` because shifting a single candidate point only reads the algorithm's `bandwidth_` (via `gaussianKernel()`) and does not modify any internal state of `MeanShift`. Conversely, `estimateBandwidth()` is non-`const` because its explicit purpose is to update the private `bandwidth_` member variable.
- **Compile-Time Parametric Polymorphism (Templates):** Used `std::min<size_t>(500, data.size())` and `std::vector<double>` (`Point`) so `shiftPoint()` works identically on 1D, 2D, 3D, or 100D points without changing a single line of code.

---

## 5. Automated Unit Testing & Verification (`tests/test_meanshift.cpp`)

Both `shiftPoint()` and `estimateBandwidth()` are directly verified by dedicated unit tests in `tests/test_meanshift.cpp` (Lines 30–52):

```cpp
void testShiftPoint() {
    meanshift::MeanShift ms(2.0, 1e-3, 100);
    meanshift::Data data = {
        {0.0, 0.0}, {2.0, 2.0}
    };
    meanshift::Point mid = {1.0, 1.0};
    meanshift::Point shifted = ms.shiftPoint(mid, data);
    if (abs(shifted[0] - 1.0) > 1e-6 || abs(shifted[1] - 1.0) > 1e-6) {
        throw runtime_error("testShiftPoint failed");
    }
    cout << "testShiftPoint passed." << endl;
}

void testEstimateBandwidth() {
    meanshift::MeanShift ms;
    meanshift::Data data = {
        {0.0, 0.0}, {0.1, 0.1}, {0.2, 0.2},
        {10.0, 10.0}, {10.1, 10.1}, {10.2, 10.2}
    };
    ms.estimateBandwidth(data);
    ms.fit(data);
    if (ms.getClusters().size() != 2) throw runtime_error("testEstimateBandwidth failed");
    cout << "testEstimateBandwidth passed." << endl;
}
```
- **Symmetry Test in `testShiftPoint()`:** Places two points symmetrically at `{0.0, 0.0}` and `{2.0, 2.0}` and shifts their midpoint `{1.0, 1.0}`. Because both points are at equal distance $\sqrt{2}$ from `{1.0, 1.0}`, they receive identical Gaussian weights, verifying that the weighted average remains exactly `{1.0, 1.0}` within $10^{-6}$.
- **Auto-Bandwidth Test in `testEstimateBandwidth()`:** Instantiates `MeanShift ms;` with default `bandwidth = -1.0`, runs `ms.estimateBandwidth(data)` and `ms.fit(data)`, and verifies that it discovers exactly `2` clusters.
- **In-Memory Example (`examples/example2.cpp`):** Verifies that 9 points in 3 groups converge to `(1.50006, 1.35186)`, `(8.47972, 8.33061)`, and `(1.52013, 8.30793)` with evaluation score `0.622962`.

---

## 6. Git Collaboration and AI Use Declaration

- **Git Workflow:** Implemented `estimateBandwidth()`, `shiftPoint()`, and `example2.cpp` on dedicated branches and merged via Pull Requests into `main`.
- **AI Use Declaration:**
  - **Tools Used:** Gemini / ChatGPT
  - **Purpose:** Understanding how the Nadaraya–Watson kernel regression update relates to density gradient ascent in `shiftPoint()`.
  - **Reason:** To ensure our C++ implementation of the numerator/denominator weighted average was dimension-agnostic and numerically safe.

---

## 7. Key Presentation & Viva Questions (Self-Preparation)

- **Q1: Why does `shiftPoint()` loop over the original `data` instead of the moving `shiftedPoints`?**  
  *Answer:* In standard (non-blurring) Mean Shift, the underlying data distribution $X$ defines the fixed probability density surface, and each query point climbs that fixed surface independently.
- **Q2: Why do we check `if (denominator > 0)` at the end of `shiftPoint()`?**  
  *Answer:* If `data` is empty or if numerical underflow causes all Gaussian weights `exp(...)` to round to `0.0` for an extreme outlier with a tiny bandwidth, dividing by `denominator == 0.0` would produce `NaN`. The check safely returns the unshifted point `p`.
