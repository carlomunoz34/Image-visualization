#version 400

in vec3 colorToFragment;
out vec4 pixelColor;

void main() { 
  //pixelColor = vec4(1, 1, 1, 1.0); 
  pixelColor = vec4(colorToFragment, 1.0); 
} 
