#include "shader.h"

class Gouraud_Shader : public Shader{
    public:
    Gouraud_Shader(){ }
    ~Gouraud_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        
        return vertex_input;
    }

    Vec3i fragment_shader(const Fragment_Shader_Payload& frag_data){
        return Vec3i(frag_data.base_color.x*frag_data.light_intensity, 
                    frag_data.base_color.y*frag_data.light_intensity, 
                    frag_data.base_color.z*frag_data.light_intensity);
    }
    
};