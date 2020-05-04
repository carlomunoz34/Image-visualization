#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 modelColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 ambientLight;
uniform vec3 materialA;
uniform vec3 diffuseLight;
uniform vec3 lightPosition;
uniform vec3 materialD;

out vec3 vertexColorToFS;

void main() {
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1);
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
    // Modelo de phong
    vec3 vertexToLight = normalize(lightPosition - worldPosition.xyz);
    mat4 G = transpose(inverse(modelMatrix));
    vec4 worldNormal = G * vec4(vertexNormal, 0);
    float factorD = dot(vertexToLight, normalize(worldNormal.xyz));
    factorD = clamp(factorD, 0, 1);

    vertexColorToFS = ambientLight * materialA + diffuseLight * materialD * factorD;
}
