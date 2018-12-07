//
//  FrameSubtractor.h
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#ifndef FrameSubtractor_h
#define FrameSubtractor_h

#include "cinder/Capture.h"
#include "cinder/Log.h"

/////////////////////////////////////////////
//
//
//    Utility class to execute framesubtraction
//
//
//////////////////////////////////////////////

class FrameSubtractor{
public:
    
    
    //---------------------------------
    //
    //    constructior and destructor
    //
    //---------------------------------
    FrameSubtractor(int i_width, int i_height);
    ~FrameSubtractor(){ clear(); }
    
    //---------------------------------
    //
    //    accessors
    //
    //---------------------------------
    
    cinder::Surface8uRef difference();
    cinder::Surface8uRef difference2();
    cinder::Surface8uRef currentRGB();
    cinder::Surface8uRef colorBuffer();
    cinder::Surface8uRef backgroundImage();
    
    int pixelCount();
    int width();
    int height();
    int frameCount();
    
    //---------------------------------
    //
    //    methods
    //
    //---------------------------------
    void setup_capture();
    void update();
    
    void clear();
    float lerp(float i1, float i2, float amount);
    void updateColorBuffers(cinder::Surface8uRef  & i_RGB);
    void blankDifferences();
    void updateImages(cinder::Surface8uRef  & i_RGB);
    void imageThreshhold(cinder::Surface8uRef i_newRGB, cinder::Surface8uRef i_oldRGB, cinder::Surface8uRef i_destImg,  float threshhold);
    
    
private:
    void flip_buffer(cinder::Surface8uRef  &buffer_data);
    
    
    cinder::CaptureRef _capture;
    
    int _frameCount, _width, _height, _pixelCount;
    
    cinder::Surface8uRef convert_surfaceA(cinder::Surface8uRef surf);
    cinder::Surface8uRef _lastRGB;
    cinder::Surface8uRef _currentRGB;
    cinder::Surface8uRef _colorBuffer;
    cinder::Surface8uRef _difference;
    cinder::Surface8uRef _difference2;
    
    cinder::Surface8uRef _background;
};




#endif /* FrameSubtractor_h */
