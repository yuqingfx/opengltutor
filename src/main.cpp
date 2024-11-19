
//#include "check_gl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
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

// Settings
int SRC_WIDTH = 1024;
int SRC_HEIGHT = 768;

// camera 
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.f;
float pitch = 0.0f;
float lastx = (float)SRC_WIDTH  / 2.0f;
float lastY = (float)SRC_HEIGHT / 2.0f;

float MixValue = .5f;
// Load a texture
int width1, height1, nrChannels1, width2, height2, nrChannels2, width3, height3, nrChannels3;
unsigned char* TexData1 = stbi_load(OPENGLTUTOR_HOME "assets/rock.png", &width1, &height1, &nrChannels1, 0);

unsigned char* TexData2 = stbi_load(OPENGLTUTOR_HOME "assets/container.jpg", &width2, &height2, &nrChannels2, 0);

unsigned char* TexData3 = stbi_load(OPENGLTUTOR_HOME "assets/awesomeface.png", &width3, &height3, &nrChannels3, 0);

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

    Shader shaderProgram1(OPENGLTUTOR_HOME "assets/testVert.vert", OPENGLTUTOR_HOME "assets/testFrag.frag");

    Shader shaderProgram2(OPENGLTUTOR_HOME "assets/testVert2.vert", OPENGLTUTOR_HOME "assets/testFrag2.frag");

    Shader shaderProgram3(OPENGLTUTOR_HOME "assets/CubeVert.vert", OPENGLTUTOR_HOME "assets/CubeFrag.frag");

    float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

    float first_triangle[] =
    {
        .5f, .5f, 0.f,
        .5f, -.5f, 0.f,
        -.5f, -.5f, 0.f,
        -.5f, .5f, 0.f
    };

    float first_colors[] =
    {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f,
        1.f, 1.f, 1.f,
    };

    float first_texCoords[] =
    {
        1.f, 1.f,
        1.0f, 0.f,
        0.0f, 0.0f,
        0.f, 1.f
    };

    int first_indices[] =
    {
        0,1,2,3,0,2
    };

    float second_triangle[] =
    {
        // locations  //colors       // tex coords
        -.5f, .5f, 0.f,   1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -.5f, 0.f, 0.f,  0.0f, 1.0f, 0.f,  0.0f, 0.0f,
        0.f, 0.f, 0.f, 0.f, 0.f, 1.f,    0.0f, 1.0f
    };

    int second_indices[] = { 0,1,2 };

    unsigned int VAOs[3], VBOs[3];
    
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(first_triangle), first_triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      
    GLuint ColorsVBO;
    glGenBuffers(1, &ColorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(first_colors), first_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    GLuint TexCoordsVBO;
    glGenBuffers(1, &TexCoordsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(first_texCoords), first_texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    GLuint EBO_first;
    glGenBuffers(1, &EBO_first);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_first);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(first_indices), first_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(second_triangle), second_triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(second_indices), second_indices, GL_STATIC_DRAW);

    // Cube data
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int textures[3];
    glGenTextures(3, textures);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (TexData1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, TexData1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load tex1" << std::endl;
    }
    stbi_image_free(TexData1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    
    if (TexData2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, TexData2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load tex2" << std::endl;
    }
    stbi_image_free(TexData2);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    if (TexData3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexData3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load tex3" << std::endl;
    }
    stbi_image_free(TexData3);

    glEnable(GL_DEPTH_TEST);
    // Start main game loop

    while (!glfwWindowShouldClose(window))
    {
        // Render graphics
        // game->render();
        
        processInput(window);

        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram1.use();
        shaderProgram1.setFloat("InMixValue", MixValue);
        shaderProgram1.setInt("ourTexture", 0);
        shaderProgram1.setInt("ourTexture2", 2);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.f), glm::vec3(1.0f, 0.f, 0.f));
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.f), (float)(SRC_WIDTH / SRC_HEIGHT), .1f, 100.f);

        shaderProgram1.setMat4("model", model);
        shaderProgram1.setMat4("view", view);
        shaderProgram1.setMat4("proj", projection);

        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_first);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::translate(trans2, glm::vec3(-.5f, .5f, 0));
        auto scaler = glm::abs(glm::vec3(glm::sin(glfwGetTime()), glm::sin(glfwGetTime()), glm::sin(glfwGetTime())));
        trans2 = glm::scale(trans2, scaler);
        
        shaderProgram2.use();
        shaderProgram2.setInt("InTexture", 1);
        shaderProgram2.setMat4("transform", trans2);

        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        shaderProgram3.use();
        glm::mat4 view3 = glm::mat4(1.0f);
        glm::mat4 projection3 = glm::mat4(1.0f);
        view3 = glm::translate(view3, glm::vec3(0.0f, 0.0f, -3.0f));
        projection3 = glm::perspective(glm::radians(45.0f), (float)(SRC_WIDTH / SRC_HEIGHT), 0.1f, 100.0f);

        shaderProgram3.setMat4("view", view3);
        shaderProgram3.setMat4("proj", projection3);

        glBindVertexArray(VAOs[2]);
        
        for (unsigned int i = 0; i < 10; i++)
        {

            glm::mat4 model3 = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model3 = glm::translate(model3, cubePositions[i]);

            float angle = 20.f * i;
            model3 = glm::rotate(model3, glm::radians(angle), glm::vec3(1.0f, .3f, .5f));
            
            model3 = glm::rotate(model3, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
            
            shaderProgram3.setMat4("model", model3);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    
    glDeleteProgram(shaderProgram1.ID);
    glDeleteProgram(shaderProgram2.ID);
    
    // game.reset();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        MixValue += .01f;
        if (MixValue >= 1.0f) MixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        MixValue -= .01f;
        if (MixValue <= 0.0f) MixValue = 0.0f;
    }
}

#endif


