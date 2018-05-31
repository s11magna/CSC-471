#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertexColor;
uniform mat4 P;
uniform mat4 MV;
uniform float uTime;
// passed the vertex shader
out vec3 fragmentColor; // out to the fragemnett shader
void main()
{
    vec3 temp = vertPos;
	float equation = sin(uTime)/5.2 + 0.53;
    if(vertPos.z == 0.5){
	       temp = temp * equation;
}
	gl_Position = P * MV * vec4(temp,1.0);
	fragmentColor = vertexColor;
}
