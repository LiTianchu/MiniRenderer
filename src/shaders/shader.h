#ifndef __SHADER_H__
#define __SHADER_H__

#include "../geometry.h"
#include "../tgaimage.h"
#include "../he_model.h"
#include <vector>

struct Fragment_Shader_Payload{
    float light_intensity;
    Vec3f pos;
    Vec3f norm;
    Vec2f tex_coord;
    Vec3i base_color;
    Vec3f light_dir;

    TGAImage* texture;
    Fragment_Shader_Payload(){ }
    Fragment_Shader_Payload(float light_intensity_, Vec3f light_dir_, Vec3f pos_, Vec3f norm_, Vec2f tex_coord_, Vec3i color_, TGAImage* texture_) : 
    light_intensity(light_intensity_), light_dir(light_dir_), pos(pos_), norm(norm_), tex_coord(tex_coord_), base_color(color_), texture(texture_) { }
};

// class Vertex_Shader_Payload{
//     private:
//     Vertex vertex_input;
//     public:
// };

class Shader{
    public:
    Shader(){ }
    virtual ~Shader(){ }

    virtual Vertex vertex_shader(const Vertex& vertex_input) = 0;
    virtual Vec3i fragment_shader(const Fragment_Shader_Payload& processed_vertex) = 0;
};
#endif