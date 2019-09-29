#include "voxelizer.hpp"

Command::Command(int argc, char const *argv[])
{
    std::string help(
        "-i  input mash file\n"
        "-o  output voxel file\n"
        "-rn reverse polygon normal vextor\n"
        "-sl space lower bound\n"
        "-su space upper bound\n"
        "-n  number of voxels along x y z\n");

    while (argc > 2)
    {
        argc--; argv++;
        if (!strcmp(*argv, "-i")) // input mash file name
        {
            argv++; argc--;
            inputFile = *argv;
        }
        else if (!strcmp(*argv, "-o")) // output voxel file name
        {
            argv++; argc--;
            outputFile = *argv;
        }
        else if (!strcmp(*argv, "-rn")) // reverse mash polygon normal
        {
            argv++; argc--;
            reverseFaceNormal = true;
        }
        else if (!strcmp(*argv, "-sl")) // set space lower bound
        {
            argv++; argc--;
            spaceLowerBound.x = ToRational(*argv);;
            argv++; argc--;
            spaceLowerBound.y = ToRational(*argv);;
            argv++; argc--;
            spaceLowerBound.z = ToRational(*argv);;
        }
        else if (!strcmp(*argv, "-su")) // set space upper bound
        {
            argv++; argc--;
            spaceUpperBound.x = ToRational(*argv);;
            argv++; argc--;
            spaceUpperBound.y = ToRational(*argv);;
            argv++; argc--;
            spaceUpperBound.z = ToRational(*argv);;
        }
        else if (!strcmp(*argv, "-n")) // set number of voxels along x y z
        {
            argv++; argc--;
            voxelAmt.x = std::stoi(*argv);
            argv++; argc--;
            voxelAmt.y = std::stoi(*argv);
            argv++; argc--;
            voxelAmt.z = std::stoi(*argv);
            if (!(voxelAmt.x > 0 && voxelAmt.y > 0 && voxelAmt.z > 0))
            {
                std::cerr<<"Invalid voxel number!\n";
                std::exit(1);
            }
        }
        else
        {
            std::cout<<help;
            exit(1);
        }
    }

    if (inputFile.empty())
    {
        std::cerr << "Need input file!\n"<<help;
        exit(1);
    }
    else if (outputFile.empty())
    {
        std::istringstream str(inputFile);
        std::getline(str, outputFile, '.');
        outputFile+=".raw";
    }
}

// boost::rational<int> ToRational(const char str[])
// {
//     int length, numerator, denominator;
//     double number;
//     length = std::strlen(str);
//     number = std::stod(str);
//     numerator = (int)(number * std::pow(10, length));
//     denominator = std::pow(10, length);
//     boost::rational<int> fraction(numerator, denominator);
//     return fraction;
// }

boost::rational<int> ToRational(const std::string& str)
{
    int length, numerator, denominator;
    double number;
    length = str.size();
    number = std::stod(str);
    numerator = (int)(number * std::pow(10, length));
    denominator = std::pow(10, length);
    boost::rational<int> fraction(numerator, denominator);
    return fraction;
}


Vector::Vector(const Coordinate& a, const Coordinate& b)
{
    x = b.x - a.x;
    y = b.y - a.y;
    z = b.z - a.z;
}

boost::rational<int> Vector::DotProduct(const Vector& v)
{
    return x * v.x + y * v.y + z * v.z;
}

Vector Vector::operator*(const Vector& v)
{
    Vector p;
    p.x = y * v.z - z * v.y;
    p.y = z * v.x - x * v.z;
    p.z = x * v.y - y * v.x;
    return p;
}

// Coordinate operator+(const Coordinate &a, const Coordinate &b)
// {
//     Coordinate p;
//     p.x = a.x + b.x;
//     p.y = a.y + b.y;
//     p.z = a.z + b.z;
//     return p;
// }

// Coordinate operator-(const Coordinate &a, const Coordinate &b)
// {
//     Coordinate p;
//     p.x = a.x - b.x;
//     p.y = a.y - b.y;
//     p.z = a.z - b.z;
//     return p;
// }




