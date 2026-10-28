# Final Individual Project Report — Member 6 (Muzaki Desire Wadelo.F)

**Project Title:** Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
**Group:** Group 21  
**Member Name / Role:** Member 6 (Muzaki Desire Wadelo.F) — *Cluster Formation, Post-Convergence Mode Merging & Label Assignment (`MeanShift::fit` Stage 2)*  
**Primary Files Contributed:**  
- `include/meanshift/MeanShift.hpp` (Private state members `clusters_` and `labels_`)  
- `src/meanshift.cpp` (`MeanShift::fit()` Stage 2: Lines 98–124 — mode merging within `bandwidth_ / 2.0`, populating `clusters_` and `labels_`)

---

## 1. Introduction and Role Overview

In the Group 21 Mean Shift Clustering C++ Library project, my primary responsibility was designing and implementing **Stage 2 of `MeanShift::fit(const Data& data)`** in `src/meanshift.cpp` (Lines 98–124)—the **Cluster Formation, Mode Merging, and Point Label Assignment** stage—as well as maintaining the private member declarations (`clusters_` and `labels_`) in `include/meanshift/MeanShift.hpp`.

After Stage 1 of `fit()` (implemented by **Member 5 — Nazarene**) finishes shifting every point toward its local density peak, we are left with a vector `shiftedPoints` of $N$ converged coordinate vectors. However, because floating-point arithmetic is approximate and points stop moving as soon as their step size drops below `epsilon_` ($10^{-3}$), two points that climbed the **same** density peak will stop at slightly different coordinates—for example, $(1.4998, 1.3517)$ and $(1.5002, 1.3520)$. If we simply checked for exact equality (`==`), every point would form its own separate cluster! My contribution solves this fundamental numerical challenge by merging converged points within a spatial radius of `bandwidth_ / 2.0` into unified `Cluster` structures and recording each point's assigned cluster ID in `labels_`.

---

## 2. Algorithmic Logic of Post-Convergence Mode Merging

Given the converged positions $Y = \{y_0, y_1, \dots, y_{N-1}\}$ (`shiftedPoints`) and kernel radius $h$ (`bandwidth_`):

1. **Reset Output State:** Clear any previous clusters (`clusters_.clear()`) and initialize the label vector `labels_` of length $N$ to `-1` (unassigned).
2. **Iterative Radius-Based Grouping:** For each converged point $y_i$ ($i = 0, \dots, N-1$):
   - Compare $y_i$ against the `centroid` $c_k$ of every existing cluster $C_k \in \text{clusters\_}$ using Euclidean distance:
     $$\|y_i - c_k\|_2 < \frac{h}{2.0}$$
   - **Case A (Match Found):** If $y_i$ lies within $h / 2$ of cluster $C_k$'s centroid, $y_i$ converged to the same density mode. Append the point's index $i$ to `C_k.pointIndices` and record `labels_[i] = k`.
   - **Case B (New Cluster Discovered):** If $y_i$ is farther than $h / 2$ from all existing cluster centroids, create a new `Cluster` object with `centroid = y_i`, initialize its `pointIndices` with `{i}`, append it to `clusters_`, and set `labels_[i] = clusters_.size() - 1`.

### Why Use Threshold `bandwidth_ / 2.0`?
If two density modes are closer than half the kernel bandwidth ($h / 2$), their Gaussian kernels overlap so heavily that their density peaks form a single connected basin of attraction. Conversely, distinct clusters separated by more than $h$ will have converged modes separated by much more than $h / 2$. Thus, `bandwidth_ / 2.0` reliably merges floating-point jitter around the same peak without accidentally merging separate clusters.

---

## 3. Detailed C++ Implementation Walkthrough (`src/meanshift.cpp`, Lines 98–124)

```cpp
    // Merge converged points within bandwidth / 2 into clusters
    clusters_.clear();
    labels_.assign(data.size(), -1);

    for (size_t i = 0; i < shiftedPoints.size(); ++i) {
        int clusterIdx = 0;
        bool found = false;

        for (auto& cluster : clusters_) {
            if (euclideanDistance(shiftedPoints[i], cluster.centroid) < (bandwidth_ / 2.0)) {
                cluster.pointIndices.push_back(i);
                labels_[i] = clusterIdx;
                found = true;
                break;
            }
            clusterIdx++;
        }

        if (!found) {
            Cluster newCluster;
            newCluster.centroid = shiftedPoints[i];
            newCluster.pointIndices.push_back(i);
            clusters_.push_back(newCluster);
            labels_[i] = clusters_.size() - 1;
        }
    }
}
```

