#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "AcuteSelfSetup.h"
#include "BackgroundPlane.h"
#include "resources.h"

using namespace cinder;

BackgroundPlane::BackgroundPlane(){
}

void BackgroundPlane::setup(float cell_size){
    _time = 0;
    _cell_size = cell_size;
    set_up_random_offsets(cell_size);
    gl::VboMeshRef mesh = generate_mesh(cell_size);
    
    setup_shader();
    lights();
    
    _background_batch = gl::Batch::create( mesh, _shader );
}

BackgroundPlane::~BackgroundPlane(){
    for(int i = 0; i < columns(_cell_size); ++i){
        delete[] _random_offsets[i];
    }
    delete[] _random_offsets;
}

void BackgroundPlane::draw(){
    gl::ScopedMatrices();
        camera();
        _background_batch->draw();
}

void BackgroundPlane::lights(){
    
    Light l1 = {
        vec4(AcuteSelfCV::RESOLUTION.x*0.9,AcuteSelfCV::RESOLUTION.y*0.4, 200, 1),
        vec4(1, 1, 1, 1), //colour
        vec4(1,220/255.0,200/255.0,1),      //diffuse
        vec4(1,240/255.0,222/255.0, 1.0),                //specular
        vec4(1,1,1,1),                //ambient
        0,
        0.09,
        0,
        10
    };
    
    Light l2 = {
        vec4(-AcuteSelfCV::RESOLUTION.x*.5,AcuteSelfCV::RESOLUTION.y*0.5, 500, 1),
        vec4(1,1,1,1),  //colour
        vec4(0,0,0,1), //diffuse
        vec4(0,0,0,1),   //specular
        vec4(1,1,1,1),   //ambient
        0,
        0.003,
        0,
        1
    };
    _lights.push_back(l1);
    _lights.push_back(l2);
    
    _lightsUbo = gl::Ubo::create( sizeof(Light)*_lights.size(), _lights.data(), GL_STATIC_DRAW );
    // and bind it to buffer base 0; this is analogous to binding it to texture unit 0
    _lightsUbo->bindBufferBase( 0 );
    // and finally tell the shaders that their uniform buffer 'FormulaParams' can be found at buffer base 0
    _shader->uniformBlock( "LightsBuffer", 0 );
}

void BackgroundPlane::camera(){
    cinder::CameraOrtho cam(0, AcuteSelfCV::RESOLUTION.x, AcuteSelfCV::RESOLUTION.y, 0, -1000, 1000);
    cinder::gl::setMatrices(cam);
}

void BackgroundPlane::step_points(){
    
    auto mappedPos = _background_batch->getVboMesh()->mapAttrib3f( geom::Attrib::POSITION, true );
    auto mappedNormal = _background_batch->getVboMesh()->mapAttrib3f( geom::Attrib::NORMAL, true );
    auto mappedCustom = _background_batch->getVboMesh()->mapAttrib1f( geom::Attrib::CUSTOM_0, true );
    
    for( int i = 0; i < _background_batch->getVboMesh()->getNumVertices()/3; i++ ) {

        vec3 &p1 = *mappedPos;
        mappedPos->z = z_formula(*mappedCustom);
        mappedPos++;
        mappedCustom++;
        
        vec3 &p2 = *mappedPos;
        mappedPos->z = z_formula(*mappedCustom);
        mappedPos++;
        mappedCustom++;
        
        vec3 &p3 = *mappedPos;
        mappedPos->z = z_formula(*mappedCustom);
        mappedPos++;
        mappedCustom++;
        
        *mappedNormal = normal(p1, p2, p3);
        mappedNormal++;
        *mappedNormal = normal(p1, p2, p3);
        mappedNormal++;
        *mappedNormal = normal(p1, p2, p3);
        mappedNormal++;

    }
    
    mappedPos.unmap();
    mappedNormal.unmap();
    mappedCustom.unmap();
    
    _time += AcuteSelfCV::BACKGROUND_SPEED;
}

void BackgroundPlane::setup_shader(){
    auto vert = cinder::app::loadResource(BACKGROUND_VERT);
    auto frag = cinder::app::loadResource(BACKGROUND_FRAG);
    auto format = cinder::gl::GlslProg::Format();
    format.vertex( vert );
    format.fragment( frag );
    //gl::getStockShader(gl::ShaderDef().lambert());
    _shader = cinder::gl::GlslProg::create( format );
}

