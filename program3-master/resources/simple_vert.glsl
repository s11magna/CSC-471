#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V; 
uniform int shader;
uniform vec3 ambient;
uniform vec3 kd;
uniform vec3 light; 
uniform vec3 ks;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
out vec4 fragNor;
out mat4 Model;

void main()
{

	vec3 viewVector = vec3(0,0,0); // camera is sitting at 0
	vec3 halfVector = normalize(light - viewVector);
	
// calculate the light vector  position - view vector
	vec3 diffuse = kd * clamp(dot(normalize(vertNor),normalize(light)),0,1);
	vec3 specture = ks * pow((clamp(dot(normalize(halfVector),normalize(vertNor)),0,1)),shine);
	vec3 phong = (diffuse *MatDif) +  (ambient * MatAmb) + (specture* MatSpec);
	if(shader == 0 || shader == 2 || shader == 3){
	fragNor = V * M * (vec4(vertNor,0.0));
	}
	else if(shader == 1){ /*groud shading*/
	fragNor = vec4(phong ,0.0);
	}
	gl_Position = P * V * M * vertPos;
}