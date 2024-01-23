#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "he_model.h"

HEModel::HEModel(const char *filename) : h_edges(), faces(), vertices()
{ // constructor definition
    std::cout << "HEModel constructor called" << std::endl;
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
    std::vector<std::vector<Vertex *>> triangles;
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
            std::vector<Vertex *> tri;
            iss >> trash;
            while (iss >> ipos >> trash >> itc >> trash >> inorm)
            {
                --ipos; //.obj file is 1-indexed :(
                --itc;
                --inorm;

                Vertex *v = new Vertex(); // create the vertex object
                v->index = ipos;

                std::pair pair = vertices.insert(v);

                // if has duplicate value in the set, then reference v to that duplicate value
                if (!pair.second) // it never enters this if statement
                {
                    v = *pair.first;
                    std::cout << "duplicate: " << (*pair.first) << "v: " << v << std::endl;
                }
                else
                {
                    v->pos = pos_list[ipos];
                    v->tex_coord = tex_coord_list[itc];
                    v->norm = norm_list[inorm];
                }

                tri.push_back(v);
            }

            triangles.push_back(tri);
        }
    }
    for (int i = 0; i < triangles.size(); i++)
    {
        Face* f = new Face();
        std::vector<HEdge *> h_edges_temp;

        for (int j = 0; j < triangles[i].size(); j++)
        {
            HEdge *h_edge = new HEdge();
            Vertex *v = triangles[i][j];
            v->h = h_edge;
            h_edge->v = v;
            h_edge->f = f;
            h_edges_temp.push_back(h_edge);
        }

        for (int j = 0; j < h_edges_temp.size(); j++)
        {
            int prev_index = j - 1 < 0 ? h_edges_temp.size() - 1 : j - 1;
            int next_index = j + 1 >= h_edges_temp.size() ? 0 : j + 1;

            h_edges_temp[j]->next = h_edges_temp[next_index];
            h_edges_temp[j]->prev = h_edges_temp[prev_index];

            for (int k = 0; k < h_edges.size(); k++)
            {
                if (h_edges[k]->v == h_edges_temp[j]->prev->v && h_edges[k]->prev->v == h_edges_temp[j]->v)
                {
                    h_edges_temp[j]->pair = h_edges[k];
                    h_edges[k]->pair = h_edges_temp[j];
                    break;
                }
            }

            h_edges.push_back(h_edges_temp[j]);
        }

        f->h = h_edges_temp[0];
        faces.push_back(f);
    }

    // Don't forget to clean up dynamically allocated memory (delete HEdges)

    // return 0;
}
//     //save the triangles as half edge data structure
//     for (int i = 0; i < triangles.size(); i++) //iterate through the triangles
//     {
//         Face f = Face();
//         std::vector<HEdge> h_edges_temp = std::vector<HEdge>();
//         for (int j = 0; j < triangles[i].size(); j++) //iterate through the vertices of the triangle
//         {
//             HEdge h_edge = HEdge();
//             //get the vertex at head
//             Vertex* v = triangles[i][j];
//             v->h = &h_edge;
//             h_edge.v = v;
//             h_edge.f = &f;
//             h_edges_temp.push_back(h_edge);
//         }
//         //std::cout << h_edges_temp.size() << std::endl;

//         for(int j = 0; j < h_edges_temp.size(); j++){ //iterate through the half edges of the triangle
//             int prev_index = j-1 < 0 ? h_edges_temp.size() - 1 :j - 1; //if j is 0, then prev_index is the last index
//             int next_index = j+1 >= h_edges_temp.size() ? 0 : j + 1; //if j is the last index, then next_index is 0

//             h_edges_temp[j].next = &h_edges_temp[next_index];
//             h_edges_temp[j].prev = &h_edges_temp[prev_index];

//             //try to find a pair of this edge in the vector
//             //if found, then assign the pair to each other
//             //if not found, then add this edge to the list
//             for(int k = 0; k < h_edges.size(); k++){
//                 if(h_edges[k]->v == h_edges_temp[j].prev->v && h_edges[k]->prev->v == h_edges_temp[j].v){
//                     h_edges_temp[j].pair = h_edges[k];
//                     h_edges[k]->pair = &h_edges_temp[j];
//                     break;
//                 }
//             }
//             h_edges.push_back(&h_edges_temp[j]);
//         }

//         //std::cout << &h_edges_temp[0] << std::endl;
//         //std::cout << &f << std::endl;
//         f.h = &h_edges_temp[0];
//         faces.push_back(&f);
//     }

//     std::cout << "h_edges: " << h_edges.size() << " faces: " << faces.size() << " vertices: " <<vertices.size() << std::endl;
// }

HEModel::~HEModel()
{
}
