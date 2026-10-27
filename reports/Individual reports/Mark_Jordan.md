# Mark Jordan's Report


Role: Examples, sample data and OOP diagrams

## 1. My role

The project instructions say the library must have small example programs in `examples/`. My part was to help write `example2.cpp` with a teammate, add the default dataset `data/input/my_data.csv`, and draw the OOP diagrams used in the Week 3 and Week 4 reports. I also reviewed and merged pull requests #39 and #42 (from Simon's branch) and committed the first version of the Week 4 report.

Files written by other people that I use in this report: `example3.cpp` (Desire), `biology_data.csv` (Kizza John), `finance.csv` (Nazarene), `students.csv` (Happy), `text_data.csv` (Simon).

## 2. The three examples

| Example | Input | What it shows |
| :--- | :--- | :--- |
| `example1.cpp` | A CSV file (default `data/input/my_data.csv`) | Load a CSV, cluster it, save the results to `data/output/results.csv` |
| `example2.cpp` | Numbers written directly in the code | Clustering without any file |
| `example3.cpp` | The user types the file paths | Clustering any CSV the user chooses |

## 3. example2.cpp

I made 9 points in three groups so the right answer is easy to see.

```cpp
meanshift::MeanShift ms(2.0, 1e-3, 300);

meanshift::Data data = {
    {1.0, 1.0}, {1.5, 2.0}, {2.0, 1.0},
    {8.0, 8.0}, {8.5, 9.0}, {9.0, 8.0},
    {1.0, 8.0}, {1.5, 9.0}, {2.0, 8.0}
};
ms.fit(data);
auto clusters = ms.getClusters();
```

- `MeanShift ms(2.0, 1e-3, 300)`: bandwidth 2.0, stop when points move less than 0.001, at most 300 rounds.
- `Data` is a list of points, and each point is a list of numbers.
- `fit(data)` runs the algorithm on the data directly. It does not need a CSV file.
- `getClusters()` gives back the clusters. Each has a `centroid` and the list of its points (`pointIndices`).

Output when I ran it:

```text
Number of clusters found: 3
Cluster 0 centroid: (1.50006, 1.35186) with 3 points.
Cluster 1 centroid: (8.47972, 8.33061) with 3 points.
Cluster 2 centroid: (1.52013, 8.30793) with 3 points.
Clustering evaluation score (cohesion): 0.622962
```

This is what I expected: 3 clusters with 3 points each. It shows that `MeanShift` does not depend on file loading, so the two classes are loosely coupled.

Note: the file also creates a `Dataset` object that it never uses. It is harmless.

## 4. example3.cpp (written by Desire)

It asks the user for an input path and an output path with `cin`. If the file cannot be loaded, `loadFromCSV` returns `false`, and the program prints an error and stops instead of crashing. Limitation: `cin >>` stops at a space, so paths with spaces do not work.

## 5. Datasets

| File | Added by | Description |
| :--- | :--- | :--- |
| `my_data.csv` | Mark | 30 points, 3 groups of 10. Default input for example1. |
| `biology_data.csv` | Kizza John | 30 points, 2 features |
| `finance.csv` | Nazarene | 30 points, 2 features |
| `students.csv` | Happy | Name, Math score, Science score. The name column is skipped and the two scores are used. |
| `text_data.csv` | Simon | Empty file, to test that an empty input does not crash |

## 6. OOP

The lecturer's five questions, for the two main classes.

**Dataset**
1. Class: `meanshift::Dataset`.
2. Why: it keeps file loading separate from the algorithm.
3. Data: the table of numbers (`data_`).
4. Functions: `loadFromCSV(path, hasHeader)`, `getData()` and the preprocessing functions.
5. `data_` is private so outside code cannot break it. The functions are public because users call them.

**MeanShift**
1. Class: `meanshift::MeanShift`.
2. Why: it holds the clustering algorithm.
3. Data: `bandwidth_`, `epsilon_`, `maxIterations_`, `clusters_`, `labels_`.
4. Functions: `fit()` uses the settings and fills `clusters_` and `labels_`. `getClusters()` and `evaluate()` read the results.
5. All data is private. The settings are given once in the constructor, and users work through the public functions.

**Diagrams** (in `reports/images/`)
- `encapsulation_classes.jpeg`: private data and public functions of the two classes.
- `encapsulation_namespace.jpeg`: the classes inside `namespace meanshift`.
- `inheritance_stdlib.jpeg`: inheritance in the standard library that we use (`ifstream` from `istream`, `invalid_argument` from `exception`).
- `polymorphism_overview.jpeg`: default arguments, templates like `std::vector<T>`, and `catch (const std::exception&)`.

Our own classes do not inherit from each other. `Dataset` and `MeanShift` do different jobs, so we did not need that. The inheritance in our program comes from the standard library.

## 7. Git and AI

Git: I used branches and pull requests. My commits include `my_data.csv` and the first Week 4 report.

AI use:
- Tools: Gemini and Claude
- Purpose: to understand inheritance and polymorphism in our code
- Reason: these topics were new to me and I needed them before drawing the diagrams
- I ran example2 myself and checked the diagrams against the code.

## 8. Questions

**How does example2 show low coupling?**
It clusters in-memory data with `fit(data)` and never calls `loadFromCSV()`, so `MeanShift` does not depend on files.

**Where is inheritance in your project?**
In the standard library classes we use, such as `ifstream` and the exception classes. Our own classes use composition.

**What happens if the file is empty or the path is wrong?**
`loadFromCSV` returns `false`, and the example prints an error and exits.

**How does the output work for any number of features?**
The loop runs over the size of each point, so it works for 2, 3 or more features.

**What does the bandwidth do?**
It is the neighbourhood size. Small gives many clusters, large gives few. In example2 the groups are about 7 units apart, so 2.0 keeps them separate.
