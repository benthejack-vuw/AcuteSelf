#ifndef kinectBasic_BackgroundPlane_h
#define kinectBasic_BackgroundPlane_h


#define TWO_PI 6.28318
#define SPEED 0.01
#define AMOUNT = 10


#include "cinder/Utilities.h"
#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "cinder/Perlin.h"
#include "cinder/Camera.h"
#include "light.h"

class BackgroundPlane{

public:

    BackgroundPlane();
    ~BackgroundPlane();
    
    void setup(float cell_size);
    void step_points();
    void draw();

private:
    cinder::gl::VboMeshRef generate_mesh(float cell_size);
    void setup_shader();
    
    int columns(float cell_size);
    int rows(float cell_size);
    
    void positions(float cell_size, std::vector<cinder::vec3> &pos);
        cinder::vec3 position_at(int i, int j);
        float row_offset(int index);
        float vertical_space(float cell_size);
        void set_up_random_offsets(float cell_size);
        float randAt(int i, int j);
        float z_position(int i, int j);
        float z_formula(float offset);
    
    void offsets(std::vector<cinder::vec3> &pos, std::vector<float> &out);
    void indices(size_t count, std::vector<uint16_t> &ind);
    void normals(const std::vector<cinder::vec3>& i_positions, std::vector<cinder::vec3> &norm);
        cinder::vec3 normal(const cinder::vec3 &p1, const cinder::vec3 &p2, const cinder::vec3 &p3);


    void lights();
    void camera();
    
    
    float _time;
    float _cell_size;
    float** _random_offsets;
    std::vector<Light>      _lights;
    cinder::gl::GlslProgRef _shader;
    cinder::gl::BatchRef    _background_batch;
    cinder::gl::UboRef      _lightsUbo;

};



#endif
