#include "utility3d.hpp"

mpq_class stor(const std::string& str)
{
    mpq_class fraction;
    auto dpos = str.find('.');
    if (dpos == std::string::npos)
    { 
        fraction = str;
    }
    else
    {
        mpq_class tens = 1;
        for (size_t i = str.length() - dpos - 1; i > 0; i--)
        {
            tens*=10;
        }
        mpq_class num;
        num.set_str(str.substr(0, dpos) + str.substr(dpos+1), 10);
        fraction = num/tens;
    }
    return fraction;
}

Vector Vector::operator*(const Vector& v)
{
    Vector p;
    p.x = y * v.z - z * v.y;
    p.y = z * v.x - x * v.z;
    p.z = x * v.y - y * v.x;
    return p;
}

mpq_class Vector::DotProduct(const Vector& v) const
{
    return x * v.x + y * v.y + z * v.z;
}