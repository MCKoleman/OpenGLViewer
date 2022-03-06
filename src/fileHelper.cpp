#include "fileHelper.h"

// Reads the shader at the given location to the given out string
// Help reading file line by line from:
// https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c
// --------------------------------------------------------------
std::string ReadShader(std::string fileName)
{
    std::string shader = "";

    // Open the given file
    std::ifstream file(fileName);
    if (file.is_open())
    {
        // Read each individual line of the file
        std::string line = "";
        while (std::getline(file, line))
        {
            shader.append(line);
            shader.append("\n");
        }
        shader.append("\0");
        file.close();
    }
    // If the given file could not be found, print an error message
    else
    {
        std::cout << "Error: Attempted to read shader at location [" << fileName << "], but no such file exists. Shader loading failed." << std::endl;
    }

    return shader;
}

// Reads the mesh from the .obj (and potentially .mtl) file at the given location
// ------------------------------------------------------------------------------
void ReadObjFromFile(Mesh* mesh, std::string location, std::string fileName)
{
    std::string objLoc = location + fileName;
    std::string mtlLoc = "";

    // Generate vectors to store all data
    // Vector of vertices, which are a point
    std::vector<IndVec3> vertPosList;
    // Vector of vertex normals, which are a vec3
    std::vector<IndVec3> vertNormList;
    // Vector of vertex textures, which are a vec3
    std::vector<IndVec3> vertTextList;
    // Vector of vertices
    std::vector<IndVertex> builtVertexList;
    // Vector of faces, which are vectors of verts, which are vectors of potentially vert info, texture info, and normal info
    std::vector<FaceData> faceDataList;
    // Map of groups, with name tag and a vector of face indices
    std::unordered_map<std::string, std::vector<int>> tempGroups;
    // Map of materials
    std::unordered_map<std::string, Material> tempMaterials;

    // Read Object
    // -------------

    // Open the object
    std::ifstream objFile(objLoc);
    if (objFile.is_open())
    {
        // Currently selected info
        std::string curGroup = "";
        std::string curMaterial = "";
        int curShadingGroup = 0;

        // Index info
        int curVertIndex = 1;
        int curVertNormIndex = 1;
        int curVertTextIndex = 1;
        int curFaceIndex = 1;

        // Read each individual line of the file
        std::string line = "";
        while (std::getline(objFile, line))
        {
            // Store parsed data
            std::vector<std::string> tempParse;
            ParseStringByDelim(tempParse, line, " ");

            // Parse stored data
            // If only 0 or 1 tokens are stored, the line has no significance
            // Only interpret lines with more than 1 token
            if (tempParse.size() > 1) {
                // Check if line is a comment and skip it
                if (tempParse[0] == "#") {
                    // Skip this line
                }

                // Check if line is a group
                else if (tempParse[0] == "g" || tempParse[0] == "group") {
                    curGroup = tempParse[1];

                    // If the new group doesn't exist yet, add it
                    std::unordered_map<std::string, std::vector<int>>::iterator iter = tempGroups.find(curGroup);
                    if (iter == tempGroups.end()) {
                        std::vector<int> newList;
                        tempGroups.emplace(curGroup, newList);
                    }
                }

                // Check if line is a smoothing group
                else if (tempParse[0] == "s") {
                    // Turn off shading group
                    if (tempParse[1] == "off") {
                        curShadingGroup = -1;
                    }
                    // Otherwise set the shading group number
                    else {
                        curShadingGroup = std::stoi(tempParse[1]);
                    }
                }

                // Check if line is a vertex
                else if (tempParse[0] == "v") {
                    // Don't read an incomplete vertex
                    if (tempParse.size() >= 4) {
                        vertPosList.push_back(IndVec3(curVertIndex, ReadVec3FromStrings(tempParse)));
                        curVertIndex++;
                    }
                }

                // Check if line is a vertex normal
                else if (tempParse[0] == "vn") {
                    // Don't read an incomplete normal
                    if (tempParse.size() >= 4) {
                        vertNormList.push_back(IndVec3(curVertNormIndex, ReadVec3FromStrings(tempParse)));
                        curVertNormIndex++;
                    }
                }

                // Check if line is a vertex texture
                else if (tempParse[0] == "vt") {
                    // Read u
                    float _u = std::stof(tempParse[1]);

                    // Read v (default, 0)
                    float _v = 0;
                    if (tempParse.size() >= 3)
                        _v = std::stof(tempParse[2]);

                    // Read w (default, 0)
                    float _w = 0;
                    if (tempParse.size() >= 4)
                        _w = std::stof(tempParse[3]);

                    // Store results
                    vertTextList.push_back(IndVec3(curVertTextIndex, glm::vec3(_u, _v, _w)));
                    curVertTextIndex++;
                }

                // Check if line is a face
                else if (tempParse[0] == "f") {
                    // Create a temporary face to store info into
                    std::vector<IndVertex> tempFace;

                    // Loop through the parse array and evaluate each vertex
                    for (int i = 1; i < tempParse.size(); i++) {
                        std::vector<std::string> tempFaceData;
                        ParseStringByDelim(tempFaceData, tempParse[i], "/");

                        // Add all vertex data to the face (up to a max of 3 elements: coord, texture, normal)
                        std::vector<int> tempFaceVertData;
                        int vertPosIndex = -1;
                        for (int j = 0; j < std::min((int)tempFaceData.size(), 3); j++) {
                            // If the face vertex data is not empty, store it as an int
                            if (tempFaceData[j] != "") {
                                tempFaceVertData.push_back(std::stoi(tempFaceData[j]));
                            }
                            // If the face vertex data is empty, save it as -1 (no texture/normal)
                            else {
                                tempFaceVertData.push_back(-1);
                            }
                        }

                        //
                        if (tempFaceVertData.size() > 0) {
                            vertPosIndex = tempFaceVertData[0];
                        }

                        // Find vertex
                        int vertIndex = FindVertIndex(builtVertexList, vertPosIndex);
                        if (vertIndex < 0 && vertPosIndex >= 0) {
                            glm::vec3 vertPos = glm::vec3();
                            glm::vec3 vertNorm = glm::vec3();
                            glm::vec3 vertText = glm::vec3();

                            if (tempFaceVertData.size() > 0)
                                vertPos = FindVert(vertPosList, tempFaceVertData[0]).pos;
                            if(tempFaceVertData.size() > 1)
                                vertNorm = FindVert(vertNormList, tempFaceVertData[1]).pos;
                            if (tempFaceVertData.size() > 2)
                                vertText = FindVert(vertTextList, tempFaceVertData[2]).pos;

                            Vertex newVert = Vertex(vertPos, vertNorm, vertText);
                            builtVertexList.push_back(IndVertex(vertPosIndex, newVert));
                        }

                        vertIndex = FindVertIndex(builtVertexList, tempFaceVertData[0]);
                        if(vertIndex >= 0)
                            tempFace.push_back(IndVertex(builtVertexList[vertIndex]));
                    }

                    // Push face to the face storage
                    faceDataList.push_back(FaceData(tempFace, curMaterial, curShadingGroup, curFaceIndex));

                    // Add face index to its group
                    std::unordered_map<std::string, std::vector<int>>::iterator iter = tempGroups.find(curGroup);
                    if (iter != tempGroups.end()) {
                        iter->second.push_back(curFaceIndex);
                    }

                    // Increment face index
                    curFaceIndex++;
                }

                // Check if line is a material
                
                else if (tempParse[0] == "mtllib") {
                    mtlLoc = location + tempParse[1];
                }

                // Check if line picks a material
                else if (tempParse[0] == "usemtl") {
                    curMaterial = tempParse[1];

                    // Add material to all elements in the selected group
                    std::unordered_map<std::string, std::vector<int>>::iterator iter = tempGroups.find(curGroup);
                    if (iter != tempGroups.end()) {
                        // Set the material of each face in the group to the selected material
                        for (int i = 0; i < iter->second.size(); i++) {
                            int faceIndex = FindFaceIndex(faceDataList, iter->second[i]);
                            if(faceIndex >= 0)
                                faceDataList[faceIndex].matName = curMaterial;
                        }
                    }
                }
            }
        }
        objFile.close();
    }
    // If the given file could not be found, print an error message
    else
    {
        std::cout << "Error: Attempted to read mesh at location [" << objLoc << "], but no such file exists." << std::endl;
        return;
    }


    // Read Material
    // -------------

    // Don't read material file if it is not requested
    if (mtlLoc != "") {
        // Open the material
        std::ifstream mtlFile(mtlLoc);
        if (mtlFile.is_open())
        {
            // Store current material information
            std::string curMatName = "";
            Material curMat = Material();

            // Read each individual line of the file
            std::string line = "";
            while (std::getline(mtlFile, line))
            {
                // Store parsed data
                std::vector<std::string> tempParse;
                ParseStringByDelim(tempParse, line, " ");

                // Parse stored data
                // If only 0 or 1 tokens are stored, the line has no significance
                // Only interpret lines with more than 1 token
                if (tempParse.size() > 1) {
                    // Check if line is a comment and skip it
                    if (tempParse[0] == "#") {
                        // Skip this line
                    }

                    // Check if line is newmtl
                    else if (tempParse[0] == "newmtl") {
                        // If the current material is not blank, store it in the map
                        if (curMatName != "") {
                            tempMaterials.emplace(curMatName, curMat);
                        }

                        curMatName = tempParse[1];
                    }

                    // Check if line is Ka
                    else if (tempParse[0] == "Ka") {
                        // Don't read an incomplete vector
                        if (tempParse.size() >= 4) {
                            curMat.ka = ReadVec3FromStrings(tempParse);
                        }
                    }

                    // Check if line is Kd
                    else if (tempParse[0] == "Kd") {
                        // Don't read an incomplete vector
                        if (tempParse.size() >= 4) {
                            curMat.kd = ReadVec3FromStrings(tempParse);
                        }
                    }

                    // Check if line is Ks
                    else if (tempParse[0] == "Ks") {
                        // Don't read an incomplete vector
                        if (tempParse.size() >= 4) {
                            curMat.ks = ReadVec3FromStrings(tempParse);
                        }
                    }

                    // Check if line is Ns
                    else if (tempParse[0] == "Ns") {
                        curMat.ns = std::stof(tempParse[1]);
                    }

                    // Check if line is Ni
                    else if (tempParse[0] == "Ni") {
                        curMat.ni = std::stof(tempParse[1]);
                    }

                    // Check if line is d
                    else if (tempParse[0] == "d") {
                        curMat.d = std::stof(tempParse[1]);
                    }

                    // Check if line is Tr
                    else if (tempParse[0] == "Tr") {
                        curMat.d = 1 - std::stof(tempParse[1]);
                    }
                }
            }
            tempMaterials.emplace(curMatName, curMat);
            mtlFile.close();
        }
        // If the given file could not be found, print an error message
        else
        {
            std::cout << "Error: Attempted to read material at location [" << mtlLoc << "], but no such file exists." << std::endl;
        }
    }

    // Construct mesh
    // --------------
    //vector<IndVec3> tempVertPositions;
    //vector<IndVec3> tempVertNorms;
    //vector<IndVec3> tempVertTexts;
    //vector<IndVertex> tempVertices;
    //vector<FaceData> tempFaces;
    //unordered_map<string, vector<int>> tempGroups;
    //unordered_map<string, Material> tempMaterials;

    // Create default material (Grey lambert)
    Material defaultMaterial = Material(glm::vec3(0, 0, 0), glm::vec3(0.8f, 0.8f, 0.8f));

    // Clear previous mesh
    mesh->ClearTris();

    // Build faces from vertices
    // Build indexed mesh
    if (mesh->GetVertexModel() == 1) {
        IMesh* imesh = static_cast<IMesh*>(mesh);
        BuildIMesh(imesh, faceDataList, tempMaterials, builtVertexList);
    }
    // Build separate mesh
    else {
        SMesh* smesh = static_cast<SMesh*>(mesh);
        BuildSMesh(smesh, faceDataList, tempMaterials, builtVertexList);
    }

    // Find the maximum size of vertices
    float maxSize = INT_MIN;
    for (int i = 0; i < vertPosList.size(); i++) {
        if (vertPosList[i].pos.x > maxSize)
            maxSize = vertPosList[i].pos.x;
        if (vertPosList[i].pos.y > maxSize)
            maxSize = vertPosList[i].pos.y;
        if (vertPosList[i].pos.z > maxSize)
            maxSize = vertPosList[i].pos.z;
    }

    mesh->SetSize(1 / maxSize);

    std::cout << "Successfully read object: " << fileName << "." << std::endl;
    std::cout << "Faces: [" << faceDataList.size() << "], Tris: [" << mesh->GetTriCount() << "], Verts: [" << builtVertexList.size() << "]" << std::endl;
}

