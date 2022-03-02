#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include "mesh.h"
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

std::string ReadShader(std::string fileName);
void ReadObjFromFile(Mesh* mesh, std::string location, std::string fileName);
glm::vec3 ReadVec3FromStrings(std::vector<std::string>& strings);
void ParseStringByDelim(std::vector<std::string>& out, std::string str, std::string delim);