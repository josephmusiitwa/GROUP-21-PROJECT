# Final Individual Project Report — Member 9 ( Musiitwa Joseph)

**Project Title:** Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
**Group:** Group 21  
**Member Name / Role:** Member 9 (Joseph) — *CMake Build System Architecture, Repository Integration, Automated Unit Testing Suite (`tests/test_meanshift.cpp`) & External Machine Verification*  
**Primary Files Contributed:**  
- `CMakeLists.txt` (Root static library build, target exports & installation rules)  
- `examples/CMakeLists.txt` & `tests/CMakeLists.txt`  
- `.gitignore`  
- `tests/test_meanshift.cpp` (Complete 10-function C++ automated unit test suite for `Dataset` and `MeanShift`)  
- Repository structure, code alignment & final cross-machine build verification

---

## 1. Introduction and Role Overview

In the Group 21 Mean Shift Clustering C++ Library project, my primary responsibility was designing and maintaining the **CMake Build System Architecture**, configuring the **Repository Structure and `.gitignore`**, coordinating **Code Integration across all modules**, and implementing the **10-Test Automated C++ Unit Testing Suite (`tests/test_meanshift.cpp`)**.

According to the `C++ Group Project Instructions.docx`, the project must be packaged as a **reusable C++ library** (not just a standalone script), must build cleanly from a fresh environment using `CMakeLists.txt`, and must include automated unit tests verifying normal use cases, edge cases, and invalid inputs. My work ensured that our source files (`src/Dataset.cpp` and `src/meanshift.cpp`) compile into a portable static library (`libmeanshift.a` / `meanshift.lib`), link cleanly into both the demonstration executables (`example1`, `example2`, `example3`) and the CTest test runner (`test_meanshift`), and pass all 10 automated C++ unit tests on both team and external peer machines.

---

## 2. CMake Build System and Reusable Library Packaging

### 2.1 Root `CMakeLists.txt` Architecture (`CMakeLists.txt`, Lines 1–45)

```cmake
cmake_minimum_required(VERSION 3.15)

project(MeanShift
    VERSION 1.0.0
    LANGUAGES CXX
)

add_library(meanshift STATIC
    src/Dataset.cpp
    src/meanshift.cpp
)

target_include_directories(meanshift
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(meanshift PUBLIC cxx_std_23)
```

**Key Engineering Decisions in `CMakeLists.txt`:**
1. **Static Library Target (`add_library(meanshift STATIC ...)`):** Compiles `src/Dataset.cpp` and `src/meanshift.cpp` into a reusable static archive (`libmeanshift.a`). Any downstream C++ executable only needs `target_link_libraries(<target> PRIVATE meanshift)` instead of recompiling the library source files each time.
2. **Modern Target-Based Include Directories & Generator Expressions:**
   - Using `PUBLIC` in `target_include_directories(meanshift PUBLIC ...)` automatically propagates the `include/` path both when compiling the library itself and to any executable (`example1`, `example2`, `example3`, `test_meanshift`) that links against `meanshift`.
   - `$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>` resolves to the local repository's `include/` folder during compilation, while `$<INSTALL_INTERFACE:include>` resolves to the system installation directory after `cmake --install` is run.
3. **C++ Standard Propagation (`target_compile_features`):** Setting `PUBLIC cxx_std_23` ensures modern C++ standard support is enforced across the library and all linked targets.
4. **System Installation and CMake Package Export (`install()` rules, Lines 22–38):**
   ```cmake
   install(
       TARGETS meanshift
       EXPORT MeanShiftTargets
       ARCHIVE DESTINATION lib
   )
   install(
       DIRECTORY include/meanshift
       DESTINATION include
   )
   install(
       EXPORT MeanShiftTargets
       FILE MeanShiftTargets.cmake
       NAMESPACE MeanShift::
       DESTINATION lib/cmake/MeanShift
   )
   ```
   These rules allow external C++ developers to install our library and import it via `MeanShift::meanshift` in their own CMake projects.
5. **Modular Subdirectories (`add_subdirectory`, Lines 41–44):**
   - `add_subdirectory(examples)` invokes `examples/CMakeLists.txt` to build `example1`, `example2`, and `example3` and link each with `target_link_libraries(<target> PRIVATE meanshift)`.
   - `enable_testing()` and `add_subdirectory(tests)` invoke `tests/CMakeLists.txt` to build `test_meanshift` and register it with CTest via `add_test(NAME MeanShiftTests COMMAND test_meanshift)`.

### 2.2 Repository Hygiene (`.gitignore`)
To prevent compiled binaries (`.exe`, `.o`, `.obj`, `.a`, `.lib`), CMake build directories (`build/`, `CMakeFiles/`, `CMakeCache.txt`), and IDE metadata (`.vscode/`) from polluting the GitHub repository, I configured `.gitignore` so only clean C++ source code, headers, tests, examples, datasets, and documentation are tracked.

---

## 3. Complete 10-Function Automated C++ Unit Testing Suite (`tests/test_meanshift.cpp`)

To satisfy Section 4 of the project instructions (*"Verify normal use cases, test important edge cases, test invalid inputs where applicable"*), I implemented 10 comprehensive unit test functions in `tests/test_meanshift.cpp` (Lines 1–170):