### Step-by-Step Technical Breakdown:
1. **Idempotent State Reset (Lines 99–100):**
   - `clusters_.clear();` removes any clusters left over from a previous `fit()` call on the same `MeanShift` object.
   - `labels_.assign(data.size(), -1);` resizes `labels_` to match the exact number of input points $N$ and initializes every label to `-1`.
2. **Outer Loop over Converged Points (Line 102):**
   - Uses `size_t i` to track both the current converged point `shiftedPoints[i]` and its original 0-based index `i` in `data`.
3. **Inner Search over Existing Clusters (Lines 106–114):**
   - Iterates through `clusters_` by mutable reference (`for (auto& cluster : clusters_)`) so we can directly append `i` to `cluster.pointIndices` when a match is found.
   - Calls `euclideanDistance(shiftedPoints[i], cluster.centroid) < (bandwidth_ / 2.0)`.
   - When true, pushes `i` into `cluster.pointIndices`, assigns `labels_[i] = clusterIdx`, sets `found = true`, and executes `break` to stop checking remaining clusters.
4. **Creating a New Cluster (Lines 116–122):**
   - When `!found` is true (including on the very first point `i == 0` when `clusters_` is empty), instantiates `Cluster newCluster`, sets `newCluster.centroid = shiftedPoints[i]`, adds `i` to `newCluster.pointIndices`, appends `newCluster` to `clusters_`, and assigns the new cluster's index `clusters_.size() - 1` to `labels_[i]`.

---

## 4. Object-Oriented Programming (OOP) Principles Applied

- **Encapsulation of Output State (`clusters_` and `labels_`):** Both `clusters_` (`vector<Cluster>`) and `labels_` (`vector<int>`) are declared `private` in `include/meanshift/MeanShift.hpp`. They can only be populated by `fit()` and read through `const` getters (`getClusters()` and `getLabels()`), guaranteeing that `labels_.size()` always equals `data.size()` and every cluster index in `labels_` is a valid index into `clusters_`.
- **Dual Output Representation:** Storing both `clusters_` (cluster-centric view: centroid + list of member indices) and `labels_` (point-centric view: cluster ID for each row $0 \dots N-1$) makes downstream C++ tasks effortless:
  - Printing cluster summaries or computing `evaluate()` iterates over `clusters_`.
  - Exporting labeled CSV rows (`Feature1,Feature2,Cluster_ID` in `example1.cpp` and `example3.cpp`) indexes directly into `labels_[k]` in $O(1)$ time.

---

## 5. Automated Unit Testing & Verification (`tests/test_meanshift.cpp`)

My cluster merging and label assignment logic is directly verified by `testMeanShiftClustering()` and `testGetLabels()` in `tests/test_meanshift.cpp` (Lines 18–28, 54–69):

```cpp
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

void testGetLabels() {
    meanshift::MeanShift ms(2.0, 1e-3, 100);
    meanshift::Data data = {
        {0.0, 0.0}, {0.1, 0.1}, {0.2, 0.2},
        {10.0, 10.0}, {10.1, 10.1}, {10.2, 10.2}
    };
    ms.fit(data);
    auto labels = ms.getLabels();
    if (labels.size() != 6 ||
        labels[0] != labels[1] || labels[1] != labels[2] ||
        labels[3] != labels[4] || labels[4] != labels[5] ||
        labels[0] == labels[3]) {
        throw runtime_error("testGetLabels failed");
    }
    cout << "testGetLabels passed." << endl;
}
```
- Collaborated with **Member 5 (Nazarene)** on the handoff of `shiftedPoints` inside `MeanShift::fit()` and with **Member 7 (Clive)**, whose `getClusters()`, `getLabels()`, and `evaluate()` methods read the `clusters_` and `labels_` containers populated by my code.

---

## 6. Git Collaboration and AI Use Declaration

- **Git Workflow:** Completed the private member declarations in `include/meanshift/MeanShift.hpp` and the cluster formation logic in `src/meanshift.cpp` via feature branches and GitHub Pull Requests.
- **AI Use Declaration:**
  - **Tools Used:** Gemini / ChatGPT
  - **Purpose:** Understanding post-convergence mode merging thresholds ($h / 2$) in Mean Shift clustering.
  - **Reason:** To ensure points converging to the same density peak within `epsilon_` tolerance were grouped into a single cluster rather than fragmented into multiple duplicate clusters.

---



