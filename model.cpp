#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), uv_(), norms_()
{
    std::cout << "Model constructor called" << std::endl;
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
                iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            Vec2f uv;
            iss >> uv.u;
            iss >> uv.v;
            uv.v = 1-uv.v; //uv starts from top left
            uv_.push_back(uv);
            //std::cout << uv << std::endl;
        }
        else if(!line.compare(0,3, "vn ")){
            iss >> trash >> trash;
            Vec3f norm;
            iss >> norm.x;
            iss >> norm.y;
            iss >> norm.z;
            norms_.push_back(norm);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<Vec3i> f;
            int itrash, idx, iuv, inorm;
            iss >> trash;
            while (iss >> idx >> trash >> iuv >> trash >> inorm)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                iuv--;
                inorm--;
                f.push_back(Vec3i(idx, iuv, inorm));
                //std::cout << idx << " " << iuv << " " << inorm << std::endl;
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<Vec3i> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}

Vec2f Model::uv(int i)
{
    return uv_[i];
}

Vec3f Model::norm(int i){
    return norms_[i].normalize();
}
