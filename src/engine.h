#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "tgaimage.h"
#include "shaders/shader.h"

struct Engine_Data
{
    Vec3f main_light_dir;
    float main_light_intensity;
    Vec3f camera_pos;
    float *z_buffer;
    // TGAImage *output_render;
};

class Engine
{
private:
    Vec3f main_light_dir;
    float main_light_intensity;
    Vec3f camera_pos;
    float *z_buffer;
    // TGAImage *output_render;
    void wireframe_dfs(const Face &f, bool (&faces_visited)[], TGAImage *frame_buffer);

public:
    Engine(Vec3f main_light_dir_, float main_light_intensity_,
           Vec3f camera_pos_, float *zbuffer_) : main_light_dir(main_light_dir_),
                                                 main_light_intensity(main_light_intensity_),
                                                 camera_pos(camera_pos_), z_buffer(zbuffer_) {}
    Engine_Data get_engine_data()
    {
        Engine_Data data;
        data.main_light_dir = main_light_dir;
        data.main_light_intensity = main_light_intensity;
        data.camera_pos = camera_pos;
        data.z_buffer = z_buffer;
        // data.output_render = output_render;
        return data;
    }

    void reset_zbuffer()
    {
        delete z_buffer;
        z_buffer = new float[1920 * 1920];
    }

    void render_shaded_model(HEModel model, Shader *shader, TGAImage *frame_buffer);
    void render_model_wireframe(HEModel model, TGAImage *frame_buffer);
    //void render_shadow_buffer(HEModel model, Shader *shader, TGAImage *shadow_buffer);
    void rasterize_triangle(std::vector<V2F> vert_data, Shader *shader, TGAImage *frame_buffer);
    void draw_line(int x0, int y0, int x1, int y1, TGAColor color, TGAImage *frame_buffer);
};

#endif