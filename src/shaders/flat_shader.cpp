#include "shader.h"

class Flat_Shader : public Shader{
    public:
    Flat_Shader(){ }
    ~Flat_Shader(){ }

    virtual Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    virtual Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        return frag_data.base_color;
    }
    
};