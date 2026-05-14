#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "../Anvil.h"

//====================IMPORTANTE=====================   
//Si vas a usar este script renombrar a "script.h" y agregarlo al proyecto
//El motor lo va a compilar y ejecutar automaticamente


//===================================================
//=====================**PONG**======================
//==================EN FORGE ENGINE==================
//===================================================

//=======================Definicion de objetos globales========================
//Inicializamos los objetos y las variables globales que vamos a usar en el script
//No se preocupen por esto, es solo para que el motor pueda acceder a estas variables y objetos desde el script


AnvilObject Fondo;
AnvilObject Cubo;
GLFWwindow* Window;
Collision2D colision1;
Collision2D colision2;



//=======================Funciones del script========================


void Start() {
    Fondo.CreateQuad(150, 100);
    Fondo.textureID = LoadTexture("EXAMPLE/SPRITES/FONDO.jpg");
    Cubo.CreateQuad(10,10);
    Cubo.textureID = LoadTexture("EXAMPLE/SPRITES/CUBO.png");
    Window = glfwGetCurrentContext();
    PutVsync();
}

void Update() {
    
    
    std::cout << 1.0f / deltaTime() << " FPS" << std::endl;
}