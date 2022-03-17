#include "OpenGLViewer.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Kolehmainen OpenGL Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();

    // build and compile our shader program
    // ------------------------------------
    unsigned int shaderProgram = LoadShaders("../src/source.vs", "../src/source.fs");

    // Read Options
    // ------------
    Options options = ReadOptions("../config.txt");
    # define INDEXED options.vertexModel == 1

    // Create camera
    Camera camera = Camera(
        options.camFov, options.camNearClip, options.camFarClip, 
        options.camPos, options.camLookAt, options.camUp, 
        options.camSize, options.isPerspective);

    // Read mesh
    // ---------
    IMesh* imesh;
    SMesh* smesh;
    Mesh* displayMesh;

    // Cast the mesh
    if (INDEXED) {
        imesh = new IMesh();
        displayMesh = imesh;
    }
    else {
        smesh = new SMesh();
        displayMesh = smesh;
    }

    // Set default color
    displayMesh->defaultMat = options.defaultColor;

    // Read mesh from file
    ReadObjFromFile(displayMesh, "../models/", options.objName);
    displayMesh->SetSize(options.objScale * displayMesh->GetSize());
    displayMesh->SetPos(options.objPos);
    
    // Load up model into vertice and indice structures
    int vertsSize;
    float* vertices;
    int indicesSize;
    unsigned int* indices;
    unsigned int numVertices;

    // Indexed triangle structure
    if (INDEXED) {
        // Get vertices
        vertsSize = imesh->GetVertCount() * 2;
        vertices = new float[vertsSize];
        imesh->ConvertToVertData(vertices);

        // Get indices
        indicesSize = imesh->GetIndexCount();
        indices = new unsigned int[indicesSize];
        imesh->ConvertToIndexData(indices);

        numVertices = imesh->GetVertCount();
    }
    // Separate triangle structure
    else {
        // Get vertices
        vertsSize = smesh->GetVertCount() * 6;
        vertices = new float[vertsSize];
        smesh->ConvertToVertData(vertices);

        numVertices = smesh->GetVertCount();
    }

    // Print vertices and indices
    if (options.print == 1) {
        PrintArray("Printing vertices:", vertices, vertsSize, 6);
        if (INDEXED)
            PrintArray("Printing indices:", indices, indicesSize, 6);
    }

    // Init VAO, VBO, and EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertsSize * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

    // Only use EBO for indexed vertex model
    if (INDEXED) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(indices[0]), indices, GL_STATIC_DRAW);
    }

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Get MVP location
    unsigned int matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glm::mat4 mvp = CalcMVP(camera, displayMesh);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // Enable culling
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    // Enable wireframe if requested in options
    if (options.wireframe == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        if (processInput(window)) {
            // Update window
        }

        // render
        // ------
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the object
        glUseProgram(shaderProgram);

        // Apply MVP
        mvp = CalcMVP(camera, displayMesh);
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(VAO);
        
        // Draw indexed EBO
        if (INDEXED) {
            glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
            //glBindVertexArray(0);
        }
        // Draw separate VAO
        else {
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }


        // glBindVertexArray(0); // unbind our VA no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Clear up dynamic memory usage
    // -----------------------------
    delete[] vertices;

    // Only delete what was initialized
    if (INDEXED) {
        glDeleteBuffers(1, &EBO);
        delete imesh;
        delete[] indices;
    }
    else {
        delete smesh;
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Calculates the model view perspective matrix
// --------------------------------------------
glm::mat4 CalcMVP(Camera camera, Mesh* mesh)
{
    // Projection
    glm::mat4 projection = glm::mat4(1.0f);

    // Perspective projection
    if (camera.isPerspective) {
        projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, camera.nearClip, camera.farClip);
    }
    // Orthographic projection
    else {
        glm::mat4 projection = glm::ortho(-camera.orthSize.x, camera.orthSize.x, -camera.orthSize.y, camera.orthSize.y, camera.nearClip, camera.farClip);
    }

    // Camera view
    glm::mat4 view = glm::lookAt(camera.pos, camera.lookAt, camera.up);

    // Model position
    glm::mat4 model = glm::translate(glm::mat4(1.0f), mesh->GetPos());

    return projection * view * model;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}