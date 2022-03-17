#include "input.h"

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
    // Scale up
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        return true;
    }
    // Scale down
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        return true;
    }
    // Rotate right
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        return true;
    }
    // Rotate left
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        return true;
    }
    // Mouse press
    else if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        return true;
    }
    // If nothing else was pressed, do nothing
    else {
        return false;
    }
}