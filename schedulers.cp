#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

/* ABDUL-KAMIL FUSHEINI
   CSC 564 OPERATING SYSTEMS PROJECT ASSIGNMENT */

// Process structure
struct Process {
 int pid;
    float arrivalTime;
    float burstTime;
    bool firstRun;
    float waitTime;
    Process() : pid(0), arrivalTime(0.0), burstTime(0.0), firstRun(true), waitTime(0.0){}
    Process(int id, float arrival, float burst) : pid(id), arrivalTime(arrival), burstTime(burst), firstRun(true), waitTime(0.0){}

  };


	// Global variables
	vector<Process> processes;
	vector<Process> jobList;
	deque<Process> readyQueue;
    	float currentTime = 0.0;
    	float totalTurnaroundTime = 0.0;
    	float totalWaitTime = 0.0;
    	int queueLength = 0;
    	int queueCount = 0;
    	int maxQueueLength = 0;
    	int numProcesses = 0;

	//Functions prototypes
	void createProcesses();
	void resetStats();
	void arrivalEvent();
	void sortQueue();
	void printStats(ostream& output);
	void fcfsScheduler(vector<Process>& jobList);
	void sjnScheduler(vector<Process>& jobList);
	bool operator<(const Process& a, const Process& b);
	void rrScheduler(vector<Process>& jobList, float timeQuantum, float contextSwitchTime);
 
// Function Definitions
int main() {
	
	// Different time quanta for RR scheduler
	int timeQuanta[4] = {50, 100, 250, 500};

    	// Different context switch times
    	float contextSwitchTimes[6] = {0, 5, 10, 15, 20, 25};

	//Create Processes from input file
	createProcesses();
	
	// Open output file
    	ofstream outputFile("output.txt");

	//show process list  
        for(int i =0; i<processes.size(); i++) cout << processes[i].arrivalTime << " " <<processes[i].burstTime<< endl;
	
    	// Run RR scheduler for each combination of time quanta and context switch times 
    	for (int i = 0; i < 4; i++){
 	 for (int j = 0; j < 6; j++){
	   cout << "=======================================================================" << endl; 
           cout << "Running RR scheduler with time quantum " << timeQuanta[i] << " and context switch time " << contextSwitchTimes[j] << endl;
	   jobList = processes;
	   rrScheduler(jobList, (float)timeQuanta[i]/1000, (float)contextSwitchTimes[j]/1000);
	   
	   //report stats to output file and reset counters
	   outputFile << "RR Scheduling statistics for Time Quantum " << timeQuanta[i]<<" milliseconds"; 
	   outputFile << " and Context Switch Time " << contextSwitchTimes[j]<<" milliseconds" << endl;
	   printStats(outputFile);
	   resetStats();
			
         }
	}

    	// Run FCFS scheduler 
	jobList = processes;
	cout << "=======================================================================" << endl; 
        cout << "Running FCFS Scheduler" << endl;   
	fcfsScheduler(jobList);
	
	//report stats to output file and reset counters
	outputFile << "FCFS Scheduling statistics" << endl;  
	printStats(outputFile);
	resetStats();

	// Run SJN scheduler 
	jobList = processes;
	cout << "=======================================================================" << endl; 
        cout << "Running SJN Scheduler" << endl;   
	sjnScheduler(jobList);
	
	//report stats to output file and reset counters
	outputFile << "SJN Scheduling statistics" << endl;   
	printStats(outputFile);
	resetStats();


    // Close output file
    outputFile.close(); 

    return 0;
}

void createProcesses(){
    int pid = -1;
    float arrivalTime;
    float burstTime;

    // Open input file
    ifstream inputFile("input_processes.txt");

    // Read processes from the input file
    
    while (inputFile >> arrivalTime >> burstTime) {
        pid +=1;				
        processes.push_back(Process(pid, arrivalTime, burstTime));
    }

    // Close input file
    inputFile.close();
    numProcesses = processes.size();
     
    return;
}

void resetStats(){
	currentTime = 0.0;
    	totalTurnaroundTime = 0.0;
    	totalWaitTime = 0.0;
    	queueLength = 0;
    	queueCount = 0;
    	maxQueueLength = 0;
}

void printStats(ostream& output){

    output << "Total Simulation Time: " << currentTime << " seconds" << endl;
    output << "Total Processes: " << numProcesses << endl;
    output << "Average Turnaround Time: " << totalTurnaroundTime/numProcesses << " seconds" << endl;
    output << "Average Process Wait Time: " << totalWaitTime/numProcesses <<" seconds" << endl;
    output << "Average Queue Length: " << (float)queueLength/queueCount << endl;
    output << "Maximum Queue Length: " << maxQueueLength << endl;
    output << "=====================================" << endl;  
}

void arrivalEvent(){
	
	while (!jobList.empty() && jobList.front().arrivalTime <= currentTime) {
            readyQueue.push_back(jobList.front());
            jobList.erase(jobList.begin());
        }  
}

