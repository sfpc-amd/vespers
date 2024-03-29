#include "ofMain.h"
#include "Vespers.h"

// i guess this is needed to run on rpi? we will see
// http://www.creativeapplications.net/tutorials/how-to-use-openframeworks-on-the-raspberrypi-tutorial/ls

#ifdef TARGET_OPENGLES
    #define USE_PROGRAMMABLE_RENDERER
#endif

#ifdef USE_PROGRAMMABLE_RENDERER
    #include "ofGLProgrammableRenderer.h"
#endif

int main( ){
    
    #ifdef USE_PROGRAMMABLE_RENDERER
        ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    #endif
    
    #ifndef OF_TARGET_LINUXARMV6L
        ofSetupOpenGL(1024,768,OF_WINDOW);
    #else
        ofSetupOpenGL(840,720,OF_WINDOW);
    #endif


	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new Vespers());

}
