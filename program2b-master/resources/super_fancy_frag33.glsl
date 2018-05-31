#version 330 core


out vec4 color;
in vec2 pos;

void main()
 {
     
   vec2 center = vec2(640.0, 480.0);

   float preX = (center.x/2 - pos.x)*(center.x/2 - pos.x);
   float preY = (center.y/2 - pos.y)*(center.y/2 - pos.y);
   float len = sqrt(preX + preY);

   color = vec4(0,0,0,1);
   len = len / 240; 
	if(len <= 1){
	color = vec4(1.0, 1.0, 1, 1.0); 
   }
   if(len >= 0.0 && len <= 2){
    len -= 0.6;
	color = vec4(len*3, len*3, len*3, 1.0);
   }
 }