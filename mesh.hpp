#ifndef MESH_HPP
#define MESH_HPP
#pragma once

#include "utility3d.hpp"

class Mesh
{
private:
    const std::string fileName;
    bool triangulated = false;
    bool reverseNormal = false;

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
};

#endif