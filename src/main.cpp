#include <iostream>
#include <stdio.h>
#include "tgaimage.h"
#include "model.h"
#include "he_model.h"
#include "shaders/shader.h"
#include "shaders/flat_shader.cpp"
#include "shaders/gouraud_shader.cpp"
#include "shaders/uv_shader.cpp"
#include "shaders/diffuse_map_shader.cpp"
#include "shaders/normal_map_shader.cpp"
#include "engine.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Vec3f camera_pos = Vec3f(0, 1, 3);
Vec3f light_dir = Vec3f(1, -1, 1).normalize();
float main_light_intensity = 2.0f;
Vec3f center_pos = Vec3f(0, 0, 0);
Vec3f up_dir = Vec3f(0,1,0);

enum Mode
{
    WIREFRAME,
    FLAT,
    SMOOTH,
    TEXTURE,
    UV
};

int main(int argc, char **argv)
{
    TGAImage image(1920, 1920, TGAImage::RGB);

    float *zbuffer = new float[image.get_width() * image.get_height()];

    if (argc >= 2)
    {
        HEModel he_model_loaded = HEModel("obj/african_head/african_head.obj");
        TGAImage diffuse_texture = TGAImage();
        TGAImage normal_map = TGAImage();
        diffuse_texture.read_tga_file("obj/african_head/african_head_diffuse.tga");
        normal_map.read_tga_file("obj/african_head/african_head_nm.tga");
        he_model_loaded.set_diffuse_texture(&diffuse_texture);
        he_model_loaded.set_normal_map_texture(&normal_map);
        
        std::cout << "Rendering " << argv[1] << " model" << std::endl;

        Engine engine = Engine(light_dir, main_light_intensity, camera_pos, zbuffer, &image);
        
        Shader_Global_Payload shader_payload = Shader_Global_Payload();
        shader_payload.main_light_dir = light_dir;
        shader_payload.main_light_intensity = main_light_intensity;
        shader_payload.camera_pos = camera_pos;
        shader_payload.model = &he_model_loaded;

        // transformation matrices
        Matrix rotation_matrix = Matrix::rotation(0, 0, 0);
        Matrix translation_matrix = Matrix::translation(0, 0, 0);
        Matrix scaling_matrix = Matrix::scaling(1, 1, 1);
        Matrix composite_linear_transform_mat = translation_matrix * rotation_matrix * scaling_matrix;
        Matrix view_matrix = Matrix::model_view(camera_pos, center_pos, up_dir);
        Matrix projection_matrix = Matrix::persp_projection(camera_pos);
        Matrix viewport_matrix = Matrix::viewport(0, 0, image.get_width(), image.get_height(), 0, 1);

        // store transform matrices for shader
        Matrix transform_matrix = viewport_matrix * projection_matrix * view_matrix * composite_linear_transform_mat;
        shader_payload.transform_matrix = transform_matrix;
        shader_payload.view_proj_mat = projection_matrix * view_matrix;
        shader_payload.proj_correction_mat = shader_payload.view_proj_mat.inverse_transpose().first;
        shader_payload.model_mat = composite_linear_transform_mat;
        shader_payload.view_mat = view_matrix;
        shader_payload.projection_mat = projection_matrix;
        shader_payload.viewport_mat = viewport_matrix;
        
        // he_model_loaded.qem_simplify(200);
        // HEModel he_model_loaded = HEModel("obj/diablo3_pose/diablo3_pose.obj");
        if (std::string(argv[1]) == "wireframe")
        {
            engine.render_model_wireframe(he_model_loaded);

        }
        else if (std::string(argv[1]) == "flat")
        {
            engine.render_shaded_model(he_model_loaded, new Flat_Shader(shader_payload));

        }
        else if (std::string(argv[1]) == "smooth")
        {
            engine.render_shaded_model(he_model_loaded, new Gouraud_Shader(shader_payload));
        }
        else if (std::string(argv[1]) == "texture")
        {
            // texture.read_tga_file("obj/diablo3_pose/diablo3_pose_diffuse.tga");
            engine.render_shaded_model(he_model_loaded, new Diffuse_Map_Shader(shader_payload));
        }
        else if (std::string(argv[1]) == "shaded-wireframe")
        {
            engine.render_shaded_model(he_model_loaded, new Flat_Shader(shader_payload));
            engine.render_model_wireframe(he_model_loaded);
        }
        else if (std::string(argv[1]) == "uv")
        {
            engine.render_shaded_model(he_model_loaded, new UV_Shader(shader_payload));
        }
        else if(std::string(argv[1]) == "normal"){
            engine.render_shaded_model(he_model_loaded, new Normal_Map_Shader(shader_payload));
        }
        else
        {
            std::cout << "Argument is not valid" << std::endl;
        }
    }
    else
    {
        std::cout << "Argument is empty" << std::endl;
    }
    std::cout << "Writing rendered output..." << std::endl;

    image.flip_vertically();
    image.write_tga_file("output.tga");
    std::cout << "Done!" << std::endl;
    return 0;
}