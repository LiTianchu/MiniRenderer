#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "he_model.h"
#include "shaders/shader.h"
#include "shaders/flat_shader.cpp"
#include "shaders/gouraud_shader.cpp"
#include "shaders/uv_shader.cpp"
#include "shaders/diffuse_map_shader.cpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

enum Mode
{
    WIREFRAME,
    FLAT,
    SMOOTH,
    TEXTURE,
    UV
};

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

// Bresenham's line drawing algorithm
void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
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

        draw_line(x1, y, x2, y, image, color);
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

        draw_line(x1, y, x2, y, image, color);
    }
}

Vec3f get_barycentric(Vec2f *pts, Vec2i P)
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

void rasterize_triangle(std::vector<Vertex> vertices, Shader *shader, TGAImage texture,
                        float *zbuffer, Vec3f light_dir, TGAImage &image, TGAColor color,
                        Mode mode)
{
    // compute face normal
    Vec3f face_normal = (vertices[2].pos - vertices[0].pos).cross(vertices[1].pos - vertices[0].pos);
    face_normal.normalize();

    //  extract the vertex data
    Vec2f pts[] = {vertices[0].pos_screen, vertices[1].pos_screen, vertices[2].pos_screen};
    float z_indices[] = {vertices[0].screen_z, vertices[1].screen_z, vertices[2].screen_z};
    Vec3f norms[] = {vertices[0].norm, vertices[1].norm, vertices[2].norm};
    Vec2f tex_coords[] = {vertices[0].tex_coord, vertices[1].tex_coord, vertices[2].tex_coord};

    // obtain the bounding box cooridnates
    int x_min = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
    int x_max = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
    int y_min = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
    int y_max = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));

    //  clamp the coordinates to prevent exceeding the boundary
    x_min = std::max(0, x_min);
    x_max = std::min(image.get_width() - 1, x_max);
    y_min = std::max(0, y_min);
    y_max = std::min(image.get_height() - 1, y_max);

    //  foreach pixel in bounding box, put pixel if it is inside triangle
    for (int x = x_min; x <= x_max; x++)
    {
        for (int y = y_min; y <= y_max; y++)
        {

            Vec3f b_coord = get_barycentric(pts, Vec2i(x, y)); // calculate the barycentric coordinates

            if (b_coord.x < 0 || b_coord.y < 0 || b_coord.z < 0) // if any of the barycentric coordinates is negative, it is outside of triangle
                continue;                                        // skip if it is outside of triangle

            // calculate the z value
            float interpolated_z = b_coord.x * z_indices[0] + b_coord.y * z_indices[1] + b_coord.z * z_indices[2];

            int zbuffer_index = x + y * image.get_width();
            if (interpolated_z > zbuffer[zbuffer_index]) // if pass z test, start shading the pixel
            {
                zbuffer[zbuffer_index] = interpolated_z; // update z buffer
                Fragment_Shader_Payload frag_data =
                    Fragment_Shader_Payload(0, Vec3f(0, 0, 0),
                                            Vec3f(0, 0, 0), Vec2f(0, 0),
                                            color.to_vec3(), &texture);
                if (mode == Mode::FLAT)
                {
                    frag_data.base_color = color.to_vec3() * (face_normal * light_dir);
                }
                else
                {
                    std::vector<float> I_at_vertices = {norms[0] * light_dir, norms[1] * light_dir, norms[2] * light_dir};
                    float interpolated_I = b_coord.x * I_at_vertices[0] + b_coord.y * I_at_vertices[1] + b_coord.z * I_at_vertices[2];
                    float interpolated_u = b_coord.x * tex_coords[0].u + b_coord.y * tex_coords[1].u + b_coord.z * tex_coords[2].u;
                    float interpolated_v = b_coord.x * tex_coords[0].v + b_coord.y * tex_coords[1].v + b_coord.z * tex_coords[2].v;
                    frag_data.tex_coord = Vec2f(interpolated_u, interpolated_v);
                    frag_data.light_intensity = interpolated_I;
                }
                // obtain shaded color using shader
                Vec3i color_vector = shader->fragment_shader(frag_data);
                TGAColor shade_color = TGAColor(color_vector);
                image.set(x, y, shade_color);
            }
        }
    }
}

void draw_mesh_wireframe(HEModel model, TGAImage &image)
{
    // for each face in the model
    for (std::set<Face *>::iterator face = model.faces_begin(); face != model.faces_end(); ++face)
    {
        HEdge *h_edge = (*face)->h;
        int num = 0;
        do
        {
            HEdge *prev_e = h_edge->prev;
            HEdge *next_e = h_edge->next;
            Vertex *vertex_start = prev_e->v;
            Vertex *vertex_end = h_edge->v;
            Vec3 pos_start = vertex_start->pos;
            Vec3 pos_end = vertex_end->pos;

            // map the world coordinates to image coordinates
            int x0 = (pos_start.x + 1.0) / 2.0 * image.get_width();
            int y0 = (pos_start.y + 1.0) / 2.0 * image.get_height();
            int x1 = (pos_end.x + 1.0) / 2.0 * image.get_width();
            int y1 = (pos_end.y + 1.0) / 2.0 * image.get_height();

            // draw the line
            draw_line(x0, y0, x1, y1, image, h_edge->pair == NULL ? red : white);

            h_edge = h_edge->next;
        } while ((h_edge != (*face)->h));
    }
}

