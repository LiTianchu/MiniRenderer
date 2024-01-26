#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
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
    int p_count = 0;
    int n_count = 0;
    int tc_count = 0;
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

                v->pos_index = ipos; // store the index for comparison
                v->uv_index = itc;

                v->pos = pos_list[ipos]; // store the data
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

    // lambda for hashing of edges using the vertices' position index
    auto edge_hash = [](const Edge *a)
    {
        return std::hash<int>{}(a->v1->pos_index) ^ std::hash<int>{}(a->v2->pos_index);
    };

    // lambda for equality of edges using the vertices' positionindex
    auto edge_equal = [](const Edge *a, const Edge *b)
    {
        // Compare edges based on their vertices, regardless of the order
        return (a->v1->pos_index == b->v1->pos_index && a->v2->pos_index == b->v2->pos_index) ||
               (a->v1->pos_index == b->v2->pos_index && a->v2->pos_index == b->v1->pos_index);
    };

    // a temporary hash set for checking whether an edge(undirected) is already inserted
    std::unordered_set<Edge *, decltype(edge_hash), decltype(edge_equal)> edges_temp(10, edge_hash, edge_equal);

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

            // creating an undirected edge to test for the containment of the other opposite half edge
            Edge *e = new Edge();
            e->v1 = start_v;
            e->v2 = end_v;
            e->h = h_edges_temp[j];

            // attempt to insert to undirected edge set
            std::pair insert_result = edges_temp.insert(e);
            if (!insert_result.second) // if has duplicate, it means an half edge already in the list
            {
                num_of_pairs_found++;

                // retrieve the half edge and pair the edges to each other
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
    for (std::set<Vertex *>::iterator it = vertices.begin(); it != vertices.end(); ++it)
    {
        if ((*it)->norm == Vec3f())
        {
            num_of_vertices_no_norm++;
        }
        if ((*it)->tex_coord == Vec2f())
        {
            num_of_vertices_no_uv++;
        }
        if ((*it)->pos == Vec3f())
        {
            num_of_vertices_no_pos++;
        }
    }
    std::cout << "\tnum of edges with no pair: " << num_of_edges_no_pair << std::endl;
}

Vec3f get_face_norm(const Face &f)
{
    HEdge *e1 = f.h;
    HEdge *e2 = e1->next;
    Vec3f face_normal = (e1->v->pos - e1->prev->v->pos).cross(e2->v->pos - e2->prev->v->pos);
    return face_normal.normalize();
}

// returns a vector of incident edges and a bool indicating whether the operation is successful
std::pair<std::vector<HEdge *>, bool> get_incident_edges(const Vertex &v)
{
    std::vector<HEdge *> incident_edges;
    HEdge *current_edge = v.h;
    do
    {
        incident_edges.push_back(current_edge);
        if (current_edge->pair == NULL)
        {
            std::cout << "half edge has no pair" << std::endl;
            return std::pair(incident_edges, false); // return false if the half edge has no pair (vertex on boundary)
        }
        current_edge = current_edge->pair->next->next;
    } while (current_edge != v.h);
    return std::pair(incident_edges, true);
}

Matrix get_quadric_err(const Vertex &vertex)
{
    Matrix Q = Matrix(4, 4);
    // To compute error quadric Q of the currect vertex, add all the Kp of the adjacent faces
    std::pair<std::vector<HEdge *>, bool> incident_edges = get_incident_edges(vertex);
    if (!incident_edges.second)
    {
        // set a very big error for boundary vertices
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                Q[i][j] = std::numeric_limits<float>::max();
            }
        }
        return Q;
    }

    for (HEdge *e : incident_edges.first)
    { // iterate through all the incident edges
        // The fundamental error matrix Kp is defined as:
        // Kp = a^2 ab ac ad
        //      ab b^2 bc bd
        //      ac bc c^2 cd
        //      ad bd cd d^2
        // a,b,c,d are coefficients of plane equation: ax+by+cz+d=0
        Face *f = e->f; // find the face of the incident edge
        Vec3f norm = get_face_norm(*f);
        float a = norm.x;
        float b = norm.y;
        float c = norm.z;
        float d = -(norm * e->v->pos); // n * p1

        Matrix Kp = Matrix(4, 4);
        Kp[0][0] = a * a;
        Kp[0][1] = a * b;
        Kp[0][2] = a * c;
        Kp[0][3] = a * d;
        Kp[1][0] = a * b;
        Kp[1][1] = b * b;
        Kp[1][2] = b * c;
        Kp[1][3] = b * d;
        Kp[2][0] = a * c;
        Kp[2][1] = b * c;
        Kp[2][2] = c * c;
        Kp[2][3] = c * d;
        Kp[3][0] = a * d;
        Kp[3][1] = b * d;
        Kp[3][2] = c * d;
        Kp[3][3] = d * d;
        Q = Q + Kp; // add the Kp to the quadric error matrix
    }
    return Q;
}

