# Final Individual Project Report - Member 7 (Clive)

## Project Title
Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)

## Group Information
- **Group:** Group 21
- **Member:** Member 7 (Clive)
- **Role:** Cluster Output Accessors, Intra-Cluster Cohesion Evaluation Metric (`getClusters`, `getLabels`, and `evaluate`), and Interactive Tool (`example3.cpp`)

## Primary Files Contributed
- `include/meanshift/MeanShift.hpp` - declarations for `getClusters()`, `getLabels()`, and `evaluate()`.
- `src/meanshift.cpp` - implementations of `MeanShift::getClusters()`, `MeanShift::getLabels()`, and `MeanShift::evaluate()`.
- `examples/example3.cpp` - interactive C++ console clustering tool and result export functionality.

---

## 1. Introduction and Role Overview

The Group 21 project involves designing and implementing a Mean Shift clustering library in C++. Mean Shift is an unsupervised clustering technique that identifies dense regions in a dataset by repeatedly shifting candidate points toward regions of higher estimated data density. The project organizes this functionality into a reusable C++ library so that clustering operations and result retrieval can be used by different example programs and tests.

My main responsibility was the result retrieval and cluster evaluation layer of the `MeanShift` class, implemented in `src/meanshift.cpp`, together with the corresponding declarations in `include/meanshift/MeanShift.hpp`. I implemented the following methods:

1. `MeanShift::getClusters() const` provides read-only access to the discovered clusters. Each `Cluster` contains a centroid and the indices of the data points assigned to that cluster.
2. `MeanShift::getLabels() const` provides read-only access to the integer cluster label assigned to each data point.
3. `MeanShift::evaluate(const Data& data) const` calculates a mean intra-cluster distance score. The score measures the average Euclidean distance between each original data point and the centroid of its assigned cluster.

I also contributed to `examples/example3.cpp`, an interactive console program that allows a user to provide input and output CSV file paths, run clustering, inspect a summary of the results, view the cohesion score, and export the resulting cluster labels.

These components form the layer between the clustering algorithm and the library user. The clustering algorithm produces the results, the accessor methods make those results available, and the evaluation method provides a numerical measure that can help users inspect cluster compactness. The interactive example demonstrates how these features can be used together.

## 2. Project Context and the Purpose of My Contribution

A clustering library needs more than an algorithm that assigns points to groups. Users also need a way to retrieve the resulting groups, identify the cluster assigned to each point, assess the compactness of the result, and export the assignments for further analysis.

The three methods I implemented address these requirements.

### 2.1 Retrieving cluster structures

The `getClusters()` method exposes the cluster structures produced by the fitted model. A cluster contains its centroid and the indices of the original data points assigned to it. This allows a calling program to inspect the composition and location of each discovered cluster.

### 2.2 Retrieving point labels

The `getLabels()` method returns the cluster assignment for each point. Labels are useful when the caller needs to associate every input row with its discovered cluster, for example when printing a result table or writing cluster assignments to a CSV file.

### 2.3 Evaluating cluster compactness

The `evaluate()` method calculates the average Euclidean distance from each original data point to its assigned cluster centroid. This provides an internal evaluation metric that can be used to compare compactness across clustering results, provided the compared datasets and feature scales are suitable for comparison.

In unsupervised learning, ground-truth labels are often unavailable. A clustering result therefore cannot automatically be assessed using classification accuracy. An internal measure such as mean intra-cluster distance offers one way to inspect the geometry of the result without requiring known correct labels.

The score has an important limitation: a lower score indicates that points are closer to their assigned centroids, but it does not by itself establish that the clusters are meaningful or that the number of clusters is appropriate. A method that places every point in its own cluster, for example, could achieve a score of zero. The metric should therefore be interpreted alongside the clustering configuration, the dataset, and other evidence about the quality of the grouping.

## 3. Mathematical Formulation of the Evaluation Metric

Let the dataset be

`X = {x_0, x_1, ..., x_(N-1)}`

where `N` is the number of data points. Suppose the fitted model assigns these points to `K` clusters:

`C_0, C_1, ..., C_(K-1)`

Let `c_k` denote the centroid of cluster `C_k`, and let `pointIndices` identify the original data points assigned to that cluster.

The mean intra-cluster distance used by `evaluate()` is:

`Cohesion(X, C) = (1 / N) * sum over k = 0 to K-1 [sum over i in C_k.pointIndices of ||x_i - c_k||_2]`

Here, `||x_i - c_k||_2` is the Euclidean distance between original point `x_i` and the centroid of its assigned cluster.

