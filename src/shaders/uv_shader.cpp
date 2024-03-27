#include "shader.h"

class UV_Shader : public Shader{
    public:
    UV_Shader(){ }
    UV_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~UV_Shader(){ }

    virtual V2F vertex_shader(const Vertex& vertex_input){
        
        V2F processed_v;
        processed_v.pos = vertex_input.pos;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        return Vec3i(v2f.tex_coord.u*255, 
                    v2f.tex_coord.v*255, 
                    0);
    }
    
};