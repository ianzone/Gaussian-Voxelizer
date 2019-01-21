#include "voxelizer.hpp"

int main(int argc, char const *argv[])
{
    process_command(argc, argv);

    Mesh mesh;
    mesh.read(input);

    return 0;
}