For a point with `D` features, represented as `x_i = (x_i1, x_i2, ..., x_iD)`, and a centroid `c_k = (c_k1, c_k2, ..., c_kD)`, Euclidean distance is:

`||x_i - c_k||_2 = sqrt(sum over j = 1 to D of (x_ij - c_kj)^2)`

The implementation sums the distance for every point listed in every cluster and divides the total by `data.size()`, which is `N`.

### 3.1 Interpretation of the score

- **Non-negativity:** The score is greater than or equal to zero because Euclidean distances cannot be negative.
- **Lower values indicate greater compactness:** A lower score means that the original points are, on average, closer to their assigned cluster centroids.
- **A score of zero:** The score is zero when every point included in the calculation coincides exactly with its assigned centroid.
- **Dependence on feature scale:** Euclidean distance depends on the scale and units of the features. A feature with a much larger numerical range can dominate the score. Dataset preprocessing, such as standardization or normalization, can therefore affect the score and its interpretation.
- **No guarantee of overall clustering quality:** The score measures within-cluster distance. It does not directly measure separation between different clusters and cannot independently prove that the discovered clusters are the best or most useful grouping.

The metric is referred to in this report as the mean intra-cluster distance or cohesion score. The variable name `totalVariance` in the implementation is a name used for the accumulated distance total; mathematically, the implementation does not calculate statistical variance because it sums Euclidean distances rather than squared deviations.

## 4. C++ Implementation

The implementation in `src/meanshift.cpp` is:

```cpp
const vector<Cluster>& MeanShift::getClusters() const {
    return clusters_;
}

const vector<int>& MeanShift::getLabels() const {
    return labels_;
}

double MeanShift::evaluate(const Data& data) const {
    if (clusters_.empty() || data.empty()) return 0.0;

    double totalVariance = 0.0;
    for (const auto& cluster : clusters_) {
        double clusterVar = 0.0;
        for (size_t idx : cluster.pointIndices) {
            clusterVar += euclideanDistance(data[idx], cluster.centroid);
        }
        totalVariance += clusterVar;
    }
    return totalVariance / data.size();
}
```

The code above is reproduced from the implementation details supplied for this report. It assumes that the `vector`, `Cluster`, `Data`, and `euclideanDistance` names are available through the relevant declarations and namespace context in the project.

### 4.1 `MeanShift::getClusters() const`

```cpp
const vector<Cluster>& MeanShift::getClusters() const {
    return clusters_;
}
```

This method returns a constant reference to the model's internal `clusters_` vector.

Returning a reference avoids creating a copy of the vector each time the method is called. The method can therefore provide access without copying all cluster structures and their point-index vectors. The `const` reference prevents the caller from modifying the returned vector through this accessor.

The trailing `const` on the member function also indicates that calling the method does not modify the observable state of the `MeanShift` object through that function.

A caller can use the method to iterate through the clusters, inspect their centroids, and inspect the original point indices assigned to each cluster.

### 4.2 `MeanShift::getLabels() const`

```cpp
const vector<int>& MeanShift::getLabels() const {
    return labels_;
}
```

This method returns a constant reference to `labels_`, the vector containing the cluster assignment for each data point.

The labels provide a convenient point-by-point representation of the clustering result. A program can use the label at a given index to identify the cluster assigned to the corresponding input point, assuming the label vector and input dataset retain the same ordering.

As with `getClusters()`, returning a constant reference avoids copying the vector. The method is also marked `const`, communicating that retrieving the labels is an inspection operation.

### 4.3 `MeanShift::evaluate(const Data& data) const`

The evaluation method calculates the mean distance between original data points and the centroids of their assigned clusters.

#### Step 1: Check for empty input or missing clusters

```cpp
if (clusters_.empty() || data.empty()) return 0.0;
```

The guard checks two conditions:

- `clusters_.empty()` is true when the model currently has no stored clusters.
- `data.empty()` is true when the supplied dataset contains no points.

Returning `0.0` in these cases prevents the final division by `data.size()` from dividing by zero. It also gives the caller a defined numerical result for these edge cases.

However, a returned value of zero is also a valid minimum value for the metric. Consequently, callers should not interpret zero alone as proof that a fitted clustering result is perfect. They should also know whether the model was fitted and whether the input data was valid.

#### Step 2: Initialize the accumulated distance

```cpp
double totalVariance = 0.0;
```

This variable stores the sum of the distances accumulated across all clusters. Although it is named `totalVariance`, the calculation is a sum of Euclidean distances rather than a statistical variance.

