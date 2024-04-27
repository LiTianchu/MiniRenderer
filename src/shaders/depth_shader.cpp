#include "shader.h"

class Depth_Shader : public Shader{
    public:
    Depth_Shader(){ }
    Depth_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~Depth_Shader() = default;

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        Matrix v_m = Matrix::vector2matrix(Vec4f(vertex_input.pos.x, vertex_input.pos.y, vertex_input.pos.z, 1.0f));
        v_m = (global_payload.transform_matrix * v_m).cartesian();
        processed_v.pos = Vec3f(v_m[0][0], v_m[1][0], v_m[2][0]);
        //processed_v.norm = vertex_input.norm;
        //processed_v.norm = Math::to_Vec3<float>(global_payload.proj_correction_mat * Math::to_Matrix4(vertex_input.norm, 0.0f));
        //processed_v.tex_coord = vertex_input.tex_coord;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        float z = v2f.pos.z;
        // map z to 0-255
        z = std::clamp(z/global_payload.zDepth * 255.0f, 0.0f, 255.0f);

        //float light_intensity = std::clamp(global_payload.main_light_dir * v2f.norm * global_payload.main_light_intensity, 0.0f, 1.0f);
        return Vec3i(z, z, z);
    }
    
};