#include "shader.h"

class Normal_Map_Shader : public Shader{
    public:
    Normal_Map_Shader(){ }
    Normal_Map_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~Normal_Map_Shader(){ }

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        processed_v.pos = vertex_input.pos;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        TGAColor normal = global_payload.texture->get(global_payload.texture->get_width()*v2f.tex_coord.u, 
                                                        global_payload.texture->get_height()*(1.0-v2f.tex_coord.v));
        Vec3f normal_vec = Vec3f(normal.r, normal.g, normal.b);
        //float frag_light_intensity = normal_vec ;
        Vec3i final_color = v2f.base_color;                      
        return final_color;
    }
    
};