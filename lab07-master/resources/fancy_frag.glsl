#version 330 core 
in vec4 fragColor;
out vec4 color;

void main()
{
	vec3 ambient = vec3(0.3,0.3,0.3);
	vec3 kd = vec3(0.6,0.6,0.6);
	vec3 light = normalize(vec3(1,0.5,1));
	vec3 diffuse = kd * clamp(dot(fragColor.xyz,light),0,1);

	color = vec4(ambient + diffuse,1.0);
}
