#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <boost/rational.hpp>  

using namespace std;
using namespace boost;

typedef struct vertex
{
    rational<int> x, y, z;
}vect, vox;

struct face
{
    unsigned v1, v2, v3;
};

struct intersection 
{
    rational<int> value;    // coordinate of ray-mesh interSection point
    bool enter = false;     // ray is entering meSh
    bool exit  = false;     // ray is exiting mesh
    bool touch1 = false;    // touch means the ray is on the surface
    bool touch2 = false;    
    vector<int> triNum;
};

void cross_product(vect &p, vect a, vect b);
void cross_product(vect &p, vect a, vect b)
{
    p.x = a.y*b.z - a.z*b.y;
    p.y = a.z*b.x - a.x*b.z;
    p.z = a.x*b.y - a.y*b.x;
}

int intersectant(vect dir, vertex P, vertex A, vertex B, vertex C);
int intersectant(vect dir, vertex P, vertex A, vertex B, vertex C)
{
    vect PA = {A.x-P.x, A.y-P.y, A.z-P.z};
    vect PB = {B.x-P.x, B.y-P.y, B.z-P.z};
    vect PC = {C.x-P.x, C.y-P.y, C.z-P.z};
    vect papb, pbpc, pcpa;
    cross_product(papb, PA, PB);
    cross_product(pbpc, PB, PC);
    cross_product(pcpa, PC, PA);
    if (dir.x == 1)
        if (papb.x <= 0 && pbpc.x <= 0 && pcpa.x <= 0 || papb.x >= 0 && pbpc.x >= 0 && pcpa.x >= 0)
            return 1;
    else if (dir.y == 1)
        if (papb.y <= 0 && pbpc.y <= 0 && pcpa.y <= 0 || papb.y >= 0 && pbpc.y >= 0 && pcpa.y >= 0)
            return 1;
    else if (dir.z == 1)
        if (papb.z <= 0 && pbpc.z <= 0 && pcpa.z <= 0 || papb.z >= 0 && pbpc.z >= 0 && pcpa.z >= 0)
            return 1;
    return 0;
}

