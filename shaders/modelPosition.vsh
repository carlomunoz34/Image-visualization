#version 400

in vec3 vertexPosition;
in vec3 modelColor;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 colorToFragment;

void main() {
  gl_Position = projectionMatrix * viewMatrix * modelMatrix *
                vec4(vertexPosition, 1);
  //colorToFragment = modelColor;
  colorToFragment = vec3(1, 1, 1);
}
