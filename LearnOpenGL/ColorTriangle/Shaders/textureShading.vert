#version 400
//The vertex shader operates on each vertex

in vec3 position;


void main() {
    //Set the x,y position on the screen
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}