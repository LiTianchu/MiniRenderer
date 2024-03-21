#include "shader.h"

class Normal_Map_Shader : public Shader{
    public:
    Normal_Map_Shader(){ }
    ~Normal_Map_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        TGAColor normal = frag_data.texture->get(frag_data.texture->get_width()*frag_data.tex_coord.u, 
                                                        frag_data.texture->get_height()*(1.0-frag_data.tex_coord.v));
        Vec3f normal_vec = Vec3f(normal.r, normal.g, normal.b);
        float frag_light_intensity = normal_vec * frag_data.light_dir;
        Vec3i final_color = frag_data.base_color * frag_light_intensity;                         
        return final_color;
    }
    
};