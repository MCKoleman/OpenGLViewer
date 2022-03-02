#pragma once
#include "triangle.h"
#include "material.h"
#include <vector>

class Mesh {
	std::vector<Triangle> tris;
	float size = 1.0f;
	glm::vec3 pos;
public:
	// Sets the size of the mesh to the given size
	void SetSize(float _size);
	
	// Sets the position of the mesh to the given position
	void SetPos(glm::vec3 _pos);

	// Adds the given tri to this mesh
	void AddTri(Triangle _tri);
	
	// Sets the tri at the given index to the given tri
	void SetTri(size_t index, Triangle _tri);
	
	// Returns the tri at the given index
	Triangle GetTri(size_t index);
	
	// Returns the size of the mesh if it was converted into a vertex array (needed for allocating memory for ConvertToVertData())
	int GetVertCount();

	// Converts this mesh into a vertex array and stores it in the given array
	void ConvertToVertData(float out[]);

	// Clears the mesh of all tris
	void ClearTris();

	Mesh();
	~Mesh();
};