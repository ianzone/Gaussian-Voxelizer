# Gaussian-Voxelizer
You need libboost-dev, the source code is in voxelizer.cpp, other cpp files are used to modify the mesh. 
The original voxel data is cubespher-128.raw, and the mesh we voxelize is righthand.off
I don’t have time to update the block “check mode”, it is used to compare the result and the original voxel data.
To do voxelization, type: ./a.out -i righthand.off -o righthand.raw -smax 128 128 128 -n 128 128 128
