#version 410

uniform sampler2D tex0;
in vec2           tex_coord_0;

out vec4          outColor;

void main()
{
    outColor = texture(tex0, tex_coord_0);
    outColor.a = (1.0-outColor.r)*0.9;
}
