#ifndef AcuteSelfCV
#define AcuteSelfCV

#include "cinder/Vector.h"

namespace AcuteSelfCV {
    cinder::vec2 CAMERA_RESOLUTION = cinder::vec2(640, 480);
    cinder::vec2 RESOLUTION = cinder::vec2(1920, 1080)/cinder::vec2(3, 3);
    int DECIMATION          = 12;
    int MAX_TRI_SIZE        = 100;
    int MAX_MESHES          = 20;
    int MESH_STEPBACK       = 25;
    float BACKGROUND_SPEED  = 0.03;
}

#endif
