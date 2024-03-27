#include "shader.h"

class Gouraud_Shader : public Shader{
    public:
    Shader_Global_Payload global_payload;
    Gouraud_Shader(){ }
    ~Gouraud_Shader(){ }

    Vertex vertex_shader(const Vertex& vertex_input){
        float light_intensity_at_vertex = global_payload.main_light_dir * vertex_input.norm;
        return vertex_input;
    }

    Vec3i fragment_shader(const V2F& v2f){
        return Vec3i(v2f.base_color.x*v2f.light_intensity, 
                    v2f.base_color.y*v2f.light_intensity, 
                    v2f.base_color.z*v2f.light_intensity);
    }
    
};