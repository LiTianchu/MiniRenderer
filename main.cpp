#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void draw_mesh(Model model, TGAImage &image);

int main(int argc, char **argv)
{
    TGAImage image(1920, 1920, TGAImage::RGB);
    // draw a line
    //line(20, 13, 40, 80, image, white); // x0,y0,x1,y1

    //render the model
    Model model_loaded = Model("obj/african_head.obj");
    draw_mesh(model_loaded, image);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

void draw_mesh(Model model, TGAImage &image)
{
    // for each face in the model
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> face = model.face(i); // obtain the triangle indices of the current face

        // foreach vertex of the current face, render a line from the current vertex to the next vertex
        for (int j = 0; j < 3; j++)
        {
            //std::cout << "face[" << i << "][" << j << "] = " << face[j] << std::endl;
            Vec3 vertex_start = model.vert(face[j]);
            Vec3 vertex_end = model.vert(face[(j + 1) % 3]);
            
            //std::cout << "vertex_start = " << vertex_start << std::endl;
            //std::cout << "vertex_end = " << vertex_end << std::endl;

            // map the vertex coordinates to image coordinates
            int x0 = (vertex_start.x + 1.0) / 2.0 * image.get_width();
            int y0 = (vertex_start.y + 1.0) / 2.0 * image.get_height();
            int x1 = (vertex_end.x + 1.0) / 2.0 * image.get_width();
            int y1 = (vertex_end.y + 1.0) / 2.0 * image.get_height();

            //std::cout << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
            //draw the line
            line(x0, y0, x1, y1, image, white);
        }
    }
}

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