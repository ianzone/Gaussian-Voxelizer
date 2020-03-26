#include "voxelizer.hpp"

Voxelizer::Voxelizer(const Mesh& mesh, BoundingBox& space, const Counter& voxelAmount)
{
    Voxelize(mesh, space, voxelAmount);
}

void Voxelizer::Voxelize(const Mesh& mesh, BoundingBox& space, const Counter& voxelAmount)
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

    voxelAmt = voxelAmount;

    voxelSize.x = (space.max.x - space.min.x) / (voxelAmt.x - 1);
    voxelSize.y = (space.max.y - space.min.y) / (voxelAmt.y - 1);
    voxelSize.z = (space.max.z - space.min.z) / (voxelAmt.z - 1);

    int imax, jmax;
    mpq_class unit, valmin;
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
                    mpq_class r = (itr->value - valmin) / unit;
                    int z = r.get_d();
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
                    mpq_class r = (itr->value - valmin) / unit;
                    int z = r.get_d();
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
                voxelSet << voxel[x][y][z];

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

        // get triangle bounding box on x,y plane
        BoundingBox triBound;
        Counter voxelCount;
        Coordinate startPoint;

        triBound.min.x = triBound.max.x = v1.x;
        if (v2.x < triBound.min.x)
            triBound.min.x = v2.x;
        else if (v2.x > triBound.max.x)
            triBound.max.x = v2.x;
        if (v3.x < triBound.min.x)
            triBound.min.x = v3.x;
        else if (v3.x > triBound.max.x)
            triBound.max.x = v3.x;
        mpq_class tempx = (triBound.min.x - space.min.x) / voxelSize.x;
        voxelCount.x = tempx.get_d();
        startPoint.x = space.min.x + voxelSize.x * voxelCount.x;
        
        triBound.min.y = triBound.max.y = v1.y;
        if (v2.y < triBound.min.y)
            triBound.min.y = v2.y;
        else if (v2.y > triBound.max.y)
            triBound.max.y = v2.y;
        if (v3.y < triBound.min.y)
            triBound.min.y = v3.y;
        else if (v3.y > triBound.max.y)
            triBound.max.y = v3.y;
        mpq_class tempy = (triBound.min.y - space.min.y) / voxelSize.y;
        voxelCount.y = tempy.get_d();
        startPoint.y = space.min.y + voxelSize.y * voxelCount.y;
        // get triangle bounding box on x,y plane

        mpq_class orientation = ray.DotProduct(mesh.normal[faceNum]);
        mpq_class triFormulaD = mesh.normal[faceNum].x * v1.x + mesh.normal[faceNum].y * v1.y + mesh.normal[faceNum].z * v1.z;
        mpq_class iStart, jStart, imax, jmax, iunit, junit;
        int ipos, jpos;
        iStart = startPoint.x;
        imax = triBound.max.x;
        iunit = voxelSize.x;
        ipos = voxelCount.x;
        jStart = startPoint.y;
        jmax = triBound.max.y;
        junit = voxelSize.y;
        jpos = voxelCount.y;
        
        int tempjpos = jpos;
        for (auto i = iStart; i <= imax; i+=iunit, ++ipos)
        {   jpos = tempjpos;
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
                        std::vector<mpq_class> val;
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
    mpq_class smini, sminj, uniti, unitj;
    iAmt = voxelAmt.x;
    jAmt = voxelAmt.y;
    smini = space.min.x;
    sminj = space.min.y;
    uniti = voxelSize.x;
    unitj = voxelSize.y;
    
    struct EdgeDirection
    {
        bool right2left = false;
        bool left2right = false;
    };
    struct RationalHash
    {
        size_t operator()(const mpq_class key) const noexcept
        {
            return std::hash<std::string>{}(key.get_str());
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
                    std::unordered_map<mpq_class, EdgeDirection, RationalHash> parityCount;
                    mpq_class jmin = voxelj;
                    for (int faceNum : pt.sharedFaces)
                    {
                        int v1 = mesh.face[faceNum][0];
                        int v2 = mesh.face[faceNum][1];
                        int v3 = mesh.face[faceNum][2];
                        auto& a = mesh.vertex[v1];
                        auto& b = mesh.vertex[v2];
                        auto& c = mesh.vertex[v3];

                        mpq_class ai, aj, bi, bj, ci, cj;
                        ai = a.x; aj = a.y;
                        bi = b.x; bj = b.y;
                        ci = c.x; cj = c.y;

                        // get point's opposite edge of shared triangle on xy plane
                        mpq_class p1i, p1j, p2i, p2j, pj, slope, constant;
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
                        parityCount[jmin].right2left ? pt.exit = false : pt.enter = false;
                    }
                }
            }
        }
    }
}

bool Voxelizer::IsIntersectant(const mpq_class& i, const mpq_class& j, Coordinate v1, Coordinate v2, Coordinate v3)
{
    Coordinate p{i, j, 0};
    v1.z = v2.z = v3.z = 0;
    Vector p1(p, v1), p2(p, v2), p3(p, v3);
    if ((p1*p2).z<=0 and (p2*p3).z<=0 and (p3*p1).z<=0 or (p1*p2).z>=0 and (p2*p3).z>=0 and (p3*p1).z>=0)
        return true;
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