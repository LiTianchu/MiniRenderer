#ifndef __SHADER_H__
#define __SHADER_H__

#include "geometry.h"
#include "tgaimage.h"
#include "he_model.h"
#include "model.h"
#include <vector>

// class Fragment_Shader_Payload{
//     private:
//     Vec3f light_dir;
//     std::vector<Vec3f> pos;
//     std::vector<Vec3f> norm;
//     std::vector<Vec2f> uv;
//     TGAImage* texture;
//     public:
//     Fragment_Shader_Payload(){ }
//     Fragment_Shader_Payload(Vec3f light_dir, std::vector<Vec3f> pos, std::vector<Vec3f> norm, std::vector<Vec2f> uv, TGAImage* texture){
//         this->light_dir = light_dir;
//         this->pos = pos;
//         this->norm = norm;
//         this->uv = uv;
//         this->texture = texture;
//     }
//     Vec3f get_light_dir(){ return light_dir; }
//     std::vector<Vec3f> get_pos(){ return pos; }
//     std::vector<Vec3f> get_norm(){ return norm; }
//     std::vector<Vec2f> get_uv(){ return uv; }
//     TGAImage* get_texture(){ return texture; }

// };

// class Vertex_Shader_Payload{
//     private:
//     Vertex vertex_input;
//     public:
// };

class Shader{
    public:
    Shader(){ }
    virtual ~Shader(){ }

    virtual Vertex vertex_shader(Vertex vertex_input) = 0;
    virtual Vec3f fragment_shader(Vertex processed_vertex) = 0;
};
#endif