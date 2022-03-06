#pragma once
#include "triangle.h"
#include "material.h"
#include <vector>
#include <unordered_map>

class Mesh {
protected:
	float size = 1.0f;
	glm::vec3 pos;
public:
	Material defaultMat = Material();

	float GetSize();

	// Sets the size of the mesh to the given size
	void SetSize(float _size);
	
	// Sets the position of the mesh to the given position
	void SetPos(glm::vec3 _pos);
	
	// Returns the size of the mesh if it was converted into a vertex array (needed for allocating memory for ConvertToVertData())
	virtual int GetVertCount() = 0;

	// Converts this mesh into a vertex array and stores it in the given array
	virtual void ConvertToVertData(float out[]) = 0;

	// Clears the mesh of all tris
	virtual void ClearTris() = 0;

	// Gets the number of tris in the mesh
	virtual int GetTriCount() = 0;

	// Vertex model of the mesh [0 for separate, 1 for indexed]
	virtual int GetVertexModel() = 0;

	Mesh();
	~Mesh();
};