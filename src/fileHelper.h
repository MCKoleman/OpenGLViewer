#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include "mesh.h"
#include "meshModel.h"
#include "openGLInclude.h"
#include "indexStructs.h"

// Storage of all options for the program
struct Options {
	std::string objName;
	float objScale;
	glm::vec3 objPos;
	Material defaultColor;
	int vertexModel;
	int wireframe;
	int print;
};

Options ReadOptions(std::string fileName);
std::string ReadShader(std::string fileName);
void ReadObjFromFile(Mesh* mesh, std::string location, std::string fileName);
void BuildSMesh(SMesh* smesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials, std::vector<IndVertex>& tempVertices);
void BuildIMesh(IMesh* imesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials, std::vector<IndVertex>& tempVertices);
glm::vec3 ReadVec3FromStrings(std::vector<std::string>& strings);
void ParseStringByDelim(std::vector<std::string>& out, std::string str, std::string delim);
void PrintArray(std::string startText, float array[], int n, int elemsPerLine);
void PrintArray(std::string startText, unsigned int[], int n, int elemsPerLine);