#ifndef kinectBasic_BlurShadow_h
#define kinectBasic_BlurShadow_h

#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

class BlurShadow{
    
public:
    BlurShadow();
    ~BlurShadow(){};
    
    void draw();
    void update(cinder::Surface8uRef surface);
    void set_texture(cinder::Surface8uRef _surface);
    
private:
    void setup_shader();
    void setup_buffers();
    
    void render_pass(uint8_t pass);
    
    std::vector< cinder::gl::FboRef > _buffers;
    cinder::gl::GlslProgRef           _blur_shader, _alpha_shader;
    cinder::gl::TextureRef            _texture;    
};



#endif
