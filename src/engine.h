#include "tgaimage.h"
#include "shader.h"
#ifndef ENGINE_H
#define ENGINE_H
class Engine
{
private:
    Vec3f main_light_dir;
    float main_light_intensity;
    Vec3f camera_pos;
    float *z_buffer;
    TGAImage *frame_buffer;

public:
    Engine::Engine(Vec3f main_light_dir_, float main_light_intensity_,
                   Vec3f camera_pos_, float *zbuffer_,
                   TGAImage *frame_buffer_) : main_light_dir(main_light_dir_),
                                              main_light_intensity(main_light_intensity_),
                                              camera_pos(camera_pos_), z_buffer(zbuffer_),
                                              frame_buffer(frame_buffer_) {}
                                              
    void render_shaded_model(HEModel model, Shader *shader);
    void render_model_wireframe(HEModel model);
    void rasterize_triangle(std::vector<Vertex> vertices, Shader *shader);
    void draw_line(int x0, int y0, int x1, int y1, TGAColor color);
};

#endif