// Builds a separate triangle structure mesh from the provided data
void BuildSMesh(SMesh* smesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials, std::vector<IndVertex>& tempVertices)
{
    // Build faces from vertices
    for (int faceIndex = 0; faceIndex < tempFaces.size(); faceIndex++) {
        FaceData faceData = tempFaces[faceIndex];
        Material currentMat = smesh->defaultMat;
        std::vector<Vertex> faceVertices;

        for (int vertexIndex = 0; vertexIndex < faceData.vertexInfo.size(); vertexIndex++) {
            IndVertex vertex = faceData.vertexInfo[vertexIndex];

            // Create new vertex
            faceVertices.push_back(vertex.ver);
        }

        // Get face material
        std::unordered_map<std::string, Material>::iterator iter = tempMaterials.find(faceData.matName);
        if (iter != tempMaterials.end()) {
            currentMat = iter->second;
        }

        // If the face is not an ngon, create a triangle from it
        if (faceVertices.size() == 3) {
            smesh->AddTri(STriangle(faceVertices[0], faceVertices[1], faceVertices[2], currentMat, faceData.shadingGroup));
        }
        // If the face is a quad, split it into two tris
        else if (faceVertices.size() == 4) {
            smesh->AddTri(STriangle(faceVertices[0], faceVertices[1], faceVertices[2], currentMat, faceData.shadingGroup));
            smesh->AddTri(STriangle(faceVertices[2], faceVertices[1], faceVertices[3], currentMat, faceData.shadingGroup));
        }
        // If the face is an ngon, apply polygon triangulation and then add the tris to the mesh
        else if (faceVertices.size() > 4) {
            // Simple triangulation model that does not take into account ngon shape or manifoldness
            for (int i = 0; i < faceVertices.size() - 2; i++) {
                // Attempt to alternate indexing of ngons
                if(i % 2 == 0)
                    smesh->AddTri(STriangle(faceVertices[i], faceVertices[i + 1], faceVertices[i + 2], currentMat, faceData.shadingGroup));
                else
                    smesh->AddTri(STriangle(faceVertices[i + 1], faceVertices[i], faceVertices[i + 2], currentMat, faceData.shadingGroup));
            }
        }
        // Skip lines and points as they cannot be rendered
    }
}

