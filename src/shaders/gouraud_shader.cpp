#include "shader.h"

class Gouraud_Shader : public Shader{
    public:
    Gouraud_Shader(){ }
    Gouraud_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~Gouraud_Shader(){ }

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        processed_v.pos = vertex_input.pos;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        float light_intensity = global_payload.main_light_dir * v2f.norm * global_payload.main_light_intensity;
        return Vec3i(v2f.base_color.x*light_intensity, 
                    v2f.base_color.y*light_intensity, 
                    v2f.base_color.z*light_intensity);
    }
    
};