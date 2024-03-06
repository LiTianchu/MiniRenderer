#include "shader.h"

class Diffuse_Map_Shader : public Shader{
    public:
    Diffuse_Map_Shader(){ }
    ~Diffuse_Map_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        TGAColor tex_color = frag_data.texture->get(frag_data.texture->get_width()*frag_data.tex_coord.u, 
                                                        frag_data.texture->get_height()*(1.0-frag_data.tex_coord.v));
                                             
        return Vec3i(tex_color.r, 
                    tex_color.g, 
                    tex_color.b);
    }
    
};