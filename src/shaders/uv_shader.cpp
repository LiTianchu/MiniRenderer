#include "shader.h"

class UV_Shader : public Shader{
    public:
    UV_Shader(){ }
    ~UV_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const V2F& v2f){
        return Vec3i(v2f.tex_coord.u*255, 
                    v2f.tex_coord.v*255, 
                    0);
    }
    
};