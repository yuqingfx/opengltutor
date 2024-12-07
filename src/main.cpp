
//#include "check_gl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include <iostream>
#include "Game.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _WIN32
#include <windows.h>
#include <cstdlib>
#include <clocale>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(std::string InAssetPath);

// Settings
int SRC_WIDTH = 800;
int SRC_HEIGHT = 600;

// camera 
Camera camera(glm::vec3(0.0f, 0.f, 3.f));

float lastx = (float)SRC_WIDTH / 2.0f;
float lasty = (float)SRC_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
glm::vec3 LightPos(.4f, 1.0f, .5f);
glm::vec3 LightColor(.5f, .5f, .5f);
float LightIntensityValue = .8f;
glm::vec3 LightIntensity(LightIntensityValue, LightIntensityValue, LightIntensityValue);
glm::vec3 LightSpecIntensity(1.0f, 1.0f, 1.0f);
int main() {
#ifdef _WIN32
    try {
        // this is to support Unicode in the console
        static UINT oldCCP = GetConsoleOutputCP();
        if (!SetConsoleOutputCP(CP_UTF8)) {
            std::cerr << "warning: failed to chcp 65001 for utf-8 output\n";
        }
        else {
            std::atexit(+[] { SetConsoleOutputCP(oldCCP); });
        }
        static UINT oldCP = GetConsoleCP();
        if (!SetConsoleCP(CP_UTF8)) {
            std::cerr << "warning: failed to chcp 65001 for utf-8 input\n";
        }
        else {
            std::atexit(+[] { SetConsoleCP(oldCP); });
        }
        // this is to support ANSI control characters (e.g. \033[0m)
        static HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hIn != INVALID_HANDLE_VALUE && hOut != INVALID_HANDLE_VALUE) {
            static DWORD oldOutMode = 0;
            static DWORD oldInMode = 0;
            GetConsoleMode(hOut, &oldOutMode);
            GetConsoleMode(hIn, &oldInMode);
            if (SetConsoleMode(hOut, oldOutMode | 0x000C)) {
                std::atexit(+[] { SetConsoleMode(hOut, oldOutMode); });
                if (SetConsoleMode(hIn, oldInMode | 0x0200)) {
                    std::atexit(+[] { SetConsoleMode(hIn, oldInMode); });
                }
            }
        }
        // this is to support Unicode in path name (make Windows API regard char * as UTF-8 instead of GBK)
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale
        // Windows 10 1803 or above required
        std::setlocale(LC_ALL, ".UTF-8");
    }
    catch (...) {
        std::cerr << "warning: failed to set utf-8 locale\n";
    }
