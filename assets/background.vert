#version 410

const int NR_LIGHTS = 2;

struct Light {
    vec4 position;
    vec4 color;
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float shininess;
};

layout (std140) uniform LightsBuffer{
    Light lights[NR_LIGHTS];
};

uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;
in vec4 ciPosition;
in vec3 ciNormal;

out vec4 color;

vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

///*******************************************************
// *  Fixed.vert Fixed Function Equivalent Vertex Shader  *
// *        Automatically Generated by GLSL ShaderGen     *
// *          https://github.com/mojocorp/ShaderGen       *
// *******************************************************/
void pointLight(in Light light, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
    float nDotVP;       // normal . light direction
    float nDotHV;       // normal . light half vector
    float pf;           // power factor
    float attenuation;  // computed attenuation factor
    float d;            // distance from surface to light source
    vec3  VP;           // direction from surface to light position
    vec3  halfVector;   // direction of maximum highlights

    // Compute vector from surface to light position
    VP = (ciModelView*light.position).xyz - ecPosition3;

    // Compute distance between surface and light position
    d = length(VP);

    // Normalize the vector from surface to light position
    VP = normalize(VP);

    // Compute attenuation
    attenuation = 1.0 / (light.constantAttenuation +
                         light.linearAttenuation * d +
                         light.quadraticAttenuation * d * d);

    halfVector = normalize(VP + eye);

    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));

    if (nDotVP == 0.0)
    {
        pf = 0.0;
    }
    else
    {
        pf = pow(nDotHV, light.shininess);
    }
    Ambient  += light.ambient * light.color * attenuation;
    Diffuse  += light.diffuse * light.color * nDotVP * attenuation;
    Specular += light.specular * light.color * pf * attenuation;
}

void flight(in vec3 normal, in vec4 ecPosition)
{
    vec4 color;
    vec3 ecPosition3;
    vec3 eye;

    ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
    eye = vec3 (0.0, 0.0, 1.0);

    // Clear the light intensity accumulators
    Ambient  = vec4 (0.0);
    Diffuse  = vec4 (0.0);
    Specular = vec4 (0.0);

    for(int i = 0; i < NR_LIGHTS; ++i){
        pointLight(lights[i], normal, eye, ecPosition3);
    }
}

void main()
{
	vec3 normal = normalize(ciNormalMatrix * ciNormal);
	gl_Position	=  ciModelViewProjection * ciPosition;
    
    vec4 ecPosition = ciModelView * ciPosition;
    flight(normal, ecPosition);
    
   // color = Specular;
    color = Ambient + Diffuse;
    color += Specular*1.5;
    color = clamp( color+vec4(0.4,0.4,0.4,1), 0.0, 1.0 );
}
