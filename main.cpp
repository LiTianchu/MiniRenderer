#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void draw_mesh_wireframe(Model model, TGAImage &image);
void draw_mesh_shaded(Model model, TGAImage texture, float *zbuffer, TGAImage &image, Vec3f light_dir);
void triangle_linesweeping(Vec2i tv0, Vec2i tv1, Vec2i tv2, TGAImage &image, TGAColor color);
void sort_triangle_vertices(Vec2i (&t)[]);
Vec3f get_barycentric(Vec3f *pts, Vec2i P);
void triangle_barycentric(Vec3f *pts, Vec2f *tex_coords, TGAImage texture, float *zbuffer, TGAImage &image, TGAColor color);

int main(int argc, char **argv)
{
    TGAImage image(1920, 1920, TGAImage::RGB);

    float *zbuffer = new float[image.get_width() * image.get_height()];

    if (argc >= 2)
    {
        Model model_loaded = Model("obj/african_head.obj");

        if (std::string(argv[1]) == "wireframe")
        {
            draw_mesh_wireframe(model_loaded, image);
        }
        else if (std::string(argv[1]) == "shaded")
        {
            draw_mesh_shaded(model_loaded, TGAImage(), zbuffer, image, Vec3f(0, 0, -1));
        }
        else if (std::string(argv[1]) == "texture")
        {
            TGAImage texture = TGAImage();
            texture.read_tga_file("obj/african_head_diffuse.tga");
            draw_mesh_shaded(model_loaded, texture, zbuffer, image, Vec3f(0, 0, -1));
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

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

// sort the vertices of the triangle in descending order of y
void sort_triangle_vertices(Vec2i (&t)[])
{
    for (int i = 0; i < 3 - 1; i++)
    {
        int selection = i;
        for (int j = i + 1; j < 3; j++)
        {
            if (t[j].y > t[selection].y)
            {
                selection = j;
            }
        }
        std::swap(t[i], t[selection]);
    }
}

void triangle_linesweeping(Vec2i tv0, Vec2i tv1, Vec2i tv2, TGAImage &image, TGAColor color)
{
    // rasterize the upperhalf
    int total_height = tv0.y - tv2.y;
    int segment_height_upper = tv0.y - tv1.y;
    for (int y = tv0.y; y >= tv1.y; y--)
    {
        float alpha = (float)(tv0.y - y) / total_height;        // straight edge
        float beta = (float)(tv0.y - y) / segment_height_upper; // segment edge

        // interpolate the x value
        int x1 = tv0.x + (tv2.x - tv0.x) * alpha;
        int x2 = tv0.x + (tv1.x - tv0.x) * beta;

        line(x1, y, x2, y, image, color);
    }

    // rasterize the lowerhalf
    int segment_height_lower = tv1.y - tv2.y;
    for (int y = tv1.y; y >= tv2.y; y--)
    {
        float alpha = (float)(tv0.y - y) / total_height;        // straight edge
        float beta = (float)(tv1.y - y) / segment_height_lower; // segment edge

        // interpolate the x value
        int x1 = tv0.x + (tv2.x - tv0.x) * alpha;
        int x2 = tv1.x + (tv2.x - tv1.x) * beta;

        line(x1, y, x2, y, image, color);
    }
}

void triangle_barycentric(Vec3f *pts, Vec2f *tex_coords, TGAImage texture, float *zbuffer, TGAImage &image, TGAColor color)
{
    // obtain the bounding box cooridnates
    int x_min = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
    int x_max = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
    int y_min = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
    int y_max = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));

    // clamp the coordinates to prevent exceeding the boundary
    x_min = std::max(0, x_min);
    x_max = std::min(image.get_width() - 1, x_max);
    y_min = std::max(0, y_min);
    y_max = std::min(image.get_height() - 1, y_max);
    // std::cout << "x_min = " << x_min << " x_max = " << x_max << " y_min = " << y_min << " y_max = " << y_max << std::endl;
    //  foreach pixel in bounding box, put pixel if it is inside triangle
    for (int x = x_min; x <= x_max; x++)
    {
        for (int y = y_min; y <= y_max; y++)
        {

            Vec3f b_coord = get_barycentric(pts, Vec2i(x, y)); // calculate the barycentric coordinates

            if (b_coord.x < 0 || b_coord.y < 0 || b_coord.z < 0) // if any of the barycentric coordinates is negative, it is outside of triangle
                continue;                                        // skip if it is outside of triangle

            // calculate the z value
            float interpolated_z = b_coord.x * pts[0].z + b_coord.y * pts[1].z + b_coord.z * pts[2].z;
            
            float interpolated_u = b_coord.x * tex_coords[0].x + b_coord.y * tex_coords[1].x + b_coord.z * tex_coords[2].x;
            float interpolated_v = b_coord.x * tex_coords[0].y + b_coord.y * tex_coords[1].y + b_coord.z * tex_coords[2].y;
            
            // std::cout << interpolated_z << std::endl;
            int zbuffer_index = x + y * image.get_width();
            if (interpolated_z > zbuffer[zbuffer_index])
            {
                zbuffer[zbuffer_index] = interpolated_z;
                image.set(x, y, texture.get(interpolated_u * texture.get_width(), interpolated_v * texture.get_height()));
            }
        }
    }
}

Vec3f get_barycentric(Vec3f *pts, Vec2i P)
{
    //(ABx ACx PAx) X (ABy ACy PAy)
    Vec3f cross_product = Vec3f(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x).cross(Vec3f(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y)); // obtain (u*z, v*z, 1*z)

    if (cross_product.z == 0) // if z is 0, it is a line, return a vector with negative number
        return Vec3f(-1, 1, 1);

    Vec3f c = Vec3f(cross_product.x / cross_product.z, cross_product.y / cross_product.z, 1.0); // obtain (u,v,1)
    if (c.x < 0 || c.y < 0 || c.x + c.y > 1)
        return Vec3f(-1, 1, 1);            // if u or v is negative or their sum greater than 1, it is outside of triangle, return (-1,1,1)
    return Vec3f(1 - c.x - c.y, c.x, c.y); // calculate and return the alpha, beta and gamma
}

void draw_mesh_wireframe(Model model, TGAImage &image)
{
    // for each face in the model
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<Vec3i> face = model.face(i); // obtain the triangle indices of the current face
        // foreach vertex of the current face, render a line from the current vertex to the next vertex
        for (int j = 0; j < 3; j++)
        {
            // std::cout << "face[" << i << "][" << j << "] = " << face[j] << std::endl;
            Vec3 vertex_start = model.vert(face[j].ivert);
            Vec3 vertex_end = model.vert(face[(j + 1) % 3].ivert);

            // std::cout << "vertex_start = " << vertex_start << std::endl;
            // std::cout << "vertex_end = " << vertex_end << std::endl;

            // map the world coordinates to image coordinates
            int x0 = (vertex_start.x + 1.0) / 2.0 * image.get_width();
            int y0 = (vertex_start.y + 1.0) / 2.0 * image.get_height();
            int x1 = (vertex_end.x + 1.0) / 2.0 * image.get_width();
            int y1 = (vertex_end.y + 1.0) / 2.0 * image.get_height();

            // std::cout << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
            // draw the line
            line(x0, y0, x1, y1, image, white);
        }
    }
}