#### Step 3: Iterate over each cluster

```cpp
for (const auto& cluster : clusters_) {
    double clusterVar = 0.0;
```

The outer loop visits each stored cluster. The expression `const auto&` binds a constant reference to the current cluster, avoiding an unnecessary copy of the cluster structure.

The local variable `clusterVar` accumulates the distances for the current cluster.

#### Step 4: Iterate over the original point indices

```cpp
for (size_t idx : cluster.pointIndices) {
    clusterVar += euclideanDistance(data[idx], cluster.centroid);
}
```

Each cluster stores indices identifying its assigned points in the original dataset. For each index, the implementation calculates the Euclidean distance between `data[idx]` and `cluster.centroid`, then adds the result to the current cluster's total.

The use of the original data is important. Mean Shift moves candidate points during its iterative search for high-density regions. Measuring the final shifted positions against their converged centres would tend to produce very small distances because those positions have already moved toward the centres. That would measure the remaining displacement of the shifted points rather than the compactness of the original observations assigned to the clusters.

Using `data[idx]` evaluates how far the original observation lies from its assigned centroid.

This calculation assumes that each stored index is valid for the supplied `data` vector and that the supplied dataset corresponds to the dataset used to fit the model. Passing a different dataset, changing its row ordering, or providing fewer rows could produce an invalid or misleading result. The supplied implementation does not include explicit index validation or dataset-identity checks.

#### Step 5: Add the cluster total to the overall total

```cpp
totalVariance += clusterVar;
```

After processing a cluster's member points, its accumulated distance is added to the overall sum.

#### Step 6: Divide by the number of data points

```cpp
return totalVariance / data.size();
```

The final result is the sum of all included point-to-centroid distances divided by the number of input data points.

If every point index appears in exactly one cluster's `pointIndices` list, this is the average distance per point. That interpretation depends on the cluster membership lists covering the dataset without duplicate assignments. If some points are omitted or appear more than once, the numerator will not represent one distance for every original data point even though the denominator remains `data.size()`.

## 5. Object-Oriented Programming Principles

### 5.1 Encapsulation

The methods provide controlled ways for calling code to inspect clustering results. The cluster and label storage remains part of the `MeanShift` object's internal state, while public accessors define how client code retrieves the results.

### 5.2 Const-correctness

All three methods are marked with a trailing `const`. This expresses that retrieving the clusters, retrieving the labels, and evaluating the result are intended as read-only operations on the model.

The getters also return constant references, so callers cannot modify the internal vectors through the references returned by these methods.

### 5.3 Avoiding unnecessary copies

The getters return references instead of returning the vectors by value. This avoids copying the full cluster or label vector on each call. The actual cost of accessing an element or iterating over the returned vector still depends on the operation performed by the caller.

### 5.4 Code reuse

The evaluation method calls the existing `euclideanDistance()` function rather than implementing a separate distance formula. This avoids duplicating the distance calculation and helps keep the library's distance behaviour consistent. The supplied project description identifies `euclideanDistance()` as a function implemented by Member 3, Simon.

### 5.5 Separation of responsibilities

The three methods have distinct responsibilities:

- `getClusters()` exposes the discovered cluster structures.
- `getLabels()` exposes the per-point cluster assignments.
- `evaluate()` calculates an internal compactness score.

This separation allows other project components to retrieve and use clustering results without needing to duplicate the evaluation calculation or access internal data directly.

## 6. Unit Testing and Verification

The supplied project details identify `testGetLabels()`, `testEvaluate()`, and `testEmptyData()` in `tests/test_meanshift.cpp` as tests related to this contribution.

### 6.1 Label retrieval test

The supplied `testGetLabels()` test uses six two-dimensional points arranged into two visibly separated groups:

```cpp
meanshift::Data data = {
    {0.0, 0.0}, {0.1, 0.1}, {0.2, 0.2},
    {10.0, 10.0}, {10.1, 10.1}, {10.2, 10.2}
};
```

The test fits a Mean Shift model with bandwidth `2.0`, convergence tolerance `1e-3`, and maximum iteration count `100`. It then retrieves the labels and checks that:

- The label vector contains six entries.
- The first three points share a label.
- The last three points share a label.
- The two groups have different labels.

If these conditions fail, the test throws a `runtime_error`. Otherwise, it prints `testGetLabels passed.`

This test checks that the label accessor returns a vector of the expected size and that the assignments match the intended grouping for this example dataset, assuming the clustering implementation produces the expected result.

