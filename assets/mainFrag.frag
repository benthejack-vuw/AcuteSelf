#version 410

uniform float alpha;
uniform bool reverse;

in vec4 vertColor;
out vec4 outColor;

void main()
{
	vec4 col = vec4(vertColor.rgb, alpha);
	outColor = col;
}
