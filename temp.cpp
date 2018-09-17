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

struct voxeltype
{
    unsigned x, y, z;
}voxel;

struct hit 
{
    double value;           // coordinate of ray-mesh interSection point
    bool enter = false;     // ray is entering meSh
    bool touch = false;     // touch means the ray is on the surface
    bool exit  = false;     // ray is exiting mesh
};

/*int rayset(vect dir, vertex P, vertex A, vertex B, vertex C)
{
    if (dir.x == 1)
    {
        
    }
    P.x = A.y*B.z - A.z*B.y;
    P.y = A.z*B.x - A.x*B.z;
    P.z = A.x*B.y - A.y*B.x;
    return 0;
}*/

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

    /* store cooridinates as rational number */
    vector<vertex> V(vertexNum);

    DATA >> S;
    auto len = S.size();
    double s = stod(S);
    rational<int> x(s*pow(10, len), pow(10, len));
    V[0].x = x;
    auto xmin = x, xmax = x;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> y(s*pow(10, len), pow(10, len));
    V[0].y = y;
    auto ymin = y, ymax = y;

    DATA >> S;
    len = S.size();
    s = stod(S);
    rational<int> z(s*pow(10, len), pow(10, len));
    V[0].z = z;
    auto zmin = z, zmax = z;

    for (int i = 1; i < vertexNum; i++)
    {
        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> x(s*pow(10, len), pow(10, len));
        V[i].x = x;
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> y(s*pow(10, len), pow(10, len));
        V[i].y = y;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;

        DATA >> S;
        len = S.size();
        s = stod(S);
        rational<int> z(s*pow(10, len), pow(10, len));
        V[i].z = z;
        if (z < zmin)
            zmin = z;
        else if (z > zmax)
            zmax = z;
    }
    //cout << "bounding box: " << xmin <<" "<< ymin <<" "<< zmin <<" | "<< xmax <<" "<< ymax <<" "<< zmax << endl;//空间大小不得小于该数值
    /* store cooridinates as rational number */

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
    getline(DATA, S);
    vector<vector<unsigned>> F(faceNum);
    for (int i = 0; i < faceNum; i++)
    {
        getline(DATA, S);
        istringstream is(S);
        is >> S;
        while (is >> S)
        {
            unsigned s = stoi(S);
            F[i].push_back(s);
        }
    }
    /* store faces into vector */







    int count = 1;
    for (auto f : F) // for each triangle
    {
        vect vector1, vector2, normal;
        vector1.x = V[f.v2].x - V[f.v1].x;
        vector1.y = V[f.v2].y - V[f.v1].y;
        vector1.z = V[f.v2].z - V[f.v1].z;
        vector2.x = V[f.v3].x - V[f.v2].x;
        vector2.y = V[f.v3].y - V[f.v2].y;
        vector2.z = V[f.v3].z - V[f.v2].z;

        normal.x = vector1.y*vector2.z - vector2.y*vector1.z;
        normal.y = vector2.x*vector1.z - vector1.x*vector2.z;
        normal.z = vector1.x*vector2.y - vector2.x*vector1.y;
        auto d = normal.x*V[f.v1].x + normal.y*V[f.v1].y + normal.z*V[f.v1].z;
        cout << endl << "3D triangle formula: " << normal.x <<"x + "<< normal.y <<"y + "<< normal.z <<"z = "<< d << endl;

        auto dotproduct = normal.x*dir.x + normal.y*dir.y + normal.z*dir.z;
        //cout << "dot production: " << dotproduct << endl;

        /* triangle bounding box */
        vertex triboxmin, triboxmax;
        triboxmin.x = V[f.v1].x; triboxmax.x = V[f.v1].x;
        triboxmin.y = V[f.v1].y; triboxmax.y = V[f.v1].y;
        triboxmin.z = V[f.v1].z; triboxmax.z = V[f.v1].z;
        if (dir.x == 1)
        {
            if (V[f.v2].y < triboxmin.y)
                triboxmin.y = V[f.v2].y;
            else if (V[f.v2].y > triboxmax.y)
                triboxmax.y = V[f.v2].y;
            if (V[f.v3].y < triboxmin.y)
                triboxmin.y = V[f.v3].y;
            else if (V[f.v3].y > triboxmax.y)
                triboxmax.y = V[f.v3].y;
            if (V[f.v2].z < triboxmin.z)
                triboxmin.z = V[f.v2].z;
            else if (V[f.v2].z > triboxmax.z)
                triboxmax.z = V[f.v2].z;
            if (V[f.v3].z < triboxmin.z)
                triboxmin.z = V[f.v3].z;
            else if (V[f.v3].z > triboxmax.z)
                triboxmax.z = V[f.v3].z;
            cout << count++ << " triboxmin(y, z): " << triboxmin.y << " " << triboxmin.z << " | ";
            cout << "triboxmax(y, z): " << triboxmax.y << " " << triboxmax.z << endl;

            auto trivoxelymin = rational_cast<unsigned>(triboxmin.y / voxelsize.y) * voxelsize.y + symin;
            auto trivoxelzmin = rational_cast<unsigned>(triboxmin.z / voxelsize.z) * voxelsize.z + szmin;
            cout << "trivoxelymin: " << trivoxelymin <<" trivoxelzmin: "<< trivoxelzmin << endl;

/*            for (auto y = trivoxelymin + voxelsize.y/2; y <= triboxmax.y; y+=voxelsize.y)
            {
                for (auto z = trivoxelzmin + voxelsize.z/2; z <= triboxmax.z; z+=voxelsize.z)
                {
                    rayset()
                }
            }*/
        }
        else if (dir.y == 1)
        {
            if (V[f.v2].x < triboxmin.x)
                triboxmin.x = V[f.v2].x;
            else if (V[f.v2].x > triboxmax.x)
                triboxmax.x = V[f.v2].x;
            if (V[f.v3].x < triboxmin.x)
                triboxmin.x = V[f.v3].x;
            else if (V[f.v3].x > triboxmax.x)
                triboxmax.x = V[f.v3].x;
            if (V[f.v2].z < triboxmin.z)
                triboxmin.z = V[f.v2].z;
            else if (V[f.v2].z > triboxmax.z)
                triboxmax.z = V[f.v2].z;
            if (V[f.v3].z < triboxmin.z)
                triboxmin.z = V[f.v3].z;
            else if (V[f.v3].z > triboxmax.z)
                triboxmax.z = V[f.v3].z;
            cout << count++ << " triboxmin: " << triboxmin.x << " " << triboxmin.z << " | ";
            cout << "triboxmax: " << triboxmax.x << " " << triboxmax.z << endl;

            /*multiple.x = rational_cast<unsigned>(triboxmin.x / voxelsize.x);
            multiple.z = rational_cast<unsigned>(triboxmin.z / voxelsize.z);
            cout << "multiple: " << multiple.x<<" "<< multiple.z << endl;*/
        }
        else if (dir.z == 1)
        {
            if (V[f.v2].x < triboxmin.x)
                triboxmin.x = V[f.v2].x;
            else if (V[f.v2].x > triboxmax.x)
                triboxmax.x = V[f.v2].x;
            if (V[f.v3].x < triboxmin.x)
                triboxmin.x = V[f.v3].x;
            else if (V[f.v3].x > triboxmax.x)
                triboxmax.x = V[f.v3].x;
            if (V[f.v2].y < triboxmin.y)
                triboxmin.y = V[f.v2].y;
            else if (V[f.v2].y > triboxmax.y)
                triboxmax.y = V[f.v2].y;
            if (V[f.v3].y < triboxmin.y)
                triboxmin.y = V[f.v3].y;
            else if (V[f.v3].y > triboxmax.y)
                triboxmax.y = V[f.v3].y;
            cout << count++ << " triboxmin: " << triboxmin.x << " " << triboxmin.y << " | ";
            cout << "triboxmax: " << triboxmax.x << " " << triboxmax.y << endl;

            /*multiple.y = rational_cast<unsigned>(triboxmin.y / voxelsize.y);
            multiple.x = rational_cast<unsigned>(triboxmin.x / voxelsize.x);
            cout << "multiple: " << multiple.x <<" "<< multiple.y << endl;*/
        }
        /* triangle bounding box */



    }
    output.close();
    DATA.close();
    return 0;
}