#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "Anvil.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "EXAMPLE/SIMPLEMINIGAME.h"

/*
float vertices[] = {
    -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f
};
*/
void error_callback(int error, const char* description) {
    std::cerr << "Error de GLFW (" << error << "): " << description << std::endl;
}

std::string loadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el shader: " << path << std::endl;
        return "";
    }
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    int elementosAntes = 0;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        std::cout << "Error: No se pudo inicializar GLFW" << std::endl;
        return -1;
    }

    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 650, "Ventana de ForgeEngine", nullptr, nullptr);
    if (!window) {
        std::cout << "Error: No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error: No se pudo inicializar GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    std::string vertexSource = loadFile("./SHADERS/vertex.glsl");
    std::string fragmentSource = loadFile("./SHADERS/fragment.glsl");

    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "Error cargando shaders" << std::endl;
        glfwTerminate();
        return -1;
    }

    const char* vertexShaderSource = vertexSource.c_str();
    const char* fragmentShaderSource = fragmentSource.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR VERTEX SHADER: " << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    /*
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    */
    glUseProgram(shaderProgram);

    // Inicializar SSBO vacío
    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    float aspect = 1200.0f / 800.0f;
    float matrizProye[] = {
        1.0f / aspect, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    int idUniform = glGetUniformLocation(shaderProgram, "proyeccion");
    int texturaID = glGetUniformLocation(shaderProgram, "ourTexture");
    int usetextureID = glGetUniformLocation(shaderProgram, "useTexture");
    glUniformMatrix4fv(idUniform, 1, GL_FALSE, matrizProye);

    // Inicializamos el script del juego (Crea los 10,000 objetos)
    Start();


    auto& lista = InternalPassVerticesList();
    auto& objetosLista = InternalPassReferenceObjects();

    float ultimoframe = 0.0f;
    std::vector<float> matricesParaEnviar;
    short floatsAEnviar = 12;

    struct DatosAEnviar
    {
        float EscalaPrivadaX;
        float EscalaPrivadaY;
        float PosX;
        float PosY;
        float Rotation;
        float ScaleX;
        float ScaleY;
        float ColorR;
        float ColorG;
        float ColorB;
        float padding;
        uint64_t Handle;
    };

    std::vector<DatosAEnviar> ArrayAEnviar;
    // ==================== Bucle principal ====================
    while (!glfwWindowShouldClose(window)) {
        glClearColor(GetBackgroundColor().r, GetBackgroundColor().g, GetBackgroundColor().b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        Update(); 

        // 1. Redimensionar el SSBO solo si la cantidad de objetos cambia
        if (objetosLista.size() != elementosAntes) {
            elementosAntes = objetosLista.size(); 
            ArrayAEnviar.resize(elementosAntes);  
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, elementosAntes * sizeof(DatosAEnviar), NULL, GL_DYNAMIC_DRAW);
        }
        
        

        
        
        if (elementosAntes > 0) {
            #pragma omp simd
            for (size_t i = 0; i < objetosLista.size(); i++) {
                size_t base = i * floatsAEnviar;
                ArrayAEnviar[i].EscalaPrivadaX = lista[i][0];
                ArrayAEnviar[i].EscalaPrivadaY = lista[i][1];
                ArrayAEnviar[i].PosX = objetosLista[i]->Position.x;
                ArrayAEnviar[i].PosY = objetosLista[i]->Position.y;
                if (objetosLista[i]->Rotation.z >= 360.0f) {
                    objetosLista[i]->Rotation.z = 0.0f;
                }
                if (objetosLista[i]->Rotation.z < 0.0f) {
                    objetosLista[i]->Rotation.z = 359.0f;
                }
                ArrayAEnviar[i].Rotation = objetosLista[i]->Rotation.z;
                ArrayAEnviar[i].ScaleX = objetosLista[i]->Scale.x;
                ArrayAEnviar[i].ScaleY = objetosLista[i]->Scale.y;
                ArrayAEnviar[i].ColorR = objetosLista[i]->color.r;
                ArrayAEnviar[i].ColorG = objetosLista[i]->color.g;
                ArrayAEnviar[i].ColorB = objetosLista[i]->color.b;
                ArrayAEnviar[i].Handle = objetosLista[i]->textureID; 

                
                /** 
                if (hasTexture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, objetosLista[i]->textureID);
                    glUniform1i(texturaID, 0);
                } else {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                **/
               
            }       
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ArrayAEnviar.size() * sizeof(DatosAEnviar), ArrayAEnviar.data());

            

            
        }
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, objetosLista.size());
        float deltatime = glfwGetTime() - ultimoframe;
        ultimoframe = glfwGetTime();
        InternalPassDontAsk(deltatime);
        
        glfwSwapBuffers(window);
        glfwPollEvents(); 
    }

    glfwTerminate();
    return 0;
}