void wireframe_dfs(Face& f, bool (&faces_visited)[], TGAImage& image)
{
    if (faces_visited[f.index])
    {
        return;
    }
    //draw the face
    HEdge *h_edge = f.h;
    //std::cout << "drawing face with index: " << f.index << std::endl;
    std::vector<Face> neighbor_faces;
    do
    {
        HEdge *prev_e = h_edge->prev;
        HEdge *next_e = h_edge->next;
        Vertex *vertex_start = prev_e->v;
        Vertex *vertex_end = h_edge->v;
        Vec3 pos_start = vertex_start->pos;
        Vec3 pos_end = vertex_end->pos;
        
        HEdge *pair = h_edge->pair;
        if (pair != NULL)
        {
            Face neighbor_face = *pair->f;
            neighbor_faces.push_back(neighbor_face);
        }
        // map the world coordinates to image coordinates
        int x0 = (pos_start.x + 1.0) / 2.0 * image.get_width();
        int y0 = (pos_start.y + 1.0) / 2.0 * image.get_height();
        int x1 = (pos_end.x + 1.0) / 2.0 * image.get_width();
        int y1 = (pos_end.y + 1.0) / 2.0 * image.get_height();
        
        draw_line(x0, y0, x1, y1, image, blue);
        h_edge = h_edge->next;
    } while (h_edge != f.h);

    faces_visited[f.index] = true;

    for(Face& n : neighbor_faces)
    {
        wireframe_dfs(n, faces_visited, image);
    }
    
}

void draw_mesh_wireframe_dfs(HEModel model, TGAImage &image)
{
    bool faces_visited[model.num_of_faces()] = {false};
    wireframe_dfs(**model.faces_begin(), faces_visited, image);
}


void render_model(HEModel model, TGAImage texture, Shader *shader, float *zbuffer, TGAImage &image, Vec3f light_dir, Mode mode)
{
    // for each face in the model
    for (std::set<Face *>::iterator face_itr = model.faces_begin(); face_itr != model.faces_end(); ++face_itr)
    {
        HEdge *h_edge = (*face_itr)->h; // optain the first half edge of the face
        // initialize the arrays to store vertex data
        std::vector<Vec3f> screen_coords;
        std::vector<Vertex> vertices;
        int j = 0;
        // foreach vertex of the current face, render a line from the current vertex to the next vertex
        do
        {
            Vertex curr_vertex = shader->vertex_shader(*(h_edge->v));

            // map the model coordinates to image coordinates
            int x_screen = (curr_vertex.pos.x + 1.0) / 2.0 * image.get_width();
            int y_screen = (curr_vertex.pos.y + 1.0) / 2.0 * image.get_height();
            float z_index = (curr_vertex.pos.z + 1.0) / 2.0;
            curr_vertex.pos_screen = Vec2f(x_screen, y_screen);
            curr_vertex.screen_z = z_index;
            vertices.push_back(curr_vertex);
            h_edge = h_edge->next;
            j++;
        } while (h_edge != (*face_itr)->h);

        rasterize_triangle(vertices, shader, texture, zbuffer, light_dir, image, TGAColor(255, 255, 255, 255), mode);
    }
}

int main(int argc, char **argv)
{
    TGAImage image(1920, 1920, TGAImage::RGB);

    float *zbuffer = new float[image.get_width() * image.get_height()];

    if (argc >= 2)
    {
        HEModel he_model_loaded = HEModel("obj/african_head/african_head.obj");
        he_model_loaded.qem_simplify(200);
        //HEModel he_model_loaded = HEModel("obj/diablo3_pose/diablo3_pose.obj");
        //HEModel he_model_loaded = HEModel("obj/cube.obj");
        if (std::string(argv[1]) == "wireframe")
        {
            draw_mesh_wireframe_dfs(he_model_loaded, image);
        }
        else if (std::string(argv[1]) == "flat")
        {
            render_model(he_model_loaded, TGAImage(), new Flat_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::FLAT);
        }
        else if (std::string(argv[1]) == "smooth")
        {
            render_model(he_model_loaded, TGAImage(), new Gouraud_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::SMOOTH);
        }
        else if (std::string(argv[1]) == "texture")
        {
            TGAImage texture = TGAImage();
            //texture.read_tga_file("obj/diablo3_pose/diablo3_pose_diffuse.tga");
            texture.read_tga_file("obj/african_head/african_head_diffuse.tga");
            render_model(he_model_loaded, texture, new Diffuse_Map_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::TEXTURE);
        }else if(std::string(argv[1]) == "shaded-wireframe"){
            render_model(he_model_loaded, TGAImage(), new Flat_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::FLAT);
            draw_mesh_wireframe_dfs(he_model_loaded, image);
        }
        else if (std::string(argv[1]) == "uv")
        {
            render_model(he_model_loaded, TGAImage(), new UV_Shader(), zbuffer, image, Vec3f(0, 0, -1), Mode::UV);
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