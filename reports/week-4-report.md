# Week 4 Project Report

Group 21 · Mean Shift Clustering Library in C++ (`meanshift`)  
Project Topic: Machine Learning 13 — Design and Implementation of the Mean Shift Clustering Algorithm in C++

---

## 1. Week 4 Progress

- Unit tests: increased from 2 tests to 10 in `tests/test_meanshift.cpp`. They cover normal use, scaling, bandwidth, labels, cohesion, empty data, and a missing file (`non_existent_file.csv`).
- Input data: added 5 CSV files in `data/input/`: `my_data.csv`, `biology_data.csv`, `finance.csv`, `students.csv` (text and numeric columns), and `text_data.csv` (empty file).
- Examples: added `examples/example3.cpp` (interactive, uses `std::cin` and `std::cout`) and registered `example1`, `example2` and `example3` in `examples/CMakeLists.txt`.
- Other: updated `.gitignore`, fixed a missing `#include`, completed `README.md` and all 9 individual reports.
- Testing by three students outside the group (Section 4).

### Work by member

| Member | Work this week |
| :--- | :--- |
| Joseph | Updated `.gitignore`; added a missing `#include` and aligned the code in `test_meanshift.cpp`; coordinated integration and external testing. |
| Desire | `testNormalize()`; interactive `example3.cpp`. |
| Nazarene | Added a test function to `test_meanshift.cpp`; added `finance.csv`. |
| Kizza | `testEstimateBandwidth()` and `testGetLabels()`; added `biology_data.csv`. |
| Clive | `testEvaluate()` and `testEmptyData()`. |
| Happy | `testLoadFromCSV()` and `testStandardize()`; added `students.csv`; removed `.gitkeep` from `data/input/`. |
| Simon | Added CSV data files, including `text_data.csv`. |
| Mark | Added `my_data.csv`; merged pull requests into `main`. |

### Problems and fixes

1. File name case: `CMakeLists.txt` listed `src/MeanShift.cpp`, but the file is `src/meanshift.cpp`. Linux file names are case-sensitive, so the build failed on Ubuntu. Fixed by using `src/meanshift.cpp` in `CMakeLists.txt`.
2. File paths in tests: tests that read `data/input/` failed when `ctest` was run from another folder. `testLoadFromCSV()`, `testStandardize()` and `testNormalize()` now create temporary CSV files, use them, and delete them.

---

## 2. Project Background (Weeks 1–3)

Mean Shift moves each point toward the nearest dense area. Points that end in the same area form one cluster, so the number of clusters does not have to be given in advance. The library `libmeanshift.a` is written in standard C++ and has a `Dataset` class (loads and scales CSV data), a `MeanShift` class (clustering), and a `Cluster` struct (result for one cluster). The class design and the maths are in the Week 3 report.

---

## 3. Examples

### example1 on `my_data.csv`
Finds 3 clusters in 30 points and writes a cluster number for every row.
```text
$ ./build/examples/example1 data/input/my_data.csv data/output/results.csv
Number of clusters found: 3
Clustering evaluation score: 0.259014
Successfully saved!
```

### example1 on `students.csv`
The name column is skipped and the students are clustered by Math and Science score. Result: 4 clusters, score 1.97891.

| Cluster | Centre (Math, Science) | Students | Members |
| :---: | :---: | :---: | :--- |
| 0 | (85.70, 91.73) | 4 | John Smith, Sarah Connor, Mike Johnson, Emily Davis |
| 1 | (44.67, 54.23) | 3 | Emma Watson, Bruce Wayne, Tony Stark |
| 2 | (41.10, 49.12) | 2 | Peter Parker, Arthur Curry |
| 3 | (95.13, 96.78) | 3 | Clark Kent, Diana Prince, Barry Allen |

### example2 and example3
`example2` clusters 9 points defined in the code and finds 3 clusters. `example3` asks for the input and output file names; on `biology_data.csv` it finds 3 clusters.

---

## 4. Testing

### Unit tests (all 10 passed)

| # | Test | Checks |
| :---: | :--- | :--- |
| 1 | `testEuclideanDistance()` | Distance for a 3-4-5 triangle is `5.0` |
| 2 | `testMeanShiftClustering()` | 6 points give `2` clusters |
| 3 | `testShiftPoint()` | A point halfway between two others stays in the middle |
| 4 | `testEstimateBandwidth()` | Bandwidth is estimated when `bandwidth = -1.0` |
| 5 | `testGetLabels()` | One label per point |
| 6 | `testEvaluate()` | Cohesion score is between `0.0` and `0.5` |
| 7 | `testEmptyData()` | Empty data is handled |
| 8 | `testLoadFromCSV()` | A valid file loads; a missing file is handled |
| 9 | `testStandardize()` | Values become `-1.0` and `+1.0` |
| 10 | `testNormalize()` | Values become `0.0`, `0.5` and `1.0` |

```text
1: All tests passed!
1/1 Test #1: MeanShiftTests ...................   Passed    1.07 sec
```

### Testing by students outside the group
Following the project instructions, we are sharing the project with  students from other groups to build and test the project.
The results are to be documented in detail in the final report which is in progress.

## 5. AI Use (Weeks 1–4)

Tools: Gemini, ChatGPT and Claude.
- Week 1: explaining clustering, Gaussian kernels and bandwidth.
- Week 2: a little help with header/source files and reading CSV files.
- Week 3: understanding inheritance and polymorphism in the C++ standard library.
- Week 4: checking CMake settings and test coverage.

AI was used to learn and check. Group members reviewed and understood all code, tests and reports.

---

## 6. Lessons Learned

1. Keeping data loading (`Dataset`) separate from the algorithm (`MeanShift`) made testing easier.
2. Decimal numbers should not be compared with `==`. Points that are close enough (within `bandwidth_ / 2.0`) are merged instead.
3. Testing on other computers found the Linux file name problem, which our Windows computers did not show.