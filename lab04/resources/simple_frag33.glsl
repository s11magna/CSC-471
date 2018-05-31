#version 330 core
out vec3 color;
in vec3 pos;
uniform vec2 center; 

void main()
{

	float len = sqrt((center.x/2 +pos.x)*(center.x/2 +pos.x) + (center.y/2 + pos.y)*(center.y/2 + pos.y));
	if(pos.z > 0){
		color = vec3(pos.x, pos.x, pos.y);
	}
	else{ 
		color = vec3(1-pos.y-0.3, 0.0, pos.y+0.2);
	}
	if(len < 0.1){
		color = vec3(1.0, 1.0, 1.0);	
	}
	color=clamp(color,0,1);
	color.r += 1.1 * len;
	color.g += 1.1 * len;
	color.b += 1.1 * len;
	
}
