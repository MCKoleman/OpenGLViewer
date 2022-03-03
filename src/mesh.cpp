#include "mesh.h"

float Mesh::GetSize()
{
	return size;
}

void Mesh::SetSize(float _size)
{
	size = _size;
}

void Mesh::SetPos(glm::vec3 _pos)
{
	pos = _pos;
}

Mesh::Mesh()
{
	pos = glm::vec3();
}

Mesh::~Mesh()
{

}