#endif

    // Initalize GLFW library
    if (!glfwInit()) {
        const char* errmsg = nullptr;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "failed to initialize GLFW: " << errmsg << '\n';
        return -1;
    }

    // Hint the version required: OpenGL 2.0
    constexpr int version = 20;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version % 10);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif
    if (version >= 33) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    // enable 4x MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Enable transparent framebuffer
    constexpr bool transparent = false;
    if (transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    }

    // Create main window
    constexpr char title[] = "Example";
    
    GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, title, NULL, NULL);

    // Test if window creation succeed
    if (!window) {
        check_gl::opengl_show_glfw_error_diagnose();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Switch to fullscreen mode
    constexpr bool fullscreen = false;
    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            if (mode) {
                glfwSetWindowSize(window, mode->width, mode->height);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                std::cerr << "Entered fullscreen mode: " << mode->width << 'x' << mode->height
                    << " at " << mode->refreshRate << " Hz\n";
            }
        }
    }
    else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            if (mode) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
            }
        }
    }

    // Load glXXX function pointers (only after this you may use OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD failed to load GL functions\n";
        return -1;
    }
    check_gl::opengl_try_enable_debug_message();

    // Print diagnostic information
    std::cerr << "OpenGL version: " << (const char*)glGetString(GL_VERSION) << '\n';

    // Enable V-Sync
    glfwSwapInterval(1);

    Shader CubeShaderProgram(OPENGLTUTOR_HOME "assets/CubeVert.vert", OPENGLTUTOR_HOME "assets/CubeFrag.frag");
    Shader LightSourceShader(OPENGLTUTOR_HOME "assets/lightCube.vert", OPENGLTUTOR_HOME "assets/lightCube.frag");
    float cube_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Cube data
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
     
    // LightSource data
    GLuint LightVAO;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    // Start main game loop

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    unsigned int diffuseMap = loadTexture("assets/container2.png");
    unsigned int specularMap = loadTexture("assets/container2_spec.png");
    unsigned int emissionMap = loadTexture("assets/matrix.jpg");

    while (!glfwWindowShouldClose(window))
    {
        // Render graphics
        // game->render();

        processInput(window);

        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        CubeShaderProgram.use();

        // cube vs stage
        CubeShaderProgram.setVec3("viewPos", camera.Position);

        // cube ps stage
        CubeShaderProgram.setFloat("time", glfwGetTime());

        CubeShaderProgram.setVec3("light.ambient", LightIntensity);
        CubeShaderProgram.setVec3("light.diffuse", LightColor);
        CubeShaderProgram.setVec3("light.specular", LightSpecIntensity);
        CubeShaderProgram.setVec3("light.position", LightPos);

        // only used for directional light
        CubeShaderProgram.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        CubeShaderProgram.setFloat("light.k_constant", 1.0f);
        CubeShaderProgram.setFloat("light.k_linear", 0.09);
        CubeShaderProgram.setFloat("light.k_quadratic", 0.032);

        CubeShaderProgram.setInt("mat.diffuse",  0); 
        CubeShaderProgram.setInt("mat.specular", 1);
        CubeShaderProgram.setInt("mat.emission", 2);
        CubeShaderProgram.setFloat("mat.shininess", 32.f);

        glm::mat4 projection3 = glm::mat4(1.0f);
        projection3 = glm::perspective(glm::radians(camera.Zoom), (float)(SRC_WIDTH / SRC_HEIGHT), 0.1f, 100.0f);
        CubeShaderProgram.setMat4("proj", projection3);
        
        glm::mat4 view3 = camera.GetViewMatrix();
        CubeShaderProgram.setMat4("view", view3);

        // make sure to initialize matrix to identity matrix first
       
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);
       
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model3 = glm::mat4(1.0f);
            model3 = glm::translate(model3, cubePositions[i]);
            float angle = 20.f * i;
            model3 = glm::rotate(model3, glm::radians(angle), glm::vec3(1.0f, .3f, .5f));
            model3 = glm::rotate(model3, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

            CubeShaderProgram.setMat4("model", model3);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // light source
        LightSourceShader.use();

        LightSourceShader.setVec3("lightColor", LightColor);

        glm::mat4 model_LS(1.0f);
        model_LS = glm::translate(model_LS, LightPos);
        model_LS = glm::scale(model_LS, glm::vec3(.2f));
        LightSourceShader.setMat4("model", model_LS);

        glm::mat4 view_LS = camera.GetViewMatrix();
        LightSourceShader.setMat4("view", view_LS);

        glm::mat4 projection_LS(1.0f);
        projection_LS = glm::perspective(glm::radians(camera.Zoom), (float)(SRC_WIDTH / SRC_HEIGHT), 0.1f, 100.0f);
        LightSourceShader.setMat4("projection", projection_LS);

        glBindVertexArray(LightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteProgram(CubeShaderProgram.ID);

    // game.reset();
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastx = xpos;
        lasty = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos;

    lastx = xpos;
    lasty = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    
}
    
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(std::string InAssetPath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    std::string FullAssetPath = std::string(OPENGLTUTOR_HOME) + InAssetPath;
    unsigned char* TexData = stbi_load(FullAssetPath.c_str(), &width, &height, &nrChannels, 0);
    if (TexData)
    {
        GLenum format;
        if (nrChannels == 1)
        {
            format = GL_RED;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, TexData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(TexData);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << FullAssetPath << std::endl;
        stbi_image_free(TexData);
    }

    return textureID;
}
#endif

    
