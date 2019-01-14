#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

int main(int argc, char const *argv[])
{
	if (argc!=2)
	{
		cerr<<"usage: ./fixoff.out [off file]"<<endl;
		exit(1);
	}

	ifstream data(argv[1]);
	if(!data)
    {
        cerr<<"can not open source file!"<<endl;
        exit(2);
    }

    ofstream output("righthand.off");
    if(!output)
    {
        cerr<<"can not open output file!"<<endl;
        exit(3);
    }

    string s;
    cout << "Header info:" << endl;
    getline(data,s);
    cout << s << endl;
    output << s << endl;
    auto flag = s.size() + 1;

    getline(data,s);
    cout << s << endl;
    output << s << endl;
    flag += s.size() + 1;

    int vertex;
    data >> s;
    cout << "vertex:" << s << endl;
    output << s << " ";
    flag += s.size() + 1;
    vertex = stoi(s);

    int face;
    data >> s;
    cout << "face:" << s << endl;
    output << s << " ";
    face = stoi(s);

    int edge;
    data >> s;
    cout << "edge:" << s << endl;
    output << s << endl;
    edge = stoi(s);

    getline(data, s);
    for (int i = 0; i < vertex; i++)
    {
    	getline(data, s);
        output << s << endl;
	}
    
    string v1, v2, v3;
    for (int i = 0; i < face; ++i)
    {
    	data >> s;
    	data >> v1;
        data >> v2;
        data >> v3;
        output << "3 " << v3 <<" "<< v2 <<" "<< v1 << endl;		
    }

    output.close();
    data.close();
	return 0;
}