#include "engine.h"
#include "math.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

void Engine::render_shaded_model(HEModel model, Shader *shader)
{
    // for each face in the model
    for (std::set<Face *>::iterator face_itr = model.faces_begin(); face_itr != model.faces_end(); ++face_itr)
    {
        HEdge *h_edge = (*face_itr)->h; // optain the first half edge of the face
        // initialize the arrays to store vertex data
        std::vector<Vec3f> screen_coords;
        std::vector<Vertex> vertices;
        int j = 0;

        do
        {
            Vertex curr_vertex = shader->vertex_shader(*(h_edge->v));
            Matrix curr_vertex_matrix = Matrix::vector2matrix(Vec4f(curr_vertex.pos.x, curr_vertex.pos.y, curr_vertex.pos.z, 1.0f));
            Matrix rotation_matrix = Matrix::rotation(0, 45, 45);
            Matrix translation_matrix = Matrix::translation(0.5, 0.5, 0);
            Matrix view_matrix = Matrix::model_view(camera_pos, Vec3f(0, 0, 0), Vec3f(0, 1, 0));
            Matrix projection_matrix = Matrix::persp_projection(camera_pos);
            Matrix viewport_matrix = Matrix::viewport(0, 0, frame_buffer->get_width(), frame_buffer->get_height(), 0, 1);
            curr_vertex_matrix = (viewport_matrix * projection_matrix * view_matrix * curr_vertex_matrix).cartesian();

            curr_vertex.pos = Vec3f(curr_vertex_matrix[0][0], curr_vertex_matrix[1][0], curr_vertex_matrix[2][0]);

            // curr_vertex.pos = dynamic_cast<Vec3f*>(&Matrix::matrix2vector(curr_vertex_matrix));

            // map the model coordinates to image coordinates
            // int x_screen = (curr_vertex.pos.x + 1.0) / 2.0 * image.get_width();
            // int y_screen = (curr_vertex.pos.y + 1.0) / 2.0 * image.get_height();
            // float z_index = (curr_vertex.pos.z + 1.0) / 2.0;

            curr_vertex.pos_screen = Vec2f(curr_vertex_matrix[0][0], curr_vertex_matrix[1][0]);
            curr_vertex.screen_z = curr_vertex_matrix[2][0];
            vertices.push_back(curr_vertex);
            h_edge = h_edge->next;
            j++;
        } while (h_edge != (*face_itr)->h);
        Vec3f face_normal = Math::get_face_normal(vertices);
        Fragment_Shader_Payload frag_data = Fragment_Shader_Payload(0, main_light_dir, Vec3f(0, 0, 0),
                                                                    Vec3f(0, 0, 0), face_normal, Vec2f(0, 0),
                                                                    TGAColor(255, 255, 255, 255).to_vec3(), model.get_diffuse_texture());
        rasterize_triangle(vertices, shader, frag_data);
    }
}

void Engine::render_model_wireframe(HEModel model)
{
     bool faces_visited[model.num_of_faces()] = {false};
    Engine::wireframe_dfs(**model.faces_begin(), faces_visited);
}

void Engine::wireframe_dfs(const Face& f, bool (&faces_visited)[])
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
        int x0 = (pos_start.x + 1.0) / 2.0 * frame_buffer -> get_width();
        int y0 = (pos_start.y + 1.0) / 2.0 * frame_buffer -> get_height();
        int x1 = (pos_end.x + 1.0) / 2.0 * frame_buffer -> get_width();
        int y1 = (pos_end.y + 1.0) / 2.0 * frame_buffer -> get_height();
        
        draw_line(x0, y0, x1, y1, blue);
        h_edge = h_edge->next;
    } while (h_edge != f.h);

    faces_visited[f.index] = true;

    for(Face& n : neighbor_faces)
    {
        wireframe_dfs(n, faces_visited);
    }
    
}

void Engine::rasterize_triangle(std::vector<Vertex> vertices, Shader *shader, Fragment_Shader_Payload &frag_data)
{
    // Vec3f face_normal = (vertices[2].pos - vertices[0].pos).cross(vertices[1].pos - vertices[0].pos);
    // face_normal.normalize();

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
    x_max = std::min(frame_buffer->get_width() - 1, x_max);
    y_min = std::max(0, y_min);
    y_max = std::min(frame_buffer->get_height() - 1, y_max);

    //  foreach pixel in bounding box, put pixel if it is inside triangle
    for (int x = x_min; x <= x_max; x++)
    {
        for (int y = y_min; y <= y_max; y++)
        {

            Vec3f b_coord = Math::get_barycentric(pts, Vec2i(x, y)); // calculate the barycentric coordinates

            if (b_coord.x < 0 || b_coord.y < 0 || b_coord.z < 0) // if any of the barycentric coordinates is negative, it is outside of triangle
                continue;                                        // skip if it is outside of triangle

            // calculate the z value
            float interpolated_z = b_coord.x * z_indices[0] + b_coord.y * z_indices[1] + b_coord.z * z_indices[2];

            int zbuffer_index = x + y * frame_buffer->get_width();
            if (interpolated_z > z_buffer[zbuffer_index]) // if pass z test, start shading the pixel
            {
                // interpolation stage
                z_buffer[zbuffer_index] = interpolated_z; // update z buffer
                // Fragment_Shader_Payload frag_data =
                //     Fragment_Shader_Payload(0, main_light_dir, Vec3f(0, 0, 0),
                //                             Vec3f(0, 0, 0), Vec2f(0, 0),
                //                             TGAColor(255, 255, 255, 255), model );
                // if (mode == Mode::FLAT)
                // {
                //     frag_data.base_color = color.to_vec3() * (face_normal * main_light_dir);
                // }
                // else
                // {
                
                // TODO: calculate the interpolated normal
                std::vector<float> I_at_vertices = {norms[0] * main_light_dir, norms[1] * main_light_dir, norms[2] * main_light_dir};
                float interpolated_I = b_coord.x * I_at_vertices[0] + b_coord.y * I_at_vertices[1] + b_coord.z * I_at_vertices[2];
                float interpolated_u = b_coord.x * tex_coords[0].u + b_coord.y * tex_coords[1].u + b_coord.z * tex_coords[2].u;
                float interpolated_v = b_coord.x * tex_coords[0].v + b_coord.y * tex_coords[1].v + b_coord.z * tex_coords[2].v;
                
                frag_data.tex_coord = Vec2f(interpolated_u, interpolated_v);
                frag_data.light_intensity = interpolated_I * main_light_intensity;
                // }
                // obtain shaded color using shader
                Vec3i color_vector = shader->fragment_shader(frag_data);
                TGAColor shade_color = TGAColor(color_vector);
                frame_buffer->set(x, y, shade_color);
            }
        }
    }
}

void Engine::draw_line(int x0, int y0, int x1, int y1, TGAColor color)
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
            frame_buffer->set(y, x, color);
        }
        else
        {
            frame_buffer->set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) // check if the error is greater than the distance between the line and the pixel
        {
            y += (y1 > y0 ? 1 : -1); // if y1 above y0, increase by 1, else decrese by 1
            error2 -= dx * 2;
        }
    }
}