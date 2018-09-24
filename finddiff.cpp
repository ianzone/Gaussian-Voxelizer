#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
	string string1, string2;
	argv++; argc--;
	string1 = *argv;
	argv++; argc--;
	string2 = *argv;

    ifstream s1(string1);
    if(!s1)
    {
        cerr<<"can not open 1st input file!"<<endl;
        exit(1);
    }

    ifstream s2(string2);
    if(!s2)
    {
        cerr<<"can not open 2nd input file!"<<endl;
        exit(2);
    }

    char char1, char2;
    vector<int> coo = {-1, -1, -1};
    vector<vector<int>> coolist1, coolist2;
    for (int i = 0; i < 128; ++i)
    {
    	for (int j = 0; j < 128; ++j)
    	{
    		for (int k = 0; k < 128; ++k)
    		{
    			s1 >> char1;
    			s2 >> char2;
    			if (char1 - char2 > 0)
    			{
    				coo = {k, i, j};
    				coolist1.push_back(coo);
    			}
    			else if (char2 - char1 > 0)
    			{
    				coo = {k, i, j};
    				coolist2.push_back(coo);
    			}
    		}
    	}
    }

    cout << string1 << " - " << string2 << endl;
    for (auto cor : coolist1)
    {
    	cout << cor[0] <<", "<< cor[1]<<", "<< cor[2]<<endl;
    }

	cout << endl << string2 << " - " << string1 << endl;
    for (auto cor : coolist2)
    {
    	cout << cor[0] <<", "<< cor[1]<<", "<< cor[2]<<endl;
    }
	return 0;
}