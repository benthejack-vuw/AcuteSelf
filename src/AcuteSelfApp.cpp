#include "AcuteSelfSetup.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "PointDetector.h"
#include "MeshCollection.h"
#include "CinderOpenCv.h"
#include "BackgroundPlane.h"
#include "cinder/Font.h"
#include "BlurShadow.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AcuteSelf_CVApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void convert_pts(const vector<cv::KeyPoint> &points, vector<vec2> &out);
    void keyDown(KeyEvent event) override;

  private:
    
    std::shared_ptr<BackgroundPlane> _background;
    std::shared_ptr<PointDetector> _detector;
    std::shared_ptr<FrameSubtractor> _subtractor;
    std::shared_ptr<MeshCollection> _mesh_collection;
    std::shared_ptr<BlurShadow> _blur_shadow;

    int _frameCount;
    gl::TextureRef        mTexture;

    std::vector<ci::vec2> _keyPoints;
    cinder::Font mFont;

};

void AcuteSelf_CVApp::keyDown( KeyEvent event ) {
    if(event.getCode() == 27){//esc key pressed
        quit();
    }
}

void AcuteSelf_CVApp::setup()
{
   
    setFullScreen( true );
    setFrameRate(24);
    
    _frameCount = 0;
    _background = std::shared_ptr<BackgroundPlane>(new BackgroundPlane());
    _subtractor = std::shared_ptr<FrameSubtractor>(new FrameSubtractor(AcuteSelfCV::CAMERA_RESOLUTION.x, AcuteSelfCV::CAMERA_RESOLUTION.y));
    _detector = std::shared_ptr<PointDetector>(new PointDetector(AcuteSelfCV::DECIMATION));
    _mesh_collection = std::shared_ptr<MeshCollection>(new MeshCollection());
    _blur_shadow = std::shared_ptr<BlurShadow>(new BlurShadow());
    
    _background->setup(10);
    _blur_shadow->set_texture(_subtractor->difference2());
    
	//mTexture = gl::Texture::create(AcuteSelfCV::CAMERA_RESOLUTION.x, AcuteSelfCV::CAMERA_RESOLUTION.y);
    hideCursor();
}

void AcuteSelf_CVApp::mouseDown( MouseEvent event )
{
}

void AcuteSelf_CVApp::update()
{
    _subtractor->update();
    _blur_shadow->update(_subtractor->difference2());
	//mTexture->update(*(_subtractor->colorBuffer()));
    _keyPoints = _detector->points_from_surface(_subtractor->difference());
    if(_keyPoints.size() > 10 ){
        cinder::Surface8uRef buff = _subtractor->colorBuffer();
        _mesh_collection->add_mesh(_keyPoints, buff);
    }
    
    
}

void AcuteSelf_CVApp::draw()
{
	gl::clear( Color( 1, 1, 1) );
 
    _background->draw();
    _blur_shadow->draw();
    _mesh_collection->draw();

	//gl::draw(mTexture);
}

CINDER_APP( AcuteSelf_CVApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
