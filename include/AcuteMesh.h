//
//  AcuteMesh.h
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#ifndef AcuteMesh_h
#define AcuteMesh_h

#include "cinder/gl/gl.h"
#include "CinderOpenCv.h"


class AcuteMesh{

public:
    //---------------------------------
    //
    //    constructior and destructor
    //
    //---------------------------------
    AcuteMesh(std::vector<cinder::vec2> points, cinder::Surface8uRef colour_source, cinder::gl::GlslProgRef shader);
    ~AcuteMesh(){}
    
    //---------------------------------
    //
    //    methods
    //
    //---------------------------------
    void step_back(float amount);
    void draw();
    
private:
    
    cinder::gl::VboMeshRef build_mesh(std::vector<cinder::vec2> &points, cinder::Surface8uRef &colour_source);
    std::vector<cv::Vec6f> delaunay_triangulation(std::vector<cinder::vec2> &points);
    void insert_points_into_delaunay_subdiv(std::vector<cinder::vec2> &points, cv::Subdiv2D &subdiv);
    cinder::gl::VboMeshRef build_mesh_from_triangulation(std::vector<cv::Vec6f> &delaunay_points, cinder::Surface8uRef &colour_source);
    cinder::ColorA colour_at_point(cinder::vec2 &point, cinder::Surface8uRef &colour_source);
    
    void draw_fill();
    void draw_outlines();
    
    int                     _depth;
    cinder::gl::BatchRef    _batch;
    cinder::gl::GlslProgRef _shader;
};



#endif
