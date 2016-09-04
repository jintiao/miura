
const char *vsSrc = R"(
/////// begin vertex shader ///////

#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in vec3 vertexColor;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mvn;

uniform sampler2D WaveTexture;

out vec3 viewPos;
out vec3 viewNormal;
out vec3 fragmentColor;

void main(){
    vec4 newVertex = vec4(vertexPos, 1);
    newVertex.y = textureLod(WaveTexture, vertexUv, 0.0).y;
    gl_Position = mvp * newVertex;
    fragmentColor = vertexColor;
    viewPos = (mv * newVertex).xyz;
    viewNormal = (mvn * newVertex).xyz;
}

/////// end vertex shader ///////
)";



const char *psSrc = R"(
/////// begin pixel shader ///////

#version 330 core

in vec3 viewPos;
in vec3 fragmentColor;

out vec3 color;

void main(){
    color = fragmentColor;
}

/////// end pixel shader ///////
)";