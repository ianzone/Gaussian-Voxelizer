#include "voxelizer.hpp"

Command::Command(int argc, char const *argv[])
{
    std::string help(
        "-i   input mash file\n"
        "-o   output voxel file\n"
        "-rn  reverse polygon normal vextor\n"
        "-sl  space lower bound\n"
        "-su  space upper bound\n"
        "-n   number of voxels along x y z\n"
        "-ray ray casting orientation\n");

    while (argc > 1)
    {
        argc--; argv++;
        if (!strcmp(*argv, "-i")) // input mash file name
        {
            argv++; argc--;
            meshFile = *argv;
            std::istringstream iss(meshFile);
            std::string suffix;
            getline(iss, suffix, '.');
            iss>>suffix;
            if (suffix != "off")
            {
                std::cerr<<"Unsupported mesh format! Supported format is .off\n";
                exit(1);
            }
        }
        else if (!strcmp(*argv, "-o")) // output voxel file name
        {
            argv++; argc--;
            voxelFile = *argv;
            std::istringstream iss(voxelFile);
            std::string suffix;
            getline(iss, suffix, '.');
            iss>>suffix;
            if (suffix != "raw")
            {
                std::cerr<<"Unsupported voxel format! Supported format is .raw\n";
                exit(1);
            }
        }
        else if (!strcmp(*argv, "-rn")) // reverse mash polygon normal
        {
            insideout = true;
        }
        else if (!strcmp(*argv, "-sl")) // set space lower bound
        {
            argv++; argc--;
            space.min.x = stor(*argv);
            argv++; argc--;
            space.min.y = stor(*argv);
            argv++; argc--;
            space.min.z = stor(*argv);
        }
        else if (!strcmp(*argv, "-su")) // set space upper bound
        {
            argv++; argc--;
            space.max.x = stor(*argv);
            argv++; argc--;
            space.max.y = stor(*argv);
            argv++; argc--;
            space.max.z = stor(*argv);
        }
        else if (!strcmp(*argv, "-n")) // set number of voxels along x y z
        {
            argv++; argc--;
            voxelAmount.x = std::stoi(*argv);
            argv++; argc--;
            voxelAmount.y = std::stoi(*argv);
            argv++; argc--;
            voxelAmount.z = std::stoi(*argv);
            if (!(voxelAmount.x > 0 and voxelAmount.y > 0 and voxelAmount.z > 0))
            {
                std::cerr<<"Invalid voxel number!\n";
                exit(1);
            }
        }
        else if (!strcmp(*argv, "-ray"))
        {
            argv++; argc--;
            if (!strcmp(*argv, "x"))
                ray = {1,0,0};
            else if (!strcmp(*argv, "y"))
                ray = {0,1,0};
            else if (!strcmp(*argv, "z"))
                ray = {0,0,1};
            else
            {
                std::cerr<<"Invalid ray orientation!\n";
                exit(1);
            }
        }
        else
        {
            std::cout<<help;
            exit(1);
        }
    }

    if (meshFile.empty())
    {
        std::cerr << "Need input file!\n"<<help;
        exit(1);
    }
    else if (voxelFile.empty())
    {
        std::istringstream str(meshFile);
        std::getline(str, voxelFile, '.');
        voxelFile+=".raw";
    }
}

