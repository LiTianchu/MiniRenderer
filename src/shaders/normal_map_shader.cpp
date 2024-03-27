#include "shader.h"

class Normal_Map_Shader : public Shader{
    public:
    Normal_Map_Shader(){ }
    ~Normal_Map_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const V2F& v2f){
        TGAColor normal = v2f.texture->get(v2f.texture->get_width()*v2f.tex_coord.u, 
                                                        v2f.texture->get_height()*(1.0-v2f.tex_coord.v));
        Vec3f normal_vec = Vec3f(normal.r, normal.g, normal.b);
        float frag_light_intensity = normal_vec * v2f.light_dir;
        Vec3i final_color = v2f.base_color * frag_light_intensity;                         
        return final_color;
    }
    
};