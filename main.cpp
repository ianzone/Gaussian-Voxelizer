#include "voxelizer.hpp"

int main(int argc, char const *argv[])
{
    Command com(argc, argv);
    Mesh mesh(com.meshFile, com.insideout);
    Voxelizer voxelizer(mesh, com.space, com.ray, com.voxelAmount);
    voxelizer.WriteOut(com.voxelFile);
    
    // for (auto itr = voxelizer.voxel.begin(); itr != voxelizer.voxel.end(); ++itr)
    // {
    //     for (auto itr2 = itr->begin(); itr2 != itr->end(); ++itr2)
    //     {
    //         for (auto itr3 = itr2->begin(); itr3 != itr2->end(); ++itr3)
    //         {
    //             std::cout<<*itr3<<" ";
    //         }std::cout<<"\n";
    //     }std::cout<<"\n";
    // }
    
    return 0;
}
