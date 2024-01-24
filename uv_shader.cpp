#include "shader.h"

class UV_Shader : public Shader{
    public:
    UV_Shader(){ }
    ~UV_Shader(){ }

    Vertex vertex_shader(Vertex vertex_input){
        return vertex_input;
    }

    Vec3f fragment_shader(Vertex processed_vertex){
        return Vec3f(processed_vertex.tex_coord.u, processed_vertex.tex_coord.v, 0);
    }
    
};