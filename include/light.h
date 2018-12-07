#ifndef AcuteSelfCV_light_h
#define AcuteSelfCV_light_h

#include "cinder/Vector.h"

struct Light {
    cinder::vec4 position;
    cinder::vec4 color;
    cinder::vec4 diffuse;
    cinder::vec4 specular;
    cinder::vec4 ambient;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float shininess;
};

#endif