boost::rational<int> stor(const std::string& str)
{
    double number = std::stod(str);
    int length = str.size();
    int numerator = (int)(number * std::pow(10, length));
    int denominator = std::pow(10, length);
    boost::rational<int> fraction(numerator, denominator);
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

boost::rational<int> Vector::DotProduct(const Vector& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

Mesh::Mesh(const std::string& meshFile,  const bool insideout) : fileName(meshFile), reverseNormal(insideout)
{
    std::ifstream data(fileName);
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

    vertex.resize(vertexAmt);
    data >> str;
    vertex[0].x = stor(str);
    bound.min.x = bound.max.x = vertex[0].x;
    data >> str;
    vertex[0].y = stor(str);
    bound.min.y = bound.max.y = vertex[0].y;
    data >> str;
    vertex[0].z = stor(str);
    bound.min.z = bound.max.z = vertex[0].z;

    for (int i = 1; i < vertexAmt; ++i)
    {
        data >> str;
        vertex[i].x = stor(str);
        if(vertex[i].x < bound.min.x)
            bound.min.x = vertex[i].x;
        else if(vertex[i].x > bound.max.x)
            bound.max.x = vertex[i].x;

        data >> str;
        vertex[i].y = stor(str);
        if(vertex[i].y < bound.min.y)
            bound.min.y = vertex[i].y;
        else if(vertex[i].y > bound.max.y)
            bound.max.y = vertex[i].y;

        data >> str;
        vertex[i].z = stor(str);
        if(vertex[i].z < bound.min.z)
            bound.min.z = vertex[i].z;
        else if(vertex[i].z > bound.max.z)
            bound.max.z = vertex[i].z;
    }

    // read faces
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
            last = aFace[2];
            addEdges += count;
            
            if (reverseNormal)
            {
                std::array<int, 3> rFace{aFace[2], aFace[1], aFace[0]};
                face.push_back(rFace);
                Vector a(vertex[rFace[0]], vertex[rFace[1]]);
                Vector b(vertex[rFace[1]], vertex[rFace[2]]);
                normal.push_back(a * b);
            }
            else
            {
                face.push_back(aFace);
                Vector a(vertex[aFace[0]], vertex[aFace[1]]);
                Vector b(vertex[aFace[1]], vertex[aFace[2]]);
                normal.push_back(a * b);
            }
        }
    }
    data.close();

    if (face.size()!=faceAmt)
    {
        triangulated = true;
        std::cout << "Mash triangulated, added "<<face.size()-faceAmt<<" faces.\n";
        edgeAmt +=addEdges;
        faceAmt = face.size();
        std::cout << "Mash info: vertices("<<vertexAmt<<") faces("<<faceAmt<<") edges("<<edgeAmt<<") ";
    }
    std::cout<<"lower bound("<<bound.min.x<<", "<<bound.min.y<<", "<<bound.min.z<<
    ")  upper bound("<<bound.max.x<<", "<<bound.max.y<<", "<<bound.max.z<<")\n";
}

void Mesh::WriteOut(std::string meshFile)
{
    if (triangulated or reverseNormal)
    {
        if (meshFile.empty() or meshFile == fileName)
        {
            std::istringstream iss(fileName);
            getline(iss, meshFile, '.');
            meshFile += (triangulated?"_triangulated":"");
            meshFile += (reverseNormal?"_insideout":"");
            meshFile += ".off";
        }
        
        std::ofstream triMesh(meshFile);
        if (!triMesh)
        {
            std::cerr << "can not create triangle mesh file!\n";
            exit(1);
        }
        triMesh << "OFF\n#" <<(triangulated?" triangulated":"")<<(reverseNormal?" inside out":"")<<" version of "<<fileName<<'\n';
        triMesh << vertexAmt <<' '<< faceAmt <<' '<< edgeAmt <<'\n';
        
        for (auto itr = vertex.begin(); itr != vertex.end(); ++itr)
        {
            triMesh << boost::rational_cast<double>(itr->x)<<' '<<boost::rational_cast<double>(itr->y)<<' '<<boost::rational_cast<double>(itr->z)<<'\n';
        }

        for (auto itr = face.begin(); itr != face.end(); ++itr)
        {
            triMesh << "3 " << (*itr)[2] <<' '<< (*itr)[1] <<' '<< (*itr)[0] <<'\n';
        }
    }
    else
    {
        std::cout << "The mesh has not been triangulated or reversed polygon normal\n";
    }
}

