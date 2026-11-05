# Mark Jordan's Report


Role: Examples, sample data, OOP diagrams, presenting the examples and the `.gitignore` explanation

## 1. My role

The project instructions say the library must have small example programs in `examples/`. My part was to help write `example2.cpp` with a teammate, add the default dataset `data/input/my_data.csv`, and draw the OOP diagrams used in the Week 3 and Week 4 reports. I also reviewed and merged pull requests #39 and #42 (from Simon's branch) and committed the first version of the Week 4 report. In the presentation I demonstrate all three example programs and explain the project's `.gitignore` file (section 7).

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
| `text_data.csv` | Simon | 9 items (fruit, vehicles, animals) with text columns mixed in. The text is skipped and the two numeric features give 3 clusters. |

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

## 7. The .gitignore file

`.gitignore` is in the root of the repository. It lists files and folders that Git must never track. The rule is simple: we commit what people write (code, data, reports) and ignore what the computer generates or what belongs to one person's machine.

| Group | Patterns | Why it is ignored |
| :--- | :--- | :--- |
| Build folders | `build/`, `build-*/`, `bin/`, `out/` | CMake creates everything in `build/` when we run `cmake -S . -B build`. It can be rebuilt at any time and it contains paths from one person's computer. |
| Object files | `*.o`, `*.obj`, `*.lo`, `*.slo` | In-between files the compiler makes from each `.cpp` before linking. |
| Precompiled headers | `*.gch`, `*.pch` | Compiler caches that speed up building. |
| Libraries | `*.a`, `*.lib`, `*.la`, `*.lai`, `*.so`, `*.dylib`, `*.dll` | Our library `libmeanshift.a` is a static library, so it is covered by `*.a`. |
| Executables | `*.exe`, `*.out`, `*.app` | Built programs. On Linux our programs (`example1`, `test_meanshift`) have no extension, but they are inside `build/`, which is already ignored. |
| Fortran modules | `*.mod`, `*.smod` | Part of the standard C++ template from GitHub. We have no Fortran, so these lines are not used. |
| Operating system files | `.DS_Store`, `._*`, `.Spotlight-V100`, `.Trashes`, `Thumbs.db`, `ehthumbs.db` | Hidden files that macOS and Windows create by themselves. |
| Editor settings | `.vscode/` | Each person's VS Code settings. One person's setup should not overwrite another's. |

**Why this matters for a team of nine**
- The repository stays small and only holds files we actually wrote.
- People on Linux, Windows and macOS do not commit each other's build output or system files, so there are fewer merge conflicts.
- Anyone can clone the repository and rebuild everything with `cmake -S . -B build && cmake --build build`.

## 8. Git and AI

Git: I used branches and pull requests. My commits include `my_data.csv` and the first Week 4 report.

AI use:
- Tools: Gemini and Claude
- Purpose: to understand inheritance and polymorphism in our code, and to understand each part of the `.gitignore` file
- Reason: these topics were new to me and I needed them before drawing the diagrams and presenting the `.gitignore`
- I ran example2 myself and checked the diagrams against the code. I checked the `.gitignore` explanation against the real file.

## 9. Questions

**How does example2 show low coupling?**
It clusters in-memory data with `fit(data)` and never calls `loadFromCSV()`, so `MeanShift` does not depend on files.

**Where is inheritance in your project?**
In the standard library classes we use, such as `ifstream` and the exception classes. Our own classes use composition.

**What happens if the file is empty or the path is wrong?**
If the path is wrong, `loadFromCSV` returns `false`, and the example prints an error and exits with code 1. If the file exists but is empty, it loads without error, `fit()` returns straight away, and the program reports 0 clusters, a score of 0, and writes a `results.csv` that only has the header line.

**How does the output work for any number of features?**
The loop that writes each row runs over the size of each point, so the data rows work for 2, 3 or more features. The header line is hard-coded as `Feature1,Feature2,Cluster_ID`, so with 3 or more features the header has too few column names. Building the header from the number of features would fix this.

**What does the bandwidth do?**
It is the neighbourhood size. Small gives many clusters, large gives few. In example2 the groups are about 7 units apart, so 2.0 keeps them separate.

**What does `.gitignore` do?**
It lists files Git must never track: build output, compiled objects and libraries, executables, operating system files and editor settings. These are generated or machine-specific and can always be rebuilt from the source.

**Why is `libmeanshift.a` not in the repository?**
It is a static library built from our source code, and `*.a` and `build/` are both ignored. Anyone gets it by running CMake.

**Why are our executables ignored if `*.exe` only matches Windows?**
They are built inside `build/`, and the whole `build/` folder is ignored.
