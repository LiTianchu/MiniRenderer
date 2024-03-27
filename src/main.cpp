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
#include "engine.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Vec3f cameraPos = Vec3f(0, 0, 3);

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
        diffuse_texture.read_tga_file("obj/african_head/african_head_diffuse.tga");
        he_model_loaded.set_diffuse_texture(&diffuse_texture);
        
        std::cout << "Rendering " << argv[1] << " model" << std::endl;

        Vec3f main_light_dir = Vec3f(0, 0, -1);
        float main_light_intensity = 1;
        Engine engine = Engine(main_light_dir, main_light_intensity, cameraPos, zbuffer, &image);
        
        Shader_Global_Payload shader_payload = Shader_Global_Payload();
        shader_payload.main_light_dir = main_light_dir;
        shader_payload.main_light_intensity = main_light_intensity;
        shader_payload.camera_pos = cameraPos;
        shader_payload.texture = &diffuse_texture;

        // he_model_loaded.qem_simplify(200);
        // HEModel he_model_loaded = HEModel("obj/diablo3_pose/diablo3_pose.obj");
        // HEModel he_model_loaded = HEModel("obj/cube.obj");
        if (std::string(argv[1]) == "wireframe")
        {
            //draw_mesh_wireframe_dfs(he_model_loaded, image);
            engine.render_model_wireframe(he_model_loaded);

        }
        else if (std::string(argv[1]) == "flat")
        {
            //render_model(he_model_loaded, TGAImage(), new Flat_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::FLAT);

        }
        else if (std::string(argv[1]) == "smooth")
        {
            //render_model(he_model_loaded, TGAImage(), new Gouraud_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::SMOOTH);
            engine.render_shaded_model(he_model_loaded, new Gouraud_Shader(shader_payload));
        }
        else if (std::string(argv[1]) == "texture")
        {
            //TGAImage texture = TGAImage();
            // texture.read_tga_file("obj/diablo3_pose/diablo3_pose_diffuse.tga");
            //texture.read_tga_file("obj/african_head/african_head_diffuse.tga");
            //render_model(he_model_loaded, texture, new Diffuse_Map_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::TEXTURE);
            engine.render_shaded_model(he_model_loaded, new Diffuse_Map_Shader(shader_payload));
        }
        else if (std::string(argv[1]) == "shaded-wireframe")
        {
            //render_model(he_model_loaded, TGAImage(), new Flat_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::FLAT);
            //draw_mesh_wireframe_dfs(he_model_loaded, image);
            engine.render_model_wireframe(he_model_loaded);
            engine.render_shaded_model(he_model_loaded, new Flat_Shader(shader_payload));
        }
        else if (std::string(argv[1]) == "uv")
        {
            //render_model(he_model_loaded, TGAImage(), new UV_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::UV);
            engine.render_shaded_model(he_model_loaded, new UV_Shader(shader_payload));
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