void insertPoint(vector<intersection> &pl, intersection &point, int count);
void insertPoint(vector<intersection> &pl, intersection &point, int count)
{
	point.triNum.push_back(count);
	if (pl.empty())
        pl.push_back(point);
    else for (auto pt = pl.begin(); pt != pl.end(); pt++)
    {
        if (point.value == (*pt).value)
        {
            (*pt).enter  = ((*pt).enter  || point.enter);
            (*pt).exit   = ((*pt).exit   || point.exit);
            (*pt).touch1 = ((*pt).touch1 || point.touch1);
            (*pt).touch2 = ((*pt).touch2 || point.touch2);
            (*pt).triNum.push_back(count);
            break;
        }
        else if (point.value < (*pt).value)
        {
            pl.insert(pt, point); // std::vector.insert()
            break;
        }
        if (pt == pl.end() - 1)
        {
            pl.push_back(point);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    /* reading inputs */
    argv++; argc--;
    string input, output, checkfile;
    vect dir = {1, 0, 0};
    int nx = 128, ny = 128, nz = 128;
    int sxmin = 0, symin = 0, szmin = 0;
    int sxmax = 255, symax = 255, szmax = 255;
    bool checkmode = false;
    while (argc > 0)
    {
        if ((*argv)[0] == '-')
        {
            if (!strcmp(*argv, "-i"))
            {
                argv++; argc--;
                input = *argv;
            }
            else if (!strcmp(*argv, "-o"))
            {
                argv++; argc--;
                output = *argv;
            }
            else if (!strcmp(*argv, "-d"))
            {
                argv++; argc--;
                if (!strcmp(*argv, "x")) {}
                else if (!strcmp(*argv, "y"))
                    dir = {0, 1, 0};
                else if (!strcmp(*argv, "z"))
                    dir = {0, 0, 1};
                else 
                    cerr << "unknown direction" << endl;
            }
            else if (!strcmp(*argv, "-smin"))
            {
                argv++; argc--;
                sxmin = stoi(*argv);
                argv++; argc--;
                symin = stoi(*argv);
                argv++; argc--;
                szmin = stoi(*argv);
            }
            else if (!strcmp(*argv, "-smax"))
            {
                argv++; argc--;
                sxmax = stoi(*argv);
                argv++; argc--;
                symax = stoi(*argv);
                argv++; argc--;
                szmax = stoi(*argv);
            }
            else if (!strcmp(*argv, "-n"))
            {
                argv++; argc--;
                nx = stoi(*argv);
                argv++; argc--;
                ny = stoi(*argv);
                argv++; argc--;
                nz = stoi(*argv);
                if (!(nx > 0 && ny > 0 && nz > 0))
                {
                    cerr<<"invalid voxel number"<<endl;
                    exit(0);
                }
            }
            else if (!strcmp(*argv, "-check"))
            {
                argv++; argc--;
                checkfile = *argv;
                checkmode = true;
            }
        }
        argv++; argc--;
    }

    ifstream DATA(input);
    if(!DATA)
    {
        cerr<<"can not open input file!"<<endl;
        exit(0);
    }
    auto dataHead = DATA.tellg();

    ofstream OUTPUT(output);
    if(!OUTPUT)
    {
        cerr<<"can not open output file!"<<endl;
        exit(0);
    }

    ifstream checkData;
    ofstream checkMesh;
    ofstream lostVoxel26Connected;
    if (checkmode == true)
    {
    	checkMesh.open("checkMesh.off");
	    if (!checkMesh.is_open())
	    {
	    	cerr<<"can not create checkMesh.off!"<<endl;
	        exit(0);
	    }

	    checkData.open(checkfile);
		if(!checkData.is_open())
	    {
	        cerr<<"can not open check file!"<<endl;
	        exit(0);
	    }

	    lostVoxel26Connected.open("lostVoxel26Connected.raw");
	    if (!lostVoxel26Connected.is_open())
	    {
	    	cerr<<"can not create lostVoxel26Connected.raw!"<<endl;
	        exit(0);
	    }
    }
 
    /* reading inputs */

    /* skip first two lines */
    string S;
    getline(DATA,S);
    getline(DATA,S);

    int vertexAmt;
    DATA >> S;
    cout << "vertex:" << S;
    vertexAmt = stoi(S);

    int faceAmt;
    DATA >> S;
    cout << "  face:" << S;
    faceAmt = stoi(S);

    int edgeAmt;
    DATA >> S;
    cout << "  edge:" << S << endl;
    edgeAmt = stoi(S);
    /* skip first two lines */

    /* shift mesh space, store cooridinates as rational number and check user input*/
    vector<vertex> Vertex(vertexAmt);

    DATA >> S;
    auto len = S.size();
    double s = stod(S);
    rational<int> x(s*pow(10, len), pow(10, len));
    Vertex[0].x = x - sxmin;
    auto xmin = x, xmax = x;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> y(s*pow(10, len), pow(10, len));
    Vertex[0].y = y - symin;
    auto ymin = y, ymax = y;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> z(s*pow(10, len), pow(10, len));
    Vertex[0].z = z - szmin;
    auto zmin = z, zmax = z;

    for (int i = 1; i < vertexAmt; i++)
    {
        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> x(s*pow(10, len), pow(10, len));
        Vertex[i].x = x - sxmin;
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> y(s*pow(10, len), pow(10, len));
        Vertex[i].y = y - symin;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> z(s*pow(10, len), pow(10, len));
        Vertex[i].z = z - szmin;
        if (z < zmin)
            zmin = z;
        else if (z > zmax)
            zmax = z;
    }
    //cout << "bounding box: " << xmin <<" "<< ymin <<" "<< zmin <<" | "<< xmax <<" "<< ymax <<" "<< zmax << endl;//空间大小不得小于该数值
    /* shift mesh space, store cooridinates as rational number and check user input*/

    vox voxelsize;
    voxelsize.x = rational<int>(sxmax - sxmin, nx);
    voxelsize.y = rational<int>(symax - symin, ny);
    voxelsize.z = rational<int>(szmax - szmin, nz);
    cout << "mesh space min: " << sxmin <<" "<< symin <<" "<< szmin <<" | max: "<< sxmax <<" "<< symax <<" "<< szmax << endl;
    cout << "voxel size: " << voxelsize.x <<" "<< voxelsize.y <<" "<< voxelsize.z << endl;

    if (dir.x == 1)
        cout << "ray direction: x" << endl;
    else if (dir.y == 1)
        cout << "ray direction: y" << endl;
    else if (dir.z == 1)
        cout << "ray direction: z" << endl;

    /* store faces into vector */
    vector<face> Face(faceAmt);
    for (int i = 0; i < faceAmt; ++i)
    {
        DATA >> S;
        DATA >> S;
        unsigned s = stoi(S);
        Face[i].v1 = s;
        DATA >> S;
        s = stoi(S);
        Face[i].v2 = s;
        DATA >> S;
        s = stoi(S);
        Face[i].v3 = s;
    }
    /* store faces into vector */


    vector<vect> normalList(faceAmt);
    vector<vector<vector<intersection>>> pointList(ny);
    for (int i = 0; i < ny; ++i)
        pointList[i].resize(nz);

	int count = 0;
    for (auto f : Face) // for each triangle
    {
        auto x1 = Vertex[f.v1].x, y1 = Vertex[f.v1].y, z1 = Vertex[f.v1].z;
        auto x2 = Vertex[f.v2].x, y2 = Vertex[f.v2].y, z2 = Vertex[f.v2].z;
        auto x3 = Vertex[f.v3].x, y3 = Vertex[f.v3].y, z3 = Vertex[f.v3].z;
        vect vector1, vector2, normal;
        vector1.x = x2 - x1;
        vector1.y = y2 - y1;
        vector1.z = z2 - z1;
        vector2.x = x3 - x2;
        vector2.y = y3 - y2;
        vector2.z = z3 - z2;

        cross_product(normal, vector1, vector2);
        normalList.push_back(normal);
        auto d = normal.x*x1 + normal.y*y1 + normal.z*z1;
        //cout << endl << count++ << " triangle formula: " << normal.x <<"x + "<< normal.y <<"y + "<< normal.z <<"z = "<< d << endl;

        auto dotproduct = normal.x*dir.x + normal.y*dir.y + normal.z*dir.z;
        //cout << "dot production: " << dotproduct <<" "<<x1<<" "<<y1<<" "<<z1<<" | "<<x2<<" "<<y2<<" "<<z2<<" | "<<x3<<" "<<y3<<" "<<z3<< endl;

        /* triangle bounding box */
        vertex triboundmin, triboundmax;
        triboundmin.x = x1; triboundmax.x = x1;
        triboundmin.y = y1; triboundmax.y = y1;
        triboundmin.z = z1; triboundmax.z = z1;
        if (dir.x == 1)
        {
            // find triangle bounding box minimum and maximun coordinate
            if (y2 < triboundmin.y)
                triboundmin.y = y2;
            else if (y2 > triboundmax.y)
                triboundmax.y = y2;
            if (y3 < triboundmin.y)
                triboundmin.y = y3;
            else if (y3 > triboundmax.y)
                triboundmax.y = y3;
            if (z2 < triboundmin.z)
                triboundmin.z = z2;
            else if (z2 > triboundmax.z)
                triboundmax.z = z2;
            if (z3 < triboundmin.z)
                triboundmin.z = z3;
            else if (z3 > triboundmax.z)
                triboundmax.z = z3;
            /*cout << "triboundmin(y, z): " << triboundmin.y << " " << triboundmin.z << " | ";
            cout << "triboundmax(y, z): " << triboundmax.y << " " << triboundmax.z << endl;*/

            // minimum coordinate of triangle's related voxel
            unsigned numy = rational_cast<unsigned>(triboundmin.y / voxelsize.y);
            unsigned numz = rational_cast<unsigned>(triboundmin.z / voxelsize.z);
            auto trivoxelymin = numy * voxelsize.y;
            auto trivoxelzmin = numz * voxelsize.z;
            //cout << "voxelmin(y, z): " << trivoxelymin <<" "<< trivoxelzmin << endl;

            intersection point;

            unsigned i = numy;
            for (auto y = trivoxelymin; y <= triboundmax.y; y+=voxelsize.y, i++)
            {
                unsigned j = numz;
                for (auto z = trivoxelzmin; z <= triboundmax.z; z+=voxelsize.z, j++)
                {
                    vertex P = {0, y, z};
                    vertex A = {0, y1, z1};
                    vertex B = {0, y2, z2};
                    vertex C = {0, y3, z3};
                    if (intersectant(dir, P, A, B, C))
                    {
			            if (dotproduct < 0)
			            {
			                point.enter = true;                        	
                        	point.value = (d - normal.y*P.y - normal.z*P.z)/normal.x;
                        	insertPoint(pointList[i][j], point, count);
			            }
			            else if (dotproduct > 0)
			            {
			                point.exit = true;                        	
	                        point.value = (d - normal.y*P.y - normal.z*P.z)/normal.x;
	                        insertPoint(pointList[i][j], point, count);
			            } 
			            else 
			            {
                            rational<int> value1 = -1, value2 = -1, temp[] = {-1, -1, -1, -1, -1, -1}; 
                            if (y1 != y2)
                                temp[0] = (x2 - x1) * (y - y1) / (y2 - y1) + x1;
                            if (z1 != z2)
                                temp[1] = (x2 - x1) * (z - z1) / (z2 - z1) + x1;
                            if (y2 != y3)
                            	temp[2] = (x2 - x3) * (z - z3) / (z2 - z3) + x3;
                            if (z2 != z3)
                            	temp[3] = (x2 - x3) * (z - z3) / (z2 - z3) + x3;
                            if (y1 != y3)
                                temp[4] = (x3 - x1) * (y - y1) / (y3 - y1) + x1;
                            if (z1 != z3)
                                temp[5] = (x3 - x1) * (z - z1) / (z3 - z1) + x1;

                            int t = 0;
                            for (t ; t < 6; ++t)
                            {
                            	if (temp[t] != -1)
                            	{
                            		value1 = temp[t];
                            		break;
                            	}
                            }
                            for (++t; t < 6; ++t)
                            {
                            	if (temp[t] != -1)
                            	{
                            		value2 = temp[t];
                            		break;
                            	}
                            }

                            if (value1 < value2)
                            {                       		
                            	point.value = value1;
                            	point.touch1 = true;
                            	insertPoint(pointList[i][j], point, count);
                            	point.value = value2;
                            	point.touch1 = false;
                            	point.touch2 = true;
                            	insertPoint(pointList[i][j], point, count);
                            } 
                            else if (value1 > value2)
                            {                       		
                            	point.value = value2;
                            	point.touch1 = true;
                            	insertPoint(pointList[i][j], point, count);
                            	point.value = value1;
                            	point.touch1 = false;
                            	point.touch2 = true;
                            	insertPoint(pointList[i][j], point, count);
                            } else {                           	
                            	point.value = value1;
                            	point.touch1 = true;
                            	point.touch2 = true;
                            	insertPoint(pointList[i][j], point, count);
                            }
                        }
                    }
                }
            }
        }
        else if (dir.y == 1)
        {
        }
        else
        {
        }
        /* triangle bounding box */
        count++;
    }


/*    cout<<"------------------------------"<<endl;
    for (int i = 0; i < ny/10; ++i)
    {
        for (int j = 0; j < nz/10; ++j)
        {
            for (auto pot : pointList[i][j])
            {
                cout<<"(y="<<i<<", z="<<j<<", x="<<rational_cast<unsigned>(pot.value / voxelsize.x)<<", value="<<pot.value<<") ";
                if (pot.enter)
                    cout<<"enter ";
                if (pot.exit)
                    cout<<"exit ";
                if (pot.touch1)
                    cout<<"touch1";
                if (pot.touch2)
                    cout<<"touch2";
                cout<< endl;
            }
        }
    }
    cout<<"------------------------------"<<endl;*/


    vector<vector<vector<unsigned char>>> voxel(ny);
    for (int i = 0; i < ny; ++i)
        voxel[i].resize(nz);
    for (int i = 0; i < ny; ++i)
        for (int j = 0; j < nz; ++j)
            voxel[i][j].resize(nx);

    for (int i = 0; i < ny; ++i)
    {
        for (int j = 0; j < nz; ++j)
        {
            if (!pointList[i][j].empty())
            {   
                
                for (auto pt = pointList[i][j].begin(); pt != pointList[i][j].end(); pt++)
                {                    
                    auto startValue = (*pt).value; 
                    auto endValue = startValue;           
                    unsigned k1 = rational_cast<unsigned>(startValue / voxelsize.x);
                    auto voxelcenter1 = k1*voxelsize.x;

                    if (pointList[i][j].size() > 1)
                    {
                        if ((*pt).enter == true && (*pt).exit == false)
                        {   
                            while (!((*pt).enter == false && (*pt).touch1 == false && (*pt).touch2 == false && (*pt).exit == true
                            	  || (*pt).enter == false && (*pt).touch2 == true  && (*pt).exit == true))
                            {
                                pt++;
                                if (pt == pointList[i][j].end())
                                    cout<< "over"<<endl;                                
                            }
                            endValue = (*pt).value;
                            
                            if (voxelcenter1 < startValue)
                                k1++;
                            
                            unsigned k2 = rational_cast<unsigned>(endValue / voxelsize.x);
                            auto voxelcenter2 = k2*voxelsize.x;
                            if (voxelcenter2 > endValue)
                                k2--;
                            
                            while (k1 <= k2)
                            {
                                voxel[i][j][k1++] = 255;
                            }
                        }
                    }
                    else if ((*pt).enter == true && (*pt).exit == true && (*pt).value == voxelcenter1)
                    {
                        voxel[i][j][k1] = 255;
                    }
                }
            }
        }
    }

    unsigned char check;
    vector<int> checkvox = {-1, -1, -1};
    vector<vector<int>> lostVoxelList, extraVoxelList;
    vector<vector<vector<intersection>>> lostPointList(ny);
    for (int i = 0; i < ny; ++i)
        lostPointList[i].resize(nz);
    auto extraPointList = lostPointList;
    for (int i = 0; i < ny; ++i)
    {
        for (int j = 0; j < nz; ++j)
        {
            for (int k = 0; k < nx; ++k)
            {
                if (checkmode == true)
                {
                    checkData >> check;
                    if (check - voxel[i][j][k] > 0)
                    {
                        checkvox = {k, i, j};
                        lostVoxelList.push_back(checkvox);
                        lostPointList[i][j] = pointList[i][j];
                        //lostVoxel26Connected << voxel[i][j][k];
                    }
                    else if (voxel[i][j][k] - check > 0)
                    {
                        checkvox = {k, i, j};
                        extraVoxelList.push_back(checkvox);
                        extraPointList[i][j] = pointList[i][j];
                    }
                }
                //OUTPUT << voxel[i][j][k];
            }
        }
    }

	
    for (int j = 0; j < nz; ++j)
        for (int i = 0; i < ny; ++i)
            for (int k = 0; k < nx; ++k)
            	OUTPUT << voxel[i][j][k];

    if (checkmode == true)
    {
        vector<vector<int>> checkRay(ny);
        for (int i = 0; i < ny; ++i)
        	checkRay[i].resize(nz);

        cout << endl << "--------------------------Olddata - Newdata--------------------------" << endl;
        for (auto cor : lostVoxelList)
        {
        	checkRay[cor[1]][cor[2]] = 1;
            cout << "--------------------------" << endl;
            cout << "Lost voxel: (" << cor[0] <<", "<< cor[1]<<", "<< cor[2] <<")";
            cout << "	Fiji coordinate: (" << cor[0] <<", "<< cor[2]<<", "<< cor[1] <<")"<< endl << endl;
            for (auto checkpoint : lostPointList[cor[1]][cor[2]])
            {
                cout << "x = " << checkpoint.value << "  condition:";
                if (checkpoint.enter)
                	cout << " enter";
                if (checkpoint.exit)
                    cout << " exit";
                if (checkpoint.touch1)
                    cout << " touch1";
                if (checkpoint.touch2)
                    cout << " touch2";
                cout << endl;

                for (auto trinum : checkpoint.triNum)
                {
                    cout <<" v1("<< Vertex[Face[trinum].v1].x <<", "<< Vertex[Face[trinum].v1].y <<", "<< Vertex[Face[trinum].v1].z <<"),";
                    cout <<" v2("<< Vertex[Face[trinum].v2].x <<", "<< Vertex[Face[trinum].v2].y <<", "<< Vertex[Face[trinum].v2].z <<"),";
                    cout <<" v3("<< Vertex[Face[trinum].v3].x <<", "<< Vertex[Face[trinum].v3].y <<", "<< Vertex[Face[trinum].v3].z <<")"<<endl;
                    cout << endl;
                }
            }
        }
        cout << endl << "--------------------------Newdata - Olddata--------------------------" << endl;
        for (auto cor : extraVoxelList)
        {
        	checkRay[cor[1]][cor[2]] = 1;
            cout << "--------------------------" << endl;
            cout << "Extra voxel: (" << cor[0] <<", "<< cor[1]<<", "<< cor[2] <<")";
            cout << "	Fiji coordinate: (" << cor[0] <<", "<< cor[2]<<", "<< cor[1] <<")"<< endl << endl;
            for (auto checkpoint : extraPointList[cor[1]][cor[2]])
            {
                cout << "x = " << checkpoint.value << "  condition:";
                if (checkpoint.enter)
                	cout << " enter";
                if (checkpoint.exit)
                    cout << " exit";
                if (checkpoint.touch1)
                    cout << " touch1";
                if (checkpoint.touch2)
                    cout << " touch2";
                cout << endl;

                for (auto trinum : checkpoint.triNum)
                {
                    cout <<" v1("<< Vertex[Face[trinum].v1].x <<", "<< Vertex[Face[trinum].v1].y <<", "<< Vertex[Face[trinum].v1].z <<"),";
                    cout <<" v2("<< Vertex[Face[trinum].v2].x <<", "<< Vertex[Face[trinum].v2].y <<", "<< Vertex[Face[trinum].v2].z <<"),";
                    cout <<" v3("<< Vertex[Face[trinum].v3].x <<", "<< Vertex[Face[trinum].v3].y <<", "<< Vertex[Face[trinum].v3].z <<")"<<endl;
                    cout << endl;
                }
            }
        }

        int checkRayAmt = 0;
        for (int i = 0; i < ny; ++i)
        {
        	for (int j = 0; j < nz; ++j)
        	{
        		checkRayAmt += checkRay[i][j];
        	}
        }

        DATA.seekg(dataHead);
        getline(DATA,S);
        checkMesh << S << endl;
        getline(DATA,S);
        checkMesh << S << endl;
        getline(DATA,S);
        checkMesh << vertexAmt + checkRayAmt*4 <<' '<< faceAmt + checkRayAmt <<' '<< edgeAmt + checkRayAmt*4 << endl;

        for (int i = 0; i < vertexAmt; ++i)
        {
        	getline(DATA,S);
        	checkMesh << S << endl;
        }

        for (int i = 0; i < ny; ++i)
        {
        	for (int j = 0; j < nz; ++j)
        	{
        		if (checkRay[i][j] == 1)
        		{
        			checkMesh << '0'<<' '<< i <<' '<< j << endl;
        			checkMesh << nx <<' '<< i <<' '<< j << endl;
        			checkMesh << nx <<' '<< i <<' '<< j - 0.01 << endl;
        			checkMesh << '0'<<' '<< i <<' '<< j - 0.01 << endl;
        		}
        	}
        }

        for (int i = 0; i < faceAmt; ++i)
        {
        	getline(DATA,S);
        	checkMesh << S << endl;
        }

        for (int i = 0; i < checkRayAmt; ++i)
        {
        	checkMesh <<"4 "<< vertexAmt + i*4 <<' '<< vertexAmt + 1 + i*4 <<' '<< vertexAmt + 2 + i*4 <<' '<< vertexAmt + 3 + i*4 << endl;
        }
        
    }
    
	return 0;
}