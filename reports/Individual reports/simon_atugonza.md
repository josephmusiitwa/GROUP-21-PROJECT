# Individual Project Report — Member 3 (Simon)

**Project:** Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
**Group:** Group 21  
**Role:** `MeanShift` class architecture, Euclidean distance, Gaussian kernel, and command-line CSV workflow  
**Main files contributed:**
- `include/meanshift/MeanShift.hpp` — `Cluster` structure and `MeanShift` class interface
- `src/meanshift.cpp` — constructor, `euclideanDistance()`, and `gaussianKernel()`
- `examples/example1.cpp` — command-line clustering workflow for CSV data

---

## 1. Introduction and Responsibilities

The Group 21 project developed a C++ library for Mean Shift clustering. My contribution focused on defining the `Cluster` data structure and `MeanShift` class interface, implementing two mathematical operations used by the algorithm, and developing a command-line example that loads data from a CSV file, performs clustering, displays results, and exports cluster labels.

The two mathematical operations I implemented were Euclidean distance and the Gaussian kernel. Euclidean distance measures how far apart two points are in a multidimensional space. The Gaussian kernel converts a distance into a weight, allowing nearby points to have a stronger influence than distant points. These operations support other parts of the library, including bandwidth estimation, point shifting, fitting, and evaluation.

## 2. Class Architecture and Object-Oriented Design

### 2.1 The `Cluster` structure

In `include/meanshift/MeanShift.hpp`, I defined a `Cluster` structure inside the `meanshift` namespace:

```cpp
struct Cluster {
    Point centroid;
    vector<size_t> pointIndices;
};
```

The structure stores two pieces of information:

- **`centroid` (`Point`):** The coordinates of the cluster's converged density mode.
- **`pointIndices` (`std::vector<size_t>`):** The zero-based indices of the original data points assigned to the cluster.

Storing indices instead of copying every point into each cluster avoids duplicating point data and preserves the relationship between each clustered point and its original row in the dataset.

### 2.2 The `MeanShift` class

The class interface declares the constructor, the Euclidean-distance function, the Gaussian-kernel function, and private state:

```cpp
class MeanShift {
public:
    MeanShift(double bandwidth = -1.0,
              double epsilon = 1e-3,
              int maxIterations = 300);

    static double euclideanDistance(const Point& a, const Point& b);
    double gaussianKernel(double distance) const;

private:
    double bandwidth_;
    double epsilon_;
    int maxIterations_;

    vector<Cluster> clusters_;
    vector<int> labels_;
};
```

The three parameters control the algorithm:

- **`bandwidth_`:** Controls the scale of the Gaussian kernel and the neighborhood influence.
- **`epsilon_`:** Represents the convergence tolerance.
- **`maxIterations_`:** Sets the maximum number of iterations.

The class keeps these parameters and the result containers private. This encapsulation prevents external code from directly changing the internal state and helps keep cluster information and labels managed through the class interface.

The constructor uses a member-initializer list:

```cpp
MeanShift::MeanShift(double bandwidth,
                     double epsilon,
                     int maxIterations)
    : bandwidth_(bandwidth),
      epsilon_(epsilon),
      maxIterations_(maxIterations) {}
```

Default arguments declared in the header allow several initialization styles:

```cpp
MeanShift ms;
MeanShift ms(2.0);
MeanShift ms(2.0, 1e-4);
MeanShift ms(2.0, 1e-4, 500);
```

In the first form, the bandwidth is set to `-1.0`, which the project uses to indicate automatic bandwidth estimation. The other arguments use their declared defaults unless custom values are supplied.

## 3. Mathematical Operations

### 3.1 Euclidean distance

For two points in \(D\)-dimensional space,

\[
a=(a_0,a_1,\ldots,a_{D-1}), \qquad
b=(b_0,b_1,\ldots,b_{D-1}),
\]

the Euclidean distance is

\[
d(a,b)=\|a-b\|_2
=\sqrt{\sum_{i=0}^{D-1}(a_i-b_i)^2}.
\]

The implementation in `src/meanshift.cpp` follows this formula:

```cpp
double MeanShift::euclideanDistance(const Point& a, const Point& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(sum);
}
```

For each coordinate, the function calculates the difference between the two points, squares it, and adds it to a running total. It then returns the square root of that total.

The function is declared `static` because it depends only on its two input points; it does not need to access the state of a `MeanShift` object. It can therefore be called directly, for example:

```cpp
double distance = MeanShift::euclideanDistance(p1, p2);
```

The implementation uses multiplication to square each difference rather than calling `std::pow()` for every coordinate. This is a straightforward approach for a calculation that may be repeated many times during clustering.

**Implementation consideration:** The function loops over the coordinates in `a` and accesses the corresponding coordinates in `b`. The input points should therefore have the same dimensionality. A robust implementation should validate this precondition or document it clearly.

### 3.2 Gaussian kernel

The Gaussian kernel assigns a weight based on the distance between points. The implemented formula is

\[
K(d)=\exp\left(-\frac{1}{2}\left(\frac{d}{h}\right)^2\right)
=\exp\left(-\frac{d^2}{2h^2}\right),
\]

where \(d\) is the distance and \(h\) is the bandwidth.

