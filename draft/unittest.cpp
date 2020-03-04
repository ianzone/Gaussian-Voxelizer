#include <sstream>
#include "voxelizer.hpp"

int main()
{
    // process_command() test start
    try
    {
        char const *argv[] = {"a.out", "-i", "test.off", "-o", "test.raw", "-cn", "-sl", "-1", "-1", "-1", "-su", "129", "129", "129", "-n", "128", "128", "128"};
        int argc = sizeof(argv) / sizeof(argv[0]);

        process_command(argc, argv);
        
        if (input != "test.off")
            throw "input";
        if (output != "test.raw")
            throw "output";
        if (convert_normal != true)
            throw "convert_normal";
        if (space_lower_bound.x != -1)
            throw "space_lower_bound.x";
        if (space_lower_bound.y != -1)
            throw "space_lower_bound.y";
        if (space_lower_bound.z != -1)
            throw "space_lower_bound.z";
        if (space_upper_bound.x != 129)
            throw "space_upper_bound.x";
        if (space_upper_bound.y != 129)
            throw "space_upper_bound.y";
        if (space_upper_bound.z != 129)
            throw "space_upper_bound.z";
        if (number_of_voxels.x != 128)
            throw "number_of_voxels.x";
        if (number_of_voxels.y != 128)
            throw "number_of_voxels.y";
        if (number_of_voxels.z != 128)
            throw "number_of_voxels.z";
    }
    catch(const char *msg)
    {
        cerr << "\nError Unit:  process_command() " << msg << " error\n";
    }
    // process_command() test end

    // Mesh::read() test start
    try
    {
        string str = "unittest.off";

        Mesh mesh;
        mesh.read(str);

        if (mesh.number_of_vertices != 5)
            throw "number_of_vertices";
        if (mesh.number_of_faces != 5)
            throw "number_of_faces";
        if (mesh.number_of_edges != 8)
            throw "number_of_edges";
        if (!(mesh.lower_bound.x == -1 && mesh.lower_bound.y == -1 && mesh.lower_bound.z == 0))
            throw "lower_bound";
        if (!(mesh.upper_bound.x == 1 && mesh.upper_bound.y == 1 && mesh.upper_bound.z == 1))
            throw "upper_bound";
        if (!(mesh.vertex[3].x == -1 && mesh.vertex[3].y == 0 && mesh.vertex[3].z == 0))
            throw "vertex";
        if (!(mesh.face[4][0] == 4 && mesh.face[4][1] == 3 && mesh.face[4][2] == 2 && mesh.face[4][3] == 1))
            throw "face";
        if (!(mesh.face_normal[4].x == 0 && mesh.face_normal[4].y == 0 && mesh.face_normal[4].z == -2))
        {
            cerr << "Error Value: " << mesh.face_normal[4].x << " " << mesh.face_normal[4].y << " " << mesh.face_normal[4].z;
            throw "face_normal";
        }
        if (mesh.not_triangle_face[0] != 4)
            throw "not_triangle_face";
    }
    catch(const char *msg)
    {
        cerr << "\nError Unit:  Mesh::read() " << msg << "\n";
    }
    // Mesh::read() test end

    return 0;
}