gl::VboMeshRef BackgroundPlane::generate_mesh(float cell_size){
    std::vector<vec3> pos(columns(cell_size)* rows(cell_size) * 6);
    positions(cell_size, pos);
    
    std::vector<float> vertex_offsets;
    offsets(pos, vertex_offsets);
    
    std::vector<vec3> norm;
    normals(pos, norm);
    
    std::vector<uint16_t> ind;
    indices(pos.size(), ind);
    
    std::vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::NORMAL,   3 ),
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::CUSTOM_0, 1 )
    };
    
    gl::VboMeshRef mesh = gl::VboMesh::create(pos.size(), GL_TRIANGLES, bufferLayout, ind.size());
    mesh->bufferAttrib(geom::Attrib::POSITION, pos.size()*sizeof(vec3), pos.data());
    mesh->bufferAttrib(geom::Attrib::NORMAL, norm.size()*sizeof(vec3), norm.data());
    mesh->bufferAttrib(geom::Attrib::CUSTOM_0, vertex_offsets.size()*sizeof(float), vertex_offsets.data());
    mesh->bufferIndices(ind.size()*sizeof(uint16_t), ind.data());
    
    return mesh;
}

void BackgroundPlane::positions(float cell_size, std::vector<vec3> &pos){
    
    size_t index = 0;
    
    for(int i = 0; i < columns(cell_size); ++i){
        for(int j = 0; j < rows(cell_size); ++j){
            
            if(j%2 == 0){
                pos[index++] = position_at(i, j);
                pos[index++] = position_at(i, j+1);
                pos[index++] = position_at(i+1, j);
                
                pos[index++] = position_at(i+1, j);
                pos[index++] = position_at(i, j+1);
                pos[index++] = position_at(i+1, j+1);
            }else{
                pos[index++] = position_at(i, j);
                pos[index++] = position_at(i, j+1);
                pos[index++] = position_at(i+1, j+1);

                pos[index++] = position_at(i, j);
                pos[index++] = position_at(i+1, j+1);
                pos[index++] = position_at(i+1, j);
            }
        }
    }
    
}

void BackgroundPlane::offsets(std::vector<vec3> &pos, std::vector<float> &out){
    for(auto p: pos){
        out.push_back(p.z);
    }
}


vec3 BackgroundPlane::position_at(int i, int j){
    return vec3(i * _cell_size+row_offset(j)-_cell_size, j * vertical_space(_cell_size), z_position(i, j));
}

float BackgroundPlane::z_position(int i, int j){
    return z_formula(randAt(i, j));
}

float BackgroundPlane::z_formula(float offset){
    return (sin(offset + _time)+1) * (_cell_size/1.0);
}

float BackgroundPlane::row_offset(int row){
    return row%2==0 ? 0 : _cell_size/2.0;
}

void BackgroundPlane::set_up_random_offsets(float cell_size){
    
    _random_offsets = new float*[columns(cell_size)+1];
    cinder::Perlin perlin(8);
    
    for(int i = 0; i <= columns(cell_size); ++i){
        _random_offsets[i] = new float[rows(cell_size)+1];
        for(int j = 0; j <= rows(cell_size); ++j){
            _random_offsets[i][j] = (perlin.noise((float)i / columns(cell_size) * 25,(float)j / rows(cell_size) * 25) + 1) * 3.1415926;
        }
    }
}

float BackgroundPlane::randAt(int i, int j){
    return _random_offsets[i][j];
}

void BackgroundPlane::indices(size_t count, std::vector<uint16_t> &ind){
    for( int i = 0; i < count; ++i ) {
        ind.push_back(i);
    }
}

void BackgroundPlane::normals(const std::vector<vec3>& i_positions, std::vector<vec3> &norm){
    
    vec3 p1, p2, p3, n;
    
    for(int i = 0; i < i_positions.size()/3; i++){
        p1 = i_positions[i*3];
        p2 = i_positions[i*3+1];
        p3 = i_positions[i*3+2];
        n = normal(p1, p2, p3);
        norm.push_back(n);
        norm.push_back(n);
        norm.push_back(n);
    }
}

vec3 BackgroundPlane::normal(const vec3 &p1, const vec3 &p2, const vec3 &p3){
    vec3 U = p3 - p1;
    vec3 V = p2 - p1;
    return normalize(cinder::cross(U, V));
}


int BackgroundPlane::columns(float cell_size){
    return AcuteSelfCV::RESOLUTION.x/cell_size + 1;
}

int BackgroundPlane::rows(float cell_size){
    return AcuteSelfCV::RESOLUTION.y/vertical_space(cell_size)+1;
}

float BackgroundPlane::vertical_space(float cell_size){
    return sin(TWO_PI/6.0)*cell_size;
}


