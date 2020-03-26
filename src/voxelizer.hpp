#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP
#pragma once

#include "mesh.hpp"

class Voxelizer
{
private:
    struct Intersection
    {
        mpq_class value;                // coordinate of intersection point along a given ray
        bool enter      = false;        // ray is entering the mesh
        bool exit       = false;        // ray is exiting the mesh
        bool touchStart = false;        // touch means the ray is on the surface
        bool touchEnd   = false;
        std::vector<int> sharedFaces;   // face that sharing this intersection point
    };
    Vector ray{0,0,1};
    Counter voxelAmt;
    Coordinate voxelSize;
    std::vector<std::vector<std::list<Intersection>>> intersectionOnRay;
    void FindIntersection(const Mesh& mesh, BoundingBox& space);
    bool IsIntersectant(const mpq_class& x, const mpq_class& y, Coordinate v1, Coordinate v2, Coordinate v3);
    void InsertIntersection(const int i, const int j, Intersection& point, int faceNum);

public:
    Voxelizer()=default;
    Voxelizer(const Mesh& mesh, BoundingBox& space, const Counter& voxelAmount);
    ~Voxelizer(){};

    void Voxelize(const Mesh& mesh, BoundingBox& space, const Counter& voxelAmount);
    void WriteOut(const std::string& voxelFile);
    std::vector<std::vector<std::vector<unsigned char>>> voxel;
};

#endif