| # | Test Function in `tests/test_meanshift.cpp` | Component Tested | Category | What Invariant is Verified |
| :--- | :--- | :--- | :--- | :--- |
| 1 | `testEuclideanDistance()` (Lines 10–16) | `MeanShift::euclideanDistance` | Normal Use | Exact 3-4-5 right triangle distance between `{0.0, 0.0}` and `{3.0, 4.0}` equals `5.0` |
| 2 | `testMeanShiftClustering()` (Lines 18–28) | `MeanShift::fit` & `getClusters` | Normal Use | 6 points in two well-separated groups (`{0,0}..{0.2,0.2}` and `{10,10}..{10.2,10.2}`) converge into exactly `2` clusters |
| 3 | `testShiftPoint()` (Lines 30–41) | `MeanShift::shiftPoint` | Mathematical Symmetry | Shifting midpoint `{1.0, 1.0}` between `{0.0, 0.0}` and `{2.0, 2.0}` stays at `{1.0, 1.0}` within `1e-6` |
| 4 | `testEstimateBandwidth()` (Lines 42–52) | `MeanShift::estimateBandwidth` | Edge / Auto-Config | Default `MeanShift ms;` (`bandwidth = -1.0`) estimates a valid positive bandwidth and discovers `2` clusters |
| 5 | `testGetLabels()` (Lines 54–69) | `MeanShift::getLabels` | Normal Use | Verifies `labels.size() == 6`, points `0,1,2` share one label, points `3,4,5` share another label, and `labels[0] != labels[3]` |
| 6 | `testEvaluate()` (Lines 70–80) | `MeanShift::evaluate` | Normal Use | Verifies intra-cluster cohesion score on the 2-cluster dataset satisfies `0.0 < score <= 0.5` |
| 7 | `testEmptyData()` (Lines 81–89) | `MeanShift::fit`, `getClusters`, `getLabels`, `evaluate` | Edge / Invalid Input | Passing empty `Data` (`{}`) produces empty `clusters_`, empty `labels_`, and `evaluate() == 0.0` without crashing |
| 8 | `testLoadFromCSV()` (Lines 90–110) | `Dataset::loadFromCSV` & `getData` | Normal & Invalid Input | Creates temporary CSV `test_temp_data.csv`, verifies `data.size() == 2` and values `1.0`, `4.0`, removes temp file, and verifies `"non_existent_file.csv"` returns `false` |
| 9 | `testStandardize()` (Lines 112–132) | `Dataset::standardize` | Preprocessing | Creates `test_temp_std.csv` (`{1.0, 10.0}`, `{3.0, 30.0}`), runs `standardize()`, verifies Z-scores equal `-1.0` and `+1.0` within `1e-6` |
| 10 | `testNormalize()` (Lines 133–154) | `Dataset::normalize` | Preprocessing | Creates `test_temp_norm.csv` (`{10,50}`, `{20,100}`, `{30,150}`), runs `normalize()`, verifies Min-Max values equal `0.0`, `0.5`, `1.0` within `1e-6` |

### Exact CTest Execution Output:
```text
1: Test command: build\tests\test_meanshift.exe
1: testEuclideanDistance passed.
1: testMeanShiftClustering passed.
1: testShiftPoint passed.
1: testEstimateBandwidth passed.
1: testGetLabels passed.
1: testEvaluate passed.
1: testEmptyData passed.
1: Error: Cannot open file non_existent_file.csv
1: testLoadFromCSV passed.
1: testStandardize passed.
1: testNormalize passed.
1: All tests passed!
1/1 Test #1: MeanShiftTests ...................   Passed    1.07 sec

100% tests passed out of 1
```

---

## 4. Challenges Encountered and How They Were Addressed

1. **Include Path & Filename Case Sensitivity Across OS Platforms:** When team members initially merged separate Pull Requests, headers were in `include/` instead of `include/meanshift/`, and `CMakeLists.txt` referenced `src/MeanShift.cpp` while the file on disk was `src/meanshift.cpp`. I restructured the header directory to `include/meanshift/` and aligned `CMakeLists.txt` to `src/meanshift.cpp` so the project builds cleanly on both Windows and case-sensitive Linux/macOS filesystems.
2. **Self-Cleaning Unit Tests for File I/O:** To test `loadFromCSV()`, `standardize()`, and `normalize()` in `test_meanshift.cpp` without depending on external working directory paths, I wrote each test to generate its own temporary CSV file (`ofstream out(tempFile)`), load it into `Dataset`, and immediately delete it using `remove(tempFile.c_str())` so CTest can be run from any directory.

---

## 5. Git Collaboration and AI Use Declaration

- **Git Workflow:** Managed repository structure, created and reviewed Pull Requests across branches, aligned coding style across files, and verified clean builds using CMake and CTest.
- **AI Use Declaration:**
  - **Tools Used:** Gemini
  - **Purpose:** Reviewing modern CMake target export syntax (`$<BUILD_INTERFACE:...>` and `install(EXPORT ...)`) and designing self-contained CTest unit test cases.
  - **Reason:** To ensure our `CMakeLists.txt` followed professional C++ library packaging standards and built cleanly on any machine.
