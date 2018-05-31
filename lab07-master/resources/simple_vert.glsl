#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P; // projection
uniform mat4 V; // view matrix
uniform mat4 M; // model view 
out vec4 fragColor;


void main()
{


	
	vec3 diffuse = kd * clamp(dot(vertNor.xyz,light),0,1);
	vec3 phong = diffuse +  ambient;
	vec3 spotlight  = clamp(dot())

	fragColor = V * M * phong;
	
	gl_Position = P * V * M * vertPos;
	
	
}