### 6.2 Evaluation test

The supplied `testEvaluate()` uses the same six points and model configuration. It fits the model, calls `evaluate(data)`, and checks:

```cpp
if (score <= 0.0 || score > 0.5)
    throw runtime_error("testEvaluate failed");
```

This condition requires the score to be greater than zero and no greater than `0.5`.

For the test data, this checks that the evaluation result is within the expected range. It does not establish that every possible dataset, bandwidth, or clustering configuration will produce a score in that range. The expected interval is specific to this test case.

The condition also means that a score of exactly zero fails this test, even though zero is mathematically possible for the metric when every included point coincides with its assigned centroid. That is acceptable for this particular test if its purpose is to validate the expected non-zero result for the supplied dataset.

### 6.3 Empty-data test

The supplied project details identify `testEmptyData()` as another relevant test. The implementation guard in `evaluate()` returns `0.0` when the supplied data is empty or the model has no stored clusters. This behaviour is intended to avoid division by zero.

The exact body of `testEmptyData()` was not included in the supplied material, so its precise assertions cannot be confirmed in this report.

### 6.4 Scope of the tests

The described tests cover useful basic cases: retrieving labels, calculating a score for a simple dataset with two separated groups, and handling empty input. Additional tests could strengthen verification by checking mismatched datasets, invalid point indices, duplicate or missing cluster memberships, one-point datasets, and cases where the score should be exactly zero.

## 7. Interactive Console Tool: `examples/example3.cpp`

The interactive example demonstrates how the library's clustering and result-retrieval features can be used in a practical workflow. The supplied project description states that `example3.cpp` was co-developed with Member 8, Mark.

The tool follows this general workflow:

1. Prompt the user to enter the path of an input CSV file.
2. Prompt the user to enter the path of an output CSV file.
3. Load the dataset and access its data through `dataset.getData()`.
4. Run clustering by calling `ms.fit(dataset.getData())`.
5. Print a summary of the discovered clusters using `ms.getClusters()`.
6. Calculate and display the cohesion score using `ms.evaluate(dataset.getData())`.
7. Write each point alongside its cluster label to the specified output file, using the labels returned by `ms.getLabels()`.

This example connects the library's main operations into a user-facing workflow. It demonstrates how data can be loaded, clustered, inspected, evaluated, and exported without requiring a caller to reproduce the internal clustering logic.

The exact prompts, CSV schema, error handling, and output formatting depend on the source code in `examples/example3.cpp`. Those implementation details were not fully included in the supplied material and are therefore not asserted here.

## 8. Time Complexity and Resource Considerations

Let:

- `N` be the number of data points.
- `D` be the number of features per point.
- `K` be the number of clusters.

### 8.1 `getClusters()` and `getLabels()`

Each getter returns a reference to an existing vector. Returning the reference itself takes constant time, conventionally described as `O(1)`, and does not copy the vector. Any subsequent work performed by the caller while iterating through the returned data is separate from the cost of the getter.

### 8.2 `evaluate()`

The outer loop visits the clusters, and the inner loop visits the point indices stored in each cluster. If each of the `N` points appears exactly once across all cluster membership lists, the inner loop performs `N` distance calculations in total.

Each Euclidean distance calculation across `D` features takes `O(D)` time. Therefore, the overall time complexity is:

`O(N * D)`

The work needed to visit the cluster structures adds `O(K)` loop overhead, so a more explicit expression is `O(K + N * D)`. In the usual case where each cluster contains at least one point, `K` is at most `N`, and the expression is commonly simplified to `O(N * D)` for positive feature dimension.

The evaluation method uses a small number of scalar accumulators and does not allocate a new distance matrix. Its additional working memory is `O(1)`, excluding the storage already occupied by the model's clusters and labels.

## 9. Limitations and Considerations

The implementation is lightweight and suitable for a basic internal compactness measure, but its result must be interpreted carefully.

### 9.1 The metric does not measure cluster separation

The score measures the distances of points to their own centroids. It does not measure the distances between different cluster centroids or whether clusters overlap. A low score should therefore not be treated as a complete measure of clustering quality.

### 9.2 Scores depend on feature scaling

Euclidean distance is affected by the numerical scales of the features. Comparing scores before and after standardization or normalization can help examine the effect of preprocessing, but the values are then measured in different transformed feature spaces. The interpretation should take that change into account.

### 9.3 The model and supplied dataset must correspond

