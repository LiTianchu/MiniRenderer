#include "shader.h"

class Flat_Shader : public Shader{
    public:
    Flat_Shader(){}
    Flat_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~Flat_Shader(){ }

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        processed_v.pos = vertex_input.pos;
        
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        return v2f.base_color;
    }
    
};