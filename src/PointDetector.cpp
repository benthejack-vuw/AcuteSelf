//
//  MeshBuilder.cpp
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#include "PointDetector.h"
#include "AcuteSelfSetup.h"

PointDetector::PointDetector(int i_decimate):
_decimate(i_decimate)
{
    _feature_detector = cv::Ptr<cv::FastFeatureDetector>(cv::FastFeatureDetector::create());
}

cv::Mat PointDetector::decimateImage(int original_width, int original_height, cv::Mat image){
    int scaledWidth = original_width / _decimate;
    int scaledHeight = original_height / _decimate;
    cv::Mat smallImg( scaledHeight, scaledWidth, CV_8UC1 );
    _small_image_size = cinder::vec2( smallImg.size().width , smallImg.size().height);
    cv::resize( image, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
    return smallImg;
}

std::vector<cinder::vec2> PointDetector::points_from_surface(cinder::Surface8uRef image){
    cv::Mat grayCameraImage( toOcv( *image, CV_8UC1 ) );
    cv::Mat smallImage = decimateImage( image->getWidth(), image->getHeight(), grayCameraImage );
    std::vector<cv::KeyPoint> points;
    _feature_detector->detect(smallImage, points);
    
    return convert_keypoints(points);
}

std::vector<cinder::vec2> PointDetector::convert_keypoints(const std::vector<cv::KeyPoint> &points){
    std::vector<cinder::vec2> out;
    
    
    for(auto i: points){
        cinder::vec2 scaled_pt = (cinder::vec2(i.pt.x, i.pt.y)/_small_image_size)*AcuteSelfCV::RESOLUTION;
        out.push_back(scaled_pt);
    }
    
    return out;
}

