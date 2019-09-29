#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>

#include "boost/rational.hpp"

struct Coordinate
{
    boost::rational<int> x, y, z;
};

class Vector : public Coordinate
{
    public:
    Vector()=default;
    Vector(const Coordinate& a, const Coordinate& b);
    ~Vector(){};
    boost::rational<int> DotProduct(const Vector& v);
    // Vector operator+(const Coordinate &a, const Coordinate &b);
    // Vector operator-(const Coordinate &a, const Coordinate &b);
    Vector operator*(const Vector& v);
};

// struct Intersection
// {
//     boost::rational<int> value; // coordinate of ray-mesh intersection point in this case is z axis
//     bool enter       = false;   // ray is entering the mesh
//     bool exit        = false;   // ray is exiting the mesh
//     bool touch_start = false;   // touch means the ray is on the surface
//     bool touch_end   = false;
//     std::vector<int> sharedFaces;    // faces that sharing this intersection point
// };

class Mesh
{
    public: 
    Mesh(const std::string& inputFile);
    ~Mesh() {}
    
    // void Triangulate();
    // void convert_faces_normal();
    // void Voxelize(const Coordinate &spaceLowerBound, const Coordinate &spaceUpperBound, const Coordinate &voxelAmt, const std::string &output, bool reverseFaceNormal);

    // initialized in LoadFile()
    int vertexAmt;
    int faceAmt;
    int edgeAmt;
    Coordinate lowerBound;
    Coordinate upperBound;
    std::vector<Coordinate> vertex;
    std::vector<std::array<int, 3>> face;
    std::vector<Vector> faceNormal;
    // initialized in LoadFile()

    // private:
    // const Coordinate ray{0, 0, 1};
    // std::vector<std::vector<std::vector<Intersection>>> intersection_points_along_ray;
    // bool IsIntersectant(boost::rational<int> x, boost::rational<int> y, Coordinate A, Coordinate B, Coordinate C);
    // void insert_intersection_point_along_ray(int i, int j, const Intersection &point, int face_num);
    // void process_ray_triangle_intersection(const Coordinate &spaceLowerBound, const Coordinate &voxel_size, int face_num);
    // void tag_ray_mesh_intersection_points(const Coordinate &voxel_size);
};


// boost::rational<int> ToRational(const char str[]);
boost::rational<int> ToRational(const std::string& str);
//void ProcessCommand(int argc, char const *argv[]);

class Command
{
    public:
    Command(int argc, char const *argv[]);
    ~Command(){};
    
    std::string inputFile, outputFile;
    bool reverseFaceNormal = false;
    Coordinate spaceLowerBound{0,0,0}, spaceUpperBound{255,255,255}, voxelAmt{255,255,255};
};

#endif