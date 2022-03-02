#include "mesh.h"

void Mesh::SetSize(float _size)
{
	size = _size;
}

void Mesh::SetPos(glm::vec3 _pos)
{
	pos = _pos;
}

void Mesh::AddTri(Triangle _tri)
{
	tris.push_back(_tri);
}

void Mesh::SetTri(size_t index, Triangle _tri)
{
	tris[index] = _tri;
}

Triangle Mesh::GetTri(size_t index)
{
	return tris[index];
}

int Mesh::GetVertCount()
{
	return tris.size() * 3;
}

void Mesh::ConvertToVertData(float out[])
{
	// Track out indices separate from loop
	int outIndex = 0;
	for (int i = 0; i < tris.size(); i++) {
		for (int j = 0; j < TRI_VERTS; j++) {
			glm::vec3 vertPos = tris[i].vertices[j].pos * size;
			out[outIndex] = vertPos.x + pos.x;
			outIndex++;
			out[outIndex] = vertPos.y + pos.y;
			outIndex++;
			out[outIndex] = vertPos.z + pos.z;
			outIndex++;
		}
	}
}

void Mesh::ClearTris() {
	tris.clear();
}

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	tris.clear();
}