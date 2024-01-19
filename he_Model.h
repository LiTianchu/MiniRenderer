#ifndef __HE_MODEL_H__
#define __HE_MODEL_H__

#include "geometry.h"
#include <vector>
#include <set>
#include <string>

struct Face; // forward declare to work around cyclic dependency
struct Vertex;

struct HEdge
{
	HEdge *pair;
	HEdge *next;
	Vertex *v;
	Face *f;
	HEdge() : pair(), next(), v(), f() {}
	HEdge(HEdge *pair_, HEdge *next_, Vertex *v_, Face *f_) : pair(pair_), next(next_), v(v_), f(f_) {}
};

struct Face
{
	HEdge *h;
	Face() : h() {}
	Face(HEdge *h_) : h(h_) {}
};

struct Vertex
{
	HEdge *h;
	Vec3f pos;
	Vec3f norm;
	Vec2f tex_coord;
	Vertex() : h(), pos(), norm(), tex_coord() {}
	Vertex(HEdge *_h, Vec3f _pos, Vec3f _norm, Vec2f _tex_coord) : h(_h), pos(_pos), norm(_norm), tex_coord(_tex_coord) {}
	
};

// class VertexCompare{
// 	bool operator() (Vertex a, Vertex b) const {
//         return false;
//     }
// };

class HEModel
{
private:
	std::set<HEdge> edges;
	std::set<Face> faces;
	std::set<Vertex> vertices;
	
public:
	HEModel(const char *filename);
	~HEModel();

};

#endif