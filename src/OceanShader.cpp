
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

void main (){

float vertexScale = 1.0;
float waveHeight = 30.0;
float dispLambda = -1.5;

vec3 disp = textureLod (DisplacementTexture, vertexUv, 0.0).xyz * 2 - 1;
disp = disp * waveHeight;

vec4 newVertex = vec4 ((vertexPos.x + disp.x * dispLambda) * vertexScale, disp.y, (vertexPos.z + disp.z * dispLambda) * vertexScale, 1);

viewPos = vec4 (mv * newVertex).xyz;

vec3 vnormal = textureLod (NormalTexture, vertexUv, 0.0).xyz * 2 - 1;
viewNormal = normalize (vec4 (mvn * vec4 (vnormal, 0)).xyz);

gl_Position = mvp * newVertex;
}

/////// end vertex shader ///////
)";



const char *psSrc = R"(
/////// begin pixel shader ///////

#version 330 core

uniform mat4 mv;

in vec3 viewPos;
in vec3 viewNormal;

out vec3 color;

void main(){

vec3 lightDir = normalize (vec3 (1.0, 1.0, 0.0));
vec3 upwellingColor = vec3 (0.0, 0.2, 0.3);
vec3 skyColor = vec3 (0.69, 0.84, 1);
vec3 ambiantColor = vec3 (0.1, 0.1, 0.1);
float snell = 1.34;
float diffuse = 0.91;

float reflectivity;
vec4 ldir = mv * vec4 (lightDir, 0);
float costhetai = abs (dot (viewNormal, ldir.xyz));
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

float dist = length (viewPos) * 0.001 * diffuse;
dist = exp (-dist);

color = dist * (reflectivity * skyColor + (1 - reflectivity) * upwellingColor) + (1 - dist) * ambiantColor;
}

/////// end pixel shader ///////
)";
