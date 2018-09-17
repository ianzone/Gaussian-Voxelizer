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
    bool touch = false;     // touch means the ray is on the surface
    bool exit  = false;     // ray is exiting mesh
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

int main(int argc, char **argv)
{
    /* reading inputs */
    argv++; argc--;
    string off, raw;
    vect dir = {1, 0, 0};
    int nx = 128, ny = 128, nz = 128;
    int sxmin = 0, symin = 0, szmin = 0;
    int sxmax = 255, symax = 255, szmax = 255;
    while (argc > 0)
    {
        if ((*argv)[0] == '-')
        {
            if (!strcmp(*argv, "-i"))
            {
                argv++; argc--;
                off = *argv;
            }
            else if (!strcmp(*argv, "-o"))
            {
                argv++; argc--;
                raw = *argv;
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
                    exit(3);
                }
            }
        }
        argv++; argc--;
    }

    ifstream DATA(off);
    if(!DATA)
    {
        cerr<<"can not open input file!"<<endl;
        exit(1);
    }

    ofstream output(raw);
    if(!output)
    {
        cerr<<"can not open output file!"<<endl;
        exit(2);
    }
    /* reading inputs */

    /* skip first two lines */
    string S;
    getline(DATA,S);
    getline(DATA,S);

    int vertexNum;
    DATA >> S;
    cout << "vertex:" << S;
    vertexNum = stoi(S);

    int faceNum;
    DATA >> S;
    cout << "  face:" << S;
    faceNum = stoi(S);

    int edgeNum;
    DATA >> S;
    cout << "  edge:" << S << endl;
    edgeNum = stoi(S);
    /* skip first two lines */

    /* shift mesh space, store cooridinates as rational number and check user input*/
    vector<vertex> V(vertexNum);

    DATA >> S;
    auto len = S.size();
    double s = stod(S);
    rational<int> x(s*pow(10, len), pow(10, len));
    V[0].x = x - sxmin;
    auto xmin = x, xmax = x;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> y(s*pow(10, len), pow(10, len));
    V[0].y = y - symin;
    auto ymin = y, ymax = y;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> z(s*pow(10, len), pow(10, len));
    V[0].z = z - szmin;
    auto zmin = z, zmax = z;

    for (int i = 1; i < vertexNum; i++)
    {
        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> x(s*pow(10, len), pow(10, len));
        V[i].x = x - sxmin;
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> y(s*pow(10, len), pow(10, len));
        V[i].y = y - symin;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> z(s*pow(10, len), pow(10, len));
        V[i].z = z - szmin;
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
    vector<face> F(faceNum);
    for (int i = 0; i < faceNum; ++i)
    {
        DATA >> S;
        DATA >> S;
        unsigned s = stoi(S);
        F[i].v1 = s;
        DATA >> S;
        s = stoi(S);
        F[i].v2 = s;
        DATA >> S;
        s = stoi(S);
        F[i].v3 = s;
    }
    /* store faces into vector */



    int count = 1;
    vector<vector<vector<intersection>>> pointlist(ny);
    for (int i = 0; i < ny; ++i)
        pointlist[i].resize(nz);
    for (auto f : F) // for each triangle
    {
        auto x1 = V[f.v1].x, y1 = V[f.v1].y, z1 = V[f.v1].z;
        auto x2 = V[f.v2].x, y2 = V[f.v2].y, z2 = V[f.v2].z;
        auto x3 = V[f.v3].x, y3 = V[f.v3].y, z3 = V[f.v3].z;
        vect vector1, vector2, normal;
        vector1.x = x2 - x1;
        vector1.y = y2 - y1;
        vector1.z = z2 - z1;
        vector2.x = x3 - x2;
        vector2.y = y3 - y2;
        vector2.z = z3 - z2;

        cross_product(normal, vector1, vector2);
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
            if (dotproduct < 0)
                point.enter = true;
            else if (dotproduct > 0)
                point.exit = true;
            else             
            {    
                point.touch = true;
                if (y1 == y2 && z1 == z2)
                {
                    if (x1 < x2)
                    {
                        x2 = x3;
                        y2 = y3;
                        z2 = z3;
                    } else {
                        x1 = x3;
                        y1 = y3;
                        z1 = z3;
                    }
                }
                else if (y2 == y3 && z2 == z3)
                {
                    if (x3 < x2)
                    {
                        x2 = x3;
                    }
                }
                else if (y1 == y3 && z1 == z3)
                {
                    if (x3 < x1)
                    {
                        x1 = x3;
                    }
                } else {
                    if (x1 < x2)
                    {
                        if (x3 < x2)
                        {
                            x2 = x3;
                            y2 = y3;
                            z2 = z3;
                        }
                    } 
                    else if (x3 < x1)
                    {
                        x1 = x3;
                        y1 = y3;
                        z1 = z3;
                    }                    
                }
            }
            

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
                        if (dotproduct == 0)
                        {
                            //rational<int> temp1 = -1, temp2 = -1, temp3 = -1;
                            if (y1 != y2)
                                point.value = (x2 - x1) * (y - y1) / (y2 - y1) + x1;                            
                            else                           
                                point.value = (x2 - x1) * (z - z1) / (z2 - z1) + x1;
                            
                            // if (point.value == x1 || point.value == x2)
                            // {
                            //     continue;
                            // }

                        }
                        else point.value = (d - normal.y*P.y - normal.z*P.z)/normal.x;

                        if (pointlist[i][j].empty())
                            pointlist[i][j].push_back(point);
                        else for (auto pt = pointlist[i][j].begin(); pt != pointlist[i][j].end(); pt++)
                        {
                            if (point.value == (*pt).value)
                            {
                                (*pt).enter = ((*pt).enter || point.enter);
                                (*pt).touch = ((*pt).touch || point.touch);
                                (*pt).exit = ((*pt).exit || point.exit);
                                break;
                            }
                            else if (point.value < (*pt).value)
                            {
                                pointlist[i][j].insert(pt, point);
                                break;
                            }
                            if (pt == pointlist[i][j].end() - 1)
                            {
                                pointlist[i][j].push_back(point);
                                break;
                            }
                        }                        
                    }
                }
            }
        }
        else if (dir.y == 1)
        {
        }
        else if (dir.z == 1)
        {
        }
        /* triangle bounding box */
    }

