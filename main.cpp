#include "voxelizer.hpp"

int main(int argc, char const *argv[])
{
    process_command(argc, argv);

    try
    {
        if (!(space_lower_bound.x < space_upper_bound.x && space_lower_bound.y < space_upper_bound.y && space_lower_bound.z < space_upper_bound.z))
            throw "set smaller sl than su!\n";
    }
    catch (const char *msg)
    {
        cerr << msg;
    }

    ifstream DATA(input);
    if (!DATA)
    {
        cerr << "can not open input file!\n";
        exit(1);
    }

    Mesh mesh;
    mesh.read(DATA);

    return 0;
}
