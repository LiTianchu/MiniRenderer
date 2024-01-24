#include "shader.h"

class Diffuse_Map_Shader : public Shader{
    public:
    Diffuse_Map_Shader(){ }
    ~Diffuse_Map_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        return Vec3i(0,0, 0);
    }
    
};