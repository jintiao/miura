#pragma once


const char *vsSrc = "\n"
"#version 330 core \n"
"layout(location = 0) in vec3 pos; \n"
"layout(location = 1) in vec2 uv; \n"
"layout(location = 2) in vec3 vertexColor; \n"
"uniform mat4 MVP; \n"
"uniform sampler2D heightMap; \n"
"out vec3 fragmentColor; \n"
"void main(){ \n"
"    vec4 newVertex; \n"
"    newVertex = vec4(pos,1); \n"
"    newVertex.y = textureLod(heightMap, uv, 0.0).y; \n"
"    gl_Position = MVP * newVertex; \n"
"    fragmentColor = vertexColor; \n"
"} \n";


const char *psSrc = "\n"
"#version 330 core \n"
"in vec3 fragmentColor; \n"
"out vec3 color; \n"
"void main(){ \n"
"    color = fragmentColor; \n"
"} \n";