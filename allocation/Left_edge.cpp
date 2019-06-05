#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include <algorithm> 
using namespace std;

class Line{
	private:
		string id;
		float start_cycle;
		float end_cycle;
	public:
		Line(){}
		Line(string id="", float start_cycle=0.0, float end_cycle=0.0):id(id), start_cycle(start_cycle), end_cycle(end_cycle){}
		string getID() const;
		float getStart() const;
		float getEnd() const;
		
};
bool cmp(Line *l, Line *r)
{
	return l->getStart() < r->getStart();
}
int main()
{
	while(1)
	{
		string fname, line_name;
		int reg_index = 0;
		float start = 0.0, end = 0.0, last = 0.0;
		map<int, vector<string> > register_map;
		list<Line*> L;
		list<Line*>::iterator curr_index;
		list<Line*>::iterator temp_index;
		
		cout << "Please input name of file(input 0 to exit): ";
		cin >> fname;
		if(fname == "0")
		{
			break;
		}
		ifstream file(fname.c_str(), ifstream::in);
		if(!file.is_open())
		{
			cout << "File doesn't exist!" << endl;
			exit(1);
		}
		while(file >> line_name >> start >> end)
		{
			Line *new_ele = new Line(line_name, start, end);
			L.push_back(new_ele);
		}
		//sort by start times of operations in ascending order
		L.sort(cmp);
		//Left edge algorithm
		while(!L.empty())
		{
			reg_index++;
			Line *curr = L.front();
			Line *temp;
			curr_index = L.begin();
			last = 0.0;
			while(curr_index != L.end())
			{
				//put in the same register
				if(curr->getStart() >= last)
				{
					register_map[reg_index].push_back(curr->getID());
					last = curr->getEnd();
					temp = curr;
					temp_index = curr_index;
					curr_index++;
					curr = *curr_index;
					L.erase(temp_index);
					delete temp;
				}
				else
				{
					curr_index++;
					curr = *curr_index;
				} 
			}
		}
		//output
		cout << endl << fname << ':' << endl;
		map<int, vector<string> >::iterator iter;
		for(iter = register_map.begin(); iter != register_map.end(); ++iter)
		{
			cout << 'r' << iter->first << ":";
			for(int i = 0; i < iter->second.size(); ++i)
			{
				cout << ' ' << iter->second[i]; 
			}
			cout << endl;
		}
		cout << endl;
	}
	return 0;
}

string Line::getID() const
{
	return id;
}

float Line::getStart() const
{
	return start_cycle;
}

float Line::getEnd() const
{
	return end_cycle;
}

