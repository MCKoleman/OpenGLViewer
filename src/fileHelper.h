#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include "mesh.h"
#include "meshModel.h"
#include "openGLInclude.h"

// Data needed for constructing a face
struct FaceData {
	std::vector<std::vector<int>> vertexInfo;
	std::string matName;
	int shadingGroup;

	FaceData() : matName(""), shadingGroup(-1) {}
	FaceData(std::vector<std::vector<int>>& _verts) : matName(""), vertexInfo(_verts), shadingGroup(-1) {}
	FaceData(std::vector<std::vector<int>>& _verts, std::string _mat) : matName(_mat), vertexInfo(_verts), shadingGroup(-1) {}
	FaceData(std::vector<std::vector<int>>& _verts, std::string _mat, int _sg) : matName(_mat), vertexInfo(_verts), shadingGroup(_sg) {}
};

// A string and an int. A strint, if you will
struct Strint {
	std::string str;
	int id;

	Strint() : id(-1), str("") {}
	Strint(int _id) : id(_id), str("") {}
	Strint(int _id, std::string _str) : id(_id), str(_str) {}
};

// An indexed vertex. Used to facilitate indexed triangle structures
struct IndVertex {
	Vertex ver;
	int id;

	IndVertex() : id(-1), ver(Vertex()) {}
	IndVertex(int _id) : id(_id), ver(Vertex()) {}
	IndVertex(int _id, Vertex _ver) : id(_id), ver(_ver) {}
};

// Storage of all options for the program
struct Options {
	std::string objName;
	float objScale;
	glm::vec3 objPos;
	Material defaultColor;
	int vertexModel;
	int wireframe;
};

Options ReadOptions(std::string fileName);
std::string ReadShader(std::string fileName);
void ReadObjFromFile(Mesh* mesh, std::string location, std::string fileName);
void BuildSMesh(SMesh* smesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials,
	std::vector<glm::vec3>& tempVerts, std::vector<glm::vec3>& tempVertNorms, std::vector<glm::vec3>& tempVertTexts);
void BuildIMesh(IMesh* imesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials,
	std::vector<glm::vec3>& tempVerts, std::vector<glm::vec3>& tempVertNorms, std::vector<glm::vec3>& tempVertTexts);
glm::vec3 ReadVec3FromStrings(std::vector<std::string>& strings);
void ParseStringByDelim(std::vector<std::string>& out, std::string str, std::string delim);