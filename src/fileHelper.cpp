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
    std::vector<glm::vec3> tempVerts;
    // Vector of vertex normals, which are a vec3
    std::vector<glm::vec3> tempVertNorms;
    // Vector of vertex textures, which are a vec3
    std::vector<glm::vec3> tempVertTexts;
    // Vector of faces, which are vectors of verts, which are vectors of potentially vert info, texture info, and normal info
    std::vector<FaceData> tempFaces;
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
        int curFaceIndex = 0;

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
                        tempVerts.push_back(ReadVec3FromStrings(tempParse));
                    }
                }

                // Check if line is a vertex normal
                else if (tempParse[0] == "vn") {
                    // Don't read an incomplete normal
                    if (tempParse.size() >= 4) {
                        tempVertNorms.push_back(ReadVec3FromStrings(tempParse));
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
                    tempVertTexts.push_back(glm::vec3(_u, _v, _w));
                }

                // Check if line is a face
                else if (tempParse[0] == "f") {
                    // Create a temporary face to store info into
                    std::vector<std::vector<int>> tempFace;

                    // Loop through the parse array and evaluate each vertex
                    for (int i = 1; i < tempParse.size(); i++) {
                        std::vector<std::string> tempFaceData;
                        ParseStringByDelim(tempFaceData, tempParse[i], "/");

                        // Add all vertex data to the face (up to a max of 3 elements: coord, texture, normal)
                        std::vector<int> tempFaceVertData;
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
                        tempFace.push_back(tempFaceVertData);
                    }

                    // Push face to the face storage
                    tempFaces.push_back(FaceData(tempFace, curMaterial, curShadingGroup));

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
                            tempFaces[iter->second[i]].matName = curMaterial;
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
    //vector<vec3> tempVerts;
    //vector<vec3> tempVertNorms;
    //vector<vec3> tempVertTexts;
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
        BuildIMesh(imesh, tempFaces, tempMaterials, tempVerts, tempVertNorms, tempVertTexts);
    }
    // Build separate mesh
    else {
        SMesh* smesh = static_cast<SMesh*>(mesh);
        BuildSMesh(smesh, tempFaces, tempMaterials, tempVerts, tempVertNorms, tempVertTexts);
    }

    // Find the maximum size of vertices
    float maxSize = INT_MIN;
    for (int i = 0; i < tempVerts.size(); i++) {
        if (tempVerts[i].x > maxSize)
            maxSize = tempVerts[i].x;
        if (tempVerts[i].y > maxSize)
            maxSize = tempVerts[i].y;
        if (tempVerts[i].z > maxSize)
            maxSize = tempVerts[i].z;
    }

    mesh->SetSize(1 / maxSize);

    std::cout << "Successfully read object: " << fileName << "." << std::endl;
}

// Builds a separate triangle structure mesh from the provided data
void BuildSMesh(SMesh* smesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials,
    std::vector<glm::vec3>& tempVerts, std::vector<glm::vec3>& tempVertNorms, std::vector<glm::vec3>& tempVertTexts)
{
    // Build faces from vertices
    for (int faceIndex = 0; faceIndex < tempFaces.size(); faceIndex++) {
        FaceData faceData = tempFaces[faceIndex];
        Material currentMat = smesh->defaultMat;
        std::vector<Vertex> faceVertices;

        for (int vertexIndex = 0; vertexIndex < faceData.vertexInfo.size(); vertexIndex++) {
            std::vector<int> vertex = faceData.vertexInfo[vertexIndex];

            // Store vertex info
            glm::vec3 vertCoord = glm::vec3();
            glm::vec3 vertNorm = glm::vec3();
            glm::vec3 vertText = glm::vec3();

            // Collect info
            for (int infoIndex = 0; infoIndex < vertex.size(); infoIndex++) {
                // Skip invalid indices
                int tempIndex = vertex[infoIndex] - 1;
                if (tempIndex < 0)
                    continue;

                // Get vertex information from the respective arrays
                if (infoIndex == 0) {
                    vertCoord = tempVerts[tempIndex];
                }
                else if (infoIndex == 1) {
                    vertText = tempVertTexts[tempIndex];
                }
                else if (infoIndex == 2) {
                    vertNorm = tempVertNorms[tempIndex];
                }
            }

            // Create new vertex
            faceVertices.push_back(Vertex(vertCoord, vertNorm, vertText));
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
            smesh->AddTri(STriangle(faceVertices[2], faceVertices[3], faceVertices[0], currentMat, faceData.shadingGroup));
        }
        // If the face is an ngon, apply polygon triangulation and then add the tris to the mesh
        else if (faceVertices.size() > 4) {
            // Simple triangulation model that does not take into account ngon shape or manifoldness
            for (int i = 1; i < faceVertices.size() - 1; i++) {
                smesh->AddTri(STriangle(faceVertices[0], faceVertices[i], faceVertices[i + 1], currentMat, faceData.shadingGroup));
            }
        }
        // Skip lines and points as they cannot be rendered
    }
}

