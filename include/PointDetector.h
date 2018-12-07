//
//  MeshBuilder.h
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#ifndef PointDetector_h
#define PointDetector_h

#include "FrameSubtractor.h"
#include "CinderOpenCv.h"

class PointDetector{
public:
    
    //---------------------------------
    //
    //    constructior and destructor
    //
    //---------------------------------
    PointDetector(int i_decimate);
    ~PointDetector(){}
    
    //---------------------------------
    //
    //    methods
    //
    //---------------------------------
    std::vector<cinder::vec2> points_from_surface(cinder::Surface8uRef image);
    cv::Mat decimateImage(int original_width, int original_height, cv::Mat image);
    
private:
    std::vector<cinder::vec2> convert_keypoints(const std::vector<cv::KeyPoint> &points);
    cv::Ptr<cv::FastFeatureDetector> _feature_detector;
    int _decimate;
    cinder::vec2 _small_image_size;
};



#endif
