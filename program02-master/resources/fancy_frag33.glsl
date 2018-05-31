#version 330 core

uniform vec3 uColor;
uniform vec2 uWindow;

out vec4 color;
in vec2 pos;

void main()
 {
     
   vec2 center = vec2(640.0, 480.0);

   float preX = (center.x/2 - pos.x)*(center.x/2 - pos.x);
   float preY = (center.y/2 - pos.y)*(center.y/2 - pos.y);
   float len = sqrt(preX + preY);

   color = vec4(0,0,0,1);
   len = len / 320; 
   if(len <= 0.5){
	color = vec4(0, 0.0, 1, 1.0); 
   }
   if(len > 0.5 && len < 1){
    len -= 0.5;
	color = vec4(0, 0.0, 1.0 - len*2.0, 1.0);
   }
 }