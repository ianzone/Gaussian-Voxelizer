#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <functional>
#include <unordered_map>

#include "boost/rational.hpp"

boost::rational<int> stor(const std::string& str);

struct Counter
{
    int x,y,z;
};

struct Coordinate
{
    Coordinate()=default;
    Coordinate(const boost::rational<int> i, const boost::rational<int> j, const boost::rational<int> k):x(i), y(j), z(k){};
    boost::rational<int> x, y, z;
};

struct BoundingBox
{
    Coordinate min, max;
};

struct Vector : public Coordinate
{
    Vector()=default;
    Vector(const boost::rational<int> x, const boost::rational<int> y, const boost::rational<int> z) : Coordinate(x,y,z){};
    Vector(const Coordinate& a, const Coordinate& b) : Coordinate(b.x - a.x, b.y - a.y, b.z - a.z){};
    ~Vector(){};
    Vector operator*(const Vector& v);
    boost::rational<int> DotProduct(const Vector& v) const;
};

class Mesh
{
    public:
    Mesh()=default;
    Mesh(const std::string& meshFile, const bool insideout);
    ~Mesh(){}

    int vertexAmt;
    int faceAmt;
    int edgeAmt;
    BoundingBox bound;
    std::vector<Coordinate> vertex;
    std::vector<std::array<int, 3>> face;
    std::vector<Vector> normal;
    void WriteOut(std::string meshFile="");

    private:
    const std::string fileName;
    bool triangulated = false;
    bool reverseNormal = false;
};

class Voxelizer
{
    public:
    Voxelizer()=default;
    Voxelizer(const Mesh& mesh, BoundingBox& space, const Vector& Ray, const Counter& voxelAmount);
    ~Voxelizer(){};

    void Voxelize(const Mesh& mesh, BoundingBox& space, const Vector& Ray, const Counter& voxelAmount);
    void WriteOut(const std::string& voxelFile);
    std::vector<std::vector<std::vector<unsigned char>>> voxel;

    private:
    struct Intersection
    {
        boost::rational<int> value; // coordinate of intersection point along a given ray
        bool enter      = false;    // ray is entering the mesh
        bool exit       = false;    // ray is exiting the mesh
        bool touchStart = false;    // touch means the ray is on the surface
        bool touchEnd   = false;
        std::vector<int> sharedFaces;    // face that sharing this intersection point
    };
    Vector ray;
    Counter voxelAmt;
    Coordinate voxelSize;
    std::vector<std::vector<std::list<Intersection>>> intersectionOnRay;
    void FindIntersection(const Mesh& mesh, BoundingBox& space);
    bool IsIntersectant(const boost::rational<int>& x, const boost::rational<int>& y, Coordinate v1, Coordinate v2, Coordinate v3);
    void InsertIntersection(const int i, const int j, Intersection& point, int faceNum);
};

struct Command
{
    Command(int argc, char const *argv[]);
    ~Command(){};
    
    std::string meshFile, voxelFile;
    bool insideout = false;
    Vector ray{0,0,1};
    BoundingBox space;
    Counter voxelAmount{255,255,255};
};

#endif