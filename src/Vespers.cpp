#include "Vespers.h"

//--------------------------------------------------------------
void Vespers::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    ofEnableSmoothing();

	// window dimensions for config mode
	configWindowWidth = 1160;
	configWindowHeight = 480;
	// window dimensions for sequence mode
	sequenceWindowWidth = 640;
	sequenceWindowHeight = 480;
	// camera source dimensions
	camWidth = 640;
	camHeight = 480;
	// processing image dimensions
	procWidth = 320;
	procHeight = 240;
	// config mode gui dimensions
	guiWidth = 200;

	// toggle fullscreen mode
	isFullScreen = false;

	// shader vars
	period = 10;

	// just targeting opengl for now
	camShader.load("shadersGL2/camShader");
	starShader.load("shadersGL2/starShader");

	// set our framerate and initialize video grabber
	cam.setDesiredFrameRate(30);
	cam.initGrabber(camWidth,camHeight);

	// by default, set the north star to be in the center
	// since we're doubling canvas width/height in the
	// final version, this should put the star in the center
	northStar = ofPoint(camWidth/2, camHeight/2);

	mainFbo.allocate(camWidth, camHeight);
	mainFbo.begin();
		ofClear(0, 0, 0, 0);
	mainFbo.end();

	starsFbo.allocate(camWidth, camHeight);
	starsFbo.begin();
        ofClear(0, 0, 0, 0);
	starsFbo.end();

    drawGui = false;
	// setup gui
	gui.setup();
	gui.add(prepLabel.setup("// IMAGE PREP", ""));

	// image prep
	gui.add(useBilateralFilter.setup("Smoothing", true));
	// gui.add(bfDiameter.setup("Diameter", 10, 0, 20));
	bfDiameter = 10;
	// gui.add(bfSigmaColor.setup("Sigma Color", 140.0, 0.0, 300.0));
	bfSigmaColor = 140.0;
	// gui.add(bfSigmaSpace.setup("Sigma Space", 140.0, 0.0, 300.0));
	bfSigmaSpace = 140.0;
	// gui.add(useNormalize.setup("Normalize", true));
	useNormalize = true;
	// gui.add(useManualThreshold.setup("Manual threshold", true));
	gui.add(thresh.setup("Thresh level", 100.0, 0.0, 255.0));
	// gui.add(useAutoThreshold.setup("Auto threshold", false));
	// gui.add(useDilate.setup("Dilate", true));
	gui.add(dilateIterations.setup("Dilate iterations", 1, 0, 10));
	// gui.add(useErode.setup("Erode", true));
	gui.add(erodeIterations.setup("Erode iterations", 1, 0, 10));
	gui.add(dilateErodeInvert.setup("Invert order", false));

	// stars
	gui.add(starsLabel.setup("// STARS", ""));
	gui.add(showStars.setup("Show stars", true));
	gui.add(maxStarRadius.setup("Max star radius", 2.0, 0.0, 10.0));
	gui.add(minStarRadius.setup("Min star radius", 0.5, 0.0, 10.0));
	gui.add(maxStars.setup("Max star count", 40, 1, 100));
	// gui.add(maxStarSize.setup("max star size", 1.5, 0.0, 10.0));
	// gui.add(minStarSize.setup("min star size", 0.5, 0.0, 10.0));
	gui.add(qualityLevel.setup("Star quality level", 0.01, 0.0, 1.0));
	gui.add(minDistance.setup("Star min distance", 20.0, 0.0, 100.0));
	gui.add(blockSize.setup("Star block size", 3, 0, 10));
	gui.add(drawStarsAsPoints.setup("Draw points", false));

	timeline.setup();
	timeline.setFrameRate(30);
	timeline.setDurationInFrames(300);
	timeline.setLoopType(OF_LOOP_NORMAL);

	timeline.addCurves("Vignette Radius", ofRange(0, 1));
	timeline.addCurves("Stars Alpha", ofRange(0, 1));
    timeline.addBangs("Capture Stars");
    ofAddListener(timeline.events().bangFired, this, &Vespers::receivedBang);
    timeline.play();

}

//--------------------------------------------------------------
void Vespers::update(){

	cam.update();

	if(cam.isFrameNew()) {
		// update all images
		base.update();
		gray.update();
	}
}

