#version 410

uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec4 ciColor;

uniform bool reverse;
uniform float alpha;
uniform float glow;

in float depth;
out vec4 vertColor;

void main()
{
	if(reverse){
		vertColor = ciColor + ((ciColor*(depth-0.5)))/1.5;
        vertColor = vertColor+ (vertColor*glow);
	}
	else
		vertColor = ciColor;


	gl_Position	=  ciModelViewProjection * ciPosition;
}
