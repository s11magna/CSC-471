#version 330 core
out vec4 color;
in vec3 fragmentColor;
uniform vec2 uWindow;
vec3 white;
void main()
{   
       vec2 center  = uWindow.xy/2.0;

	   float distance = distance(uWindow, center);
	   float length = length(uWindow-center);
	      color = vec4( length(gl_FragCoord.xy - center)*0.01,length(gl_FragCoord.xy - center) *0.001,0,1.0);

	     if(length(gl_FragCoord.xy - center) < 85){
	   color = vec4(0.7,0.7,0,0);
	   }

   

	 
}