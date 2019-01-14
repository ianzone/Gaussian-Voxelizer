#include "voxelizer.hpp"

string input = "0", output = "0";
Coordinate space_lower_bound{0,0,0}, space_upper_bound{255,255,255}, number_of_voxels{255,255,255};

void process_command(int argc, char const *argv[])
{
    argc--; argv++;

    while (argc > 0)
    {
        if ((*argv)[0] == '-')
        {
            if (!strcmp(*argv, "-i"))
            {
                argv++; argc--;
                input = *argv;
            }
            else if (!strcmp(*argv, "-o"))
            {
                argv++; argc--;
                output = *argv;
            }
            else if (!strcmp(*argv, "-sl"))
            {
                for(int i = 0; i < 3; i++)
                {
                    argv++; argc--;
                    auto coordinate = to_rational(*argv);
                    if(i == 0)
                        space_lower_bound.x = coordinate;
                    else if(i == 1)
                        space_lower_bound.y = coordinate;
                    else
                        space_lower_bound.z = coordinate;
                }
            }
            else if (!strcmp(*argv, "-su"))
            {
            	for(int i = 0; i < 3; i++)
                {
                    argv++; argc--;
                    auto coordinate = to_rational(*argv);
                    if(i == 0)
                        space_upper_bound.x = coordinate;
                    else if(i == 1)
                        space_upper_bound.y = coordinate;
                    else
                        space_upper_bound.z = coordinate;
                }
            }
            else if (!strcmp(*argv, "-n"))
            {
                argv++; argc--;
                number_of_voxels.x = stoi(*argv);
                argv++; argc--;
                number_of_voxels.y = stoi(*argv);
                argv++; argc--;
                number_of_voxels.z = stoi(*argv);
                if (!(number_of_voxels.x > 0 && number_of_voxels.y > 0 && number_of_voxels.z > 0))
                {
                    cerr<<"invalid voxel number\n";
                    exit(1);
                }
            }
        }
        argv++; argc--;
    }
}

rational<int> to_rational(const char str[])
{
    int length, numerator, denominator;
    double number;
    length = strlen(str);
    number = stod(str);
    numerator = (int)(number * pow(10, length));
    denominator = pow(10, length);
    rational<int> fraction(numerator, denominator);
    return fraction;
}

rational<int> to_rational(string str)
{
    int length, numerator, denominator;
    double number;
    length = str.size();
    number = stod(str);
    numerator = (int)(number * pow(10, length));
    denominator = pow(10, length);
    rational<int> fraction(numerator, denominator);
    return fraction;
}

void Mesh::read(istream &data)
{
    Face a_face;
    Coordinate a_vertex;

    string str;
    getline(data,str);
    
    data >> str;
    while (str == "#")
    {
        getline(data, str);
        data >> str;
    }
    
    number_of_vertex = stoi(str);
    data >> str;
    number_of_faces = stoi(str);
    data >> str;
    number_of_edges = stoi(str);
    
    data >> str;
    a_vertex.x = to_rational(str);
    lower_bound.x = upper_bound.x = a_vertex.x;
    data >> str;
    a_vertex.y = to_rational(str);
    lower_bound.y = upper_bound.y = a_vertex.y;
    data >> str;
    a_vertex.z = to_rational(str);
    lower_bound.z = upper_bound.z = a_vertex.z;

    vertex.push_back(a_vertex);

    for(int i = 1; i < number_of_vertex; i++)
    {
        data >> str;
        a_vertex.x = to_rational(str);
        if(a_vertex.x < lower_bound.x)
            lower_bound.x = a_vertex.x;
        else if(a_vertex.x > upper_bound.x)
            upper_bound.x = a_vertex.x;

        data >> str;
        a_vertex.y = to_rational(str);
        if(a_vertex.y < lower_bound.y)
            lower_bound.y = a_vertex.y;
        else if(a_vertex.y > upper_bound.y)
            upper_bound.y = a_vertex.y;

        data >> str;
        a_vertex.z = to_rational(str);
        if(a_vertex.z < lower_bound.z)
            lower_bound.z = a_vertex.z;
        else if(a_vertex.z > upper_bound.z)
            upper_bound.z = a_vertex.z;

        vertex.push_back(a_vertex);
    }

    while (lower_bound.x < space_lower_bound.x || lower_bound.y < space_lower_bound.y || lower_bound.z < space_lower_bound.z)
    {
        cout << "set space lower bound smaller than (" << lower_bound.x << ", " << lower_bound.y << ", " << lower_bound.z << ")\n-sl ";
        cin >> str;
        space_lower_bound.x = to_rational(str);
        cin >> str;
        space_lower_bound.y = to_rational(str);
        cin >> str;
        space_lower_bound.z = to_rational(str);
        cout << "new space lower bound set: (" << space_lower_bound.x << ", " << space_lower_bound.y << ", " << space_lower_bound.z << ")" << endl;
    }
    
    while(upper_bound.x > space_upper_bound.x || upper_bound.y > space_upper_bound.y || upper_bound.z > space_upper_bound.z)
    {
        cout << "set space upper bound bigger than (" << upper_bound.x << ", " << upper_bound.y << ", " << upper_bound.z << ")\n-su ";
        cin >> str;
        space_upper_bound.x = to_rational(str);
        cin >> str;
        space_upper_bound.y = to_rational(str);
        cin >> str;
        space_upper_bound.z = to_rational(str);
        cout << "new space upper bound set: (" << space_upper_bound.x << ", " << space_upper_bound.y << ", " << space_upper_bound.z << ")" << endl;
    }
    
    for(int i = 0; i < number_of_faces; i++)
    {
        data >> str;
        data >> str;
        a_face.v1 = stoi(str);
        data >> str;
        a_face.v2 = stoi(str);
        data >> str;
        a_face.v3 = stoi(str);
        face.push_back(a_face);
    }
}