#pragma once
#include "vertex.h"
#include "material.h"

const unsigned int TRI_VERTS = 3;

struct Triangle {
	Vertex vertices[TRI_VERTS];
	Material mat;
	int shadingGroup = -1;

	void SetMat(Material _mat);

	Triangle();
	Triangle(Vertex v0, Vertex v1, Vertex v2);
	Triangle(Vertex v0, Vertex v1, Vertex v2, Material _mat);
	Triangle(Vertex v0, Vertex v1, Vertex v2, Material _mat, int _sg);
};