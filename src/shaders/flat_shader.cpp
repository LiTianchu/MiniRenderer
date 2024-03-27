#include "shader.h"

class Flat_Shader : public Shader{
    public:
    Flat_Shader(){}
    ~Flat_Shader(){ }

    virtual Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        return v2f.base_color * (v2f.face_norm * v2f.light_dir);
    }
    
};