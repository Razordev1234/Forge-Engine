#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable



out vec3 ourColor;
out vec2 TexCoord;
flat out uint64_t vTexHandle; 

uniform mat4 proyeccion;


struct DatosAEnviar {
    float EscalaPrivadaX, EscalaPrivadaY;
    float PosX, PosY;
    float Rotation;
    float ScaleX, ScaleY;
    float ColorR, ColorG, ColorB;
    float padding;
    uint64_t Handle; 
};


layout(std430, binding = 0) buffer ObjetoBuffer {
    DatosAEnviar objetos[];
};

void main() {

    vec2 posiciones[4] = vec2[](
        vec2(-1.0,  1.0), 
        vec2(-1.0, -1.0), 
        vec2( 1.0,  1.0), 
        vec2( 1.0, -1.0)  
    );

    vec2 uvs[4] = vec2[](
        vec2(0.0, 1.0),
        vec2(0.0, 0.0),
        vec2(1.0, 1.0),
        vec2(1.0, 0.0)
    );

    vec2 posLocal = posiciones[gl_VertexID % 4];
    TexCoord = uvs[gl_VertexID % 4];


    DatosAEnviar obj = objetos[gl_InstanceID];

    mat4 mTras = mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(obj.PosX / 100.0, obj.PosY / 100.0, 0.0, 1.0)
    );
    
    float c = cos(radians(obj.Rotation));
    float s = sin(radians(obj.Rotation));
    mat4 mRot = mat4(
        vec4(c, s, 0.0, 0.0),
        vec4(-s, c, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 mEsc = mat4(
        vec4(obj.ScaleX / 100.0, 0.0, 0.0, 0.0),
        vec4(0.0, obj.ScaleY / 100.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 mPriv = mat4(
        vec4(obj.EscalaPrivadaX, 0.0, 0.0, 0.0),
        vec4(0.0, obj.EscalaPrivadaY, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    ourColor = vec3(obj.ColorR / 255.0, obj.ColorG / 255.0, obj.ColorB / 255.0);
    vTexHandle = obj.Handle;

    gl_Position = proyeccion * mTras * mRot * mEsc * mPriv * vec4(posLocal, 0.0, 1.0);
}