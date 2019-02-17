/*
	Irving Alvarez

	Round Robin Scheduler Simulator

	A simulation of how a computer with a single
	processor schedules processes for execution.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "queue.h"

using namespace std;

// A useful named type
typedef double TIME;

// Reads the file that contains the testing data for
// the simulation(the processes metrics) and inserts them in a queue
void importData(Queue* _LIST)
{
	ifstream rr_data("test_data/data_set");

	if(rr_data.is_open()){

		double metric_A, metric_B;

		// Read all data
		rr_data >> metric_A >> metric_B;
		while(!rr_data.eof()){
			_LIST->addProcess(metric_A, metric_B);
			//cout << inputA << " " << inputB << endl; // Debug
			rr_data >> metric_A >> metric_B;
		}
	}
}
void setInterval(double &Time_Quantum, double &Context_Switch)
{
	// Prompt user
	cout << "Time Quantum: ";
	cin >> Time_Quantum;
	cout << "Context Switch: ";
	cin >> Context_Switch;

	// Convert to ms
	Time_Quantum = Time_Quantum/1000;
	Context_Switch = Context_Switch/1000;
}
int main()
{
	// Scheduler variables
	TIME Time_Quantum;
	TIME Context_Switch;

	// Metric variables
	TIME Exec_Time;
	TIME Avg_Wait_Time, Avg_Turnaround_Time;
	TIME Extra_Time, Shared_Time;
	TIME time_unit = 0.001; // 1ms, one CPU unit time

	Queue _LIST;	// Ready list process's
	
	// Prompt user
	printf("***Round Robin Simulator***\n\n""Enter a Time Quantum and a Context Switch time:\n");

	// Set the simulation parameters
	setInterval(Time_Quantum, Context_Switch);

	// Read the simulation data
	importData(&_LIST);
	_LIST.display();
	
	// Begin round robin
	while(!_LIST.empty())
	{
		//_LIST.display(); // Debug

		// Dispatch process
		_LIST.curr_pro = _LIST.dispatch();
		Extra_Time = Shared_Time;

		// Set the waiting time
		if(!_LIST.curr_pro->served){
			_LIST.curr_pro->wait_time = abs(Exec_Time-_LIST.curr_pro->arrival_time);
			_LIST.curr_pro->served = true;
		}
		
		// Execute process
		Shared_Time = 0;
		for(double T=time_unit; T<=Time_Quantum+Extra_Time; T+=time_unit){

			// If process completes...
			if(_LIST.curr_pro->rem_service_time <= 0){
				_LIST.curr_pro->rem_service_time = 0;
				_LIST.curr_pro->turnaround_time = Exec_Time;

				Avg_Wait_Time += _LIST.curr_pro->wait_time;
				Avg_Turnaround_Time += _LIST.curr_pro->turnaround_time;

				Shared_Time = Time_Quantum - T;

				break;
			}
			_LIST.curr_pro->rem_service_time -= time_unit;
			Exec_Time += time_unit;
		}

		// Context switch
		for(double T=time_unit; T<=Context_Switch; T+=time_unit)
			Exec_Time += time_unit;

		// Preempt process if it needs more execution time
		if(_LIST.curr_pro->rem_service_time > 0)
			_LIST.preempt(_LIST.curr_pro);
		else
			delete _LIST.curr_pro;
	}

	cout << "Total processes: " << _LIST.dataSize() << endl;

	// Display results
	cout << endl << "Done..." << endl;
	printf("************************************************\n");
	cout << "Time Quantum: " << Time_Quantum*1000 << endl << "Context Switch: " << Context_Switch*1000 << endl;
	cout << "Total Execution Time: " << Exec_Time << "(ms)" << endl;
	cout << "Average Wait Time: " << Avg_Wait_Time/_LIST.dataSize() << "(ms)" << endl;
	cout << "Average Turnaround Time: " << Avg_Turnaround_Time/_LIST.dataSize() << "(ms)" << endl;
	printf("************************************************\n");

	return 0;
}