Voxelizer::Voxelizer(const Mesh& mesh, BoundingBox& space, const Vector& Ray, const Counter& voxelAmount)
{Voxelize(mesh, space, Ray, voxelAmount);}
void Voxelizer::Voxelize(const Mesh& mesh, BoundingBox& space, const Vector& Ray, const Counter& voxelAmount)
{
    std::string str;
    while (mesh.bound.min.x < space.min.x or mesh.bound.min.y < space.min.y or mesh.bound.min.z < space.min.z)
    {
        std::cout << "Set space lower bound smaller than (" << mesh.bound.min.x << ", " << mesh.bound.min.y << ", " << mesh.bound.min.z << ")\n-sl ";
        std::cin >> str;
        space.min.x = stor(str);
        std::cin >> str;
        space.min.y = stor(str);
        std::cin >> str;
        space.min.z = stor(str);
    }
    
    while(mesh.bound.max.x > space.max.x or mesh.bound.max.y > space.max.y or mesh.bound.max.z > space.max.z)
    {
        std::cout << "Set space upper bound bigger than (" << mesh.bound.max.x << ", " << mesh.bound.max.y << ", " << mesh.bound.max.z << ")\n-su ";
        std::cin >> str;
        space.max.x = stor(str);
        std::cin >> str;
        space.max.y = stor(str);
        std::cin >> str;
        space.max.z = stor(str);
    }

    voxelAmt = voxelAmount; ray = Ray;

    voxelSize.x = (space.max.x - space.min.x) / (voxelAmt.x - 1);
    voxelSize.y = (space.max.y - space.min.y) / (voxelAmt.y - 1);
    voxelSize.z = (space.max.z - space.min.z) / (voxelAmt.z - 1);

    int imax, jmax;
    boost::rational<int> unit, valmin;
    if (ray.z)
    {
        intersectionOnRay.resize(voxelAmt.x);
        voxel.resize(voxelAmt.x);
        for (int i = 0; i < voxelAmt.x; ++i)
        {
            intersectionOnRay[i].resize(voxelAmt.y);
            voxel[i].resize(voxelAmt.y);
            for (int j = 0; j < voxelAmt.y; ++j)
                voxel[i][j].resize(voxelAmt.z);
        }
        imax = voxelAmt.x;
        jmax = voxelAmt.y;
        unit = voxelSize.z;
        valmin = space.min.z;
    }
    else if (ray.y)
    {
        intersectionOnRay.resize(voxelAmt.x);
        voxel.resize(voxelAmt.x);
        for (int i = 0; i < voxelAmt.x; ++i)
        {
            intersectionOnRay[i].resize(voxelAmt.z);
            voxel[i].resize(voxelAmt.z);
            for (int j = 0; j < voxelAmt.z; ++j)
                voxel[i][j].resize(voxelAmt.y);
        }
        imax = voxelAmt.x;
        jmax = voxelAmt.z;
        unit = voxelSize.y;
        valmin = space.min.y;
    }
    else //if (ray.x)
    {
        intersectionOnRay.resize(voxelAmt.y);
        voxel.resize(voxelAmt.y);
        for (int i = 0; i < voxelAmt.y; ++i)
        {
            intersectionOnRay[i].resize(voxelAmt.z);
            voxel[i].resize(voxelAmt.z);
            for (int j = 0; j < voxelAmt.z; ++j)
                voxel[i][j].resize(voxelAmt.x);
        }
        imax = voxelAmt.y;
        jmax = voxelAmt.z;
        unit = voxelSize.x;
        valmin = space.min.x;
    }

    FindIntersection(mesh, space);
    // identify voxels
    Coordinate voxelCor;
    for (int i = 0; i < imax; ++i)
    {
        for (int j = 0; j < jmax; ++j)
        {
            for (auto itr = intersectionOnRay[i][j].begin(); itr != intersectionOnRay[i][j].end(); ++itr)
            {
                if ((itr->enter == true and itr->exit == false or itr->touchStart == true) and intersectionOnRay[i][j].size() > 1)
                {
                    auto r = (itr->value - valmin) / unit;
                    int z = boost::rational_cast<int>(r);
                    if (z == r)
                        voxel[i][j][z] = 255;
                    ++z;
                    while (!(itr->exit == true and itr->enter == false and itr->touchStart == false))
                    {
                        ++itr;
                    }
                    while (valmin + z*unit <= itr->value)
                    {
                        voxel[i][j][z] = 255;
                        ++z;
                    }
                }
                else if (intersectionOnRay[i][j].size() == 1)
                {
                    auto r = (itr->value - valmin) / unit;
                    int z = boost::rational_cast<int>(r);
                    if (z == r) {
                        voxel[i][j][z] = 255;
                    }
                }
            }
        }
    }
}

