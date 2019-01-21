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

struct Intersection
{
    rational<int> value;        // z coordinate of ray-mesh intersection point
    bool enter = false;         // ray is entering mesh
    bool exit = false;          // ray is exiting mesh
    bool touch_start = false;    // touch means the ray is on the surface
    bool touch_end = false;
    vector<int> shared_face;         // related triangle number
};

class Mesh
{
  private:
    const Coordinate ray{0, 0, 1};
    vector<vector<vector<Intersection>>> intersection_points_along_ray;
    void insert_intersection_point(int i, int j, Intersection &point, int face_num);
    bool intersectant(rational<int> x, rational<int> y, Coordinate &A, Coordinate &B, Coordinate &C);
    void ray_triangle_intersection(Coordinate &space_lower_bound, Coordinate &voxel_size, int face_num);

  public:
    Mesh() { }
    ~Mesh() { }
    void read(string input);
    void translate_vertex();
    void triangulate_face();
    void convert_face_normal();
    void voxelize(Coordinate &space_lower_bound, Coordinate &space_upper_bound, Coordinate &number_of_voxels, bool convert_normal);
    
    // initialized in read()
    int number_of_vertices;
    int number_of_faces;
    int number_of_edges;
    Coordinate lower_bound;
    Coordinate upper_bound;
    vector<Coordinate> vertex;
    vector<vector<int>> face;
    vector<Coordinate> face_normal;
    vector<int> not_triangle_face;
    // initialized in read()
};

extern bool convert_normal;
extern string input, output;
extern Coordinate space_lower_bound, space_upper_bound, number_of_voxels;

rational<int> to_rational(const char str[]);
rational<int> to_rational(string str);
rational<int> dot_product(Coordinate a, Coordinate b);
Coordinate operator+(const Coordinate &a, const Coordinate &b);
Coordinate operator-(const Coordinate &a, const Coordinate &b);
Coordinate operator*(const Coordinate &a, const Coordinate &b);
void process_command(int argc, char const *argv[]);

#endif