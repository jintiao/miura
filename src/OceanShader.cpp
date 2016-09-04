
const char *vsSrc = R"(
/////// begin vertex shader ///////

#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUv;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mvn;

uniform sampler2D WaveTexture;

out vec4 viewPos;
out vec4 viewNormal;

void main (){
    vec4 h0 =  textureLod (WaveTexture, vertexUv, 0.0);
    
    vec2 h10uv = vec2 (vertexUv.x + 1.0 / 64.0, vertexUv.y);
	vec4 h10 = textureLod (WaveTexture, h10uv, 0.0);

    vec2 h11uv = vec2 (vertexUv.x, vertexUv.y + 1.0 / 64.0);
	vec4 h11 = textureLod (WaveTexture, h11uv, 0.0);

    vec4 newVertex = vec4 (vertexPos, 1);
    newVertex.y = h0.y;

    gl_Position = mvp * newVertex;

    viewPos = mv * newVertex;

    vec3 vnormal = vec3 ((h0.y - h10.y), (h0.y - h11.y), 8);
	vnormal = normalize(vnormal);
    viewNormal = mvn * vec4 (vnormal, 0);
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