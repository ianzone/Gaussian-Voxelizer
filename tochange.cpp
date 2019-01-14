#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include "boost/rational.hpp"

using namespace std;
using namespace boost;

typedef struct Vertex
{
    rational<int> x, y, z;
}Vector, Voxel, Space;		// vector, voxel

struct Face
{
    unsigned v1, v2, v3;
};

struct Intersection 
{
    rational<int> value;    	// coordinate of ray-mesh interSection point
    bool enter 		= false;    // ray is entering meSh
    bool exit  		= false;    // ray is exiting mesh
    bool touchStart = false;    // touch means the ray is on the surface
    bool touchEnd 	= false;
    vector<int> triNum;			// related triangle number
};

rational<int> dot_product(Vector a, Vector b);
rational<int> dot_product(Vector a, Vector b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

void cross_product(Vector &p, Vector a, Vector b);
void cross_product(Vector &p, Vector a, Vector b)
{
    p.x = a.y*b.z - a.z*b.y;
    p.y = a.z*b.x - a.x*b.z;
    p.z = a.x*b.y - a.y*b.x;
}

int intersectant(Vector ray, Vertex P, Vertex A, Vertex B, Vertex C);
int intersectant(Vector ray, Vertex P, Vertex A, Vertex B, Vertex C)
{
    Vector PA = {A.x-P.x, A.y-P.y, A.z-P.z};
    Vector PB = {B.x-P.x, B.y-P.y, B.z-P.z};
    Vector PC = {C.x-P.x, C.y-P.y, C.z-P.z};
    Vector papb, pbpc, pcpa;
    cross_product(papb, PA, PB);
    cross_product(pbpc, PB, PC);
    cross_product(pcpa, PC, PA);
    if (papb.z <= 0 && pbpc.z <= 0 && pcpa.z <= 0 || papb.z >= 0 && pbpc.z >= 0 && pcpa.z >= 0)
        return 1;
    return 0;
}

void insert_point(vector<Intersection> &pl, Intersection &point, int count);
void insert_point(vector<Intersection> &pl, Intersection &point, int count)
{
	point.triNum.push_back(count);
	if (pl.empty())
        pl.push_back(point);
    else for (auto pt = pl.begin(); pt != pl.end(); pt++)
    {
        if (point.value == (*pt).value)
        {
            (*pt).enter 	 = ((*pt).enter  	 || point.enter);
            (*pt).exit 		 = ((*pt).exit   	 || point.exit);
            (*pt).touchStart = ((*pt).touchStart || point.touchStart);
            (*pt).touchEnd	 = ((*pt).touchEnd 	 || point.touchEnd);
            (*pt).triNum.push_back(count);
            break;
        }
        else if (point.value < (*pt).value)
        {
            pl.insert(pt, point);
            break;
        }
        if (pt == pl.end() - 1)
        {
            pl.push_back(point);
            break;
        }
    }
}

struct Intersection2D
{
	rational<int> x, y;
	struct coorxy
	{
		rational<int> x, y;
	} predecessor, successor;
};

void insert_point2D(unsigned &parityCount, vector<Intersection2D> &pl, Intersection2D point2D);
void insert_point2D(unsigned &parityCount, vector<Intersection2D> &pl, Intersection2D point2D)
{
	if (pl.empty())
        pl.push_back(point2D);
    else for (auto pt = pl.begin(); pt != pl.end(); pt++)
    {
        if (point2D.y == (*pt).y && (point2D.successor.x - point2D.predecessor.x) * ((*pt).successor.x - (*pt).predecessor.x) > 0)
        {
            parityCount++;
            auto k = ((*pt).successor.y - (*pt).predecessor.y)/((*pt).successor.x - (*pt).predecessor.x);
            if (point2D.x == point2D.predecessor.x)
            {
            	if (point2D.successor.y > k * point2D.successor.x)
	            {
	            	(*pt).predecessor.x = point2D.predecessor.x;
	            	(*pt).successor.x = point2D.successor.x;
	            }
            }else{
            	if (point2D.predecessor.y > k * point2D.predecessor.x)
	            {
	            	(*pt).predecessor.x = point2D.predecessor.x;
	            	(*pt).successor.x = point2D.successor.x;
	            }
            }
            return;
        }
        else if (point2D.y > (*pt).y)
        {
            pl.insert(pt, point2D);
            return;
        }
        if (pt == pl.end() - 1)
        {
            pl.push_back(point2D);
            return;
        }
    }
}

void parity_count(unsigned &parityCount, vector<Intersection2D> &pl, Vertex point, Vertex edgeHead, Vertex edgeTail);
void parity_count(unsigned &parityCount, vector<Intersection2D> &pl, Vertex point, Vertex edgeHead, Vertex edgeTail)
{
	if (edgeHead.x <= point.x && point.x <= edgeTail.x || edgeTail.x <= point.x && point.x <= edgeHead.x)
	{
		if (edgeHead.x = edgeTail.x)
		{
			return;
		} 
		else if (point.y > (edgeTail.y - edgeHead.y)/(edgeTail.x - edgeHead.x) * point.x)
		{
			Intersection2D pt;
			pt.x = point.x;
			pt.y = (edgeTail.y - edgeHead.y)/(edgeTail.x - edgeHead.x) * point.x;
			pt.predecessor.x = edgeHead.x;
			pt.predecessor.y = edgeHead.y;
			pt.successor.x   = edgeTail.x;
			pt.successor.y   = edgeTail.y;
			parityCount++;
			insert_point2D(parityCount, pl, pt);
		}
	}
}

int main(int argc, char **argv)
{
    /*-----------------------------------------------------------------reading inputs-----------------------------------------------------------------*/
    argv++; argc--;
    string input, output, checkfile;
    Vector ray = {0, 0, 1};
    Space spacemin, spacemax;
    spacemin.x = 0, spacemin.y = 0, spacemin.z = 0;
    spacemax.x = 255, spacemax.y = 255, spacemax.z = 255;
    int nx = 255, ny = 255, nz = 255;
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
            else if (!strcmp(*argv, "-smin"))
            {
            	string coor; int len; double x, y, z;
                argv++; argc--;
                coor = *argv;
                len = coor.size();
                x = stod(coor); 
                rational<int> spacex(x*pow(10, len), pow(10, len));
                spacemin.x = spacex+0;

                argv++; argc--;
                coor = *argv;
                len = coor.size();
                y = stod(coor);
                rational<int> spacey(y*pow(10, len), pow(10, len));
                spacemin.y = spacey+0;
                
                argv++; argc--;
                coor = *argv;
                len = coor.size();
                z = stod(coor);
                rational<int> spacez(z*pow(10, len), pow(10, len));
                spacemin.z = spacez+0;
                cout<<spacemin.x<<endl<<spacemin.y<<endl<<spacemin.z<<endl;
                if (spacemin.x * spacemin.y * spacemin.z < 0)
                {
                	cerr<<"please give non-negtive value for space smin\n";
                	exit(0);
                }
            }
            else if (!strcmp(*argv, "-smax"))
            {
            	string coor; int len; double x, y, z;
                argv++; argc--;
                coor = *argv;
                len = coor.size();
                x = stod(coor);
                rational<int> spacex(x*pow(10, len), pow(10, len));
                spacemax.x = spacex+0;

                argv++; argc--;
                coor = *argv;
                len = coor.size();
                y = stod(coor);
                rational<int> spacey(y*pow(10, len), pow(10, len));
                spacemax.y = spacey+0;
                
                argv++; argc--;
                coor = *argv;
                len = coor.size();
                z = stod(coor);
                rational<int> spacez(z*pow(10, len), pow(10, len));
                spacemax.z = spacez+0;
                
                if (spacemax.x < 0 || spacemax.y < 0 || spacemax.z < 0)
                {
                	cerr<<"please give non-negtive value for space smin\n";
                	exit(0);
                }
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
                    cerr<<"invalid voxel number\n";
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
        cerr<<"can not open input file!\n";
        exit(0);
    }
    auto dataHead = DATA.tellg();

    ofstream OUTPUT(output);
    if(!OUTPUT)
    {
        cerr<<"can not open output file!\n";
        exit(0);
    }

    ifstream checkData;
    ofstream checkMesh;
    ofstream lostVoxel_26connected;
    if (checkmode == true)
    {
    	checkMesh.open("checkMesh.off");
	    if (!checkMesh.is_open())
	    {
	    	cerr<<"can not create checkMesh.off!\n";
	        exit(0);
	    }

	    checkData.open(checkfile);
		if(!checkData.is_open())
	    {
	        cerr<<"can not open check file!\n";
	        exit(0);
	    }

	    lostVoxel_26connected.open("lostVoxel_26connected.raw");
	    if (!lostVoxel_26connected.is_open())
	    {
	    	cerr<<"can not create lostVoxel_26connected.raw!\n";
	        exit(0);
	    }
    }
    /*-----------------------------------------------------------------reading inputs-----------------------------------------------------------------*/

    /*-------------------------------------------------------------------mesh info--------------------------------------------------------------------*/
    string S;
    getline(DATA,S);
    getline(DATA,S);

    int vertexAmt;
    DATA >> S;
    cout << "Vertex:" << S;
    vertexAmt = stoi(S);

    int faceAmt;
    DATA >> S;
    cout << "  Face:" << S;
    faceAmt = stoi(S);

    int edgeAmt;
    DATA >> S;
    cout << "  Edge:" << S << endl;
    edgeAmt = stoi(S);
    /*-------------------------------------------------------------------mesh info--------------------------------------------------------------------*/

    /*---------------------------check user input, shift mesh Space, store cooridinates as rational number and store faces----------------------------*/
    int len;
    double s;
    vector<Vertex> vertexData(vertexAmt);
    for (int i = 0; i < vertexAmt; i++)
    {
        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> x(s*pow(10, len), pow(10, len));
	    if (x - spacemin.x < 0)
	    {
	    	cerr << "please give a smaller x of Space size\n";
	    	exit(0);
	    }
	    else if (spacemax.x - x < 0)
	    {
	    	cerr << "please give a larger x of Space size\n";
	    	exit(0);
	    }
        vertexData[i].x = x - spacemin.x;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> y(s*pow(10, len), pow(10, len));
        if (y - spacemin.y < 0)
	    {
	    	cerr << "please give a smaller y of Space size\n";
	    	exit(0);
	    }
	    else if (spacemax.y - y < 0)
	    {
	    	cerr << "please give a larger y of Space size\n";
	    	exit(0);
	    }
        vertexData[i].y = y - spacemin.y;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> z(s*pow(10, len), pow(10, len));
        if (z - spacemin.z < 0)
	    {
	    	cerr << "please give a smaller z of Space size\n";
	    	exit(0);
	    }
	    else if (spacemax.z - z < 0)
	    {
	    	cerr << "please give a larger z of Space size\n";
	    	exit(0);
	    }
        vertexData[i].z = z - spacemin.z;
    }

    vector<Face> faceData(faceAmt);
    for (int i = 0; i < faceAmt; ++i)
    {
        DATA >> S;
        DATA >> S;
        unsigned s = stoi(S);
        faceData[i].v1 = s;
        DATA >> S;
        s = stoi(S);
        faceData[i].v2 = s;
        DATA >> S;
        s = stoi(S);
        faceData[i].v3 = s;
    }

    Voxel voxelSize;
    voxelSize.x = (spacemax.x - spacemin.x) / nx;
    voxelSize.y = (spacemax.y - spacemin.y) / ny;
    voxelSize.z = (spacemax.z - spacemin.z) / nz;
    cout << "mesh Space min: " << spacemin.x <<" "<< spacemin.y <<" "<< spacemin.z <<" | max: "<< spacemax.x <<" "<< spacemax.y <<" "<< spacemax.z << endl;
    cout << "voxel size: " << voxelSize.x <<" "<< voxelSize.y <<" "<< voxelSize.z << endl;
    /*---------------------------check user input, shift mesh Space, store cooridinates as rational number and store faces----------------------------*/


    /*--------------------------------------------------------------triangle processing---------------------------------------------------------------*/
    vector<Vector> normalData(faceAmt);
    vector<vector<vector<Intersection>>> pointList(nx);
    for (int i = 0; i < nx; ++i)
        pointList[i].resize(ny);

	int count = 0;
    for (auto f : faceData) // for each triangle
    {
        auto x1 = vertexData[f.v1].x, y1 = vertexData[f.v1].y, z1 = vertexData[f.v1].z;
        auto x2 = vertexData[f.v2].x, y2 = vertexData[f.v2].y, z2 = vertexData[f.v2].z;
        auto x3 = vertexData[f.v3].x, y3 = vertexData[f.v3].y, z3 = vertexData[f.v3].z;
        Vector vector1, vector2, normal;
        vector1.x = x2 - x1;
        vector1.y = y2 - y1;
        vector1.z = z2 - z1;
        vector2.x = x3 - x2;
        vector2.y = y3 - y2;
        vector2.z = z3 - z2;
        cross_product(normal, vector1, vector2);
        normalData.push_back(normal);
        auto d = normal.x*x1 + normal.y*y1 + normal.z*z1;
        //cout << endl << count++ << " triangle formula: " << normal.x <<"x + "<< normal.y <<"y + "<< normal.z <<"z = "<< d << endl;

        auto dotproduct = dot_product(normal, ray);
        //cout << "dot production: " << dotproduct <<" "<<x1<<" "<<y1<<" "<<z1<<" | "<<x2<<" "<<y2<<" "<<z2<<" | "<<x3<<" "<<y3<<" "<<z3<< endl;

        // find triangle bounding box 
        Vertex triboundmin, triboundmax;
        triboundmin.x = x1; triboundmax.x = x1;
        triboundmin.y = y1; triboundmax.y = y1;
        if (x2 < triboundmin.x)
            triboundmin.x = x2;
        else if (x2 > triboundmax.x)
            triboundmax.x = x2;
        if (x3 < triboundmin.x)
            triboundmin.x = x3;
        else if (x3 > triboundmax.x)
            triboundmax.x = x3;
        if (y2 < triboundmin.y)
            triboundmin.y = y2;
        else if (y2 > triboundmax.y)
            triboundmax.y = y2;
        if (y3 < triboundmin.y)
            triboundmin.y = y3;
        else if (y3 > triboundmax.y)
            triboundmax.y = y3;

        // minimum coordinate of triangle's related voxel
        unsigned numx = rational_cast<unsigned>(triboundmin.x / voxelSize.x);
        unsigned numy = rational_cast<unsigned>(triboundmin.y / voxelSize.y);
        auto trivoxelxmin = numx * voxelSize.x;
        auto trivoxelymin = numy * voxelSize.y;

        Intersection point;

        unsigned i = numx;
        for (auto x = trivoxelxmin; x <= triboundmax.x; x+=voxelSize.x, i++)
        {
            unsigned j = numy;
            for (auto y = trivoxelymin; y <= triboundmax.y; y+=voxelSize.y, j++)
            {
                Vertex P = {x, y, 0};
                Vertex A = {x1, y1, 0};
                Vertex B = {x2, y2, 0};
                Vertex C = {x3, y3, 0};
                if (intersectant(ray, P, A, B, C))
                {
		            if (dotproduct < 0)
		            {
		                point.enter = true;
                    	point.value = (d - normal.x*P.x - normal.y*P.y)/normal.z;
                    	insert_point(pointList[i][j], point, count);
		            }
		            else if (dotproduct > 0)
		            {
		                point.exit = true;                        	
                        point.value = (d - normal.x*P.x - normal.y*P.y)/normal.z;
                        insert_point(pointList[i][j], point, count);
		            } 
		            else 
		            {	// tagging touch_start touch_end
                        rational<int> value1 = -1, value2 = -1, temp[] = {-1, -1, -1, -1, -1, -1}; 
                        if (x1 != x2)
                            temp[0] = (z2 - z1) * (x - x1) / (x2 - x1) + z1;
                        if (y1 != y2)
                            temp[1] = (z2 - z1) * (y - y1) / (y2 - y1) + z1;
                        if (x2 != x3)
                        	temp[2] = (z2 - z3) * (x - x3) / (x2 - x3) + z3;
                        if (y2 != y3)
                        	temp[3] = (z2 - z3) * (y - y3) / (y2 - y3) + z3;
                        if (x1 != x3)
                            temp[4] = (z3 - z1) * (x - x1) / (x3 - x1) + z1;
                        if (y1 != y3)
                            temp[5] = (z3 - z1) * (y - y1) / (y3 - y1) + z1;

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
                        	point.touchStart = true;
                        	insert_point(pointList[i][j], point, count);
                        	point.value = value2;
                        	point.touchStart = false;
                        	point.touchEnd = true;
                        	insert_point(pointList[i][j], point, count);
                        } 
                        else if (value1 > value2)
                        {                       		
                        	point.value = value2;
                        	point.touchStart = true;
                        	insert_point(pointList[i][j], point, count);
                        	point.value = value1;
                        	point.touchStart = false;
                        	point.touchEnd = true;
                        	insert_point(pointList[i][j], point, count);
                        } else {                           	
                        	point.value = value1;
                        	insert_point(pointList[i][j], point, count);
                        }
                    }
                }
            }
        }        
        count++;
    }
	/*--------------------------------------------------------------triangle processing---------------------------------------------------------------*/

	/*----------------------------------------------------------------special tagging-----------------------------------------------------------------*/
	for (int i = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (auto pt : pointList[i][j])
			{
				if (pt.enter == true && pt.exit == true && pt.touchStart == false && pt.touchEnd == false)
				{
					if (pt.triNum.size() > 2)
					{
			    		unsigned parityCount = 0;
			    		vector<Intersection2D> pointList2D;
						for (auto n = pt.triNum.begin(); n != pt.triNum.end() ; n++)
						{
							auto v1 = vertexData[faceData[pt.triNum[*n]].v1];
							auto v2 = vertexData[faceData[pt.triNum[*n]].v2];
							auto v3 = vertexData[faceData[pt.triNum[*n]].v3];

							if (v1.z == pt.value)
							{
								parity_count(parityCount, pointList2D, v1, v2, v3);
							}
							else if (v2.z == pt.value)
							{
								parity_count(parityCount, pointList2D, v2, v3, v1);
							}
							else
							{
								parity_count(parityCount, pointList2D, v3, v1, v2);
							}	
						}

						if (parityCount/2 == 1)
						{
							if (pointList2D[0].successor.x - pointList2D[0].predecessor.x < 0)
							{
								pt.exit = false;
							} 
							else{
								pt.enter = false;
							}
						}
					}
				}
			}
		}
	}
	/*----------------------------------------------------------------special tagging-----------------------------------------------------------------*/

    /*------------------------------------------------------------------voxelization------------------------------------------------------------------*/
    vector<vector<vector<unsigned char>>> voxel(nx);
    for (int i = 0; i < nx; ++i)
        voxel[i].resize(ny);
    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j)
            voxel[i][j].resize(nz);

    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            if (!pointList[i][j].empty())
            {   
                for (auto pt = pointList[i][j].begin(); pt != pointList[i][j].end(); pt++)
                {
                    auto startValue = (*pt).value; 
                    auto endValue = startValue; 
                    unsigned k1 = rational_cast<unsigned>(startValue / voxelSize.z);
                    auto voxelCenter1 = k1*voxelSize.z;

                    if (pointList[i][j].size() > 1)
                    {
                        if ((*pt).enter == true && (*pt).exit == false || (*pt).enter == true && (*pt).exit == true && (*pt).touchStart == true)
                        {   
                            while (!((*pt).enter == false && (*pt).touchStart == false && (*pt).exit == true))
                            {
                                pt++;
                                if (pt == pointList[i][j].end())
                                    cout<< "over\n";                                
                            }
                            endValue = (*pt).value;
                            
                            if (voxelCenter1 < startValue)
                                k1++;
                            
                            unsigned k2 = rational_cast<unsigned>(endValue / voxelSize.z);
                            auto voxelCenter2 = k2*voxelSize.z;
                            if (voxelCenter2 > endValue)
                                k2--;
                            
                            while (k1 <= k2)
                            {
                                voxel[i][j][k1++] = 255;
                            }
                        }

                    }
                    else if ((*pt).enter == true && (*pt).exit == true && (*pt).touchStart == false && (*pt).touchEnd == false && (*pt).value == voxelCenter1)
                    {
                        voxel[i][j][k1] = 255;
                    }
                }
            }
        }
    }

    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i)
            	OUTPUT << voxel[i][j][k];
    /*------------------------------------------------------------------voxelization------------------------------------------------------------------*/

    /*-------------------------------------------------------------------check mode-------------------------------------------------------------------*/
    if (checkmode == true)
    {
        unsigned char check;
	    vector<int> checkvox = {-1, -1, -1};
	    vector<vector<int>> lostVoxelList, extraVoxelList;
	    vector<vector<vector<Intersection>>> lostPointList(ny);
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
	                        //lostVoxel_26connected << voxel[i][j][k];
	                    }
	                    else if (voxel[i][j][k] - check > 0)
	                    {
	                        checkvox = {k, i, j};
	                        extraVoxelList.push_back(checkvox);
	                        extraPointList[i][j] = pointList[i][j];
	                    }
	                }
	            }
	        }
	    }

        vector<vector<int>> checkRay(ny);
        for (int i = 0; i < ny; ++i)
        	checkRay[i].resize(nz);

        cout << endl << "--------------------------Olddata - Newdata--------------------------" << endl;
        for (auto cor : lostVoxelList)
        {
        	checkRay[cor[1]][cor[2]] = 1;
            cout << "--------------------------" << endl;
            cout << "Lost voxel: (" << cor[0] <<", "<< cor[1]<<", "<< cor[2] <<")"<< endl << endl;
            for (auto checkpoint : lostPointList[cor[1]][cor[2]])
            {
                cout << "x = " << checkpoint.value << "  condition:";
                if (checkpoint.enter)
                	cout << " enter";
                if (checkpoint.exit)
                    cout << " exit";
                if (checkpoint.touchStart)
                    cout << " touchStart";
                if (checkpoint.touchEnd)
                    cout << " touchEnd";
                cout << endl;

                for (auto trinum : checkpoint.triNum)
                {
                    cout <<" v1("<< vertexData[faceData[trinum].v1].x <<", "<< vertexData[faceData[trinum].v1].y <<", "<< vertexData[faceData[trinum].v1].z <<"),";
                    cout <<" v2("<< vertexData[faceData[trinum].v2].x <<", "<< vertexData[faceData[trinum].v2].y <<", "<< vertexData[faceData[trinum].v2].z <<"),";
                    cout <<" v3("<< vertexData[faceData[trinum].v3].x <<", "<< vertexData[faceData[trinum].v3].y <<", "<< vertexData[faceData[trinum].v3].z <<")\n";
                    cout << endl;
                }
            }
        }
        cout << endl << "--------------------------Newdata - Olddata--------------------------" << endl;
        for (auto cor : extraVoxelList)
        {
        	checkRay[cor[1]][cor[2]] = 1;
            cout << "--------------------------" << endl;
            cout << "Extra voxel: (" << cor[0] <<", "<< cor[1]<<", "<< cor[2] <<")"<< endl << endl;
            for (auto checkpoint : extraPointList[cor[1]][cor[2]])
            {
                cout << "x = " << checkpoint.value << "  condition:";
                if (checkpoint.enter)
                	cout << " enter";
                if (checkpoint.exit)
                    cout << " exit";
                if (checkpoint.touchStart)
                    cout << " touchStart";
                if (checkpoint.touchEnd)
                    cout << " touchEnd";
                cout << endl;

                for (auto trinum : checkpoint.triNum)
                {
                    cout <<" v1("<< vertexData[faceData[trinum].v1].x <<", "<< vertexData[faceData[trinum].v1].y <<", "<< vertexData[faceData[trinum].v1].z <<"),";
                    cout <<" v2("<< vertexData[faceData[trinum].v2].x <<", "<< vertexData[faceData[trinum].v2].y <<", "<< vertexData[faceData[trinum].v2].z <<"),";
                    cout <<" v3("<< vertexData[faceData[trinum].v3].x <<", "<< vertexData[faceData[trinum].v3].y <<", "<< vertexData[faceData[trinum].v3].z <<")\n";
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
    /*-------------------------------------------------------------------check mode-------------------------------------------------------------------*/

	return 0;
}