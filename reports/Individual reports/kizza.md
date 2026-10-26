Final Individual Project Report 

Project Title: Design and Implementation of the Mean Shift Clustering Library in C++ (`meanshift`)  
Group: Group 21  
Report by; KIZZA JOHNSON SSOZI 25/U/08587/PS
Role:  Data Structures, `Dataset` Class Architecture, CSV Ingestion Engine & Statistical Moments 
Primary Files Contributed:  
 `include/meanshift/Dataset.hpp` (`Point`, `Data`, and `class Dataset` declaration)  
 `src/Dataset.cpp` (`Dataset::Dataset()`, `Dataset::loadFromCSV()`, `Dataset::getData()`, and column mean & standard deviation calculations in `Dataset::standardize()`)  
 `data/input/my_data.csv`, `data/input/students.csv`


 1. Introduction and Role Overview

In the Group 21 Mean Shift Clustering C++ Library project, my primary responsibility was designing and implementing the Data Ingestion and Storage Layer of the library. A reusable C++ machine learning library must cleanly separate file input/output (I/O) and data representation from the core mathematical clustering algorithm. Without this separation, the clustering engine would be tightly coupled to specific file formats, making it harder to test, maintain, or reuse on in-memory C++ vectors.

To achieve this architectural separation, I designed the foundational data type aliases (`Point` and `Data`), structured the public interface of the `Dataset` class in `include/meanshift/Dataset.hpp`, and implemented the default constructor `Dataset::Dataset()`, the fault-tolerant CSV file parser `Dataset::loadFromCSV()`, the read-only data accessor `Dataset::getData()`, and the column mean and population standard deviation accumulation passes for `Dataset::standardize()` in `src/Dataset.cpp`.


2. Library Design and Object-Oriented Programming (OOP) Principles
 2.1 Namespace Isolation (`namespace meanshift`)
All declarations in `include/meanshift/Dataset.hpp` and definitions in `src/Dataset.cpp` are wrapped inside `namespace meanshift`. In large C++ applications, generic names like `Dataset`, `Point`, or `Data` can easily collide with identifiers from other libraries. Encapsulating our types within `meanshift::` guarantees safe modular integration.

2.2 Core Data Type Aliases (`Point` and `Data`)
At the top of `include/meanshift/Dataset.hpp` (Lines 11–12), I defined two type aliases using modern C++ `using` syntax:

cpp
namespace meanshift {

using Point = vector<double>;
using Data = vector<Point>;

`Point` (`std::vector<double>`): Represents a single dimensional data point (x_1, x_2, \dots, x_D) in continuous Euclidean space. Using `std::vector<double>` instead of a fixed-size array allows the library to support datasets with an arbitrary number of features (2D, 3D, or higher dimensions) determined dynamically at runtime when reading a CSV file.
`Data` (`std::vector<Point>`): Represents the complete dataset of $N$ points as a 2D dynamic matrix.

2.3 Encapsulation and Access Control in `class Dataset`
The `Dataset` class bundles the raw dataset storage with the operations that load and transform it:

cpp
class Dataset {
public:
    Dataset();

    bool loadFromCSV(const string& filename, bool hasHeader = true);
    const Data& getData() const;

    // Z-score normalization
    void standardize();

    // Min-Max scaling to [0, 1]
    void normalize();

private:
    Data data_;
};


Why `data_` is `private`: Making `Data data_` private enforces strict encapsulation. External code cannot accidentally clear, resize, or insert malformed points into `data_` outside the class's controlled methods.
Why `getData()` returns `const Data&` and is marked `const`.
  cpp
  const Data& Dataset::getData() const {
      return data_;
  }
  
  1. Zero-Copy Efficiency: Returning a reference (`&`) avoids copying potentially thousands of floating-point vectors when passing the dataset into `MeanShift::fit(dataset.getData())`.
  2. Immutability Guarantee: Returning a `const` reference prevents the caller from modifying the internal `data_` vector through the getter, while the trailing `const` qualifier promises that calling `getData()` does not mutate the `Dataset` object itself.



3. Detailed C++ Implementation Walkthrough

3.1 CSV Ingestion (`Dataset::loadFromCSV`, `src/Dataset.cpp`, Lines 11–42)

```cpp
Dataset::Dataset() {}
bool Dataset::loadFromCSV(const string& filename, bool hasHeader) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return false;
    }

    string line;
    if (hasHeader) {
        getline(file, line);
    }

    data_.clear();
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Point p;
        while (getline(ss, token, ',')) {
            try {
                p.push_back(stod(token));
            } catch (const exception& e) {
            }
        }
        if (!p.empty()) {
            data_.push_back(p);
        }
    }
    return true;
}
```