// Builds an indexed triangle mesh from the provided data
void BuildIMesh(IMesh* imesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials, std::vector<IndVertex>& tempVertices)
{
    // Build faces from vertices
    for (int faceIndex = 0; faceIndex < tempFaces.size(); faceIndex++) {
        FaceData faceData = tempFaces[faceIndex];
        Material currentMat = imesh->defaultMat;
        std::vector<IndVertex> faceVertices;

        for (int vertexIndex = 0; vertexIndex < faceData.vertexInfo.size(); vertexIndex++) {
            IndVertex vertex = faceData.vertexInfo[vertexIndex];

            faceVertices.push_back(vertex);
            imesh->AddVert(vertex.id, vertex.ver);
        }

        // Get face material
        std::unordered_map<std::string, Material>::iterator iter = tempMaterials.find(faceData.matName);
        if (iter != tempMaterials.end()) {
            currentMat = iter->second;
        }

        // If the face is not an ngon, create a triangle from it
        if (faceVertices.size() == 3) {
            imesh->AddTri(ITriangle(faceVertices[0].id, faceVertices[1].id, faceVertices[2].id, currentMat, faceData.shadingGroup));
        }
        // If the face is a quad, split it into two tris
        else if (faceVertices.size() == 4) {
            imesh->AddTri(ITriangle(faceVertices[0].id, faceVertices[1].id, faceVertices[2].id, currentMat, faceData.shadingGroup));
            imesh->AddTri(ITriangle(faceVertices[2].id, faceVertices[1].id, faceVertices[3].id, currentMat, faceData.shadingGroup));
        }
        // If the face is an ngon, apply polygon triangulation and then add the tris to the mesh
        else if (faceVertices.size() > 4) {
            // Simple triangulation model that does not take into account ngon shape or manifoldness
            for (int i = 0; i < faceVertices.size() - 2; i++) {
                // Attempt to alternate indexing of ngons
                if (i % 2 == 0)
                    imesh->AddTri(ITriangle(faceVertices[i].id, faceVertices[i + 1].id, faceVertices[i + 2].id, currentMat, faceData.shadingGroup));
                else
                    imesh->AddTri(ITriangle(faceVertices[i + 1].id, faceVertices[i].id, faceVertices[i + 2].id, currentMat, faceData.shadingGroup));
            }
        }
        // Skip lines and points as they cannot be rendered
    }
}

