#include "voxelizer.hpp"

int main(int argc, char const *argv[])
{
    std::string meshFile, voxelFile;
    bool insideout = false;
    BoundingBox space;
    Counter voxelAmount{255,255,255};

    std::string help(
        "-i   input mash file\n"
        "-o   output voxel file\n"
        "-rn  reverse polygon normal vextor\n"
        "-sl  space lower bound\n"
        "-su  space upper bound\n"
        "-n   number of voxels along x y z\n");

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
        auto pos = voxelFile.find_last_of('/');
        if (pos != std::string::npos)
            voxelFile = voxelFile.substr(pos+1);
        voxelFile+=".raw";
    }
    
    Mesh mesh(meshFile, insideout);
    Voxelizer voxelizer(mesh, space, voxelAmount);
    voxelizer.WriteOut(voxelFile);

    return 0;
}
