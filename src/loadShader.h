#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "openGLInclude.h"

std::string ReadShaderFromFile(std::string filename);
unsigned int LoadShaders(std::string vertexFile, std::string fragmentFile);