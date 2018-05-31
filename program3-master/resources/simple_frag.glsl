#version 330 core 
in vec4 fragNor;
in mat4 Model;
out vec4 color;
uniform int shader;
/*groud&phong shading elements*/

uniform vec3 ambient;
uniform vec3 kd;
uniform vec3 light; 
uniform vec3 ks;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
void main()
{
	
	vec3 normal =  normalize(fragNor.xyz);
	vec3 Ncolor = 0.5*normal + 0.5;
	vec3 viewVector = vec3(0,0,0); // camera is sitting at 0
	vec3 halfVector = normalize(light - viewVector);


	float siloShader;
	vec3 specture = ks * pow((clamp(dot(normalize(halfVector),normalize(fragNor.xyz)),0,1)),(shine));
	vec3 diffuse = kd * clamp(dot(fragNor.xyz,light),0,1);


	vec3 phong = (diffuse *MatDif) +  (ambient * MatAmb) + (specture* MatSpec);

	if(shader == 0){ // normals
	   color = vec4(Ncolor , 1.0);
	}
	else if( shader == 1){ // goroud Shading
	  color = vec4(fragNor.xyz ,1.0);
	}
	else if(shader == 2){ //phong shading
		color = vec4(phong,1.0);
	}
	else if(shader == 3) // silos shading
	{
	  siloShader = clamp(dot(normalize(-gl_FragCoord.xyz),normal),0,1);
	 if(siloShader > 0 && siloShader < 0.9){
	  color = vec4(vec3(1,1,1),1.0);
	  }
	  else{
		color = vec4(vec3(0,0,0),1.0);
	  }
	}
	

}