//
//  MeshBuilder.cpp
//  AcuteSelf2
//
//  Created by Ben Jack on 8/08/18.
//

#include "AcuteMesh.h"
#include "MeshCollection.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "AcuteSelfSetup.h"

using namespace cinder;

AcuteMesh::AcuteMesh(std::vector<cinder::vec2> points, cinder::Surface8uRef colour_source, cinder::gl::GlslProgRef shader):
_depth(0),
_shader(shader)
{
    gl::VboMeshRef mesh = build_mesh(points, colour_source);
    _batch = gl::Batch::create( mesh, _shader );
}

gl::VboMeshRef AcuteMesh::build_mesh(std::vector<cinder::vec2> &points, cinder::Surface8uRef &colour_source){
    
    std::vector<cv::Vec6f> delaunay_points = delaunay_triangulation(points);
    return build_mesh_from_triangulation(delaunay_points, colour_source);
    
}

void AcuteMesh::step_back(float amount){
    
    auto mappedPosAttrib = _batch->getVboMesh()->mapAttrib3f( geom::Attrib::POSITION, false );
    for( int i = 0; i < _batch->getVboMesh()->getNumVertices(); i++ ) {
        vec3 &pos = *mappedPosAttrib;
        mappedPosAttrib->z = pos.z - amount;
        ++mappedPosAttrib;
    }
    mappedPosAttrib.unmap();
    
    _depth++;
}

void AcuteMesh::draw(){
    draw_outlines();
    draw_fill();
}

std::vector<cv::Vec6f> AcuteMesh::delaunay_triangulation(std::vector<cinder::vec2> &points){
    cv::Rect rect(0, 0, cinder::app::getWindowWidth(), cinder::app::getWindowHeight());
    cv::Subdiv2D subdiv(rect); //poorly named delaunay triangulator

    insert_points_into_delaunay_subdiv(points, subdiv);
    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    
    return triangleList;
}

void AcuteMesh::insert_points_into_delaunay_subdiv(std::vector<cinder::vec2> &points, cv::Subdiv2D &subdiv){
    for(auto p: points){
        cv::Point2f pt(p.x,p.y);
        subdiv.insert(pt);
    }
}

cinder::gl::VboMeshRef AcuteMesh::build_mesh_from_triangulation(std::vector<cv::Vec6f> &delaunay_points, cinder::Surface8uRef &colour_source){
    
    Rectf r(0, 0, AcuteSelfCV::RESOLUTION.x, AcuteSelfCV::RESOLUTION.y);
    
    std::vector<uint16_t> indices;
    std::vector<vec3> positions;
    std::vector<ColorAf> colors;
    std::vector<float> customs;
    uint16_t index = 0;
    
    for(auto p: delaunay_points){
        vec2 p1(p[0], p[1]);
        vec2 p2(p[2], p[3]);
        vec2 p3(p[4], p[5]);
        vec2 cent = (p1+p2+p3)/vec2(3.0,3.0);
        
        if(r.contains(p1) && r.contains(p2) && r.contains(p3) && distance(p1, p2) < AcuteSelfCV::MAX_TRI_SIZE && distance(p2, p3) < AcuteSelfCV::MAX_TRI_SIZE && distance(p3, p1) < AcuteSelfCV::MAX_TRI_SIZE)
        {
            positions.push_back(vec3(p1.x,p1.y, 1.0));
            colors.push_back(colour_at_point(cent, colour_source));
            customs.push_back(cinder::randFloat());
            indices.push_back(index++);
            
            positions.push_back(vec3(p2.x,p2.y,  1.0));
            colors.push_back(colour_at_point(cent, colour_source));
            customs.push_back(cinder::randFloat());
            indices.push_back(index++);
            
            positions.push_back(vec3(p3.x,p3.y,  1.0));
            colors.push_back(colour_at_point(cent, colour_source));
            customs.push_back(cinder::randFloat());
            indices.push_back(index++);
        }
    }
    
    std::vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
        gl::VboMesh::Layout().attrib( geom::Attrib::COLOR, 4 ),
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::CUSTOM_0, 1 )
    };
    
    gl::VboMeshRef mesh = gl::VboMesh::create(positions.size(), GL_TRIANGLES, bufferLayout, indices.size());
    mesh->bufferAttrib(geom::Attrib::POSITION, positions.size()*sizeof(vec3), positions.data());
    mesh->bufferAttrib(geom::Attrib::COLOR, colors.size()*sizeof(ColorAf), colors.data());
    mesh->bufferAttrib(geom::Attrib::CUSTOM_0, customs.size()*sizeof(float), customs.data());
    mesh->bufferIndices(indices.size()*sizeof(uint16_t), indices.data());

    return mesh;
}

cinder::ColorA AcuteMesh::colour_at_point(cinder::vec2 &point, cinder::Surface8uRef &colour_source){
    return colour_source->getPixel(point);
}

void AcuteMesh::draw_fill(){
    float alpha = cinder::lmap((float)_depth, 0.0f, 2*AcuteSelfCV::MAX_MESHES/3.0f, 1.0f, 0.0f);
   
    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL ) ;
    _shader->uniform("reverse", true);
    _shader->uniform("glow", 0.0f);
    _shader->uniform("alpha", alpha);
    _batch->draw();
}


float clamp(float in, float min, float max) {
	float top_clamp = in < max ? in : max;
	float clamp = top_clamp > min ? top_clamp : min;
	return clamp;
}

float smoothstep(float edge0, float edge1, float x) {
	// Scale, bias and saturate x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}

void AcuteMesh::draw_outlines(){
    float alpha = (float)(_depth)/(AcuteSelfCV::MAX_MESHES+0.0);
    alpha = alpha - smoothstep(0.5f, 1.0f, alpha);
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;
    _shader->uniform("reverse", true);
    _shader->uniform("glow", 0.15f);
    _shader->uniform("alpha", alpha);
    _batch->draw();
}