The implementation is:

```cpp
double MeanShift::gaussianKernel(double distance) const {
    return exp(-0.5 * (distance * distance)
               / (bandwidth_ * bandwidth_));
}
```

The function behaves as follows:

- **At zero distance:** \(K(0)=\exp(0)=1\), the maximum weight.
- **As distance increases:** The weight decreases smoothly toward zero.
- **When bandwidth changes:** A smaller positive bandwidth causes the weight to decrease more rapidly with distance. A larger positive bandwidth allows points farther away to retain more influence.

The function is a `const` member function because it reads `bandwidth_` but does not modify the object's state.

**Implementation consideration:** The formula divides by `bandwidth_ * bandwidth_`. The bandwidth must be nonzero for the calculation to be valid, and the algorithm should ensure that the bandwidth used by the kernel is positive and finite.

## 4. Command-Line CSV Application

I also developed `examples/example1.cpp` to demonstrate how the library can be used in a practical workflow. The program accepts CSV file paths, loads a dataset, performs clustering, displays a summary, and exports the results.

### 4.1 Input and clustering workflow

The program uses default paths and allows users to override them with command-line arguments:

```cpp
int main(int argc, char* argv[]) {
    string csvFilePath = "data/input/my_data.csv";
    string outFilePath = "data/output/results.csv";

    if (argc > 1) {
        csvFilePath = argv[1];
    }
    if (argc > 2) {
        outFilePath = argv[2];
    }

    meanshift::Dataset dataset;
    if (!dataset.loadFromCSV(csvFilePath, true)) {
        cerr << "Error: Failed to load external data from "
             << csvFilePath << endl;
        return 1;
    }

    meanshift::MeanShift ms(2.0, 1e-3, 300);
    ms.fit(dataset.getData());
    // Display and export results...
}
```

The workflow consists of these steps:

1. **Read file paths:** Use the default input and output paths unless alternative paths are supplied as command-line arguments.
2. **Load the dataset:** Call `dataset.loadFromCSV(csvFilePath, true)`. If loading fails, display an error message and terminate with a nonzero return value.
3. **Configure the model:** Construct `MeanShift` with a bandwidth of `2.0`, convergence tolerance of `1e-3`, and a maximum of `300` iterations.
4. **Fit the model:** Call `ms.fit(dataset.getData())` to run clustering on the loaded data.
5. **Display results:** Print the number of clusters, each cluster's centroid and point count, and the evaluation score.
6. **Export results:** Write the data points and their assigned cluster labels to the output CSV file.

### 4.2 Example output

The report's example run produced the following output:

```text
Number of clusters found: 3
Cluster 0 centroid: (2.00108, 2.0977) with 10 points.
Cluster 1 centroid: (7.92302, 8.01874) with 10 points.
Cluster 2 centroid: (2.07534, 7.94359) with 10 points.
Clustering evaluation score: 0.259014
```

These results show three discovered clusters, each containing ten points. The centroid coordinates summarize the locations of the clusters. The evaluation score is reported by the program; its interpretation depends on the definition of `evaluate()` elsewhere in the project.

The output CSV uses the header:

```text
Feature1,Feature2,Cluster_ID
```

Each row contains the point's feature values and its assigned cluster label. This makes the results easier to inspect in spreadsheet software or use in subsequent data analysis.

## 5. Testing, Challenges, and Collaboration

### 5.1 Unit testing

The Euclidean-distance function was tested using a familiar right-triangle example:

```cpp
void testEuclideanDistance() {
    meanshift::Point p1 = {0.0, 0.0};
    meanshift::Point p2 = {3.0, 4.0};

    double dist = meanshift::MeanShift::euclideanDistance(p1, p2);

    if (dist != 5.0) {
        throw runtime_error("testEuclideanDistance failed");
    }
    cout << "testEuclideanDistance passed." << endl;
}
```

The expected distance is

\[
d=\sqrt{(3-0)^2+(4-0)^2}
=\sqrt{9+16}=5.
\]

The test checks that the function returns `5.0`. According to the project report, the Gaussian kernel is also exercised through `testShiftPoint()` and `testMeanShiftClustering()`.

### 5.2 Development challenge

During Week 2, multiple pull requests modified the `private:` section of `MeanShift.hpp`, resulting in merge conflicts. The team addressed this by clarifying module ownership and carefully reviewing pull-request differences before merging. This reinforced the importance of coordinating shared-file changes in a collaborative software project.

### 5.3 Use of AI tools

The project report records the use of Gemini and Claude to verify the Gaussian-kernel equation and review C++ best practices relating to `static` and `const` member functions.

## 6. Conclusion

My contribution to the Group 21 project covered the `Cluster` structure and `MeanShift` class interface, the Euclidean-distance and Gaussian-kernel functions, and a command-line CSV example. Together, these components provide core mathematical functionality and a practical demonstration of how the library can be used with external data.

The work also highlighted the value of encapsulation, focused unit tests, clear module ownership, and careful collaboration when several developers work on shared C++ files. As a final quality check, the implementation should ensure that points have matching dimensions and that the bandwidth supplied to the Gaussian kernel is valid before the calculation is performed.
