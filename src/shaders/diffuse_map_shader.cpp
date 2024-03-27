#include "shader.h"

class Diffuse_Map_Shader : public Shader{
    public:
        Diffuse_Map_Shader(){ }
        ~Diffuse_Map_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        return vertex_input;
    }

    Vec3i fragment_shader(const V2F& v2f){
        TGAColor tex_color = v2f.texture->get(v2f.texture->get_width()*v2f.tex_coord.u, 
                                                        v2f.texture->get_height()*(1.0-v2f.tex_coord.v));
                                             
        return Vec3i(tex_color.r, 
                    tex_color.g, 
                    tex_color.b);
    }
    
};