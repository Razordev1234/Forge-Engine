#pragma once
#include <GL/glew.h>    // ¡Siempre antes de GLFW!
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>

 #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

class AnvilObject;


//listas para que el motor las pueda recorrer y renderizar y aplicar sus propiedades como posicion, rotacion etc....
inline std::vector<std::vector<float>> cosasARenderizar;
inline std::vector<AnvilObject*> objetosReferenciados;
//delta time oculto para pasar desde el motor
inline float dontAskWhy;



struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    Vector2 operator+(const Vector2& other) {
        return Vector2(x + other.x, y + other.y);
    }
    Vector2 operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vector2 operator-(const Vector2& other) {
        return Vector2(x - other.x, y - other.y);
    }
    Vector2 operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vector2 operator*(const Vector2& other) {
        return Vector2(x * other.x, y * other.y);
    }
    Vector2 operator*=(const Vector2& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    Vector2 operator/(const Vector2& other) {
        return Vector2(x / other.x, y / other.y);
    }
    Vector2 operator/=(const Vector2& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }
};


struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vector3 operator+(const Vector3& other) {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
};


/**
 * Define un color para un objeto
 */
struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {} // Constructor
    Color operator+(const Color& other) {
        return Color(r + other.r, g + other.g, b + other.b);
    }
};

class AnvilObject;

enum PrimitiveType {
    TRIANGLE_STRIP,
    TRIANGLE_FAN
};

class Collision2D {
    public:
    bool Bool;
    AnvilObject* Object;
};