// Help parsing a string by token from:
// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
void ParseStringByDelim(std::vector<std::string>& out, std::string str, std::string delim)
{
    // Make sure out vec is cleared
    out.clear();

    // Parse each part of the string
    size_t spos = 0;
    std::string token;
    std::string tempStr = str;
    while ((spos = tempStr.find(delim)) != std::string::npos) {
        // Parse token
        token = tempStr.substr(0, spos);
        out.push_back(token);
        tempStr.erase(0, spos + delim.length());
    }
    out.push_back(tempStr);
}

// Reads a vector 3 from the given strings. Strings must have size >= 4 to be read, and the first element is ignored
glm::vec3 ReadVec3FromStrings(std::vector<std::string>& strings)
{
    // Avoid errors
    if (strings.size() >= 4)
        return glm::vec3(std::stof(strings[1]), std::stof(strings[2]), std::stof(strings[3]));
    else
        return glm::vec3();
}

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
                    // Only read position if there are more than 5 elements (pos = x y z)
                    if (tempParse.size() >= 5) {
                        options.objPos = glm::vec3(std::stof(tempParse[2]), std::stof(tempParse[3]), std::stof(tempParse[4]));
                    }
                    else {
                        std::cout << "Options file does not have a valid position. Defaulting to 0" << std::endl;
                    }
                }
                // Check for default color
                else if (tempParse[0] == "dcolor") {
                    options.defaultColor = Material(glm::vec3(0, 0, 0), glm::vec3(std::stof(tempParse[2]), std::stof(tempParse[3]), std::stof(tempParse[4])));
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

// Prints the given array
void PrintArray(std::string startText, float arr[], int n, int elemsPerLine)
{
    using namespace std;
    cout << startText << endl;
    cout << "Size: " << n << endl;

    cout << fixed << setprecision(3);

    // Print every element in the array
    for (int i = 0; i < n; i++) {
        // Print index on first line
        if (i % elemsPerLine == 0)
            cout << "[" << (int)(i / 6) << "] ";

        // Print element
        cout << "(" << arr[i] << ")";

        // Swap rows every elemsPerLine
        if ((i + 1) % elemsPerLine == 0)
            cout << endl;
        // Otherwise separate lines
        else
            cout << ", ";
    }

    // Finish with newlines
    cout << endl << endl;
}

// Prints the given array
void PrintArray(std::string startText, unsigned int arr[], int n, int elemsPerLine)
{
    using namespace std;
    cout << startText << endl;
    cout << "Size: " << n << endl;

    // Print every element in the array
    for (int i = 0; i < n; i++) {
        // Print index on first line
        if (i % elemsPerLine == 0)
            cout << "[" << (int)(i / 6) << "] ";

        // Print element
        cout << "(" << arr[i] << ")";

        // Swap rows every elemsPerLine
        if ((i + 1) % elemsPerLine == 0)
            cout << endl;
        // Otherwise separate lines
        else
            cout << ", ";
    }

    // Finish with newlines
    cout << endl << endl;
}