void Voxelizer::WriteOut(const std::string& voxelFile)
{
    std::ofstream voxelSet(voxelFile);
    if (!voxelSet)
    {
        std::cerr << "Can not create voxel file!\n";
        exit(1);
    }

    for (int z = 0; z < voxelAmt.z; ++z)
        for (int y = 0; y < voxelAmt.y; ++y)
            for (int x = 0; x < voxelAmt.x; ++x)
            {
                if (ray.z)
                    voxelSet << voxel[x][y][z];
                else if (ray.y)
                    voxelSet << voxel[x][z][y];
                else
                    voxelSet << voxel[y][z][x];
            }

    voxelSet.close();
}

void Voxelizer::FindIntersection(const Mesh& mesh, BoundingBox& space)
{
    for (int faceNum = 0; faceNum < mesh.faceAmt; ++faceNum)
    {
        // three vertices of the triangle
        Coordinate v1{mesh.vertex[mesh.face[faceNum][0]].x, mesh.vertex[mesh.face[faceNum][0]].y, mesh.vertex[mesh.face[faceNum][0]].z};
        Coordinate v2{mesh.vertex[mesh.face[faceNum][1]].x, mesh.vertex[mesh.face[faceNum][1]].y, mesh.vertex[mesh.face[faceNum][1]].z};
        Coordinate v3{mesh.vertex[mesh.face[faceNum][2]].x, mesh.vertex[mesh.face[faceNum][2]].y, mesh.vertex[mesh.face[faceNum][2]].z};

        // get triangle bounding box on x,y,z plane
        BoundingBox triBound;
        Counter voxelCount;
        Coordinate startPoint;
        if (ray.x == 0)
        {
            triBound.min.x = triBound.max.x = v1.x;
            if (v2.x < triBound.min.x)
                triBound.min.x = v2.x;
            else if (v2.x > triBound.max.x)
                triBound.max.x = v2.x;
            if (v3.x < triBound.min.x)
                triBound.min.x = v3.x;
            else if (v3.x > triBound.max.x)
                triBound.max.x = v3.x;
            voxelCount.x = boost::rational_cast<int>((triBound.min.x - space.min.x) / voxelSize.x);
            startPoint.x = space.min.x + voxelSize.x * voxelCount.x;
        }
        if (ray.y == 0)
        {
            triBound.min.y = triBound.max.y = v1.y;
            if (v2.y < triBound.min.y)
                triBound.min.y = v2.y;
            else if (v2.y > triBound.max.y)
                triBound.max.y = v2.y;
            if (v3.y < triBound.min.y)
                triBound.min.y = v3.y;
            else if (v3.y > triBound.max.y)
                triBound.max.y = v3.y;
            voxelCount.y = boost::rational_cast<int>((triBound.min.y - space.min.y) / voxelSize.y);
            startPoint.y = space.min.y + voxelSize.y * voxelCount.y;
        }
        if (ray.z == 0)
        {
            triBound.min.z = triBound.max.z = v1.z;
            if (v2.z < triBound.min.z)
                triBound.min.z = v2.z;
            else if (v2.z > triBound.max.z)
                triBound.max.z = v2.z;
            if (v3.z < triBound.min.z)
                triBound.min.z = v3.z;
            else if (v3.z > triBound.max.z)
                triBound.max.z = v3.z;
            voxelCount.z = boost::rational_cast<int>((triBound.min.z - space.min.z) / voxelSize.z);
            startPoint.z = space.min.z + voxelSize.z * voxelCount.z;
        }
        // get triangle bounding box on x,y,z plane

        auto orientation = ray.DotProduct(mesh.normal[faceNum]);
        auto triFormulaD = mesh.normal[faceNum].x * v1.x + mesh.normal[faceNum].y * v1.y + mesh.normal[faceNum].z * v1.z;
        boost::rational<int> iStart, jStart, imax, jmax, iunit, junit;
        int ipos, jpos;
        if (ray.z)
        {
            iStart = startPoint.x;
            imax = triBound.max.x;
            iunit = voxelSize.x;
            ipos = voxelCount.x;
            jStart = startPoint.y;
            jmax = triBound.max.y;
            junit = voxelSize.y;
            jpos = voxelCount.y;
        }
        else if (ray.y)
        {
            iStart = startPoint.x;
            imax = triBound.max.x;
            iunit = voxelSize.x;
            ipos = voxelCount.x;
            jStart = startPoint.z;
            jmax = triBound.max.z;
            junit = voxelSize.z;
            jpos = voxelCount.z;
        }
        else //if (ray.x)
        {
            iStart = startPoint.y;
            imax = triBound.max.y;
            iunit = voxelSize.y;
            ipos = voxelCount.y;
            jStart = startPoint.z;
            jmax = triBound.max.z;
            junit = voxelSize.z;
            jpos = voxelCount.z;
        }
        
        int temp = jpos;
        for (auto i = iStart; i <= imax; i+=iunit, ++ipos)
        {   jpos = temp;
            for (auto j = jStart; j <= jmax; j+=junit, ++jpos)
            {
                if (IsIntersectant(i, j, v1, v2, v3))
                {
                    Intersection point;

                    if (orientation < 0)
                    {   // tag enter
                        point.enter = true;
                        point.value = (triFormulaD - mesh.normal[faceNum].x * i - mesh.normal[faceNum].y * j) / mesh.normal[faceNum].z;
                        InsertIntersection(ipos, jpos, point, faceNum);
                    }
                    else if (orientation > 0)
                    {   // tag exit
                        point.exit = true;
                        point.value = (triFormulaD - mesh.normal[faceNum].x * i - mesh.normal[faceNum].y * j) / mesh.normal[faceNum].z;
                        InsertIntersection(ipos, jpos, point, faceNum);
                    }
                    else
                    {   // tag touchStart and touchEnd
                        std::vector<boost::rational<int>> val;
                        if (ray.z)
                        {
                            if (v1.x != v2.x)   // xz plane
                                val.push_back((v2.z - v1.z) * (i - v1.x) / (v2.x - v1.x) + v1.z);
                            if (v1.y != v2.y)   // yz plane
                                val.push_back((v2.z - v1.z) * (j - v1.y) / (v2.y - v1.y) + v1.z);
                            if (v2.x != v3.x)
                                val.push_back((v2.z - v3.z) * (i - v3.x) / (v2.x - v3.x) + v3.z);
                            if (v2.y != v3.y)
                                val.push_back((v2.z - v3.z) * (j - v3.y) / (v2.y - v3.y) + v3.z);
                            if (v1.x != v3.x)
                                val.push_back((v3.z - v1.z) * (i - v1.x) / (v3.x - v1.x) + v1.z);
                            if (v1.y != v3.y)
                                val.push_back((v3.z - v1.z) * (j - v1.y) / (v3.y - v1.y) + v1.z);
                        }
                        else if (ray.y)
                        {
                            if (v1.x != v2.x)
                                val.push_back((v2.y - v1.y) * (i - v1.x) / (v2.x - v1.x) + v1.y);
                            if (v1.z != v2.z)
                                val.push_back((v2.y - v1.y) * (j - v1.z) / (v2.z - v1.z) + v1.y);
                            if (v2.x != v3.x)
                                val.push_back((v2.y - v3.y) * (i - v3.x) / (v2.x - v3.x) + v3.y);
                            if (v2.z != v3.z)
                                val.push_back((v2.y - v3.y) * (j - v3.z) / (v2.z - v3.z) + v3.y);
                            if (v1.x != v3.x)
                                val.push_back((v3.y - v1.y) * (i - v1.x) / (v3.x - v1.x) + v1.y);
                            if (v1.z != v3.z)
                                val.push_back((v3.y - v1.y) * (j - v1.z) / (v3.z - v1.z) + v1.y);
                        }
                        else
                        {
                            if (v1.y != v2.y)
                                val.push_back((v2.x - v1.x) * (i - v1.y) / (v2.y - v1.y) + v1.x);
                            if (v1.z != v2.z)
                                val.push_back((v2.x - v1.x) * (j - v1.z) / (v2.z - v1.z) + v1.x);
                            if (v2.y != v3.y)
                                val.push_back((v2.x - v3.x) * (i - v3.y) / (v2.y - v3.y) + v3.x);
                            if (v2.z != v3.z)
                                val.push_back((v2.x - v3.x) * (j - v3.z) / (v2.z - v3.z) + v3.x);
                            if (v1.y != v3.y)
                                val.push_back((v3.x - v1.x) * (i - v1.y) / (v3.y - v1.y) + v1.x);
                            if (v1.z != v3.z)
                                val.push_back((v3.x - v1.x) * (j - v1.z) / (v3.z - v1.z) + v1.x);
                        }

                        if (val[0] < val[1])
                        {
                            point.value = val[0];
                            point.touchStart = true;
                            InsertIntersection(ipos, jpos, point, faceNum);
                            point.value = val[1];
                            point.touchStart = false;
                            point.touchEnd = true;
                            InsertIntersection(ipos, jpos, point, faceNum);
                        }
                        else if (val[0] > val[1])
                        {
                            point.value = val[1];
                            point.touchStart = true;
                            InsertIntersection(ipos, jpos, point, faceNum);
                            point.value = val[0];
                            point.touchStart = false;
                            point.touchEnd = true;
                            InsertIntersection(ipos, jpos, point, faceNum);
                        }
                        else
                        {   // if touchStart and touchEnd is the same point, leave it, the point would be tagged when processing other triangles
                            point.value = val[0];
                            InsertIntersection(ipos, jpos, point, faceNum);
                        }
                    }
                }
            }
        }
    }

    // deal with the point on vertex
    // project the point and bottom edge of shared triangles to the plane that is perpendicular to the ray
    // if the point is inside projected polygon, the ray is entering
    int iAmt, jAmt;
    boost::rational<int> smini, sminj, uniti, unitj;
    if (ray.z)
    {
        iAmt = voxelAmt.x;
        jAmt = voxelAmt.y;
        smini = space.min.x;
        sminj = space.min.y;
        uniti = voxelSize.x;
        unitj = voxelSize.y;
    }
    else if (ray.y)
    {
        iAmt = voxelAmt.x;
        jAmt = voxelAmt.z;
        smini = space.min.x;
        sminj = space.min.z;
        uniti = voxelSize.x;
        unitj = voxelSize.z;
    }
    else
    {
        iAmt = voxelAmt.y;
        jAmt = voxelAmt.z;
        smini = space.min.y;
        sminj = space.min.z;
        uniti = voxelSize.y;
        unitj = voxelSize.z;
    }
    
    struct EdgeDirection
    {
        bool right2left = false;
        bool left2right = false;
    };
    struct RationalHash
    {
        size_t operator()(const boost::rational<int> key) const noexcept
        {
            return std::hash<std::string>{}(std::to_string(key.numerator())+"/"+std::to_string(key.denominator()));
        }
    };
    for (int i = 0; i < iAmt; ++i)
    {
        for (int j = 0; j < jAmt; ++j)
        {
            auto voxeli = smini + uniti*i;    // projected voxel coordinate
            auto voxelj = sminj + unitj*j;
            for (auto& pt : intersectionOnRay[i][j])
            {
                if (pt.sharedFaces.size() > 2 and pt.enter == true and pt.exit == true and pt.touchStart == false and pt.touchEnd == false)
                {
                    std::unordered_map<boost::rational<int>, EdgeDirection, RationalHash> parityCount;
                    boost::rational<int> jmin = voxelj;
                    for (int faceNum : pt.sharedFaces)
                    {
                        int v1 = mesh.face[faceNum][0];
                        int v2 = mesh.face[faceNum][1];
                        int v3 = mesh.face[faceNum][2];
                        auto& a = mesh.vertex[v1];
                        auto& b = mesh.vertex[v2];
                        auto& c = mesh.vertex[v3];

                        boost::rational<int> ai, aj, bi, bj, ci, cj;
                        if (ray.z)
                            {ai = a.x; aj = a.y;     bi = b.x; bj = b.y;     ci = c.x; cj = c.y;}
                        else if (ray.y)
                            {ai = a.x; aj = a.z;     bi = b.x; bj = b.z;     ci = c.x; cj = c.z;}
                        else
                            {ai = a.y; aj = a.z;     bi = b.y; bj = b.z;     ci = c.y; cj = c.z;}

                        // get point's opposite edge of shared triangle on xy plane
                        boost::rational<int> p1i, p1j, p2i, p2j, pj, slope, constant;
                        if (ai == voxeli and aj == voxelj) // voxel on a
                            {p1i = bi;    p1j = bj;    p2i = ci;    p2j = cj;}
                        else if (bi == voxeli and bj == voxelj) // voxel on b
                            {p1i = ci;    p1j = cj;    p2i = ai;    p2j = aj;}
                        else // voxel on c
                            {p1i = ai;    p1j = aj;    p2i = bi;    p2j = bj;}

                        if (p1i != p2i and (p1i <= voxeli and voxeli <= p2i || p2i <= voxeli and voxeli <= p1i))
                        {
                            slope = (p2j - p1j) / (p2i - p1i);
                            constant = p1j - slope * p1i;
                            pj = slope * voxeli + constant;

                            if (pj < voxelj)   // edge is below the ray
                            {
                                auto itr = parityCount.find(pj);
                                if (p1i < p2i)
                                {
                                    if (itr != parityCount.end() and itr->second.right2left)
                                        parityCount.erase(itr);
                                    else
                                        parityCount[pj].left2right = true;
                                }
                                else
                                {
                                    if (itr != parityCount.end() and itr->second.left2right)
                                        parityCount.erase(itr);
                                    else
                                        parityCount[pj].right2left = true;
                                }
                            }
                        }
                    }
                    
                    if (parityCount.size() % 2 == 1)
                    {
                        for (auto itr = parityCount.begin(); itr != parityCount.end(); ++itr)
                            if (itr->first < jmin)
                                jmin = itr->first;
                        parityCount[jmin].right2left ? pt.exit = false : pt.enter = false;}
                }
            }
        }
    }
}

