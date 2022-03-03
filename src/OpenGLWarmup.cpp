#include "OpenGLWarmup.h"

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
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


    // vertex shader
    // -------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Load vertex shader from source.vs
    std::string tempVertShader = ReadShader("../src/source.vs");
    char* vertexShaderFile = new char[tempVertShader.size()+1];
    strcpy(vertexShaderFile, tempVertShader.c_str());

    glShaderSource(vertexShader, 1, &vertexShaderFile, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // --------------------
    // End of vertex shader


    // fragment shader
    // ---------------
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load fragment shader from source.fs
    std::string tempFragShader = ReadShader("../src/source.fs");
    char* fragmentShaderFile = new char[tempFragShader.size() + 1];
    strcpy(fragmentShaderFile, tempFragShader.c_str());

    glShaderSource(fragmentShader, 1, &fragmentShaderFile, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // ----------------------
    // End of fragment shader


    // link shaders
    // ------------
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // ----------------------
    // End of linking shaders


    // Read Options
    // ------------
    Options options = ReadOptions("../config.txt");

    // Read mesh
    // ---------
    IMesh* imesh;
    SMesh* smesh;
    Mesh* displayMesh;

    // Cast the mesh
    if (options.vertexModel == 1) {
        imesh = new IMesh();
        displayMesh = imesh;
    }
    else {
        smesh = new SMesh();
        displayMesh = smesh;
    }

    // Set default color
    displayMesh->defaultMat = options.defaultColor;

    ReadObjFromFile(displayMesh, "../models/", options.objName);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /*
    float vertices[] = {
        -0.5f, -.25f, 0.0f, // left  
         0.5f, -.75f, 0.0f, // right 
         0.0f,  0.5f, 0.0f, // top   

        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f, -1.0f, 0.0f  // bottom
    };
    */

    displayMesh->SetSize(options.objScale * displayMesh->GetSize());
    displayMesh->SetPos(options.objPos);
    
    // Load up model into vertice and indice structures
    int vertsSize;
    float* vertices;
    int indicesSize;
    unsigned int* indices;
    unsigned int numVertices;

    // Indexed triangle structure
    if (options.vertexModel == 1) {
        // Get vertices
        vertsSize = imesh->GetVertCount() * 6;
        vertices = new float[vertsSize];
        imesh->ConvertToVertData(vertices);

        // Get indices
        indicesSize = imesh->GetIndexCount() * 3;
        indices = new unsigned int[indicesSize];
        imesh->ConvertToIndexData(indices);

        numVertices = displayMesh->GetVertCount();
    }
    // Separate triangle structure
    else {
        // Get vertices
        vertsSize = smesh->GetVertCount() * 6;
        vertices = new float[vertsSize];
        smesh->ConvertToVertData(vertices);

        numVertices = displayMesh->GetVertCount();
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertsSize, vertices, GL_STATIC_DRAW);

    // Only use EBO for indexed vertex model
    if (options.vertexModel == 1) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    }

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
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

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
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
    delete[] vertexShaderFile;
    delete[] fragmentShaderFile;
    delete[] vertices;

    // Only delete what was initialized
    if (options.vertexModel == 1) {
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
bool processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return true;
    }
    // Move forward
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        return true;
    }
    // Move back
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        return true;
    }
    // Move right
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        return true;
    }
    // Move left
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        return true;
    }
    // Move up
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        return true;
    }
    // Move down
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        return true;
    }
    // If nothing else was pressed, do nothing
    else {
        return false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}