//
//  MeshBuilder.cpp
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#include "MeshCollection.h"
#include "AcuteSelfSetup.h"
#include "cinder/app/App.h"
#include "resources.h"


MeshCollection::MeshCollection(){
    _frameCount = 0;
    setup_shader();
}

MeshCollection::~MeshCollection(){
}


void MeshCollection::add_mesh(std::vector<cinder::vec2> &points, cinder::Surface8uRef &colour_source){
    step_meshes_back();
    _meshes.push_back(std::shared_ptr<AcuteMesh>(new AcuteMesh(points, colour_source, _shader)));
    if(_meshes.size() > MAX_MESHES){
        _meshes.erase(_meshes.begin());
    }
}

void MeshCollection::draw(){
    cinder::gl::ScopedMatrices();
        setup_cameras();
        draw_meshes();
}

void MeshCollection::setup_shader(){
    auto vert = cinder::app::loadResource(MAIN_VERT);
    auto frag = cinder::app::loadResource(MAIN_FRAG);
    auto format = cinder::gl::GlslProg::Format().vertex( vert )
    .fragment( frag )
    .attrib( cinder::geom::CUSTOM_0, "depth");
    
    _shader = cinder::gl::GlslProg::create( format );
}

void MeshCollection::setup_cameras(){
    float rotation = ((sin(_frameCount/100.0))/2.0)*3.1415926/4.0;
    _frameCount++;
    
    cinder::CameraPersp camera;
    
    camera.setPerspective( 25.0f, cinder::app::getWindowAspectRatio(), 1.0f, 3000.0f );
    
    cinder::vec3 center      = cinder::vec3( AcuteSelfCV::RESOLUTION.x/2, AcuteSelfCV::RESOLUTION.y/2, 0 );
    cinder::vec3 mEye        = cinder::vec3( center.x + cos(rotation+3.14159/2.0)*1000, AcuteSelfCV::RESOLUTION.y/2, -(center.z + sin(rotation+3.14159/2.0)*800) );
    cinder::vec3 mUp         = cinder::vec3(0,-1,0);
    
    camera.lookAt    (mEye, center, mUp);
    cinder::gl::setMatrices(camera);
    
    
    cinder::CameraOrtho cam2(0, AcuteSelfCV::RESOLUTION.x, AcuteSelfCV::RESOLUTION.y, 0, -3000, 3000);
    cam2.setViewDirection(cinder::vec3(cos(rotation+3.14159/2.0)*1000, 0, -(center.z + sin(rotation+3.14159/2.0)*800)));
    cinder::gl::setMatrices(cam2);
}

void MeshCollection::draw_meshes(){
    for(auto mesh: _meshes){
        mesh->draw();
    }
}

void MeshCollection::step_meshes_back(){
    for(auto mesh: _meshes){
        mesh->step_back(AcuteSelfCV::MESH_STEPBACK);
    }
}
