#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"uniform mat4 model;" 
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 fragmentColor;\n"
"void main()\n"
"{\n"
"    fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

float pitch = 0.0f; 
float yaw = -90.0f;
const unsigned int window_width = 1000;
const unsigned int window_height = 800;
bool first_window_enter = true;

//kamera
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static float previousX = static_cast<float>(window_width) / 2.0f;
    static float previousY = static_cast<float>(window_height) / 2.0f;

    if (first_window_enter) {
        previousX = xpos;
        previousY = ypos;
        first_window_enter = false;
    }

    float xdifference = xpos - previousX;
    float ydifference = previousY - ypos;
    previousX = xpos;
    previousY = ypos;

    const float sensitivity = 0.075f;
    xdifference *= sensitivity;
    ydifference *= sensitivity;

    yaw += xdifference;
    pitch += ydifference;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 cameraFront_new;
    cameraFront_new.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront_new.y = sin(glm::radians(pitch));
    cameraFront_new.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront_new);
}

void renderCube()
{
    // Wierzchołki sześcianu
    float vertices[] = {
        // Przednia ściana
        -0.5f, -0.5f, 0.5f, // lewy dolny
        0.5f, -0.5f, 0.5f,  // prawy dolny
        0.5f, 0.5f, 0.5f,   // prawy górny
        -0.5f, 0.5f, 0.5f,  // lewy górny

        // Tylna ściana
        -0.5f, -0.5f, -0.5f, // lewy dolny
        0.5f, -0.5f, -0.5f,  // prawy dolny
        0.5f, 0.5f, -0.5f,   // prawy górny
        -0.5f, 0.5f, -0.5f,  // lewy górny
    };

    // Indeksy wierzchołków tworzących trójkąty
    unsigned int indices[] = {
        // Przednia ściana
        0, 1, 2,  // Pierwszy trójkąt
         2, 3, 0,  // Drugi trójkąt

        // Lewa ściana
        4, 0, 3,  // Pierwszy trójkąt
        3, 7, 4,  // Drugi trójkąt

        // Prawa ściana
        1, 5, 6,  // Pierwszy trójkąt
        6, 2, 1,  // Drugi trójkąt

        // Tylna ściana
        5, 4, 7,  // Pierwszy trójkąt
        7, 6, 5,  // Drugi trójkąt

        // Górna ściana
        3, 2, 6,  // Pierwszy trójkąt
        6, 7, 3,  // Drugi trójkąt

        // Dolna ściana
        1, 0, 4,  // Pierwszy trójkąt
        4, 5, 1   // Drugi trójkąt
    };

    // Tworzenie buforów
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Wiązanie VAO
    glBindVertexArray(VAO);

    // Wypełnienie bufora wierzchołków
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Wypełnienie bufora indeksów
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Konfiguracja atrybutów wierzchołka
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Renderowanie sześcianu
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    // Czyszczenie stanu
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

int main()
{
    // Inicjalizacja GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "grafika komputerowa", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint status;
    GLchar error_message[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, error_message);
        std::cout << "Error (Vertex shader): " << error_message << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, error_message);
        std::cout << "Error (Fragment shader): " << error_message << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
        std::cout << "Error (Shader program): " << error_message << std::endl;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    const float cameraSpeed = 0.03f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    // Pętla zdarzeń
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPosition += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPosition -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        // Renderowanie
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Ustawienie koloru tła na czarny
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Kostka
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("err: %d\n", err);
        }

        //Kamera
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        //glm::mat4 view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
       
        //Rzutowanie
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection"); glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        
        renderCube();

        // Zamiana buforów i obsługa zdarzeń
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteShader(shaderProgram);
    // Zwalnianie zasobów
    glfwTerminate();
    return 0;
}