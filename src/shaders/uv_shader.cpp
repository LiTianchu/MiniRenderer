#include "shader.h"

class UV_Shader : public Shader{
    public:
    UV_Shader(){ }
    UV_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~UV_Shader() = default;

    virtual V2F vertex_shader(const Vertex& vertex_input){
       V2F processed_v;
        Matrix v_m = Matrix::vector2matrix(Vec4f(vertex_input.pos.x, vertex_input.pos.y, vertex_input.pos.z, 1.0f));
        v_m = (global_payload.transform_matrix * v_m).cartesian();
        processed_v.pos = Vec3f(v_m[0][0], v_m[1][0], v_m[2][0]);
        processed_v.norm = vertex_input.norm;
        processed_v.tex_coord = vertex_input.tex_coord;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        return Vec3i(v2f.tex_coord.u*255, 
                    v2f.tex_coord.v*255, 
                    0);
    }
    
};