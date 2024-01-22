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
	HEdge *pair; // oppositely oriented half-edge
	HEdge *next; // next half-edge around the face
	HEdge *prev; // previous half-edge in the face
	Vertex *v; // vertex at the end of the half-edge
	Face *f; // face the half-edge borders
	HEdge() : pair(), next(), v(), f() {}
	HEdge(HEdge *pair_, HEdge *next_, Vertex *v_, Face *f_) : pair(pair_), next(next_), v(v_), f(f_) {}
};


struct Face
{
	HEdge *h; // one of the half-edges bordering the face
	Face() : h() {}
	Face(HEdge *h_) : h(h_) {}
};

struct Vertex
{
	HEdge *h; // one of the half-edges emanting from the vertex
	int index;
	Vec3f pos;
	Vec3f norm;
	Vec2f tex_coord;
	Vertex() : h(), pos(), norm(), tex_coord() {}
	Vertex(HEdge *_h, Vec3f _pos, Vec3f _norm, Vec2f _tex_coord) : h(_h), pos(_pos), norm(_norm), tex_coord(_tex_coord) {}
	//inline Vertex operator<(const Vertex &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); } //cross product operator
	inline bool operator<(const Vertex &v) const { return index < v.index; };
	inline bool operator==(const Vertex &v) const { return index == v.index; };
	inline bool operator>(const Vertex &v) const { return index > v.index; };
};

// class VertexCompare{
// 	bool operator() (Vertex a, Vertex b) const {
//         return false;
//     }
// };

class HEModel
{
private:
	std::vector<HEdge*> h_edges;
	std::vector<Face*> faces;
	std::set<Vertex*> vertices;
	
public:
	HEModel(const char *filename);
	~HEModel();

};

#endif