void draw_mesh_shaded(Model model, TGAImage texture, float *zbuffer, TGAImage &image, Vec3f light_dir)
{
    // for each face in the model

    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<Vec3i> face = model.face(i); // obtain the triangle indices of the current face
        Vec3f t_screen[3];
        Vec3f t_world[3];
        Vec2f t_uv[3];
        // foreach vertex of the current face, render a line from the current vertex to the next vertex
        for (int j = 0; j < 3; j++)
        {
            Vec3 vertex_world = model.vert(face[j].ivert);
            Vec2f uv = model.uv(face[j].iuv);
            // map the world coordinates to image coordinates
            int x_screen = (vertex_world.x + 1.0) / 2.0 * image.get_width();
            int y_screen = (vertex_world.y + 1.0) / 2.0 * image.get_height();
            t_world[j] = vertex_world;
            t_screen[j] = Vec3f(x_screen, y_screen, (vertex_world.z + 1.0) / 2.0);
            t_uv[j] = uv;
        }
        // compute normal
        Vec3f normal = (t_world[2] - t_world[0]).cross(t_world[1] - t_world[0]);
        normal.normalize();

        float intensity = normal * light_dir;

        if (intensity > 0)
        {
            triangle_barycentric(t_screen, t_uv, texture, zbuffer, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255));
        }
    }
}

// Bresenham's line drawing algorithm
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    /*if y is greator than x, the line drawn will have gaps,
    so need to swap the x and y coordinates before drawing*/
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    /*//if x0 greater than x1,
    swap them so that drawing always goes left to right*/
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    // error2 is the distance between the line and the pixel, multiplied by 2
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;

    int y = y0;

    for (int x = x0; x < x1; x++)
    {
        if (steep)
        { // the drawing is transposed after swapping, so transpose it back
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) // check if the error is greater than the distance between the line and the pixel
        {
            y += (y1 > y0 ? 1 : -1); // if y1 above y0, increase by 1, else decrese by 1
            error2 -= dx * 2;
        }
    }
}