//--------------------------------------------------------------
void Vespers::draw(){
    ofClear(0,0,0);


 	// resize window for sequence mode
	if(!isFullScreen && sequenceMode && ofGetWidth() != sequenceWindowWidth) {
		ofSetWindowShape(sequenceWindowWidth, sequenceWindowHeight);
	// resize window for config mode
	} else if (!isFullScreen && !sequenceMode && ofGetWidth() != configWindowWidth) {
		ofSetWindowShape(configWindowWidth, configWindowHeight);
	}


    // draw stars
    Vespers::drawStars(
       ofColor(255, 255, 255)
       , minStarRadius
       , maxStarRadius
       );

    // render the main fbo
    mainFbo.begin();
        ofClear(0, 0, 0, 255);
        camShader.begin();

            // set uniforms
            camShader.setUniform2f("center", ofMap(northStar.x, 0, ofGetWidth(), 0, 1), ofMap(northStar.y, 0, ofGetHeight(), 0, 1));
            camShader.setUniform1f("radius", timeline.getValue("Vignette Radius"));
            camShader.setUniform1f("softness", 1);
            camShader.setUniform1f("opacity", 1.0);
            camShader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
            camShader.setUniform1f("time", ofMap(mouseY, 0, ofGetHeight(), 0, 1));
            // draw our image plane
            cam.draw(0, 0);

        // end the shader
        camShader.end();
    mainFbo.end();




    // show gui
    if(drawGui) {
        gui.setPosition(0, 0);
        gui.draw();
    }


    ofPushMatrix();
        // move everything to the right of gui
        ofTranslate(guiWidth, 0);
        // draw the main image FBO
        mainFbo.draw(0, 0);
        // switch to additive blend mode
        starsFbo.draw(0, 0);
        // draw base image in greyscale
        base.draw(sequenceWindowWidth,0);
        // draw thresholded image
        gray.draw(sequenceWindowWidth, procHeight);

    ofPopMatrix();


    // draw the Hud
    Vespers::drawHud(guiWidth + 5, 15);

    timeline.draw();

}

void Vespers::receivedBang(ofxTLBangEventArgs& bang) {
    ofLogNotice("Bang fired from track " + bang.track->getName());
    if(bang.track->getName() == "Capture Stars"){
        Vespers::findStars();
    }
}


void Vespers::drawHud(int x, int y) {
    // draw HUD
    string hud = "";
    // display frames per second
    hud += ofToString(ofGetFrameRate(), 2) + " fps";
    // display elapsed time
    hud += " / " + ofToString(t, 1) + "sec";
    // draw hud
    ofDrawBitmapStringHighlight(hud, x, y);
}


void Vespers::findStars() {
	// create base image
    VespersCv::createBaseImage(
		cam
		, base
		, procWidth
		, procHeight
		, true
	);

	// prep image for corner detection
	VespersCv::prepImage(
		base
		, gray
		, useBilateralFilter
		, bfDiameter
		, bfSigmaColor
		, bfSigmaSpace
		, thresh
		, dilateIterations
		, erodeIterations
		, dilateErodeInvert
	);


	stars = VespersCv::findPoints(
		gray,
		maxStars,
		qualityLevel,
		minDistance,
		blockSize,
		camWidth,
		camHeight
	);
	stars.push_back(northStar);
}

void Vespers::drawStars(
	ofColor color
	, float minRadius
	, float maxRadius
) {

    float starRadius;

    starsFbo.begin();
        ofClear(0,0,0,0);

        starShader.begin();

            starShader.setUniform3f("color",
                ofMap(color.r, 0, 255, 0, 1)
                , ofMap(color.g, 0,  255, 0, 1)
                , ofMap(color.b, 0,  255, 0, 1)
            );

            starShader.setUniform1f("alpha", timeline.getValue("Stars Alpha"));


            if(stars.size() > 0) {
                for(int i = 0; i< stars.size(); i++) {
                    // calculate radius based on star "quality" (order) and
                    // star radius max/min
                    starRadius = (((maxRadius-minRadius)/stars.size())*i)+minRadius;

                    if(!drawStarsAsPoints) {
                        // we are drawing this at 2x scale
                        ofCircle(stars[i], starRadius);
                    } else {
                        ofBeginShape();
                            ofVertex(stars[i].x, stars[i].y, ofMap(i, 0, stars.size(), 9, 1));
                        ofEndShape();
                    }
                }
            }
        starShader.end();

    starsFbo.end();
}


//--------------------------------------------------------------
void Vespers::keyPressed(int key){

	switch(key) {
		case 's': sequenceMode = !sequenceMode; break;
        case 't' :
            drawGui = !timeline.toggleShow();
            break;
//		case ' ' :
//			Vespers::findStars();
//			Vespers::resetSequenceTime();
//			break;
//		case 'f': isFullScreen = !isFullScreen; ofToggleFullscreen(); break;
	}
}

//--------------------------------------------------------------
void Vespers::keyReleased(int key){

}

//--------------------------------------------------------------
void Vespers::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void Vespers::mouseDragged(int x, int y, int button){
	int transX = x-guiWidth;
	int transY = y-procHeight;

	if(canvas.inside(transX, transY)) {
		northStar.x = transX;
		northStar.y = transY;
	}
}

//--------------------------------------------------------------
void Vespers::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void Vespers::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void Vespers::windowResized(int w, int h){

}

//--------------------------------------------------------------
void Vespers::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void Vespers::dragEvent(ofDragInfo dragInfo){

}