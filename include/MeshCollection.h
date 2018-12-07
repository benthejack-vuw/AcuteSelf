//
//  MeshCollection/h
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#ifndef MeshCollection_h
#define MeshCollection_h

#include "AcuteMesh.h"

class MeshCollection{
public:    
    //---------------------------------
    //
    //    constructior and destructor
    //
    //---------------------------------
    MeshCollection();
    ~MeshCollection();
    
    //---------------------------------
    //
    //    methods
    //
    //---------------------------------
    void add_mesh(std::vector<cinder::vec2> &points, cinder::Surface8uRef &colour_source);
    void draw();

private:
    void setup_shader();
    void setup_cameras();
    void draw_meshes();
    void step_meshes_back();
    
    int _frameCount;
    cinder::gl::GlslProgRef _shader;
    std::vector<std::shared_ptr<AcuteMesh>>  _meshes;
};



#endif
