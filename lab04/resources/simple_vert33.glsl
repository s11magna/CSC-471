#version 330 core
layout(location = 0) in vec3 vertPos;
uniform mat4 P;
uniform mat4 MV;
out vec3 pos;

void main()
{
	gl_Position = P * MV * vec4(vertPos, 1.0);
	gl_Position.x *=2;
	gl_Position.y *=1.2;
	gl_Position.y -=0.1;
	pos = gl_Position.xyz;
	pos.z = vertPos.z;
	 
}
