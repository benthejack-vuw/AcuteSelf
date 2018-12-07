//
//  FrameSubtractor.cpp
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#include <stdio.h>
#include "AcuteSelfSetup.h"
#include "FrameSubtractor.h"

////////////////////////////////////////////////////////////
//
//    FRAME SUBTRACTOR IMPLEMENTATIONS
//
////////////////////////////////////////////////////////////

FrameSubtractor::FrameSubtractor(int i_width, int i_height):
_frameCount(0),
_width(i_width),
_height(i_height),
_pixelCount(i_width*i_height)
{
    _difference  = cinder::Surface8u::create(i_width, i_height, true, cinder::SurfaceChannelOrder::BGR);
    _difference2 = cinder::Surface8u::create(i_width, i_height, true, cinder::SurfaceChannelOrder::BGR);
    _background  = cinder::Surface8u::create(i_width, i_height, false, cinder::SurfaceChannelOrder::BGR);
    _lastRGB     = cinder::Surface8u::create(i_width, i_height, false, cinder::SurfaceChannelOrder::BGR);
    _currentRGB  = cinder::Surface8u::create(i_width, i_height, false, cinder::SurfaceChannelOrder::BGR);
    _colorBuffer = cinder::Surface8u::create(i_width, i_height, false, cinder::SurfaceChannelOrder::BGR);
    _background  = cinder::Surface8u::create(i_width, i_height, false, cinder::SurfaceChannelOrder::BGR);
    
    blankDifferences();
    setup_capture();
};

////////////////////////////////////////////////////////////
//    FRAME SUBTRACTOR ACCESSORS
////////////////////////////////////////////////////////////

cinder::Surface8uRef FrameSubtractor::difference(){
    return _difference;
}

cinder::Surface8uRef FrameSubtractor::difference2(){
    return _difference2;
}

cinder::Surface8uRef FrameSubtractor::currentRGB(){
    return _currentRGB;
}

cinder::Surface8uRef FrameSubtractor::colorBuffer(){
    return _colorBuffer;
}

cinder::Surface8uRef FrameSubtractor::backgroundImage(){
    return _background;
}

int FrameSubtractor::pixelCount(){
    return _pixelCount;
}

int FrameSubtractor::width(){
    return _width;
}

int FrameSubtractor::height(){
    return _height;
}

int FrameSubtractor::frameCount(){
    return _frameCount;
}

////////////////////////////////////////////////////////////
//    FRAME SUBTRACTOR IMPLEMENTATIONS
////////////////////////////////////////////////////////////