bool Voxelizer::IsIntersectant(const boost::rational<int>& i, const boost::rational<int>& j, Coordinate v1, Coordinate v2, Coordinate v3)
{
    if (ray.z)
    {
        Coordinate p{i, j, 0};
        v1.z = v2.z = v3.z = 0;
        Vector p1(p, v1), p2(p, v2), p3(p, v3);
        if ((p1*p2).z<=0 and (p2*p3).z<=0 and (p3*p1).z<=0 or (p1*p2).z>=0 and (p2*p3).z>=0 and (p3*p1).z>=0)
            return true;
    }
    else if (ray.y)
    {
        Coordinate p{i, 0, j};
        v1.y = v2.y = v3.y = 0;
        Vector p1(p, v1), p2(p, v2), p3(p, v3);
        if ((p1*p2).y<=0 and (p2*p3).y<=0 and (p3*p1).y<=0 or (p1*p2).y>=0 and (p2*p3).y>=0 and (p3*p1).y>=0)
            return true;
    }
    else if (ray.x)
    {
        Coordinate p{0, i, j};
        v1.x = v2.x = v3.x = 0;
        Vector p1(p, v1), p2(p, v2), p3(p, v3);
        if ((p1*p2).x<=0 and (p2*p3).x<=0 and (p3*p1).x<=0 or (p1*p2).x>=0 and (p2*p3).x>=0 and (p3*p1).x>=0)
            return true;
    }
    return false;
}

void Voxelizer::InsertIntersection(const int i, const int j, Intersection& point, int faceNum)
{
    point.sharedFaces.push_back(faceNum);

    if (intersectionOnRay[i][j].empty())
        intersectionOnRay[i][j].push_back(point);
    else
    {
        for (auto pt = intersectionOnRay[i][j].begin(); pt != intersectionOnRay[i][j].end(); pt++)
        {
            if (point.value == pt->value)
            {   // update existing point
                pt->enter      = (pt->enter      || point.enter);
                pt->exit       = (pt->exit       || point.exit);
                pt->touchStart = (pt->touchStart || point.touchStart);
                pt->touchEnd   = (pt->touchEnd   || point.touchEnd);
                pt->sharedFaces.push_back(faceNum);
                break;
            }
            else if (point.value < pt->value)
            {
                intersectionOnRay[i][j].insert(pt, point);
                break;
            }
            if (intersectionOnRay[i][j].back().value < point.value)
            {   // be the largest point
                intersectionOnRay[i][j].push_back(point);
                break;
            }
        }
    }
}