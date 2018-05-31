#version 330 core
uniform vec2 posVec;
in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;
uniform vec2 dir;

uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 
											0.0540540541, 0.0162162162 );

void main(){
 
 			color = vec4(texture(texBuf, texCoord)) * weight[0] ;

	for (int i=1; i < 5; i ++) {
			color += vec4(texture(texBuf, texCoord + (posVec * offset[i])/512.0))  * weight[i] * 2;
			//color += vec4(texture(texBuf, posVec-vec2(offset[i], 0)/512.0).rgb, 1)* weight[i];
	}
}