Mesh::Mesh(const std::string& inputFile)
{
    std::ifstream data(inputFile);
    if (!data)
    {
        std::cerr << "can not open input file!\n";
        exit(1);
    }

    std::string str;
    std::getline(data,str);
    
    data >> str;
    while (str == "#")
    {
        std::getline(data, str);
        data >> str;
    }

    vertexAmt = std::stoi(str);
    data >> str;
    faceAmt = std::stoi(str);
    data >> str;
    edgeAmt = std::stoi(str);
    std::cout << "Mash info: vertices("<<vertexAmt<<") faces("<<faceAmt<<") edges("<<edgeAmt<<")\n";

    Coordinate aVertex;
    data >> str;
    aVertex.x = ToRational(str);
    lowerBound.x = upperBound.x = aVertex.x;
    data >> str;
    aVertex.y = ToRational(str);
    lowerBound.y = upperBound.y = aVertex.y;
    data >> str;
    aVertex.z = ToRational(str);
    lowerBound.z = upperBound.z = aVertex.z;
    vertex.push_back(aVertex);

    for (int i = 1; i < vertexAmt; ++i)
    {
        data >> str;
        aVertex.x = ToRational(str);
        if(aVertex.x < lowerBound.x)
            lowerBound.x = aVertex.x;
        else if(aVertex.x > upperBound.x)
            upperBound.x = aVertex.x;

        data >> str;
        aVertex.y = ToRational(str);
        if(aVertex.y < lowerBound.y)
            lowerBound.y = aVertex.y;
        else if(aVertex.y > upperBound.y)
            upperBound.y = aVertex.y;

        data >> str;
        aVertex.z = ToRational(str);
        if(aVertex.z < lowerBound.z)
            lowerBound.z = aVertex.z;
        else if(aVertex.z > upperBound.z)
            upperBound.z = aVertex.z;

        vertex.push_back(aVertex);
    }

    
    std::getline(data,str);
    int addEdges = 0;
    for(int i = 0; i < faceAmt; i++)
    {
        std::getline(data,str);
        std::istringstream line(str);
        line >> str;
        std::array<int, 3> aFace;
        line >> str;
        aFace[0] = std::stoi(str);
        line >> str;
        aFace[1] = std::stoi(str);

        int count = -1;
        int last = aFace[1];
        while (line >> str)
        {
            ++count;
            aFace[1] = last;
            aFace[2] = std::stoi(str);
            face.push_back(aFace);
            Vector v1(vertex[aFace[0]], vertex[aFace[1]]);
            Vector v2(vertex[aFace[1]], vertex[aFace[2]]);
            faceNormal.push_back(v1 * v2);
            last = aFace[2];
            addEdges += count;
        }
    }
    data.close();

    if (addEdges!=0)
    {
        edgeAmt+=addEdges;
        faceAmt+=addEdges;
        std::cout << "Mash triangulated, added "<<addEdges<<" faces.\n";
        std::cout << "Mash info: vertices("<<vertexAmt<<") faces("<<faceAmt<<") edges("<<edgeAmt<<")\n";
    }
    std::cout<<"           lower bound("<<lowerBound.x<<", "<<lowerBound.y<<", "<<lowerBound.z<<
    ")  upper bound("<<upperBound.x<<", "<<upperBound.y<<", "<<upperBound.z<<")\n";
}

/*
    while (lowerBound.x < com.spaceLowerBound.x || lowerBound.y < com.spaceLowerBound.y || lowerBound.z < com.spaceLowerBound.z)
    {
        std::cout << "set space lower bound smaller than (" << lowerBound.x << ", " << lowerBound.y << ", " << lowerBound.z << ")\n-sl ";
        std::cin >> str;
        com.spaceLowerBound.x = ToRational(str);
        std::cin >> str;
        com.spaceLowerBound.y = ToRational(str);
        std::cin >> str;
        com.spaceLowerBound.z = ToRational(str);
    }
    cout << "new space lower bound set: (" << com.spaceLowerBound.x << ", " << com.spaceLowerBound.y << ", " << com.spaceLowerBound.z << ")\n";
    
    while(upperBound.x > com.spaceUpperBound.x || upperBound.y > com.spaceUpperBound.y || upperBound.z > com.spaceUpperBound.z)
    {
        std::cout << "set space upper bound bigger than (" << upperBound.x << ", " << upperBound.y << ", " << upperBound.z << ")\n-su ";
        std::cin >> str;
        com.spaceUpperBound.x = ToRational(str);
        std::cin >> str;
        com.spaceUpperBound.y = ToRational(str);
        std::cin >> str;
        com.spaceUpperBound.z = ToRational(str);
    }
    cout << "new space upper bound set: (" << com.spaceUpperBound.x << ", " << com.spaceUpperBound.y << ", " << com.spaceUpperBound.z << ")\n";
*/