#include <sstream>
#include "voxelizer.hpp"

int main()
{
    try
    {
        char const *argv[] = {"a.out", "-i", "test.off", "-o", "test.raw", "-sl", "-1", "-1", "-1", "-su", "129", "129", "129", "-n", "128", "128", "128"};
        int argc = sizeof(argv) / sizeof(argv[0]);

        process_command(argc, argv);
        
        if (input != "test.off")
            throw "input error";
        if (output != "test.raw")
            throw "output error";
        if (space_lower_bound.x != -1)
            throw "space_lower_bound.x error";
        if (space_lower_bound.y != -1)
            throw "space_lower_bound.y error";
        if (space_lower_bound.z != -1)
            throw "space_lower_bound.z error";
        if (space_upper_bound.x != 129)
            throw "space_upper_bound.x error";
        if (space_upper_bound.y != 129)
            throw "space_upper_bound.y error";
        if (space_upper_bound.z != 129)
            throw "space_upper_bound.z error";
        if (number_of_voxels.x != 128)
            throw "number_of_voxels.x error";
        if (number_of_voxels.y != 128)
            throw "number_of_voxels.y error";
        if (number_of_voxels.z != 128)
            throw "number_of_voxels.z error";
    }
    catch(const char *msg)
    {
        cerr << msg << '\n';
    }
    
    try
    {
        string str = "OFF\n4 4 6\n0 0 0\n1 0 0\n0 1 0\n0 0 1\n3 0 2 1\n3 0 1 3\n3 0 3 2\n3 1 2 3";
        istringstream stream(str);

        Mesh mesh;
        mesh.read(stream);

        if (mesh.number_of_vertex != 4)
            throw "number_of_vertex error";
        if (mesh.number_of_faces != 4)
            throw "number_of_faces error";
        if (mesh.number_of_edges != 6)
            throw "number_of_edges error";
        if (mesh.lower_bound.x != 0 || mesh.lower_bound.y != 0 || mesh.lower_bound.z != 0)
            throw "lower_bound error";
        if (mesh.upper_bound.x != 1 || mesh.upper_bound.y != 1 || mesh.upper_bound.z != 1)
            throw "upper_bound error";
        if (mesh.vertex[3].x != 0 || mesh.vertex[3].y != 0 || mesh.vertex[3].z != 1)
            throw "vertex error";
        if (mesh.face[3].v1 != 1 || mesh.face[3].v2 != 2 || mesh.face[3].v3 != 3)
            throw "face error";
    }
    catch(const char *msg)
    {
        cerr << msg << '\n';
    }
    
    return 0;
}
