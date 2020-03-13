#include "mesh.hpp"

Mesh::Mesh(const std::string& meshFile,  const bool insideout) : fileName(meshFile), reverseNormal(insideout)
{
    #ifdef DBG
        dbgmsg(__func__, __LINE__);
    #endif
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
    std::cout<<"lower bound("<<bound.min.x<<", "<<bound.min.y<<", "<<bound.min.z<<")  upper bound("<<bound.max.x<<", "<<bound.max.y<<", "<<bound.max.z<<")\n";
}

void Mesh::WriteOut(std::string meshFile)
{
    #ifdef DBG
        dbgmsg(__func__, __LINE__);
    #endif
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
            triMesh << itr->x.get_d() <<' '<< itr->y.get_d() <<' '<< itr->z.get_d() <<'\n';
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