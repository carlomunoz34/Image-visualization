#version 330

in vec3 vertexPosition;
in vec3 modelColor;
in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 lightPosition;
uniform float exponent;
uniform vec3 cameraPosition;

uniform vec3 materialA;
uniform vec3 materialD;
uniform vec3 materialS;

out vec3 colorToFragment;

void main() {
    // TODO: Multiply vertexNormilized with viewMatrix, so the vectors can have the correct orientation
    vec3 vertexNormalized = vec3(0, 0, 1);//normalize(vertexNormal);
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1);
    gl_Position = projectionMatrix * viewMatrix * worldPosition;

    // Modelo de phong
    vec3 vertexToLight = normalize(lightPosition - worldPosition.xyz);
    mat4 G = transpose(inverse(modelMatrix));
    vec3 worldNormal = normalize((G * vec4(vertexNormalized, 0)).xyz);

    float factorD = dot(vertexToLight, worldNormal);
    factorD = clamp(factorD, 0, 1);

    vec3 vertexToCamera = normalize(cameraPosition - worldPosition.xyz);
    vec3 r = normalize(2 * worldNormal * dot(worldNormal, vertexToLight) - vertexToLight);
    float factorS = pow(clamp(dot(r, vertexToCamera), 0, 1), exponent);

    colorToFragment = ambientLight * materialA + diffuseLight
            * (materialD * factorD + materialS * factorS);

    colorToFragment = clamp(colorToFragment * modelColor, 0, 1);
}
