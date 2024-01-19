#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "he_model.h"

HEModel::HEModel(const char *filename) : edges(), faces(), vertices()
{ // constructor definition
    std::ifstream in;
    in.open(filename, std::ifstream::in);

    if (in.fail())
    {
        std::cout << "Cannot open model file " << filename << std::endl;
        return;
    }
    std::string line;

    std::vector<Vec3f> pos_list;
    std::vector<Vec3f> norm_list;
    std::vector<Vec2f> tex_coord_list;
    int p_count;
    int n_count;
    int tc_count;

    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        if (!line.compare(0, 2, "v "))
        { // vertex position data
            iss >> trash;
            Vec3f pos;
            iss >> pos.x;
            iss >> pos.y;
            iss >> pos.z;
            pos_list.push_back(pos);
        }
        else if (!line.compare(0, 3, "vt "))
        { // vertex texture coordinate
            iss >> trash >> trash;
            Vec2f tex_coord;
            iss >> tex_coord.u;
            iss >> tex_coord.v;
            tex_coord_list.push_back(tex_coord);
        }
        else if (!line.compare(0, 3, "vn "))
        { // vertex normal data
            iss >> trash >> trash;
            Vec3f norm;
            iss >> norm.x;
            iss >> norm.y;
            iss >> norm.z;
            norm_list.push_back(norm);
        }
        else if (!line.compare(0, 2, "f "))
        {
            int ipos, itc, inorm;
            std::vector<Vertex> tri;
            iss >> trash;
            while (iss >> ipos >> trash >> itc >> trash >> inorm)
            {
                --ipos; //.obj file is 1-indexed :(
                --itc;
                --inorm;

                Vertex v; //create the vertex object
                v.pos = norm_list[ipos];
                v.tex_coord = tex_coord_list[itc];
                v.norm = norm_list[inorm];
                tri.push_back(v);
            }

            //saving half edge
            HEdge he1;
            HEdge he2;
            HEdge he3;
            Face f;

            he1.next = &he2;
            he2.next = &he3;
            he3.next = &he1;
            he1.v = &tri[1];
            he2.v = &tri[2];
            he3.v = &tri[0];
            he1.f = &f;
            he2.f = &f;
            he3.f = &f;

            f.h = &he1;

            tri[0].h = &he3;
            tri[1].h = &he1;
            tri[2].h = &he2;

            edges.insert(he1);
            edges.insert(he2);
            edges.insert(he3);
            faces.insert(f);
            vertices.insert(tri[0]);
            vertices.insert(tri[1]);
            vertices.insert(tri[2]);

            
        }
    }

    std::cout << "h_edges: " << edges.size() << "faces: " << faces.size() << "vertices: " <<vertices.size() << std::endl; 

    //find the pairs of each half edge
}

HEModel::~HEModel()
{
}