 Step-by-Step Technical Walkthrough:
1. Default Constructor (`Dataset::Dataset()`): Initializes an empty `Dataset` instance, relying on `std::vector`'s default constructor to initialize `data_` with zero elements.
2. File Stream Validation: Uses `std::ifstream file(filename)` to open the CSV file. If the path is invalid or the file does not exist (`!file.is_open()`), it outputs `"Error: Cannot open file <filename>"` to `std::cerr` and returns `false` so the calling C++ program can handle the failure gracefully instead of crashing.
3. Compile-Time Polymorphism via Default Argument (`hasHeader = true`): Most CSV files include column names on the first row (e.g., `X_Coordinate,Y_Coordinate` in `my_data.csv` or `StudentName,Math_Score,Science_Score` in `students.csv`). By setting `bool hasHeader = true` in the header declaration, callers can write `dataset.loadFromCSV("data/input/my_data.csv")` to automatically consume and discard the header line via `getline(file, line)`, or explicitly pass `false` for headerless files.
4. Resetting State (`data_.clear()`): Ensures that if the same `Dataset` object is reused to load multiple files, points from the previous file are cleared before loading new rows.
5. Line-by-Line and Token-by-Token Parsing:
    The outer loop `while (getline(file, line))` reads each row from the file stream. Empty lines (`if (line.empty()) continue;`) are skipped.
    Each non-empty row is wrapped in a `std::stringstream ss(line)`. The inner loop `while (getline(ss, token, ','))` splits the line at every comma `','`.
   Each substring `token` is converted into a `double` using `std::stod(token)` inside a `try-catch` block and appended to `Point p`.
    For mixed-type CSV files such as `data/input/students.csv` (where each row has `StudentName,Math_Score,Science_Score`, e.g., `John Smith,85,92`), `stod("John Smith")` throws `std::invalid_argument`, which is caught and ignored, allowing `loadFromCSV()` to automatically extract the numeric feature vector `{85.0, 92.0}`!
    Once all columns in the row are parsed, `if (!p.empty()) data_.push_back(p);` appends the point to `data_`.

 3.2 Column Mean and Standard Deviation Calculations in `Dataset::standardize()` (`src/Dataset.cpp`, Lines 48–71)

In collaboration with a fellow group member ATUZARIRWE HAPPY, I also implemented the first two statistical passes of `Dataset::standardize()` to compute the per-column mean and population standard deviation:

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
```
ATUZARIRWE HAPPY then completed the in-place Z-score normalization loop and `Dataset::normalize()`.


 4. Inheritance, Polymorphism, and Defensive Error Handling

Even though `Dataset` is a standalone concrete class, my implementation leverages both inheritance and polymorphism from the C++ Standard Library:
1. Stream Inheritance Hierarchy (`std::istream`): Both `std::ifstream` (used to read lines from the physical CSV file) and `std::stringstream` (used to split each line by commas) inherit from the base class `std::istream`. This allows the exact same overloaded function `std::getline(std::istream&, ...)` to operate polymorphically on both file streams and in-memory string streams.
2. Exception Hierarchy & Runtime Polymorphism (`std::exception`): When `std::stod(token)` encounters a non-numeric string (such as `"John Smith"` in `students.csv`), it throws `std::invalid_argument`, and if a number exceeds the range of `double`, it throws `std::out_of_range`. Both derive from `std::exception`. By catching `const std::exception& e` by base-class reference, `loadFromCSV()` polymorphically intercepts any conversion error, ignores the non-numeric token, and continues parsing the numeric columns without crashing.


5. Automated Unit Testing & Verification (`tests/test_meanshift.cpp`)

My `loadFromCSV()` and `getData()` methods are directly verified by `testLoadFromCSV()` in `tests/test_meanshift.cpp`:

```cpp
void testLoadFromCSV() {
    const string tempFile = "test_temp_data.csv";
    ofstream out(tempFile);
    out << "Feature1,Feature2\n";
    out << "1.0,2.0\n";
    out << "3.0,4.0\n";
    out.close();

    meanshift::Dataset dataset;
    bool loaded = dataset.loadFromCSV(tempFile, true);
    remove(tempFile.c_str());

    const auto& data = dataset.getData();
    if (!loaded || data.size() != 2 || data[0][0] != 1.0 || data[1][1] != 4.0) {
        throw runtime_error("testLoadFromCSV failed");
    }
    if (dataset.loadFromCSV("non_existent_file.csv", true)) {
        throw runtime_error("testLoadFromCSV failed");
    }
    cout << "testLoadFromCSV passed." << endl;
}
```
 Normal Use Case: Verifies that a 2-row CSV with a header loads `data.size() == 2` with exact coordinates `data[0][0] == 1.0` and `data[1][1] == 4.0`.
Invalid Input Case: Verifies that calling `dataset.loadFromCSV("non_existent_file.csv", true)` returns `false` and prints `"Error: Cannot open file non_existent_file.csv"` without crashing.
Empty File Edge Case: Tested against `data/input/text_data.csv` (0 bytes), verifying that `data_` remains empty (`data_.empty() == true`) and downstream calls to `standardize()`, `normalize()`, and `MeanShift::fit()` return safely.

 6. Challenges Encountered and How They Were Addressed

1. Handling Headers and Non-Numeric Columns in CSV Files: Initially, attempting to run `std::stod()` on the first row of a CSV file containing column titles or on rows with text labels (like `StudentName` in `students.csv`) caused an uncaught `std::invalid_argument` exception that terminated the program. I resolved this by adding the `hasHeader` flag to skip the header line and wrapping `std::stod(token)` inside a `try-catch` block.
2. Header Include Path Consistency: Early in Week 2, headers were placed directly in `include/` while source files included `"meanshift/Dataset.hpp"`. Moving the headers into `include/meanshift/Dataset.hpp` aligned our directory layout with the CMake `target_include_directories` configuration.


 7. Git Collaboration and AI Use Declaration

Git Workflow: All changes to `include/meanshift/Dataset.hpp` and `src/Dataset.cpp` were developed on dedicated feature branches, committed with descriptive messages, and merged into `main` via peer-reviewed GitHub Pull Requests.
AI Use Declaration:
  Tools Used: Gemini / ChatGPT
  Purpose: Reviewing C++ `std::stringstream` comma-tokenization idioms and verifying how `std::stod` exceptions inherit from `std::exception`.
  Reason: To ensure our CSV parser handled malformed and mixed-type inputs safely using standard C++ without external third-party CSV libraries.