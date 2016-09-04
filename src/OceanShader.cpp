
const char *vsSrc = R"(
/////// begin vertex shader ///////

#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUv;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mvn;

uniform sampler2D DisplacementTexture;
uniform sampler2D NormalTexture;

out vec4 viewPos;
out vec4 viewNormal;

void main (){
    vec4 disp = textureLod (DisplacementTexture, vertexUv, 0.0);

    vec4 newVertex = vec4 (vertexPos, 1);
    newVertex.y = disp.y;

    gl_Position = mvp * newVertex;

    viewPos = mv * newVertex;

    vec4 vnormal = textureLod (NormalTexture, vertexUv, 0.0);
    viewNormal = mvn * vec4 (vnormal.xyz, 0);
}

/////// end vertex shader ///////
)";



const char *psSrc = R"(
/////// begin pixel shader ///////

#version 330 core

uniform mat4 mv;

in vec4 viewPos;
in vec4 viewNormal;

out vec3 color;

void main(){
    vec3 lightDir = normalize (vec3 (1.0, 1.0, 1.0));
    vec3 skyColor = vec3 (0.69, 0.84, 1);
    vec3 ambiantColor = vec3 (0.1, 0.1, 0.1);
    
    vec4 ldir = mv * vec4 (lightDir, 0);

    color = skyColor * dot (viewNormal.xyz, ldir.xyz) + ambiantColor;
}

/////// end pixel shader ///////
)";