// Builds an indexed triangle mesh from the provided data
void BuildIMesh(IMesh* imesh, std::vector<FaceData>& tempFaces, std::unordered_map<std::string, Material>& tempMaterials,
    std::vector<glm::vec3>& tempVerts, std::vector<glm::vec3>& tempVertNorms, std::vector<glm::vec3>& tempVertTexts)
{
    // Keep a top level dictionary of all vertex definitions
    std::vector<IndVertex> vertexDefs;

    // Build faces from vertices
    for (int faceIndex = 0; faceIndex < tempFaces.size(); faceIndex++) {
        FaceData faceData = tempFaces[faceIndex];
        Material currentMat = imesh->defaultMat;
        std::vector<IndVertex> faceVertices;

        for (int vertexIndex = 0; vertexIndex < faceData.vertexInfo.size(); vertexIndex++) {
            std::vector<int> vertex = faceData.vertexInfo[vertexIndex];

            // Store vertex info
            glm::vec3 vertCoord = glm::vec3();
            glm::vec3 vertNorm = glm::vec3();
            glm::vec3 vertText = glm::vec3();

            // Collect info
            int tempVertCoord = -1;
            for (int infoIndex = 0; infoIndex < vertex.size(); infoIndex++) {
                // Skip invalid indices
                int tempIndex = vertex[infoIndex] - 1;
                if (tempIndex < 0)
                    continue;

                // Get vertex information from the respective arrays
                if (infoIndex == 0) {
                    vertCoord = tempVerts[tempIndex];
                    tempVertCoord = tempIndex;
                }
                else if (infoIndex == 1) {
                    vertText = tempVertTexts[tempIndex];
                }
                else if (infoIndex == 2) {
                    vertNorm = tempVertNorms[tempIndex];
                }
            }

            // Create new vertex
            IndVertex iVer = IndVertex(tempVertCoord, Vertex(vertCoord, vertNorm, vertText));
            faceVertices.push_back(iVer);
            vertexDefs.push_back(iVer);
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
            imesh->AddTri(ITriangle(faceVertices[2].id, faceVertices[3].id, faceVertices[0].id, currentMat, faceData.shadingGroup));
        }
        // If the face is an ngon, apply polygon triangulation and then add the tris to the mesh
        else if (faceVertices.size() > 4) {
            // Simple triangulation model that does not take into account ngon shape or manifoldness
            for (int i = 1; i < faceVertices.size() - 1; i++) {
                imesh->AddTri(ITriangle(faceVertices[0].id, faceVertices[i].id, faceVertices[i + 1].id, currentMat, faceData.shadingGroup));
            }
        }
        // Skip lines and points as they cannot be rendered
    }

    // Fill up vertex definition of mesh
    for (int i = 0; i < vertexDefs.size(); i++) {
        imesh->AddVert(vertexDefs[i].id, vertexDefs[i].ver);
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