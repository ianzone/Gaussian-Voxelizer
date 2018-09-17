#include <iostream>
#include <fstream>
#include <ios>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	if (argc!=2)
	{
		cerr<<"usage: vox2raw [vox file]"<<endl;
		exit(1);
	}

	ifstream voxdata(argv[1]);
	if(!voxdata)
    {
       cerr<<"open error!"<<endl;
       exit(2);
    }

    string s;
    getline(voxdata,s);
    int r = stoi(s);
    int space = (r+2)*(r+2)*(r+2);
    cout << "Resolution: " << r << endl;

    getline(voxdata,s);
    cout << "Header1: " << s << endl;
    getline(voxdata,s);
    cout << "Header2: " << s << endl;

    bool* mem = nullptr;
    if( !(mem = new bool[space]))
    {
       cout << "Error: out of memory." <<endl;
       exit(3);
    }
    for (int i = 0; i < space; i++)
    {
        mem[i] = 0;
    }

    cout << "Reading data" << endl;
    int x, y, z, j = 1;
    while( voxdata >> s )
    {    
        if (j == 1)
        {
            x = stoi(s) + 1;
        } 
        else if (j == 2)
        {
            y = stoi(s) + 1;
        }
        else if (j == 3)
        {
            z = stoi(s) + 1;
            mem[x + (r+2)*y + (r+2)*(r+2)*z] = 1;
            j = 0;
        }
        j++;
    }
    
    ofstream outfile("out.raw", ios::binary);
    if(!outfile)
    {
       cerr<<"output error!"<<endl;
       exit(4);
    }
    outfile.write((char*)mem, space);

    delete mem;
    outfile.close();
    voxdata.close();
	return 0;
}