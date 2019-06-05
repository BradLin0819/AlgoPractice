#include<fstream>
#include<iostream>
#include<sstream>
#include<queue>
#include<vector>
#include<map>
#define TYPE_OF_ALU 2
using namespace std;

class Vertex{
	private:
		int op;
		int in1;
		int in2;
	    int id;
		int priority;
		int finished;
		int latency;
		bool scheduled;
		vector<Vertex*> next;
	public:
		Vertex();
		Vertex(int op=0, int in1=0, int in2=0, 
			   int id=0, int latency=0, bool scheduled=false, 
			   int finished=-1):op(op), in1(in1), in2(in2), 
			   id(id), latency(latency), scheduled(scheduled), 
			   finished(finished){}
		int getop() const;
		int getin1() const;
		int getin2() const;
		int getid() const;
		int getlatency() const;
		bool isScheduled() const;
		void addNext(Vertex *v);
		const vector<Vertex*>& getNext() const;
		void setScheduled();
		void setPriority(int priority);
		int getPriority() const;
		void setFinished(int fin_cycle);
		bool isFinished(int cycle) const;
};

class ALU
{
	private:
		int latency;
		int usedCycle;
	public:
		ALU(){ latency = 0; usedCycle = -10000;}
		ALU(int latency=0, int usedCycle = -10000):latency(latency), usedCycle(usedCycle){}
		bool isAvailable(int currentCycle);
		void setUsedCycle(int cycle);	
};

struct compare
{
    bool operator()(Vertex* lhs, Vertex* rhs)
    {
    	if(lhs->getPriority() == rhs->getPriority())
    	{
    		return lhs->getid() > rhs->getid();
		}
        return lhs->getPriority() < rhs->getPriority();
    }
};

string uppercase(string str);
bool isRoot(Vertex *v, const map<int, Vertex*>& allNode);
bool hasInput(Vertex *v, const map<int, Vertex*>& allNode);
int getCriticalPath(Vertex *v);
bool canBeScheduled(Vertex *v, const map<int, Vertex*>& allNode);
int readyListSize(priority_queue<Vertex*, vector<Vertex*>, compare> ready_list[TYPE_OF_ALU]);

int main()
{
	while(1)
	{
		int op_N = 0;
		int op = 0, in1 = 0, in2 = 0, out = 0;
		int cycle = 0;
		int latency[TYPE_OF_ALU] = {0};
		int alu[TYPE_OF_ALU] = {0};
		string alu_type[] = {"Add", "Mul"};
		string fname;
		map<int, Vertex*> allNode;
		map<int, Vertex*>::iterator iter1;
		map<int, Vertex*>::iterator iter2;
		priority_queue<Vertex*, vector<Vertex*>, compare> ready_list[TYPE_OF_ALU];
		vector<ALU*> ALUs[TYPE_OF_ALU];
		
		cout << "Name of input file (Input 0 to exit program):";
		cin >> fname;
		if(fname == "0")
		{
			cout << "End." << endl;
			break;
		}
		
		ifstream file(fname.c_str(), ifstream::in);
		if(!file.is_open())
		{
			cout << "File doesn't exist!" << endl;
			continue;
		}
		//set latency of operations
		if(uppercase(fname) == "DFG1.TXT")
		{
			latency[0] = 1;
			latency[1] = 2;
		}
		else
		{
			latency[0] = 1;
			latency[1] = 1;
		}
		cout << "Number of multiplication units:";
		cin >> alu[1];
		cout << "Number of addition units:";
		cin >> alu[0];
		
		for(int i = 0; i < TYPE_OF_ALU; ++i)
		{
			for(int j = 0; j < alu[i]; ++j)
			{
				ALU *temp = new ALU(latency[i]);
				ALUs[i].push_back(temp);
			}
		}
		file >> op_N;
		while(file >> op >> in1 >> in2 >> out)
		{
			Vertex *v = new Vertex(op, in1, in2, out, latency[op-1]);
			allNode[out] = v;
		}
		//Establish edge set
		for(iter1 = allNode.begin(); iter1 != allNode.end(); ++iter1)
		{
			for(iter2 = allNode.begin(); iter2 != allNode.end(); ++iter2)
			{
				if(iter1->second->getid() == iter2->second->getin1() || iter1->second->getid() == iter2->second->getin2())
				{
					iter1->second->addNext(iter2->second);
				}
			}
		}
		//Calculate priorities of nodes
		for(iter1 = allNode.begin(); iter1 != allNode.end(); ++iter1)
		{
			iter1->second->setPriority(getCriticalPath(iter1->second));
		}
		//push root nodes into the ready list
		for(iter1 = allNode.begin(); iter1 != allNode.end(); ++iter1)
		{
			if(isRoot(iter1->second, allNode))
			{
				int type = iter1->second->getop();
				ready_list[type - 1].push(iter1->second);
			}
		}
		//List Scheduling 
		vector<Vertex*> temp_list;
		bool print_control = false;
		
		cout << endl << endl << "Name of Input file:" << fname << endl;
		cout << "Number of multiplication units: " << alu[1] << endl
			 << "Number of addition units: " << alu[0] << endl << endl
		     << "Result of scheduling:" << endl << endl
	         << "Cycle\t  Operation" << endl;
		while(readyListSize(ready_list) + temp_list.size() > 0)
		{
			if(!print_control)
			{
				cout << "  " << cycle << "\t "; 
			}
			for(int i = 0; i < TYPE_OF_ALU; ++i)
			{
				for(int j = 0; j < alu[i]; ++j)
				{
					if(!ready_list[i].empty())
					{
						Vertex *first = ready_list[i].top();
						if(ALUs[i][j]->isAvailable(cycle))
						{
							//alu is being used
							ALUs[i][j]->setUsedCycle(cycle);
							//set the end cycle of the operation
							first->setFinished(cycle + first->getlatency());
							//push the operation that is selected into the list
							temp_list.push_back(first);
							cout << alu_type[first->getop()-1] 
								 << "(" << first->getid() << ") ";
							//pop the operation that was scheduled
							ready_list[i].pop();	
						}	
					}
				}
			}
			print_control = false;
			cycle++;
			for(int i = 0; i < temp_list.size();)
			{
				vector<Vertex*> next = temp_list[i]->getNext();
				//check if the operation is done
				if(temp_list[i]->isFinished(cycle))
				{
					//update the status of the operation
					temp_list[i]->setScheduled();
					for(int j = 0; j < next.size(); ++j)
					{
						//push new operations into the ready list
						if(canBeScheduled(next[j], allNode))
						{
							ready_list[next[j]->getop() - 1].push(next[j]);
						}
					}
					temp_list.erase(temp_list.begin() + i);
				}
				//the operation is not done yet
				else
				{
					if(!print_control)
					{
						cout << endl << "  " << cycle << "\t "; 
						print_control =	true;
					} 
					cout << alu_type[temp_list[i]->getop()-1] 
					     << "(" << temp_list[i]->getid() << ") ";
					i++;
				}	
			}
			if(!print_control)
			{
				cout << endl;
			}
		}
		
		//delete all nodes
		for(iter1 = allNode.begin(); iter1 != allNode.end(); ++iter1)
		{
			delete iter1->second;
		}
		cout << endl << "------------------------" << endl;
		file.close();
	}
	return 0;
}