void FrameSubtractor::setup_capture(){
    try {
        _capture = cinder::Capture::create( _width, _height );
        _capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
}

void FrameSubtractor::update(){
    if( _capture && _capture->checkNewFrame() ) {
        cinder::Surface8uRef vidDat = _capture->getSurface();
		
		if (vidDat->hasAlpha()) {
			vidDat = convert_surfaceA(vidDat);
		}

        flip_buffer(vidDat);
        //_currentRGB = vidDat;
        updateImages(vidDat);
        updateColorBuffers(vidDat);
    }
}

void FrameSubtractor::flip_buffer(cinder::Surface8uRef &buffer_data){
    
    uint8_t * dat = buffer_data->getData();
    int x = 0;
    int y = 0;
    int tempVal;
    int tempVal2;
    
    int w = width();
    int h = height();
    
    for(int i = 0; i < 3; i++){
        for(y = 0; y < h; y++){
            for(x = 0; x < w/2 ; x++){
                
                tempVal = (int)dat[((y*w)+(w-1-x))*3+i];
                tempVal2 = (int)dat[((y*w)+(x))*3+i];
                
                dat[((y*w)+(x))*3+i] = tempVal;
                dat[((y*w)+(w-1-x))*3+i] = tempVal2;
                
            }
        }
    }
}

cinder::Surface8uRef FrameSubtractor::convert_surfaceA(cinder::Surface8uRef surf){
    cinder::Surface8uRef out = cinder::Surface8u::create(_width, _height, false);
    uint8_t * in = surf->getData();
    uint8_t * o = out->getData();
    
    for(int i = 0; i < _pixelCount; ++i){
        o[i*3] = in[i*4];
        o[i*3+1] = in[i*4+1];
        o[i*3+2] = in[i*4+2];
    }
    
    return out;
}

void FrameSubtractor::clear(){
}

float FrameSubtractor::lerp(float i1, float i2, float amount){
    
    float diff = i2-i1;
    return i1+(diff*amount);
    
}

void FrameSubtractor::updateColorBuffers(cinder::Surface8uRef  & i_RGB){
    
    float backgroundAlphaAdder = 0.001;
    
    uint8_t * newRGB = i_RGB->getData();
    uint8_t * bckGnd = _background->getData();
    
    if(_frameCount > 1){
        
        uint8_t * diff  = _difference->getData();
        uint8_t * diff2 = _difference2->getData();
        uint8_t * ocols = _colorBuffer->getData();
        uint8_t * ncols = _lastRGB->getData();
        
        
        for(int i = 0; i < _pixelCount*3; ++i){
            ocols[i] = (int)diff[i] > 255/2 ? (int)lerp((int)ocols[i], (int)ncols[i], 0.7) : ocols[i];
            
            diff2[i] = diff[i] > 0.5 ? 0  : diff2[i];
            diff2[i] = (int)diff2[i] + 25 < 255 ? (int)diff2[i] + 25 : 255;
            
            bckGnd[i] =  (newRGB[i]*backgroundAlphaAdder )+bckGnd[i]*(1-backgroundAlphaAdder);
        }
        
    }else{
        cinder::Area a(0, 0, AcuteSelfCV::RESOLUTION.x, AcuteSelfCV::RESOLUTION.y);
        _colorBuffer->copyFrom(*i_RGB, a);
        _background->copyFrom(*i_RGB, a);
    }
    
}

void FrameSubtractor::blankDifferences(){
    uint8_t * diff = _difference->getData();
    uint8_t * diff2 = _difference2->getData();
    
    for(int i = 0; i < _pixelCount*3; ++i){
        diff[i] = 0;
        diff2[i] = 0;
    }
}


void FrameSubtractor::updateImages(cinder::Surface8uRef  &i_RGB){
    
    uint8_t * in = i_RGB->getData();
    uint8_t * last = _lastRGB->getData();
    uint8_t * current = _currentRGB->getData();
    
    if(_frameCount > 2){
        imageThreshhold(i_RGB, _lastRGB, _difference, 0.15);
    }
    
    for(int i = 0; i < _pixelCount*3; ++i){
        last[i]    = current[i];
        current[i] = in[i];
    }
    
    _frameCount++;
}


void FrameSubtractor::imageThreshhold(cinder::Surface8uRef i_newRGB, cinder::Surface8uRef i_oldRGB, cinder::Surface8uRef i_destImg,  float threshhold){
    
    uint8_t * dest = i_destImg->getData();
    
    int rd;
    int gd;
    int bd;
    int all;
    
    if(_frameCount > 1){
        
        uint8_t * rgbVals = i_newRGB->getData();
        uint8_t * lastVals = i_oldRGB->getData();
        
        for(int i = 0; i < _pixelCount*3; i+= 3){
            
            rd = abs((int)rgbVals[i] - (int)lastVals[i]);
            gd = abs((int)rgbVals[i+1] - (int)lastVals[i+1]);
            bd = abs((int)rgbVals[i+2] - (int)lastVals[i+2]);
            
            all = (rd+gd+bd)/3;
            
            dest[i] = all > 255*threshhold ? 255 : 0;
            dest[i+1] = dest[i];
            dest[i+2] = dest[i];
        }
    }
}
