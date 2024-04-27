#include "shader.h"
#include <iostream>

class Normal_Map_Shader : public Shader{
    public:
    Normal_Map_Shader(){ }
    Normal_Map_Shader(Shader_Global_Payload& payload) : Shader(payload){ }
    ~Normal_Map_Shader() = default;

    virtual V2F vertex_shader(const Vertex& vertex_input){
        V2F processed_v;
        Matrix v_m = Matrix::vector2matrix(Vec4f(vertex_input.pos.x, vertex_input.pos.y, vertex_input.pos.z, 1.0f));
        v_m = (global_payload.transform_matrix * v_m).cartesian();
        processed_v.pos = Vec3f(v_m[0][0], v_m[1][0], v_m[2][0]);
        //processed_v.norm = vertex_input.norm;
        processed_v.norm = Math::to_Vec3<float>(global_payload.proj_correction_mat * Math::to_Matrix4(vertex_input.norm, 0.0f));
        processed_v.tex_coord = vertex_input.tex_coord;
        return processed_v;
    }

    virtual Vec3i fragment_shader(const V2F& v2f){
        float shadow_coeff = 1.0f;
        if(global_payload.shadow_buffer != nullptr){
            //std::cout << "shadow buffer not null" << std::endl;
            Vec4f shadow_buff_coord = Math::to_Vec4<float>(global_payload.screen_shadow_mat * Math::to_Matrix4(v2f.pos));
            shadow_buff_coord = shadow_buff_coord/shadow_buff_coord.w;
            //int shadow_buff_idx = (int)(shadow_buff_coord.x) + (int)(shadow_buff_coord.y) * global_payload.shadow_buffer->get_width();
            float shadow_buff_val = global_payload.shadow_buffer->get(int(shadow_buff_coord.x), int(shadow_buff_coord.y)).r / 255.0f;
            //std::cout << "shadow_buff_val: " << shadow_buff_val << "z: " << shadow_buff_coord.z <<std::endl;
            //std::cout << "has shadow: " << (shadow_buff_val > shadow_buff_coord.z) << std::endl;
            shadow_coeff = 0.3 + 0.7 * (shadow_buff_val < shadow_buff_coord.z/global_payload.zDepth);
        }
        TGAImage* texture = global_payload.model->get_diffuse_texture();
        TGAImage* normal_map = global_payload.model->get_normal_map_texture();
        TGAColor normal = normal_map->get(normal_map->get_width()*v2f.tex_coord.u, 
                                    normal_map->get_height()*(1.0-v2f.tex_coord.v));
        TGAColor tex_color = texture->get(texture->get_width()*v2f.tex_coord.u, 
                                    texture->get_height()*(1.0-v2f.tex_coord.v));
        Vec3f normal_vec = Vec3f(normal.r, normal.g, normal.b).normalize()*2.0f - Vec3f(1.0f, 1.0f, 1.0f);
        //std::cout << "normal_vec: " << normal_vec << std::endl;
        float mapped_light_intensity = normal_vec * global_payload.main_light_dir * global_payload.main_light_intensity;
        //std::cout << "mapped_light_intensity: " << mapped_light_intensity << std::endl;
        mapped_light_intensity = std::clamp(mapped_light_intensity, 0.0f, 1.0f);
        //std::cout << "clamped_light_intensity: " << mapped_light_intensity << std::endl;
        Vec3i final_color = tex_color.to_vec3() * mapped_light_intensity * shadow_coeff;                      
        return final_color;
    }
    
};