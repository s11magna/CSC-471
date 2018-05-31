#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;

out vec2 pos;
// passed the vertex shader
out vec3 fragmentColor; // out to the fragemnett shader


void main()
{	
	
	gl_Position = P * MV * vec4(vertPos,1.0);
	pos = vec2((gl_Position.x+1)*260, (gl_Position.y+1)*260);
	fragmentColor = vertNor;
}
