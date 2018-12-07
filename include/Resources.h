#pragma once
#include "cinder/CinderResources.h"

#define MAIN_VERT CINDER_RESOURCE( ../assets/, mainVert.vert, 128, GLSL )
#define MAIN_FRAG CINDER_RESOURCE( ../assets/, mainFrag.frag, 129, GLSL )

#define BACKGROUND_VERT CINDER_RESOURCE( ../assets/, background.vert, 130, GLSL )
#define BACKGROUND_FRAG CINDER_RESOURCE( ../assets/, background.frag, 131, GLSL )

#define EMPTY_VERT CINDER_RESOURCE( ../assets/, emptyVert.vert, 132, GLSL )
#define BLUR_FRAG CINDER_RESOURCE( ../assets/, blurFrag.frag, 133, GLSL )

#define INV_ALPHA_FRAG CINDER_RESOURCE( ../assets/, inverseAlpha.frag, 134, GLSL )


