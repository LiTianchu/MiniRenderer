#include "geometry.h"

class Math
{
public:
    static Vec3f get_barycentric(Vec2f *pts, Vec2i P)
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
};