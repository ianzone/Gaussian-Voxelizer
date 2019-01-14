#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>

#include "boost/rational.hpp"

using namespace std;
using namespace boost;

struct Coordinate
{
    rational<int> x, y, z;
};

struct Face
{
    int v1, v2, v3;
};

class Mesh
{
private:
    
public:
    Mesh() { }
    ~Mesh() { }
    void read(istream &);
    int number_of_vertex;
    int number_of_faces;
    int number_of_edges;
    Coordinate lower_bound;
    Coordinate upper_bound;
    vector<Coordinate> vertex;
    vector<Face> face;
};

extern string input, output;
extern Coordinate space_lower_bound, space_upper_bound, number_of_voxels;

rational<int> to_rational(const char str[]);
rational<int> to_rational(string str);

void process_command(int argc, char const *argv[]);

#endif