#ifndef UTILITY3D_HPP
#define UTILITY3D_HPP
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <cmath>
#include <list>
#include <vector>
#include <functional>
#include <unordered_map>
#include "gmpxx.h"
#include "config.h"

#ifdef DBG
    void dbgmsg(const std::string& func, const int line)
    {
        static std::string lastcall;
        if (lastcall != func)
        {
            std::cout << "Executing " <<func<< "() in line: " << line << std::endl;
            lastcall = func;
        }
    }
#endif

mpq_class stor(const std::string& str);

struct Counter
{
    int x,y,z;
};

struct Coordinate
{
    mpq_class x, y, z;
    Coordinate()=default;
    Coordinate(const mpq_class i, const mpq_class j, const mpq_class k):x(i), y(j), z(k) {};
};

struct BoundingBox
{
    Coordinate min, max;
};

struct Vector : public Coordinate
{
    Vector()=default;
    Vector(const mpq_class x, const mpq_class y, const mpq_class z) : Coordinate(x,y,z) {};
    Vector(const Coordinate& a, const Coordinate& b) : Coordinate(b.x - a.x, b.y - a.y, b.z - a.z) {};
    ~Vector() {};
    Vector operator*(const Vector& v);
    mpq_class DotProduct(const Vector& v) const;
};

#endif