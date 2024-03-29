#include "options.h"

// Reads the options file at the given position
Options ReadOptions(std::string fileName)
{
    Options options = Options();

    // Open the given file
    std::ifstream file(fileName);
    if (file.is_open())
    {
        // Read each individual line of the file
        std::string line = "";
        while (std::getline(file, line))
        {
            // Store parsed data
            std::vector<std::string> tempParse;
            ParseStringByDelim(tempParse, line, " ");

            // Parse stored data
            // If only 0 or 1 tokens are stored, the line has no significance
            // Only interpret lines with more than 2 tokens, skipping equals sign
            if (tempParse.size() > 2) {
                // Check for comment
                if (tempParse[0] == "#") {
                    // Skip comments
                }
                // Check for object setting
                else if (tempParse[0] == "object") {
                    options.objName = tempParse[2];
                }
                // Check for scale setting
                else if (tempParse[0] == "scale") {
                    options.objScale = std::stof(tempParse[2]);
                }
                // Check for position setting
                else if (tempParse[0] == "position") {
                    options.objPos = ReadVec3FromStrings(tempParse, 1);
                }
                // Check for default color
                else if (tempParse[0] == "dcolor") {
                    options.defaultColor = Material(glm::vec3(0, 0, 0), ReadVec3FromStrings(tempParse, 1));
                }
                // Check for vertex model setting
                else if (tempParse[0] == "vertexmodel") {
                    options.vertexModel = std::stoi(tempParse[2]);
                }
                // Check for wireframe
                else if (tempParse[0] == "wireframe") {
                    options.wireframe = std::stoi(tempParse[2]);
                }
                // Check for print
                else if (tempParse[0] == "print") {
                    options.print = std::stoi(tempParse[2]);
                }
                // Check for phonh
                else if (tempParse[0] == "phong") {
                    options.phong = std::stoi(tempParse[2]);
                }
                // Check for perspective
                else if (tempParse[0] == "perspective") {
                    options.isPerspective = std::stoi(tempParse[2]);
                }
                // Check for camera position setting
                else if (tempParse[0] == "cameraStartPos") {
                    options.camPos = ReadVec3FromStrings(tempParse, 1);
                }
                // Check for camera lookAt setting
                else if (tempParse[0] == "cameraStartLookAt") {
                    options.camLookAt = ReadVec3FromStrings(tempParse, 1);
                }
                // Check for camera up setting
                else if (tempParse[0] == "cameraStartUp") {
                    options.camUp = ReadVec3FromStrings(tempParse, 1);
                }
                // Check for fov
                else if (tempParse[0] == "cameraFov") {
                    options.camFov = std::stof(tempParse[2]);
                }
                // Check for nearClip
                else if (tempParse[0] == "cameraNearClip") {
                    options.camNearClip = std::stof(tempParse[2]);
                }
                // Check for farClip
                else if (tempParse[0] == "cameraFarClip") {
                    options.camFarClip = std::stof(tempParse[2]);
                }
                // Check for orth size
                else if (tempParse[0] == "cameraSize") {
                    if(tempParse.size() >= 4)
                        options.camSize = glm::vec2(std::stof(tempParse[2]), std::stof(tempParse[3]));
                }
            }
        }
        file.close();
    }
    // If the given file could not be found, print an error message
    else
    {
        std::cout << "Error: Attempted to read config at location [" << fileName << "], but no such file exists. Config loading failed." << std::endl;
    }

    return options;
}

SpeedConsts::SpeedConsts(float _camMove, float _camTurn, float _modelMove, float _modelTurn, float _modelScale, float _mouseMove, float _mouseTurn)
    : cameraMoveSpeed(_camMove), cameraTurnSpeed(_camTurn), modelMoveSpeed(_modelMove), modelTurnSpeed(_modelTurn), modelScaleRate(_modelScale),
    mouseMoveSpeed(_mouseMove), mouseTurnSpeed(_mouseTurn) {}