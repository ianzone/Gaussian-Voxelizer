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

    ofstream output("out.off");
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

    double coor, x, y, z, minx, miny, minz, maxx, maxy, maxz;
    int count = 1, init = 1;
    for (int i = 0; i < 3*vertex; i++)
    {
    	data >> s;
    	coor = stod(s);
    	
    	if (count == 1)
    	{
    		output << coor << " ";
    		x = coor;
    	}
    	else if (count == 2)
    	{
    		output << coor << " ";
    		y = coor;
    	}
    	else
    	{
    		output << coor << endl;
    		z = coor;
    		if (init)
	    	{
	    		minx = maxx = x;
	    		miny = maxy = y;
	    		minz = maxz = z;
	    		init = 0;
	    	}
    		count = 0;
    	}
    	count++;
    	
    	if (count == 1)
    	{
    		if (x < minx)
    		{
    			minx = x;
    		}
    		else if (x > maxx)
    		{
    			maxx = x;
    		}
    		if (y < miny)
    		{
    			miny = y;
    		}
    		else if (y > maxy)
    		{
    			maxy = y;
    		}
    		if (z < minz)
    		{
    			minz = z;
    		}
    		else if (z > maxz)
    		{
    			maxz = z;
    		}
    	}
	}

	cout << "min(x, y, z): " << minx << " " << miny << " " << minz << endl;
	cout << "max(x, y, z): " << maxx << " " << maxy << " " << maxz << endl;

    int shape, newface = face, newedge = edge;
    string tem1, tem2;
    for (int i = 0; i < face; ++i)
    {
    	data >> s;
    	shape = stoi(s);
		if (shape == 3)
		{
			output << s;
			getline(data,s);
			output << s << endl;
		}
		else if (shape == 4)
		{
			output << "3 ";
            data >> s;//1
            output << s << " ";//1
            tem1 = s;//1
            data >> s;//2
            output << s << " ";//2
            data >> s;//3
            output << s << endl;//3
            output <<"3 "<< s <<" "; 
            data >> s;//4
            output << s <<" "<< tem1 << endl;
            newface += 1;
            newedge += 1;
		}
		else if (shape == 5)
		{
			output << "3 ";
			data >> s;//1
			output << s << " ";//1
			tem1 = s; //1
			data >> s;//2
			output << s << " ";//2
			data >> s;//3
			output << s << endl;//3
			tem2 = s;//3

			output << "3 ";
			output << tem2 << " ";//3
			data >> s;//4
			output << s << " ";//4
			data >> s;//5
			output << s << " " << endl;//5
			output << "3 " << s <<" "<< tem1 <<" "<<tem2 << endl;
			newface += 2;
			newedge += 2;
		}
		else if (shape == 6)
		{
			output << "3 ";
			data >> s;//1
			output << s << " ";//1
			tem1 = s;//1
			data >> s;//2
			output << s << " ";//2
			data >> s;//3
			output << s << endl;//3

            output<<"3 ";
            output << s << " ";//3  
            tem2 = s; //3         
			data >> s;//4
			output << s << " ";//4
            data >> s;//5
            output << s <<endl;//5

            output<<"3 ";
            output << s <<" ";//5
            data >> s;//6
            output << s << " ";//6
            output << tem2 << endl;//3

			output << "3 ";
            output << s <<" ";//6
			output << tem1 << " ";//1
			output << tem2 << endl;//3
			newface += 3;
			newedge += 3;
		}
		else cout << "more than 6 vertices" << endl;
    }
    
    output.seekp(flag, ios::beg);
    output << newface;

    output.close();
    data.close();
	return 0;
}