
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

out vec3 viewPos;
out vec3 viewNormal;
out vec3 viewLightDir;

void main (){

// scale the ocean plane to [-300, 300] meters
float vertexScale = 300.0;

vec4 worldVertex = vec4 (vertexPos.x * vertexScale, vertexPos.y, vertexPos.z * vertexScale, 1);

// max wave height is 30 meters
float waveHeight = 30.0;

// use dispLambda to adjust horizontal displacement, if you set this as a positive number, the wave will be flatten
float dispLambda = -1.0;

// scale the displacement data to [-1, 1]
vec3 disp = textureLod (DisplacementTexture, vertexUv, 0.0).xyz * 2 - 1;
disp = disp * waveHeight;

worldVertex.xyz = worldVertex.xyz + vec3 (disp.x * dispLambda, disp.y, disp.z * dispLambda);

viewPos = vec4 (mv * worldVertex).xyz;

// scale the normal data to [-1, 1]
vec3 vnormal = textureLod (NormalTexture, vertexUv, 0.0).xyz * 2 - 1;
viewNormal = normalize (vec4 (mvn * vec4 (vnormal, 0)).xyz);

// viewLightDir should be a uniform, but I'm a lazy guy.
vec3 lightDir = normalize (vec3 (1.0, 1.0, 0.0));
viewLightDir = normalize (vec4 (mv * vec4 (lightDir, 0)).xyz);

gl_Position = mvp * worldVertex;
}

/////// end vertex shader ///////
)";



const char *psSrc = R"(
/////// begin pixel shader ///////

#version 330 core

uniform mat4 mv;

in vec3 viewPos;
in vec3 viewNormal;
in vec3 viewLightDir;

out vec3 color;

void main(){

vec3 upwellingColor = vec3 (0.0, 0.2, 0.3);
vec3 skyColor = vec3 (0.69, 0.84, 1);
vec3 ambiantColor = vec3 (0.1, 0.1, 0.1);
float snell = 1.34;
float diffuse = 0.91;

float reflectivity;
float costhetai = abs (dot (viewNormal, viewLightDir));
float thetai = acos (costhetai);
float sinthetat = sin (thetai) / snell;
float thetat = asin (sinthetat);
if (thetat == 0.0)
{
	reflectivity = (snell - 1) / (snell + 1);
	reflectivity = reflectivity * reflectivity;
}
else
{
	float fs = sin (thetat - thetai) / sin (thetat + thetai);
	float ts = tan (thetat - thetai) / tan (thetat + thetai);
	reflectivity = 0.5 * (fs * fs + ts * ts);
}

// use distanceScale to redue color fade rate
float distanceScale = 0.001;
float dist = length (viewPos) * distanceScale * diffuse;
dist = exp (-dist);

color = dist * (reflectivity * skyColor + (1 - reflectivity) * upwellingColor) + (1 - dist) * ambiantColor;
}

/////// end pixel shader ///////
)";
