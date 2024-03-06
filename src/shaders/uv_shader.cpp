#include "shader.h"

class UV_Shader : public Shader{
    public:
    UV_Shader(){ }
    ~UV_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        return Vec3i(frag_data.tex_coord.u*255, 
                    frag_data.tex_coord.v*255, 
                    0);
    }
    
};