//Required for std::sort() function to work for the readyQueue

bool operator<(const Process& a, const Process& b) {
          return a.burstTime < b.burstTime;
}
    

  
// SJN Scheduler
void sjnScheduler(vector<Process>& processes) {
    

    while (!processes.empty() || !readyQueue.empty()) {
        // Enqueue processes that have arrived
        arrivalEvent();
	
        if (!readyQueue.empty()) {
            //Take queue stats
            queueLength += readyQueue.size();
	    queueCount++;
            maxQueueLength = maxQueueLength > readyQueue.size()? maxQueueLength : readyQueue.size();

	    //Sort queue
	    sort (readyQueue.begin(), readyQueue.end());
	    

            // Schedule and run process
            Process currentProcess = readyQueue.front();
            readyQueue.pop_front();

	    //currentProcess.cpuRun(int currentTime, int quantum);
	    if (currentProcess.firstRun){
	    currentProcess.firstRun = false;
            // Calculate wait time
            totalWaitTime += currentTime - currentProcess.arrivalTime;
	    }

            // Calculate turnaround time
            totalTurnaroundTime += currentTime + currentProcess.burstTime - currentProcess.arrivalTime;

            
            // Process finished
            currentTime += currentProcess.burstTime;

            cout << "SJN completed process with arrival time " << currentProcess.arrivalTime
                 << " and burst time " << currentProcess.burstTime << endl;
            cout << "Completion time: " << currentTime << endl;
            cout << "-------------------------------------" << endl;
        } else {
            // No processes in the ready queue, move to the next arrival time
            currentTime = processes.front().arrivalTime;
        }
    }   
}


// FCFS Scheduler
void fcfsScheduler(vector<Process>& processes) {
    

    while (!processes.empty() || !readyQueue.empty()) {
        // Enqueue processes that have arrived
        arrivalEvent();

        if (!readyQueue.empty()) {
            // Update queue length and count
            queueLength += readyQueue.size();
	    queueCount++;

            // Update max queue length

            maxQueueLength = maxQueueLength > readyQueue.size()? maxQueueLength : readyQueue.size();

            // Schedule and run process
            Process currentProcess = readyQueue.front();
            readyQueue.pop_front();

	    //currentProcess.cpuRun(int currentTime, int quantum);
	    if (currentProcess.firstRun){
	    currentProcess.firstRun = false;
            // Calculate wait time
            totalWaitTime += currentTime - currentProcess.arrivalTime;
	    }

            // Calculate turnaround time
            totalTurnaroundTime += currentTime + currentProcess.burstTime - currentProcess.arrivalTime;

            
            // Process finished
            currentTime += currentProcess.burstTime;

            cout << "FCFS completed process with arrival time " << currentProcess.arrivalTime
                 << " and burst time " << currentProcess.burstTime << endl;
            cout << "Completion time: " << currentTime << endl;
            cout << "-------------------------------------" << endl;
        } else {
            // No processes in the ready queue, move to the next arrival time
            currentTime = processes.front().arrivalTime;
        }
    }   
}


// RR Scheduler
void rrScheduler(vector<Process>& jobList, float timeQuantum, float contextSwitchTime) {
   

    while (!jobList.empty() || !readyQueue.empty()) {
        // Enqueue processes that have arrived
        arrivalEvent();

        if (!readyQueue.empty()) {
            // Update queue length and count
            queueLength += readyQueue.size();
	    queueCount++;

            // Update max queue length

            maxQueueLength = maxQueueLength > readyQueue.size()? maxQueueLength : readyQueue.size();

            // Schedule process
            Process currentProcess = readyQueue.front();
            readyQueue.pop_front();

	    // Calculate wait time
	    if (currentProcess.firstRun){
	    currentProcess.firstRun = false;
            totalWaitTime += currentTime - currentProcess.arrivalTime;
	    }
	    
	    //Run current process
            if (currentProcess.burstTime > timeQuantum) {
                // Process wouldn't finish in this time quantum, reduce burst time
                currentProcess.burstTime -= timeQuantum;
                currentTime += timeQuantum;
		}
	    else {
             
                currentTime += (timeQuantum < currentProcess.burstTime ? timeQuantum : currentProcess.burstTime);
	        currentProcess.burstTime = 0;
	        // Calculate turnaround time
                totalTurnaroundTime += currentTime - currentProcess.arrivalTime;
	        cout << "RR completed process with arrival time " << currentProcess.arrivalTime << endl;
		printf ("Completion time: %0.2f", currentTime);
                cout << endl << "-------------------------------------" << endl;
		}

             // Simulate context switch
             arrivalEvent();
	     if(!readyQueue.empty())currentTime += contextSwitchTime;
	     if(currentProcess.burstTime) readyQueue.push_back(currentProcess);
        } else {
            // No processes in the ready queue, move to the next arrival time
            currentTime = jobList.front().arrivalTime;
        }
    }

}


// ============== END ===========================