class AnvilObject {
    //tiene sus propiedades que luego el motor las procesa y las manda a la matriz del shader
    private: 
    int _Width, _Height;
    public:
        int Width, Height;
        Vector2 Position;
        Vector3 Rotation;
        Color color; // Propiedad para declarar el color del objeto
        Vector2 Scale = Vector2(100.0f, 100.0f);
        PrimitiveType primitiveType = TRIANGLE_STRIP;
        uint64_t textureID = 0;
    //agrega el objeto creado a una lista para luego ser recorrido y aplicar sus propiedad al shader
    AnvilObject() {
        this->color = Color(255, 255, 255);
        objetosReferenciados.push_back(this);
    }
    //crea un cuadrado con los parametros ancho y alto
    //hace una media entre el alto y el ancho para que la forma este centrada
    //luego esto se envia a una lista donde el motor lo podra meter en el buffer central, recorre toda la lista y lo renderiza
    // (solo hay un buffer, no pregunten)
    /*
     * @brief Crea un cuadrado con los parametros ancho y alto
     * @param width Ancho del cuadrado
     * @param height Alto del cuadrado
     ```
     * @example
     * AnvilObject obj;
     * obj.CreateQuad(100, 100);
     ```
     */
    void CreateQuad(int width, int height)
    {
        float medidaAncho = width / 50.0f; 
        float medidaAlto  = height / 50.0f;

        float mitadAncho = medidaAncho / 2.0f;
        float mitadAlto  = medidaAlto / 2.0f;
        Width=width; 
        Height=height;
        std::vector<float> ListaVertices = {
            mitadAncho, mitadAlto, 1.0f
        };
        cosasARenderizar.push_back(ListaVertices);
    };
    /*
     * @brief Crea un circulo con el parametro del radio
     * @param radio radio de la circunferencia
     ``` 
     * @example
     * AnvilObject obj;
     * obj.CreateCircle(50);
     ```
     */
    void CreateCircle(int radio){
        float medidaRadio = radio / 50.0f;
        Width = radio * 2;
        Height = radio * 2;
        primitiveType = TRIANGLE_FAN;
        
        int numSegments = 32;
        std::vector<float> ListaVertices;
        
        // Centro del circulo (primer vertice del triangle fan)
        ListaVertices.push_back(0.0f);
        ListaVertices.push_back(0.0f);
        ListaVertices.push_back(0.0f);
        ListaVertices.push_back(1.0f);
        ListaVertices.push_back(1.0f);
        ListaVertices.push_back(1.0f);
        ListaVertices.push_back(0.5f);
        ListaVertices.push_back(0.5f);
        
        // Generar vertices alrededor del circulo
        for (int i = 0; i <= numSegments; i++) {
            float angle = (2.0f * M_PI * i) / numSegments;
            float x = cos(angle) * medidaRadio;
            float y = sin(angle) * medidaRadio;
            float u = 0.5f + cos(angle) * 0.5f;
            float v = 0.5f + sin(angle) * 0.5f;
            
            ListaVertices.push_back(x);
            ListaVertices.push_back(y);
            ListaVertices.push_back(0.0f);
            ListaVertices.push_back(1.0f);
            ListaVertices.push_back(1.0f);
            ListaVertices.push_back(1.0f);
            ListaVertices.push_back(u);
            ListaVertices.push_back(v);
        }
        
        cosasARenderizar.push_back(ListaVertices);
    }
    Collision2D CheckCollision() {
        Collision2D collision2d;
        collision2d.Bool = false;
        collision2d.Object = nullptr;

        //  AABB de "este" objeto (usamos el centro)
        float anguloRad = this->Rotation.z * (M_PI / 180.0f);
        float c = std::abs(cos(anguloRad));
        float s = std::abs(sin(anguloRad));
    
        // El tamaño envolvente
        float thisW_env = (this->Width * c) + (this->Height * s);
        float thisH_env = (this->Width * s) + (this->Height * c);

        // Calculamos los bordes de "this" desde su centro
        float thisMinX = this->Position.x - (thisW_env);
        float thisMaxX = this->Position.x + (thisW_env);
        float thisMinY = this->Position.y - (thisH_env);
        float thisMaxY = this->Position.y + (thisH_env);

        for (size_t i = 0; i < objetosReferenciados.size(); i++) {
            AnvilObject* otro = objetosReferenciados[i];

            if (otro != this) {
                // AABB 
                float angRadOtro = otro->Rotation.z * (M_PI / 180.0f);
                float cO = std::abs(cos(angRadOtro));
                float sO = std::abs(sin(angRadOtro));
            
                
                float otroW_env = (otro->Width * cO) + (otro->Height * sO);
                float otroH_env = (otro->Width * sO) + (otro->Height * cO);

                float otroMinX = otro->Position.x - (otroW_env);
                float otroMaxX = otro->Position.x + (otroW_env);
                float otroMinY = otro->Position.y - (otroH_env);
                float otroMaxY = otro->Position.y + (otroH_env);

                // Comprobación AABB 
                bool check1 = (thisMaxX >= otroMinX && otroMaxX >= thisMinX);
                bool check2 = (thisMaxY >= otroMinY && otroMaxY >= thisMinY);

                if (check1 && check2) {
                    collision2d.Bool = true;
                    collision2d.Object = otro;
                    return collision2d; 
                }
            }
        }
    return collision2d;
    };

};



inline float deltaTime()
{
    return dontAskWhy;
}

//comandos internos para pasarnos parametros
//no se ni yo lo que hace

inline std::vector<std::vector<float>>& InternalPassVerticesList() {
    return cosasARenderizar;
}

inline std::vector<AnvilObject*>& InternalPassReferenceObjects() {
    return objetosReferenciados;
}

//delta time oculto para pasar desde el motor
inline void InternalPassDontAsk(float deltaaa)
{
    dontAskWhy = deltaaa;
};

inline void SetWindowTitle(const char* title) {
    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        glfwSetWindowTitle(window, title);
    }
}

inline Color backgroundColor = Color(0.0f, 0.0f, 0.0f);

inline void SetBackgroundColor(float r, float g, float b) {
    backgroundColor = Color(r, g, b);
}

inline Color GetBackgroundColor() {
    return backgroundColor;
}

inline void PutVsync() {
    glfwSwapInterval(1);
}

inline void QuitVsync() {
    glfwSwapInterval(0);
}



inline uint64_t LoadTexture(const char* path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Configuracion de la textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Cargar la imagen, aqui es necesario el uso de una nueva libreria la cual es: stb_image.h
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Error cargando textura: " << path << std::endl;
    }
    uint64_t handle = glGetTextureHandleARB(textureID);
    glMakeTextureHandleResidentARB(handle);
    return handle;
}

//h


