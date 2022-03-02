#include "triangle.h"

void Triangle::SetMat(Material _mat)
{
	mat = _mat;
}

Triangle::Triangle()
{
	vertices[0] = Vertex();
	vertices[1] = Vertex();
	vertices[2] = Vertex();
	mat = Material();
	shadingGroup = -1;
}

Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
	mat = Material();
	shadingGroup = -1;
}

Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2, Material _mat)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
	mat = _mat;
	shadingGroup = -1;
}

Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2, Material _mat, int _sg)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
	mat = _mat;
	shadingGroup = _sg;
}