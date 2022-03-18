#include "input.h"

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
bool processInput(GLFWwindow* window, Camera* camera, Mesh* mesh, 
    float deltaTime, float mouseMoveSpeed, float mouseTurnSpeed, float moveSpeed, float turnSpeed, float scaleRate,
    int* prevX, int* prevY)
{
    bool didReceiveInput = false;

    // Get window width and height
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double halfWidth = width * 0.5f;
    double halfHeight = height * 0.5f;

    // Exit window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return true;
    }

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->pos += camera->dir * deltaTime * moveSpeed;
        didReceiveInput = true;
    }
    // Move back
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->pos -= camera->dir * deltaTime * moveSpeed;
        didReceiveInput = true;
    }
    // Move right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->pos += camera->right * deltaTime * moveSpeed;
        didReceiveInput = true;
    }
    // Move left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->pos -= camera->right * deltaTime * moveSpeed;
        didReceiveInput = true;
    }
    // Move up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->pos += camera->up * deltaTime * moveSpeed;
        didReceiveInput = true;
    }
    // Move down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        camera->pos -= camera->up * deltaTime * moveSpeed;
        didReceiveInput = true;
    }

    // Scale up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        mesh->SetScale(mesh->GetScale() + glm::vec3(scaleRate * deltaTime) * mesh->GetScale());
        didReceiveInput = true;
    }
    // Scale down
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        mesh->SetScale(mesh->GetScale() - glm::vec3(scaleRate * deltaTime) * mesh->GetScale());
        didReceiveInput = true;
    }

    // Rotate up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        didReceiveInput = true;
        camera->rotation.y += turnSpeed * deltaTime;
    }
    // Rotate down
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        didReceiveInput = true;
        camera->rotation.y -= turnSpeed * deltaTime;
    }
    // Rotate right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        didReceiveInput = true;
        camera->rotation.x += turnSpeed * deltaTime;
    }
    // Rotate left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        didReceiveInput = true;
        camera->rotation.x -= turnSpeed * deltaTime;
    }

    // Mouse input
    if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
        && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS 
            || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
        // Track mouse movement
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        int xPos = (int)glm::floor(mouseX);
        int yPos = (int)glm::floor(mouseY);

        // Reset previous positions
        if (*prevX < 0)
            *prevX = xPos;
        if (*prevY < 0)
            *prevY = yPos;

        float deltaX = float(*prevX - xPos);
        float deltaY = float(*prevY - yPos);

        // Alt + LMB to rotate
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // Apply changes to camera
            camera->rotation.x += mouseTurnSpeed * deltaTime * deltaX;
            camera->rotation.y += mouseTurnSpeed * deltaTime * deltaY;
        }
        // Alt + RMB to move
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            // Apply changes to camera
            camera->pos += camera->right * mouseMoveSpeed * deltaTime * deltaX;
            camera->pos += camera->up * mouseMoveSpeed * deltaTime * deltaY;
        }

        // Keep mouse where it was clicked
        glfwSetCursorPos(window, (float)(*prevX), (float)(*prevY));
        didReceiveInput = true;
    }
    // Reset previous click location when there 
    else {
        *prevX = -1;
        *prevY = -1;
    }

    // Recalculate camera basis if any input was received
    if(didReceiveInput)
        camera->CalcBasis();

    return didReceiveInput;
}