Vertex::Vertex()
{
	op = 0;
	in1 = 0;
	in2 = 0;
	id = 0;
	priority = 0;
	scheduled = false;
}

int Vertex::getop() const
{
	return op;
}

int Vertex::getin1() const
{
	return in1;
}

int Vertex::getin2() const
{
	return in2;
}

int Vertex::getid() const
{
	return id;
}

int Vertex::getlatency() const
{
	return latency;
}

bool Vertex::isScheduled() const
{
	return scheduled;
}

void Vertex::setScheduled()
{
	scheduled = true;
}
	
void Vertex::addNext(Vertex *v)
{
	next.push_back(v);
}

const vector<Vertex*>& Vertex::getNext() const
{
	return next;
}

void Vertex::setPriority(int priority)
{
	this->priority = priority;
}

int Vertex::getPriority() const
{
	return priority;
}

void Vertex::setFinished(int fin_cycle)
{
	finished = fin_cycle;
}

bool Vertex::isFinished(int cycle) const
{
	return cycle == finished;
}

bool ALU::isAvailable(int currentCycle)
{
	return (currentCycle - usedCycle) >= latency;
}

void ALU::setUsedCycle(int cycle)
{
	usedCycle = cycle;
}

string uppercase(string str)
{
	string temp = str;
	for(int i = 0; i < str.length(); ++i)
	{
		temp[i] = toupper(str[i]);
	}
	return temp;
}

bool isRoot(Vertex *v, const map<int, Vertex*>& allNode)
{
	int in1 = v->getin1();
	int in2 = v->getin2();
	
	return (allNode.find(in1) == allNode.end() && allNode.find(in2) == allNode.end());
}

bool hasInput(Vertex *v, const map<int, Vertex*>& allNode)
{
	int in1 = v->getin1();
	int in2 = v->getin2();
	
	return (allNode.find(in1) == allNode.end() || allNode.find(in2) == allNode.end()); 
}

bool canBeScheduled(Vertex *v, const map<int, Vertex*>& allNode)
{
	Vertex *in1 = allNode.find(v->getin1())->second;
	Vertex *in2 = allNode.find(v->getin2())->second;

	return isRoot(v, allNode) || (in1->isScheduled() && in2->isScheduled())
	       || (hasInput(v, allNode) && (in1->isScheduled() || in2->isScheduled()));
}

int getCriticalPath(Vertex *v)
{
	int weight = 0;
	int max = 0;
	
	if(v->getNext().size() == 0)
		return v->getlatency();
	for(int i = 0; i < v->getNext().size(); ++i)
	{
		weight = getCriticalPath(v->getNext()[i]);
		if(weight >= max)
		{
			max = weight;
		}
	}
	return v->getlatency() + max;
}

int readyListSize(priority_queue<Vertex*, vector<Vertex*>, compare> ready_list[TYPE_OF_ALU])
{
	int sum = 0;
	for(int i = 0; i < TYPE_OF_ALU; ++i)
	{
		sum += ready_list[i].size();
	}
	return sum;
}

