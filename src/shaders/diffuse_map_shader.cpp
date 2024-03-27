#include "shader.h"

class Diffuse_Map_Shader : public Shader{
    public:
        Diffuse_Map_Shader(){ }
        Diffuse_Map_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
        ~Diffuse_Map_Shader(){ }

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        processed_v.pos = vertex_input.pos;
        
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        TGAColor tex_color = global_payload.texture->get(global_payload.texture->get_width()*v2f.tex_coord.u, 
                                                        global_payload.texture->get_height()*(1.0-v2f.tex_coord.v));
                                             
        return Vec3i(tex_color.r, 
                    tex_color.g, 
                    tex_color.b);
    }
    
};