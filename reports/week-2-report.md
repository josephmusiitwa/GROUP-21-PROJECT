**Week 2 Progress Report**

**Group 21 · Mean Shift Clustering library (C++)**

1. Completed
The repository was restructured following the structure in the instructions and all place holders from week one in the include folder and src folder were removed and replaced with actual codes. In the include folder, Dataset.hpp that contains the Dataset class  and Meanshift.hpp that contains the Meanshift class were added. 
In the src folder, the definitions of the functions in the header files were written in the respective codes. In  Dataset.cpp, some functions in Dataset.hpp were defined and in Meanshift.cpp, some functions in Meanshift.hpp were defined.
All this was done collaboratively by members creating separate branches from main and then creating pull requests to merge the changes to main.

2. In Progress
Completing the definitions of all functions in the Meanshift.cpp and Dataset.cpp codes.
Writing examples that use the Dataset and Meanshift classes.
Writing the test codes to test the core functionalities of the codes.

3. Challenges/Blockers
Sometimes the network was slow and github would take longer to show the compare and pull request prompts which slowed down progress and was a little bit irritating for some group members.
 Otherwise the activity moved on well once all members got used to the procedures needed.

4. Next Week
Add the remaining codes in all folders.
Add the first real tests, then the sample data and example programs.
Each member prepares a short report explaining their contribution to the project.
Making sure the codes compile,run smoothly and provide the expected outputs. 
Submit the Week 3 report.
Prepare the final report and check every member’s understanding of the project in preparation for the presentation of the project.

5. AI use and declaration.
AI use was very minimal since we are now all familiar with what is needed in the project and everyone had read and knew what to do.

6. Class Structure
Our program is organised around two classes i.e. Dataset and MeanShift and one small struct (Cluster), all inside the meanshift namespace. A point is a list of numbers and a dataset is a list of points (Data). In the tables, in repo means the member is already in this repository, and to add means it is declared in the header and its definition is still to be added.

**Class 1: Dataset (include/Dataset.hpp, src/Dataset.cpp)**
Why?
To keep reading and preparing the data in one place, separate from the clustering. The rest of the program gets clean numbers without dealing with files or scaling.

What data does it hold? 
One data member, data_: all the points read from the CSV file.

What functions operate on that data?
loadFromCSV fills data_. standardize and normalize rescale it. getData lets other code read it.

Why are some members private and others public? How is the class used? 
data_ is private so nothing outside the class can change the points by accident. Only the class’s own functions change it. The functions are public because they are how the rest of the program uses the class. getData returns a read-only (const) reference, so other code can read the points but not change them. A program creates a Dataset, calls loadFromCSV, optionally calls standardize or normalize, and gives getData() to MeanShift.
----------------------------------------------------------------------
Member          Access      What it is or does                Status
----------------------------------------------------------------------
data_ (Data)    private     the points read from the file     In repo
Dataset()       public      constructor, creates an empty     In repo
                            dataset
loadFromCSV     public      reads a CSV file into data_.      In repo
(filename,                  Prints an error  and returns 
hasHeader)                  false if the file cannot be 
                            opened. Skips the header and 
                            empty lines and ignores values 
                            that are not numbers. 
getData()       public      returns the points (read-only)     To add
standardize()   public      rescales each column to mean 0     To add
                            and standard deviation 1

normalize()     public      rescales each column to the        To add
                            range 0 to 1
----------------------------------------------------------------------

**Class 2: MeanShift (include/MeanShift.hpp, src/meanshift.cpp)**
Why? 
To keep the clustering algorithm and its settings together. The user sets the settings once, calls fit, then reads the results. The steps of the algorithm (distance, kernel, bandwidth, shifting) are separate small functions so each one can be written and tested on its own.

What data does it hold? 
Three settings (bandwidth_, epsilon_, maxIterations_) and two results (clusters_, labels_).

What functions operate on that data? 
The constructor sets the settings. estimateBandwidth sets bandwidth_ when none is given. gaussianKernel uses bandwidth_ to weight a neighbour. shiftPoint uses the kernel to move one point. fit runs the whole algorithm using the settings and fills clusters_ and labels_. getClusters, getLabels and evaluate read the results.

Why are some members private and others public? How is the class used? 
The settings and results are private so they only change through the class’s own functions, which keeps the results matching the settings that produced them. The functions are public because they are what the rest of the program calls: set up (constructor), run (fit) and read the results (the getters and evaluate). The small maths functions (euclideanDistance, gaussianKernel, shiftPoint) are public so they can be tested one by one, and euclideanDistance is static because it only needs two points and no object data. After Dataset has loaded the data, the program creates a MeanShift, calls fit with getData(), then reads the clusters, labels and score. MeanShift only needs the Point and Data types, not the Dataset class itself, so it can cluster any data in that form.
----------------------------------------------------------------------
Member           Access     What it is or does                 Status
-----------------------------------------------------------------------
bandwidth_       private    how far around a point we look     In repo
(double)                    for neighbours (estimated from 
                            the data if not given)
epsilon_        private     a point stops moving once a step   In repo
(double)                    is smaller than this
maxIterations_  private     the most rounds of shifting we     In repo
(int)                       allow
clusters_       private     the final clusters                 In repo
(vector
<Cluster>)
labels_         private     the cluster number of each point   In repo
(vector<int>)
MeanShift       public      constructor, sets the three        In repo
(bandwidth,                 settings (defaults: bandwidth -1 
epsilon,                    means estimate it, epsilon 0.001, 
maxIterations)              300 rounds)
euclideanDistance public    (a,b)straight-line distance         In repo
                            between two points
gaussianKernel   public     gives a neighbour a weight:         In repo
(distance)                  closer neighbours count more
estimateBandwidth public    chooses a bandwidth from the data   In repo
(data)
shiftPoint(p,    public     one mean-shift step: moves a point  In repo
data)                       to the weighted average of the
                            data around it
fit(data)       public      runs the algorithm: moves every     To add
                            point until it stops, then groups
                             the points that end up together
getClusters()   public      returns the clusters (read-only)    To add
getLabels()     public      returns each point’s cluster        To add
                            number (read-only)
evaluate(data)  public      gives a simple score of how tight   To add
                            the clusters are
-----------------------------------------------------------------------

**How the classes work together**
Load Data: First, you create a Dataset object and call loadFromCSV(filename) to read and load your raw data from a CSV file.

Preprocess: Optionally, you call standardize() or normalize() to ensure every column is on a comparable scale before processing.

Configure the Algorithm: Use estimateBandwidth(data) to choose an appropriate bandwidth based on the dataset.
Use gaussianKernel(distance) to assign a weight to neighboring points, giving closer neighbors a higher weight.

Execute: Call fit(data) to run the main algorithm, which iteratively shifts points using shiftPoint(p, data) (moving each point to the weighted average of the surrounding data) until convergence, grouping the points that end up together.

Evaluate and Retrieve:
Call evaluate(data) to obtain a simple score assessing how tight the resulting clusters are.
Use getClusters() to access the final clusters in a read-only format.
Use getLabels() to retrieve each individual data point's assigned cluster number.



