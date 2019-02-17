/*
	Irving Alvarez
*/

#include <iostream>
#include <string>

using namespace std;

class Queue
{
	class Node
	{
		public:
			int arrival_time;
			double total_service_time;
			double rem_service_time;
			double wait_time;
			double turnaround_time;
			double exit_time;
			bool served;
			Node* next;

		Node()
		{
			next = NULL;
			arrival_time = total_service_time = rem_service_time = 0;
			wait_time = turnaround_time = exit_time = 0;		
		}
		Node(int a, double s)
		{
			next = NULL;
			arrival_time = a;
			total_service_time = s;
			rem_service_time = s;
			wait_time = turnaround_time = exit_time = 0;
			served = false;
		}
	};
	Node* head;
	Node* tail;
	int curr_pros;
	int total_pros;

	// Insert a new node to the back of the list
	// Method for new ready processes
	void enqueue(int a, double s)
	{
		Node* new_node = new Node(a, s);

		// First node
		if (empty())
			head = new_node;
		else
			tail->next = new_node;
		
		tail = new_node;
	}
	// Insert a node to the back of the list
	// Method for preempting processes
	void pushBack(Node* p)
	{
		if(head == NULL)
			head = p;
		else
			tail->next = p;

		tail = p;
		tail->next = NULL;
	}
	// Return the front node from the list
	Node* dequeue()
	{
		if(empty())
			return NULL;

		Node* front = head;

		head = head->next;

		// Last node
		if(head == NULL)
			tail = NULL;

		return front;
	}
	
	public:

	Node* curr_pro;	// Next ready process

	Queue()
	{
		total_pros = 0;
		head = tail = NULL;
	}
	~Queue()
	{
		if(head == tail){
			delete head;
		}else{
			Node* temp = head;
			while(temp != NULL){
				head = head->next;
				delete temp;
				temp = head;
			}
		}
		head = tail = NULL;
	}
	
	/* Round Robin Interface Methods */
	
	// Insert a new process in the ready list(queue)
	void addProcess(int a, double s)
	{
		enqueue(a, s);
		total_pros += 1;
	}
	// Return the process at the front of the ready list for execution
	Node* dispatch()
	{
		return dequeue();
	}
	// Stop the current process from executing and put it at the end of the list
	void preempt(Node* p)
	{
		pushBack(p);
	}
	
	/* Utitily Methods */

	// Display all current nodes in the list
	void display() const
	{
		cout << endl << "--------------PROCESSES----------------" << endl;
		Node* temp = head;
		while (temp != NULL){
			cout << temp->arrival_time << " " << temp->rem_service_time << endl;
			temp = temp->next;
		}
	}
	// Number of items(processes) in test data file
	int dataSize() const
	{
		return total_pros;
	}
	// Return true if the list is empty
	bool empty() const
	{ 
		return head == NULL; 
	}
};