float compute_error(const Vec3f &v, const Matrix &Q)
{
    // compute the error of the vertex v using the quadric error matrix Q
    // error = v^T * Q * v
    Matrix v_matrix = Matrix(4, 1);
    Matrix vt_matrix = Matrix(1, 4);

    // homogeneous coordinate
    for (int i = 0; i < 3; i++)
    {
        v_matrix[i][0] = v.raw[i];
        vt_matrix[0][i] = v.raw[i];
    }
    v_matrix[3][0] = 1.f;
    vt_matrix[0][3] = 1.f;

    Matrix error_matrix = vt_matrix * Q * v_matrix;
    return error_matrix[0][0];
}

Matrix compute_v_bar(Matrix &q1, Matrix &q2)
{
    Matrix q_bar = q1 + q2;

    Matrix a = Matrix(4, 4);
    a[0][0] = q_bar[0][0];
    a[0][1] = q_bar[0][1];
    a[0][2] = q_bar[0][2];
    a[0][3] = q_bar[0][3];
    a[1][0] = q_bar[0][1];
    a[1][1] = q_bar[1][1];
    a[1][2] = q_bar[1][2];
    a[1][3] = q_bar[1][3];
    a[2][0] = q_bar[0][2];
    a[2][1] = q_bar[1][2];
    a[2][2] = q_bar[2][2];
    a[2][3] = q_bar[2][3];
    a[3][0] = 0;
    a[3][1] = 0;
    a[3][2] = 0;
    a[3][3] = 1;

    std::pair<Matrix, bool> a_inv = a.inverse();
    if (!a_inv.second)
    {
        std::cout << "Matrix is not invertible" << std::endl;
    }

    Matrix b = Matrix(4, 1);
    b[0][0] = 0;
    b[1][0] = 0;
    b[2][0] = 0;
    b[3][0] = 1;

    Matrix v_bar_matrix = a_inv.first * b;

    return v_bar_matrix;
}

void HEModel::qem_simplify(int target_num_of_faces)
{
    // 1. compute all the Q matrices for all the vertices
    // 2. identify all the valid pairs（v1, v2） of vertices
    // 3. compute the optimal contraction position V' for each pair
    // 4. place all the pairs in a priority queue, ordered by the error metric
    // 5. pop the pair with the smallest error metric from the PQ
    // 6. contract the pair(move v1 and v2 to v', connext all incident edges of v2 to v1, delete v2 and any degenerate faces)
    // 7. update the Q matrices of all the vertices affected by the contraction
    // 8. repeat 5-7 until the target number of faces is reached

    // hash and equality lambda expressions for the vertices
    auto vertex_hash = [](const Vertex *v)
    {
        return std::hash<int>{}(v->pos_index) ^ std::hash<int>{}(v->uv_index);
    };

    auto vertex_equal = [](const Vertex *a, const Vertex *b)
    {
        return (a->pos_index == b->pos_index) && (a->uv_index == b->uv_index);
    };

    // hashmap for storing the Q matrices of all the vertices
    std::unordered_map<Vertex *, Matrix, decltype(vertex_hash), decltype(vertex_equal)> Q_matrices(10, vertex_hash, vertex_equal);

    std::set<Vertex *>::iterator v_itr = vertices_begin();

    // assign Q matrix to every vertex
    for (v_itr; v_itr != vertices_end(); ++v_itr)
    {
        Vertex *curr_v = *v_itr;
        Matrix Q = get_quadric_err(*curr_v);
        float error = compute_error(curr_v->pos, Q);

        // if (std::isnan(error))
        // {
        //     std::cout << "error is nan" << std::endl;
        // }
        // else
        // {
        //     std::cout << error << std::endl;
        // }
        Q_matrices[curr_v] = Q;
    }

    std::set<HEdge *>::iterator e_itr = h_edges_begin();

    for (e_itr; e_itr != h_edges_end(); ++e_itr)
    {
        // obtain the pair of vertices
        HEdge *curr_e = *e_itr;
        Vertex *v1 = curr_e->v;
        Vertex *v2 = curr_e->prev->v;

        // compute the optimal contraction position V bar for each pair
        Matrix Q1 = Q_matrices[v1];
        Matrix Q2 = Q_matrices[v2];
        Matrix v_bar = compute_v_bar(Q1, Q2);
    }
}
HEModel::~HEModel()
{
}