The method uses point indices stored in `clusters_` to index into the `data` argument. Correct evaluation requires the supplied dataset to match the data used when the model was fitted, including row order and compatible dimensions. The supplied implementation does not check this correspondence.

### 9.4 Empty-state return value

The method returns `0.0` if no clusters are stored or if the supplied dataset is empty. This prevents division by zero, but it makes the numerical return value ambiguous: zero can represent an empty or unfitted state, or a valid zero-distance result. A future API design could make this distinction explicit, for example through a status result or a documented exception policy.

### 9.5 Membership consistency

The formula assumes that the cluster membership lists account for each input point exactly once. The supplied implementation does not independently verify that all points are assigned, that no point appears in multiple clusters, or that every stored index is valid.

These limitations do not remove the usefulness of the metric. They define the assumptions under which the result can be interpreted correctly and identify areas where validation could be expanded.

## 10. Git Collaboration and AI Use Declaration

### 10.1 Git workflow

According to the supplied project account, the methods were implemented on a dedicated feature branch and merged into the main branch through a GitHub pull request. This workflow separates the contribution from the main development line and provides a reviewable record of the change.

### 10.2 Collaboration

The result retrieval and evaluation functions integrate with other parts of the shared library. In particular, the implementation reuses the `euclideanDistance()` function identified as Member 3 Simon's contribution. The interactive example was co-developed with Member 8, Mark, according to the supplied project information.

### 10.3 AI use declaration

Tools used: Gemini and ChatGPT.

Purpose: Reviewing internal clustering evaluation metrics, particularly intra-cluster cohesion and the Silhouette score, for a lightweight C++ library.

Reason: To support the implementation of a compactness metric with `O(N * D)` time complexity, avoiding the `O(N^2)` pairwise-distance overhead associated with straightforward implementations of some other evaluation approaches.

This declaration describes the AI use reported in the supplied material. It does not imply that the implementation or its test results were independently verified by an AI tool.

## 12. Conclusion

My contribution to the Group 21 Mean Shift clustering library focuses on retrieving and evaluating clustering results and demonstrating their use through an interactive C++ program. The `getClusters()` and `getLabels()` methods provide efficient, read-only access to the results stored by the model. The `evaluate()` method calculates the mean Euclidean distance between original data points and their assigned cluster centroids, offering a lightweight measure of within-cluster compactness.

The implementation reuses the library's existing Euclidean distance function, avoids unnecessary copies in the getter methods, handles empty input before division, and has a stated linear time complexity in the number of points and features under the assumption that each point is assigned exactly once. The described unit tests cover label retrieval, a basic cohesion-score case, and empty-data handling. The interactive example brings these capabilities together by loading data, running clustering, displaying a summary and score, and exporting labels.

The main limitation is that mean intra-cluster distance measures compactness alone. It should be interpreted alongside the dataset, feature scaling, bandwidth configuration, and other evidence about the clustering result. Subject to these assumptions, the implemented methods provide a useful result-access and evaluation layer for the project library.

---

## References

The following references provide background for the concepts discussed in this report. The implementation details and personal contribution statements are based on the project information supplied for this report; the external references provide general algorithmic and evaluation context.

1. Comaniciu, D., and Meer, P. (2002). "Mean Shift: A Robust Approach Toward Feature Space Analysis." *IEEE Transactions on Pattern Analysis and Machine Intelligence*, 24(5), 603-619. https://doi.org/10.1109/34.1000236

2. Fukunaga, K., and Hostetler, L. D. (1975). "The Estimation of the Gradient of a Density Function, with Applications in Pattern Recognition." *IEEE Transactions on Information Theory*, 21(1), 32-40. https://doi.org/10.1109/TIT.1975.1055330

3. Rousseeuw, P. J. (1987). "Silhouettes: A Graphical Aid to the Interpretation and Validation of Cluster Analysis." *Journal of Computational and Applied Mathematics*, 20, 53-65. https://doi.org/10.1016/0377-0427(87)90125-7

4. Lloyd, S. P. (1982). "Least Squares Quantization in PCM." *IEEE Transactions on Information Theory*, 28(2), 129-137. https://doi.org/10.1109/TIT.1982.1056489

5. Group 21 project materials supplied by the author: project title and role description; declarations and implementations for `getClusters()`, `getLabels()`, and `evaluate()`; descriptions of `testGetLabels()`, `testEvaluate()`, and `testEmptyData()`; the `example3.cpp` workflow; Git collaboration notes; and AI use declaration. Repository paths and test details in this report reflect the supplied description and were not independently inspected for this document.
