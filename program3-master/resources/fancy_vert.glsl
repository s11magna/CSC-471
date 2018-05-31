#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P; // projection
uniform mat4 V; // view matrix
uniform mat4 M; // model view 
out vec4 fragColor;


void main()
{
	gl_Position = P * V * M * vertPos;
	
	fragColor =vec4(vertNor,0);
	
}
