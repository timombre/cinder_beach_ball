#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include <typeinfo>
#include <math.h>
#include <cstdlib>
#include <ctime>


using namespace ci;
using namespace ci::app;


gl::TextureRef backgroundImage;

float ballRadius = 30.0;
float frictionFactor = 0.99; // air drag
float elasticity = 0.7;  // some energy is absorbed when bouncing on the floor
float gAcceleration = 0.4; // gravity
float maxBounceRollVelocity = 1.0;  // it's gotta stop somehow


// actual coordinates used for integration
float x;
float y;
float vx;
float vy;


// We'll create a new Cinder Application by deriving from the App class.
class BeachBallApp : public App {
  public:

  	void setup() override;
  	void keyDown( KeyEvent event ) override;
    void update() override;
	void draw() override;

  private:
	// This will maintain a list of points which we will draw line segments between
	std::vector<vec2> mPoints;
};


void prepareSettings( BeachBallApp::Settings* settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}


void BeachBallApp::setup()
{
	try {
		backgroundImage = gl::Texture::create(loadImage(loadResource("background_beach.jpg")));
	}
	catch( Exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image.", exc );
	}

	
	std::srand((unsigned) time(0));

	float initial_x = (float) rand() / RAND_MAX; // between 0 and 1
	float initial_y = (float) rand() / RAND_MAX; 
	float initial_vx = (float) rand() / RAND_MAX;
	float initial_vy = (float) rand() / RAND_MAX;
	
	x = app::getWindowWidth() * initial_x; //adaptation to screen size
	y = app::getWindowHeight() * initial_y; //adaptation to screen size

	std::cout << initial_x << " " << initial_y << std::endl;


	std::cout << x << " " << y << std::endl;
	vx = initial_vx * 100.0; // realistic velocity scaling
	vy = initial_vy * -100.0; //shoot upwards

	std::cout << vx << " " << vy << std::endl;

}

void BeachBallApp::update()
{
	// Boundary conditions
	if (x < ballRadius || x > app::getWindowWidth() - ballRadius) { vx = -1.0 * vx;} // Lateral reflexion

	if (y < ballRadius) {vy = -1.0 * vy;} // unrealistic sky relection to avoid escaping the screen

	else if (y > app::getWindowHeight() - ballRadius){
		if (abs(vx) < maxBounceRollVelocity) { vx = 0.0 ;}
		if (abs(vy) < maxBounceRollVelocity) { vy = 0.0 ;}
		else {vy = -1.0 * elasticity * vy;}
	}


    // Equation of motion integration
	x = x + vx;
	y = y + vy;

	vx = frictionFactor * vx;
	vy = frictionFactor * vy + gAcceleration;

	// Console logs

	std::cout << "coordinates: " << x << " " << y << std::endl;

	std::cout << "velocities: " << vx << " " << vy << std::endl;

	
}


void BeachBallApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		// Toggle full screen when the user presses the 'f' key.
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_SPACE ) {
		// Clear the list of points when the user presses the space bar.
		mPoints.clear();
	}
	else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
	}
}


void BeachBallApp::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers.


	gl::clear();

    gl::draw(backgroundImage, getWindowBounds());  // Nice background
    gl::color(0, 1, 0); // Green ball
    gl::drawSolidCircle( glm::vec2(x, y), ballRadius);
    gl::color(1, 1, 1); //reset color to avoid green screen


	gl::end();
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BeachBallApp, RendererGl, prepareSettings )
