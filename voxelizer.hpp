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
    bool enter       = false;   // ray is entering mesh
    bool exit        = false;   // ray is exiting mesh
    bool touch_start = false;   // touch means the ray is on the surface
    bool touch_end   = false;
    vector<int> shared_face;    // faces that sharing this intersection point
};

class Mesh
{
    private:
    const Coordinate ray{0, 0, 1};
    vector<vector<vector<Intersection>>> intersection_points_along_ray;
    bool intersectant(rational<int> x, rational<int> y, Coordinate A, Coordinate B, Coordinate C);
    void insert_intersection_point_along_ray(int i, int j, const Intersection &point, int face_num);
    void process_ray_triangle_intersection(const Coordinate &space_lower_bound, const Coordinate &voxel_size, int face_num);
    void tag_ray_mesh_intersection_points(const Coordinate &voxel_size)
    
    public: 
    Mesh() {}
    ~Mesh() { }
    void read(string input);
    void translate_vertices();
    void triangulate_faces();
    void convert_faces_normal();
    void voxelize(const Coordinate &space_lower_bound, const Coordinate &space_upper_bound, const Coordinate &number_of_voxels, const string &output, bool convert_normal);

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

string input, output;
extern bool convert_normal;
extern Coordinate space_lower_bound, space_upper_bound, number_of_voxels;

rational<int> to_rational(const char str[]);
rational<int> to_rational(string str);
rational<int> dot_product(Coordinate a, Coordinate b);
Coordinate operator+(const Coordinate &a, const Coordinate &b);
Coordinate operator-(const Coordinate &a, const Coordinate &b);
Coordinate operator*(const Coordinate &a, const Coordinate &b);
void process_command(int argc, char const *argv[]);

#endif