/*    cout<<"------------------------------"<<endl;
    for (int i = 0; i < ny; ++i)
    {
        for (int j = 0; j < nz; ++j)
        {
            for (auto pot : pointlist[i][j])
            {
                cout<<"("<<i<<", "<<j<<", "<<rational_cast<unsigned>(pot.value / voxelsize.x)<<", "<<pot.value<<") ";
                if (pot.enter)
                    cout<<"enter ";
                if (pot.exit)
                    cout<<"exit ";
                if (pot.touch)
                    cout<<"touch";
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
            if (!pointlist[i][j].empty())
            {   
                cout<<"listsize: "<<pointlist[i][j].size()<<endl;
                int abc = 1; cout<<"y = "<<i<<", z = "<<j<<endl;
                for (auto p : pointlist[i][j])
                {
                    cout<<abc++<<" point "<<p.value<<" ";
                    if (p.enter)
                    cout<<"enter ";
                    if (p.exit)
                        cout<<"exit ";
                    if (p.touch)
                        cout<<"touch";
                    cout<< endl;
                }cout<<endl;

                for (auto pt = pointlist[i][j].begin(); pt != pointlist[i][j].end(); pt++)
                {                    
                    auto start = (*pt).value; 
                    auto end = start;           
                    unsigned k1 = rational_cast<unsigned>(start / voxelsize.x);
                    auto voxelcenter1 = k1*voxelsize.x;

                    if (pointlist[i][j].size() > 1)
                    {
                        if ((*pt).enter == true && (*pt).exit == false)
                        {   
                            while (!((*pt).enter == false && (*pt).touch == false && (*pt).exit == true))
                            {
                                pt++;
                                if (pt == pointlist[i][j].end())
                                    cout<< "over"<<endl;                                
                            }
                            end = (*pt).value;
                            
                            if (voxelcenter1 < start)
                                k1++;
                            
                            unsigned k2 = rational_cast<unsigned>(end / voxelsize.x);
                            auto voxelcenter2 = k2*voxelsize.x;
                            if (voxelcenter2 > end)
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

    for (int i = 0; i < ny; ++i)
    {
        for (int j = 0; j < nz; ++j)
        {
            for (int k = 0; k < nx; ++k)
            {
                /*if (voxel[i][j][k] == 1)
                {
                    cout<<"("<<i<<", "<<j<<", "<<k<<")"<<endl;
                }*/
                output << voxel[i][j][k];
            }
        }
    }

    output.close();
    DATA.close();
	return 0;
}