#include "voxelizer.hpp"

int main(int argc, char const *argv[])
{
    Command com(argc, argv);

    std::cout<<com.outputFile<<std::endl;
    Mesh mesh(com.inputFile);

    for (auto i : mesh.vertex)
    {
        std::cout<<i.x<<' '<<i.y<<' '<<i.z<<std::endl;
    }
    
    for (auto i : mesh.face)
    {
        std::cout<<i[0]<<' '<<i[1]<<' '<<i[2]<<std::endl;
    }
    
    for (auto i : mesh.faceNormal)
    {
        std::cout<<i.x<<' '<<i.y<<' '<<i.z<<std::endl;
    }

    return 0;
}
