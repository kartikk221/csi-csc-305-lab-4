#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
using namespace std;

// This will be used to store the process/job information
struct Process {
    string name;
    int priority;
    int turnaroundTime;
    int startTime;
    int completionTime;
    int burstTime;
    int executionIndex;
};

// Prints the algorith execution results
void print_algorithm_results(string algorithm, vector<Process> jobs, int totalTurnaroundTime) {
    // Top Border
    cout << "============================= " << algorithm << " ==========================" << endl;

    // Sort the jobs in increasing order of job name parsed as an int
    sort(jobs.begin(), jobs.end(), [](Process a, Process b) {
        int aName = stoi(a.name.substr(5));
        int bName = stoi(b.name.substr(5));
        return aName < bName;
    });

    // Print the table header and results in job order
    cout << "Name\t\tStart Time\tCompletion Time\tTurn Around Time" << endl;
    for (int i = 0; i < jobs.size(); i++) {
        Process job = jobs[i];
        cout << job.name << "\t\t" << job.startTime << "\t\t" << job.completionTime << "\t\t" << job.turnaroundTime << endl;
    }
    cout << endl;

    // Sort the jobs in increasing order of execution index
    sort(jobs.begin(), jobs.end(), [](Process a, Process b) {
        return a.executionIndex < b.executionIndex;
    });

    // Print the job execution order aka. "Job #1 -> Job #2 -> Job #3 -> ..."
    cout << "Job Execution Order: ";
    for (int i = 0; i < jobs.size(); i++) {
        Process job = jobs[i];
        cout << job.name;
        if (i != jobs.size() - 1) cout << " -> ";
    }
    cout << endl;

    // Calculate the average turnaround time and print it
    double avgerageTurnaroundTime = (double)totalTurnaroundTime / (double)jobs.size();
    cout << "Average Turnaround Time: " << avgerageTurnaroundTime << endl;

    // Bottom Border
    cout << "=============================================================================" << endl << endl;
}

// This is the first come first serve non preemptive scheduling algorithm
void run_fcfs_non_preemptive(vector<Process> jobs) {
    // Duplicate the vector into a waiting queue
    vector<Process> queue = jobs;

    // Initialize metrics tracking variables
    int currentTime = 0;
    int totalTurnaroundTime = 0;
    for (int i = 0; i < queue.size(); i++) {
        // Get the next job in the queue
        Process job = jobs[i];

        // Calculate the job's start time
        int arrivalTime = job.startTime;
        int startTime = max(currentTime, arrivalTime);

        // Update the job's start time and completion time
        job.startTime = startTime;
        job.completionTime = startTime + job.burstTime;

        // Update the current time
        currentTime = job.completionTime;

        // Update the job's turnaround time and execution index
        job.turnaroundTime = job.completionTime - arrivalTime;
        job.executionIndex = i;

        // Update the total turnaround time
        totalTurnaroundTime += job.turnaroundTime;

        // Update the job in the jobs vector
        jobs[i] = job;
    }

    // Print the algorithm execution results
    print_algorithm_results("FCFS (Non Pre-emptive)", jobs, totalTurnaroundTime);
}

// This is the priority preemptive scheduling algorithm
void run_priority_preemptive(vector<Process> jobs) {
    // Duplicate the input vector into a waiting queue
    vector<Process> queue = jobs;

    // Initialize metrics tracking variables
    int currentTime = 0;
    int totalTurnaroundTime = 0;
    int executionIndex = 0;

    // Initialize a vector to store finished processes
    vector<Process> finished;

    // Initialize two more vectors for the arrivalTimes and the startTimes
    // The arrivalTimes are stored by default in the job startTime hencei it is needed to be copied
    vector<int> arrivalTimes;
    vector<int> startTimes(jobs.size(), -1);
    for (const Process& job : jobs) arrivalTimes.push_back(job.startTime);

    // Iterate through the queue of jobs until it is empty
    while (!queue.empty()) {
        // Find the highest priority process in the queue
        int highestPriorityIndex = -1;
        int minPriority = INT_MAX;
        for (int i = 0; i < queue.size(); i++) {
            // If the process has arrived and has a higher priority than the current highest priority process, update the highest priority process
            if (arrivalTimes[i] <= currentTime && queue[i].priority < minPriority) {
                highestPriorityIndex = i;
                minPriority = queue[i].priority;
            }
        }

        // If no process is available at the current time, increment the time and continue
        if (highestPriorityIndex == -1) {
            currentTime++;
            continue;
        }

        // Execute the process with the highest priority for the next 1-time unit
        Process job = queue[highestPriorityIndex];
        job.burstTime--;

        // Update the actual start time if it has not been set yet
        if (startTimes[highestPriorityIndex] == -1) startTimes[highestPriorityIndex] = currentTime;

        // If the process has completed aka. no more burstTime remaining to exhaust, update its metrics and add it to the finished vector
        if (job.burstTime == 0) {
            job.completionTime = currentTime + 1;
            job.turnaroundTime = job.completionTime - arrivalTimes[highestPriorityIndex];
            totalTurnaroundTime += job.turnaroundTime;
            job.executionIndex = executionIndex++;
            job.startTime = startTimes[highestPriorityIndex]; // Use the actual start time
            finished.push_back(job);

            // Remove the completed process from the waiting queue and its corresponding arrival time
            queue.erase(queue.begin() + highestPriorityIndex);
            arrivalTimes.erase(arrivalTimes.begin() + highestPriorityIndex);
            startTimes.erase(startTimes.begin() + highestPriorityIndex);
        } else {
            // If the process is not completed, update it back in the waiting queue
            queue[highestPriorityIndex] = job;
        }

        // Increment the current time
        currentTime++;
    }

    // Print the algorithm execution results
    print_algorithm_results("Priority (Pre-emptive)", finished, totalTurnaroundTime);
}

int main() {
    // Read the jobs/processes from the file
    vector<Process> jobs;
    ifstream file("jobs.txt");
    if (!file) {
        cerr << "Unable to open file jobs.txt" << endl;
        return 1;
    }

    // Read the jobs from the file while temporarily storing each piece of information in an int
    int name, priority, burstTime, arrivalTime;
    while (file >> name >> arrivalTime >> priority >> burstTime) {
        Process job;
        job.name = "Job #" + to_string(name);
        job.priority = priority;
        job.burstTime = burstTime;
        job.startTime = arrivalTime;
        jobs.push_back(job);
    }

    // Close the file as we are done with it
    file.close();

    // Run the first come first serve non preemptive scheduling algorithm
    run_fcfs_non_preemptive(jobs);

    // Run the priority preemptive scheduling algorithm
    run_priority_preemptive(jobs);

    return 0;
}