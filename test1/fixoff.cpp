#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

int main(int argc, char const *argv[])
{
	if (argc!=2)
	{
		cerr<<"usage: meshmovexyz1 [off file]"<<endl;
		exit(1);
	}

	ifstream sourceData(argv[1]);
	if(!sourceData)
    {
        cerr<<"can not open source file!"<<endl;
        exit(2);
    }

    ofstream outfile("out.off");
    if(!outfile)
    {
        cerr<<"can not open output file!"<<endl;
        exit(3);
    }

    int flag;
    string s;
    cout << "Header info:" << endl;
    getline(sourceData,s);
    cout << s << endl;
    outfile << s << endl;
    flag = s.size() + 1;
    getline(sourceData,s);
    cout << s << endl;
    outfile << s << endl;
    flag += s.size() + 1;

    int vertex;
    sourceData >> s;
    cout << "vertex:" << s << endl;
    outfile << s << " ";
    flag = flag + s.size() + 1;
    vertex = stoi(s);

    int face;
    sourceData >> s;
    cout << "face:" << s << endl;
    outfile << s << " ";
    face = stoi(s);

    int edge;
    sourceData >> s;
    cout << "edge:" << s << endl;
    outfile << s << endl;
    edge = stoi(s);

    double check, x, y, z, minx, minxy, minxz, miny, minyx, minyz, minz, minzx, minzy,
    	maxx, maxxy, maxxz, maxy, maxyx, maxyz, maxz, maxzx, maxzy;
    int count = 1, init = 1;
    for (int i = 0; i < 3*vertex; i++)
    {
    	sourceData >> s;
    	check = stod(s);
    	if (fmod(check, 0.5))
    	{
    		cout << "not an int or half-int" << endl;
    		if ((int)check + 0.5 < check)
    		{
    			check = (int)check + 1;
    		}else{
    			check = (int)check + 0.5;
    		}
    	}
    	if (count == 1)
    	{
    		outfile << check << " ";
    		x = check;
    	}
    	else if (count == 2)
    	{
    		outfile << check << " ";
    		y = check;
    	}
    	else
    	{
    		outfile << check << endl;
    		z = check;
    		if (init)
	    	{
	    		minx = minyx = minzx = maxx = maxyx = maxzx = x;
	    		miny = minxy = minzy = maxy = maxxy = maxzy = y;
	    		minz = minxz = minyz = maxz = maxxz = maxyz = z;
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
    			minxy = y;
    			minxz = z;
    		}
    		else if (x > maxx)
    		{
    			maxx = x;
    			maxxy = y;
    			maxxz = z;
    		}
    		if (y < miny)
    		{
    			miny = y;
    			minyx = x;
    			minyz = z;
    		}
    		else if (y > maxy)
    		{
    			maxy = y;
    			maxyx = x;
    			maxyz = z;
    		}
    		if (z < minz)
    		{
    			minz = z;
    			minzx = x;
    			minzy = y;
    		}
    		else if (z > maxz)
    		{
    			maxz = z;
    			maxzy = y;
    			maxzx = x;
    		}
    	}
	}

	cout << "with min x: " << minx << " " << minxy << " " << minxz << endl;
	cout << "with min y: " << minyx << " " << miny << " " << minyz << endl;
	cout << "with min z: " << minzx << " " << minzy << " " << minz << endl;
	cout << "with max x: " << maxx << " " << maxxy << " " << maxxz << endl;
	cout << "with max y: " << maxyx << " " << maxy << " " << maxyz << endl;
	cout << "with max z: " << maxzx << " " << maxzy << " " << maxz << endl;

    int shape, tem1, tem2, newface = face, newedge = edge;
    for (int i = 0; i < face; ++i)
    {
    	sourceData >> s;
    	shape = stoi(s);
		if (shape == 3)
		{
			outfile << s;
			getline(sourceData,s);
			outfile << s << endl;
		}
		
		else if (shape == 4)
		{
			outfile << s;
			getline(sourceData,s);
			outfile << s << endl;
		}
		else if (shape == 5)
		{
			outfile << "3" << " ";
			sourceData >> s;
			outfile << s << " ";
			tem1 = stoi(s);
			sourceData >> s;
			outfile << s << " ";
			sourceData >> s;
			outfile << s << endl;
			tem2 = stoi(s);
			outfile << "4" << " ";
			outfile << tem2 << " ";
			sourceData >> s;
			outfile << s << " ";
			sourceData >> s;
			outfile << s << " ";
			outfile << tem1 << endl;
			newface += 1;
			newedge += 1;
		}
		else if (shape == 6)
		{
			outfile << "4" << " ";
			sourceData >> s;
			outfile << s << " ";
			tem1 = stoi(s);
			sourceData >> s;
			outfile << s << " ";
			sourceData >> s;
			outfile << s << " ";
			sourceData >> s;
			outfile << s << endl;
			tem2 = stoi(s);
			outfile << "4" << " ";
			outfile << tem2 << " ";
			sourceData >> s;
			outfile << s << " ";
			sourceData >> s;
			outfile << s << " ";
			outfile << tem1 << endl;
			newface += 1;
			newedge += 1;
		}
		else cout << "more than 6 vertices" << endl;
    }
    
    outfile.seekp(flag, ios::beg);
    outfile << newface << " " << newedge;

    outfile.close();
    sourceData.close();
	return 0;
}