#include "triangle.h"

void Triangle::SetMat(Material _mat)
{
	mat = _mat;
}

Triangle::Triangle()
{
	mat = Material();
	shadingGroup = -1;
}

STriangle::STriangle() : STriangle(Vertex(), Vertex(), Vertex(), Material(), -1) {}
STriangle::STriangle(Vertex v0, Vertex v1, Vertex v2) : STriangle(v0, v1, v2, Material(), -1) {}
STriangle::STriangle(Vertex v0, Vertex v1, Vertex v2, Material _mat) : STriangle(v0, v1, v2, _mat, -1) {}
STriangle::STriangle(Vertex v0, Vertex v1, Vertex v2, Material _mat, int _sg)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
	mat = _mat;
	shadingGroup = _sg;
}

ITriangle::ITriangle() : ITriangle(-1, -1, -1, Material(), -1) {}
ITriangle::ITriangle(int i0, int i1, int i2) : ITriangle(i0, i1, i2, Material(), -1) {}
ITriangle::ITriangle(int i0, int i1, int i2, Material _mat) : ITriangle(i0, i1, i2, _mat, -1) {}
ITriangle::ITriangle(int i0, int i1, int i2, Material _mat, int _sg)
{
	vertices[0] = i0;
	vertices[1] = i1;
	vertices[2] = i2;
	mat = _mat;
	shadingGroup = _sg;
}