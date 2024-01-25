#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include "he_model.h"
#include <typeindex>

HEModel::HEModel(const char *filename) : h_edges(), faces(), vertices()
{ // constructor definition
    std::cout << "Loading Half-Edge model..." << std::endl;
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
    int p_count=0;
    int n_count=0;
    int tc_count=0;
    int diff_uv_count = 0;
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
            
            norm_list.push_back(norm.get_inverted());
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

                v->pos_index = ipos; //store the index for comparison
                v->uv_index = itc;

                v->pos = pos_list[ipos]; //store the data
                v->tex_coord = tex_coord_list[itc];
                v->norm = norm_list[inorm];

                std::pair pair = vertices.insert(v);

                // if has duplicate value in the set, then reference v to that duplicate value
                if (!pair.second)
                {
                    v = *pair.first; // refer the v pointer to the already stored vertex
                }
                tri.push_back(v);
            }

            triangles.push_back(tri);
        }
    }

    int num_of_pairs_found = 0;

    //lambda for hashing of edges using the vertices' position index
    auto edge_hash = [](const Edge* a){
        return std::hash<int>{}(a->v1->pos_index) ^ std::hash<int>{}(a->v2->pos_index);
    };

    //lambda for equality of edges using the vertices' positionindex
    auto edge_equal = [](const Edge* a, const Edge* b) {
    // Compare edges based on their vertices, regardless of the order
    return (a->v1->pos_index == b->v1->pos_index && a->v2->pos_index == b->v2->pos_index) || 
                (a->v1->pos_index == b->v2->pos_index && a->v2->pos_index == b->v1->pos_index);
    };

    //a temporary hash set for checking whether an edge(undirected) is already inserted
    std::unordered_set<Edge*, decltype(edge_hash), decltype(edge_equal)> edges_temp(10,edge_hash,edge_equal);

    for (int i = 0; i < triangles.size(); i++)
    {
        Face *f = new Face();              // initialize face
        std::vector<HEdge *> h_edges_temp; // initialize a temorary vector of half edges

        for (int j = 0; j < triangles[i].size(); j++)
        {
            HEdge *h_edge = new HEdge(); // initialize a half edge
            Vertex *v = triangles[i][j]; // retrieve the vertex
            v->h = h_edge;               // set the vertex's half edge to the current half edge
            h_edge->v = v;               // set the half edge's head vertex to the current vertex
            h_edge->f = f;               // set the half edge's face to the current face
            h_edges_temp.push_back(h_edge);
        }

        for (int j = 0; j < h_edges_temp.size(); j++) // iterate throught the temp vector of half edges
        {
            // get the previous and next half edges's index
            int prev_index = j - 1 < 0 ? h_edges_temp.size() - 1 : j - 1;
            int next_index = j + 1 >= h_edges_temp.size() ? 0 : j + 1;

            h_edges_temp[j]->next = h_edges_temp[next_index]; // set the next half edge
            h_edges_temp[j]->prev = h_edges_temp[prev_index]; // set the previous half edge

            Vertex *start_v = h_edges_temp[j]->prev->v;
            Vertex *end_v = h_edges_temp[j]->v;

            //creating an undirected edge to test for the containment of the other opposite half edge
            Edge *e = new Edge();
            e->v1 = start_v;
            e->v2 = end_v;
            e->h = h_edges_temp[j];

            //attempt to insert to undirected edge set
            std::pair insert_result = edges_temp.insert(e);
            if(!insert_result.second) //if has duplicate, it means an half edge already in the list
            {
                num_of_pairs_found++;

                //retrieve the half edge and pair the edges to each other
                h_edges_temp[j]->pair = ((*insert_result.first)->h);
                ((*insert_result.first)->h)->pair = h_edges_temp[j];
            }
           
            h_edges_temp[j]->index = h_edges.size();
            h_edges.insert(h_edges_temp[j]); // add the current half edge to the list of half edges
        }

        f->h = h_edges_temp[0]; // set the face's half edge to the first half edge
        f->index = faces.size();
        faces.insert(f); // add the face to the list of faces
    }

    std::cout << "Half-Edge model loading finished, analysis below:" << std::endl;
    std::cout << "\th_edges size: " << h_edges.size() << std::endl;
    std::cout << "\tpairs size: " << num_of_pairs_found << std::endl;
    std::cout << "\tfaces size: " << faces.size() << std::endl;
    std::cout << "\tunique vertices size: " << vertices.size() << std::endl;
    int num_of_edges_no_pair = 0;
    int num_of_vertices_no_norm = 0;
    int num_of_vertices_no_uv = 0;
    int num_of_vertices_no_pos = 0;
    for (std::set<HEdge *>::iterator it = h_edges.begin(); it != h_edges.end(); ++it)
    {
        if ((*it)->pair == NULL)
        {
            num_of_edges_no_pair++;
        }
    }
    for(std::set<Vertex*>::iterator it = vertices.begin();it!=vertices.end();++it){
        if((*it)->norm == Vec3f()){
            num_of_vertices_no_norm++;
        }
        if((*it)->tex_coord==Vec2f()){
            num_of_vertices_no_uv++;
        }
        if((*it)->pos==Vec3f()){
            num_of_vertices_no_pos++;
        }
    }
    std::cout << "\tnum of edges with no pair: " << num_of_edges_no_pair << std::endl;
}

void qem_simplify(){
    //implement mesh simplification using quadric error metrics
    

}
HEModel::~HEModel()
{
}
