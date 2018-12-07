#include "AcuteSelfSetup.h"
#include "BlurShadow.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/scoped.h"
#include "Resources.h"
 

using namespace cinder;

BlurShadow::BlurShadow() {
	setup_shader();
	setup_buffers();
}


void BlurShadow::update(Surface8uRef surface) {
	_texture->update(*surface);
}

void BlurShadow::draw() {
	render_pass(0);
	render_pass(1);

	vec2 expansion = vec2(app::getWindowSize().x / 25.0, app::getWindowSize().y / 25.0);

	CameraOrtho cam(0, cinder::app::getWindowHeight(), cinder::app::getWindowHeight(), 0, -1000, 1000);
	gl::ScopedMatrices();
	gl::setMatricesWindow(app::getWindowSize());

	gl::ScopedGlslProg shaderScp(_alpha_shader);
	_buffers[1]->bindTexture();
	gl::enableAlphaBlending();
	gl::drawSolidRect(Rectf(-expansion.x, -expansion.y, cinder::app::getWindowSize().x + expansion.x, cinder::app::getWindowSize().y + expansion.y));
}

void BlurShadow::render_pass(uint8_t pass) {
	vec2 horizontal(1.0f / _buffers[0]->getWidth(), 0);
	vec2 vertical(0, 1.0f / _buffers[0]->getHeight());
	_blur_shader->uniform("sampleOffset", pass == 0 ? horizontal : vertical);

	if (pass == 0) {
		_texture->bind();
	}
	else {
		_buffers[0]->bindTexture();
	}

	gl::FboRef fbo = _buffers[pass];
	gl::ScopedFramebuffer fbScp(fbo);
	gl::ScopedViewport scpViewport(fbo->getSize());
	gl::ScopedMatrices();
	gl::setMatricesWindow(fbo->getSize());
	gl::ScopedGlslProg prog(_blur_shader);
	gl::drawSolidRect(Rectf(0, 0, fbo->getSize().x, fbo->getSize().y));
}

void BlurShadow::set_texture(Surface8uRef _surface) {
	_texture = gl::Texture::create(*_surface);
}

void BlurShadow::setup_shader() {
	auto vert = app::loadResource(EMPTY_VERT);
	auto frag = app::loadResource(BLUR_FRAG);
	auto format = gl::GlslProg::Format();
	format.vertex(vert);
	format.fragment(frag);
	_blur_shader = gl::GlslProg::create(format);
	_blur_shader->uniform("tex0", 0);

	auto alpha_frag = app::loadResource(INV_ALPHA_FRAG);
	auto format2 = gl::GlslProg::Format();
	format2.vertex(vert);
	format2.fragment(alpha_frag);
	_alpha_shader = gl::GlslProg::create(format2);
	_alpha_shader->uniform("tex0", 0);
}

void BlurShadow::setup_buffers() {

	gl::FboRef buffer1 = gl::Fbo::create(cinder::app::getWindowSize().x, cinder::app::getWindowSize().y, true, false, false);
	gl::FboRef buffer2 = gl::Fbo::create(cinder::app::getWindowSize().x, cinder::app::getWindowSize().y, true, false, false);
	_buffers.push_back(buffer1);
	_buffers.push_back(buffer2);

}


