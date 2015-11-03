#include "Vespers.h"

//--------------------------------------------------------------
void Vespers::setup(){
    
    // basic oF / GL setup
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetLogLevel(OF_LOG_VERBOSE);

    // camera source dimensions
	camWidth = 640;
	camHeight = 480;
	// processing image dimensions
	procWidth = 320;
	procHeight = 240;
	// config mode gui dimensions
	guiWidth = 200;
	// window dimensions for config mode
	configWindowWidth = 1160;
	configWindowHeight = 480;
	// window dimensions for sequence mode
	sequenceWindowWidth = 800;
	sequenceWindowHeight = 480;
    

    // defaults based on target
    #ifdef TARGET_RASPBERRY_PI
        sequenceMode = true;
    #else
        sequenceMode = false;
    #endif
    
    isFullScreen = false;
    drawGui = false;
    
    // load shaders!
    #ifdef TARGET_OPENGLES
        camShader.load("shadersES2/camShader");
        starShader.load("shadersES2/starShader");
        afterImageShader.load("shadersES2/afterImageShader");
        transitionShader.load("shadersES2/transitionShader");
    #else
        camShader.load("shadersGL2/camShader");
        starShader.load("shadersGL2/starShader");
        afterImageShader.load("shadersGL2/afterImageShader");
        transitionShader.load("shadersGL2/transitionShader");
    #endif

    // sample image
    useSampleImage = false;
    sampleImage.loadImage("vespers-sample-image.png");
    sampleImage.resize(camWidth, camHeight);
	sampleImage.setImageType(OF_IMAGE_COLOR);
    
    // 3d glasses setup
	glasses.setup(400, 400);
//    glasses.disableMouseInput();
//    glasses.disableMouseMiddleButton();
	glasses.setScale(1, 1, 1);
	glasses.setPhysicalFocusDistance(120);
	glasses.setFocusDistance(50);
	glasses.setNearClip(0.1);
    
	// camera setup
	cam.setDesiredFrameRate(30);
	cam.initGrabber(camWidth,camHeight);

    // set up frame buffers
	mainFbo.allocate(camWidth, camHeight);
	starsFbo.allocate(camWidth, camHeight);
    
    // set up camera for stars view
	starsCam.setScale(1,-1,1);

    // GUI SETUP
	gui.setup();
	gui.add(prepLabel.setup("IMAGE PREP", ""));

	// image prep
	gui.add(useBilateralFilter.setup("Smoothing", true));
	gui.add(thresh.setup("Thresh level", 100.0, 0.0, 255.0));
	gui.add(dilateIterations.setup("Dilate iterations", 1, 0, 10));
	gui.add(erodeIterations.setup("Erode iterations", 1, 0, 10));
	gui.add(dilateErodeInvert.setup("Invert order", false));

	// stars
	gui.add(starsLabel.setup("STARS", ""));
    gui.add(alwaysUpdateStars.setup("Always update stars", false));
	gui.add(maxStarRadius.setup("Max star radius", 1.75, 0.0, 10.0));
	gui.add(minStarRadius.setup("Min star radius", 0.75, 0.0, 10.0));
	gui.add(maxStars.setup("Max star count", 40, 1, 100));
	gui.add(qualityLevel.setup("Star quality level", 0.01, 0.01 , 1.0));
	gui.add(minDistance.setup("Star min distance", 10.0, 0.0, 100.0));
    gui.add(maxRandomStars.setup("Max Random Stars", 100, 0, 500 ));
    gui.add(orbitStars.setup("Orbit stars", true));
	gui.add(blockSize.setup("Star block size", 3, 0, 10));
    gui.add(starsCamPan.setup("Stars Cam Pan", 0.f, 0.f, 360.f ));
    gui.add(starsCamZoom.setup("Stars Cam Zoom", 415.f, 0.f, 1000.f ));

    // TIMELINE SETUP
	timeline.setup();
	timeline.setFrameRate(30);
	timeline.setDurationInFrames(1000);
    
    timeline.setPageName("Page Main Sequence");
    timeline.addPage("Page Transition");

    timeline.setCurrentPage("Page Main Sequence");
        timeline.setLoopType(OF_LOOP_NORMAL);
        timeline.addCurves("Vignette Radius", ofRange(0, 1));
        timeline.addCurves("Stars Alpha", ofRange(0, 1));
        timeline.addCurves("Random Stars", ofRange(0, 1));
        timeline.addCurves("Stars Rotation", ofRange(-90.f,90.f));
        timeline.addBangs("Capture Stars");
        timeline.addColors("Video Color");
        timeline.addCurves("Color Mix", ofRange(0,1));
        timeline.addCurves("AfterImage Alpha", ofRange(0,1));
        timeline.addCurves("AfterImage Blur", ofRange(0,50));
        timeline.addCurves("AfterImage Threshold", ofRange(0,3));

    timeline.setCurrentPage("Page Transition");
        float moireMaxAmount = ofRandom(50.0, 100.0);

        timeline.addCurves("Triangle Z", ofRange(-10000.0, 1000.0));
        timeline.addCurves("Moire Spacing", ofRange(1.0, 45.0));
        timeline.addCurves("Moire Amount", ofRange(1.0, moireMaxAmount));
        timeline.addCurves("Text Opacity", ofRange(1.0, 255.0));
        timeline.addCurves("Shape Opacity", ofRange(0.0, 1.0));
        // wanted this to be switches but it wasnt working
//        timeline.addSwitches("Tr Enabled");
    timeline.setCurrentPage("Page Main Sequence");

    // setup listender for bangs
    ofAddListener(timeline.events().bangFired, this, &Vespers::receivedBang);
    
    // play by default
    timeline.play();
    
    // set number of random stars
    randomStars.resize(maxRandomStars);

    // by default, set the north star to be in the center
	// since we're doubling canvas width/height in the
	// final version, this should put the star in the center
	northStar = ofPoint(camWidth/2, camHeight/2);

    #ifdef TARGET_RASPBERRY_PI
        timeline.hide();
    #endif
}

//--------------------------------------------------------------
void Vespers::update(){

	cam.update();
    
    if(timeline.getValue("Shape Opacity")  > 0) {
        updateTransition();
    }
    
    
    // if we're calibrating star display,
    // this will always update them
    if(alwaysUpdateStars) {
        Vespers::findStars();
        Vespers::setAfterImage();
    }
    
    // @todo: should this wrap the whole thing?
	if(cam.isFrameNew()) {
		// update all images
		base.update();
		gray.update();
	}
    
 	// set window mode
	if(!isFullScreen && sequenceMode && ofGetWidth() != sequenceWindowWidth) {
        
        // resize window for sequence mode
		ofSetWindowShape(sequenceWindowWidth, sequenceWindowHeight);
        
	} else if (!isFullScreen && !sequenceMode && ofGetWidth() != configWindowWidth) {
    
        // resize window for config mode
		ofSetWindowShape(configWindowWidth, configWindowHeight);
	}
    
    
    
    // update stars!
    if(timeline.getValue("Stars Alpha") > 0 || (bool) alwaysUpdateStars) {
        
        float alpha = (bool) alwaysUpdateStars ? 1.0 : timeline.getValue("Stars Alpha");
        
        starShader.begin();
        starShader.setUniform1f("alpha", alpha);
        starShader.setUniform1f("time", ofGetElapsedTimef());
        starShader.end();
        
        // draw stars
        Vespers::drawStars(ofColor(255), minStarRadius, maxStarRadius);

    }
    
    // render the main fbo if the vignette isn't completely closed
    if(timeline.getValue("Vignette Radius") > 0) {
    
        // fbo for the main composition
        mainFbo.begin();
        
            // clear it!
            ofClear(0, 0, 0, 255);
        
            // stup the camera image shader
            camShader.begin();

                // translate star coordinates for shader
                float starX = ofMap(northStar.x, 0, sequenceWindowWidth, 0, 1);
                float starY = ofMap(northStar.y, 0, sequenceWindowHeight, 0, 1);

                // overlay color
                ofColor camColor = timeline.getColor("Video Color");
        
                // set uniforms
                camShader.setUniform2f("center", starX, starY);
                camShader.setUniform1f("radius", timeline.getValue("Vignette Radius"));
                camShader.setUniform1f("softness", 1);
                camShader.setUniform1f("opacity", 1.0);
                camShader.setUniform2f("resolution", camWidth, camHeight);
                camShader.setUniform3f("inputColor",
                   ofMap(camColor.r, 0, 255, 0, 1),
                   ofMap(camColor.g, 0, 255, 0, 1),
                   ofMap(camColor.b, 0, 255, 0, 1)
                );
                camShader.setUniform1f("colorMix", timeline.getValue("Color Mix"));
                camShader.setUniform1f("time", ofMap(mouseY, 0, ofGetHeight(), 0, 1));
        
        
                // either draw the sample image or the actual one
                if(useSampleImage) {
                    // draw the sample image
                    sampleImage.draw(0, 0);
                } else {
                    // draw the actual image
                    cam.draw(0, 0);
                }
                
                
                // end the shader
            camShader.end();
        mainFbo.end();
        
    }
    
    
    // setup the after image shader only if the alpha is > 0
    if (timeline.getValue("AfterImage Alpha") > 0) {
        
        afterImageShader.begin();
            afterImageShader.setUniform1f("alpha", timeline.getValue("AfterImage Alpha"));
            afterImageShader.setUniform1f("blur", timeline.getValue("AfterImage Blur"));
            afterImageShader.setUniform1f("threshold", timeline.getValue("AfterImage Threshold"));
        afterImageShader.end();
    }
    
    
    
    // if we're in sequence mode that means
    // set up the stereo camera so we can
    // show in 3d
    if(sequenceMode) {
    
        // update camera
        glasses.update(ofRectangle(0, 0, 400, 400));
        
        // capture left eye in fbo
        glasses.beginLeft();
            drawSequence();
            drawTransition();
        glasses.endLeft();
        
        // capture right eye in fbo
        glasses.beginRight();
            drawSequence();
            drawTransition();
        glasses.endRight();
    }
    
}

void Vespers::updateTransition() {
   triangleZ = timeline.getValue("Triangle Z");
   moireSpacing = timeline.getValue("Moire Spacing");
   moireAmount = timeline.getValue("Moire Amount");
   textOpacity = timeline.getValue("Text Opacity");
   shapeOpacity = timeline.getValue("Shape Opacity");


    if(shapeOpacity > 0) {

       transitionShader.begin();
            transitionShader.setUniform4f(
                "iMouse"
                , mouseX
                , mouseY
                , 0.0
                , 0.0
            );
            transitionShader.setUniform3f(
                "iResolution"
                , ofGetWidth()
                , ofGetHeight()
                , 0.0
            );
            transitionShader.setUniform1f(
                "iGlobalTime"
                , ofGetElapsedTimef()
            );// elapsed time
            transitionShader.setUniform1i(
                "moireSpacing"
                , moireSpacing
            );
            transitionShader.setUniform1i(
                "moireAmount"
                , moireAmount
            );
            transitionShader.setUniform1f(
                "shapeOpacity"
                , shapeOpacity
            );
        transitionShader.end();
    }
}


//--------------------------------------------------------------
void Vespers::draw(){
    ofClear(0);
    
    if(sequenceMode) {
    
        // if in sequence mode, we draw each eye
        // from the stereo camera separately
        glasses.drawLeft(0, 80, 400, 400);
        glasses.drawRight(400, 80, 400, 400);
    } else {
    
        // otherwise just draw normally
        drawSequence();
        drawTransition();
        
        // show gui (won't draw if timeline is enabled)
        if(drawGui) {
            gui.setPosition(camWidth, 0);
            gui.draw();
        }
        // draw base image in greyscale
        base.draw(camWidth+guiWidth,0);
        // draw thresholded image
        gray.draw(camWidth+guiWidth, procHeight);
        // draw the Hud
        Vespers::drawHud(5, ofGetHeight()-5);
        // draw the timeline
        timeline.draw();
    }
}


void Vespers::drawSequence() {
    ofClear(0);

    // in sequence mode, translating to offset camera
    if(sequenceMode) {
        ofTranslate(-(camWidth/2), -(camHeight/2));
    }

    // this is the viewing area
    canvas = ofRectangle(0, 0, sequenceWindowWidth/2, sequenceWindowHeight);

    // draw the main image FBO
    if(timeline.getValue("Vignette Radius") > 0) {
        mainFbo.draw(0, 0);
    }

    // use additive blend mode
    ofEnableBlendMode(OF_BLENDMODE_ADD);

        // draw afterimage
        if (timeline.getValue("AfterImage Alpha") > 0) {
            afterImageShader.begin();
                afterImage.draw(0, 0);
            afterImageShader.end();
        }
        
        // draw stars
        if(timeline.getValue("Stars Alpha") > 0 || (bool) alwaysUpdateStars) {
            // seemed to fix the star flickering problem when
            // i set up the shader here rather than in the fbo
            starShader.begin();
                starsFbo.draw(0, 0);
            starShader.end();
        }
    ofDisableBlendMode();
}

void Vespers::drawTransition() {
//    ofBackground(0);
       float radius = 2*(camHeight/5);
    
        if(timeline.getValue("Shape Opacity")  > 0) {
        

            transitionShader.begin();
        
                ofNoFill();

        
                for(int i = 0; i < abs(moireAmount); i++) {
                
                    ofPushMatrix();
                    ofSetColor(255, 255, 255, 0.5);

                    ofTranslate(camWidth/2, camHeight/2);

                    ofPoint p1 = ofPoint(cos(ofDegToRad(0-90))*radius, sin(ofDegToRad(0-90))*radius, triangleZ);
                    ofPoint p2 = ofPoint(cos(ofDegToRad(120-90))*radius, sin(ofDegToRad(120-90))*radius, triangleZ);
                    ofPoint p3 = ofPoint(cos(ofDegToRad(240-90))*radius, sin(ofDegToRad(240-90))*radius, triangleZ);
                
            
                    ofPushMatrix();
                        ofRotate(i*moireAmount*moireSpacing, 0, 0, 1);
                        ofTriangle(p1, p2, p3);
                    ofPopMatrix();
                    
                    ofLine(ofPoint(0, -camHeight, 0), p1);
                    ofLine(ofPoint(camWidth, camHeight, 0), p2);
                    ofLine(ofPoint(-camWidth, camHeight, 0), p3);
     
                    ofPopMatrix();
                    
               }
        
        

            transitionShader.end();

            ofSetColor(255);
        }
}

void Vespers::setAfterImage() {
        ofPixels pix;
    
        if(useSampleImage) {
            pix = sampleImage.getPixelsRef();
        } else {
            pix = cam.getPixelsRef();
        }
        
        afterImage.setFromPixels(pix);
}

void Vespers::receivedBang(ofxTLBangEventArgs& bang) {
    ofLogNotice("Bang fired from track " + bang.track->getName());
    if(bang.track->getName() == "Capture Stars"){
        Vespers::findStars();
        Vespers::setAfterImage();
    }
}


void Vespers::drawHud(int x, int y) {
    
    // draw HUD
    string hud = "";
    // display frames per second
    hud += ofToString(ofGetFrameRate(), 2) + " fps";
    // display elapsed time
    hud += " / " + ofToString(timeline.getCurrentTime(), 1) + "/" + ofToString(timeline.getDurationInSeconds()) +" sec";
    // draw hud
    ofDrawBitmapStringHighlight(hud, x, y);
}


void Vespers::findStars() {

    // some default CV values
	float bfDiameter = 10;
	float bfSigmaColor = 140.0;
	float bfSigmaSpace = 140.0;
	float useNormalize = true;
    ofPixels pix;
    
    if(useSampleImage) {
        pix = sampleImage.getPixelsRef();
    } else {
        pix = cam.getPixelsRef();
    }
    
    
    // add extra stars
    randomStars.resize(maxRandomStars);
    for(int j = 0; j < randomStars.size(); j++) {
        randomStars[j] = ofPoint(ofRandom(camWidth*1.5), ofRandom(camHeight*1.5), 0);
    }
    
	// create base image
    VespersCv::createBaseImage(
		pix
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

    // get our stars
	stars = VespersCv::findPoints(
		gray,
		maxStars,
		qualityLevel,
		minDistance,
		blockSize,
		camWidth,
		camHeight
	);
    
    // add the "north star" to the top of the stack
	stars.push_back(northStar);
    
}

void Vespers::drawStars(
	ofColor color
	, float minRadius
	, float maxRadius
) {
    

    float starRadius;
    float alpha = (bool) alwaysUpdateStars ? 1.0 : timeline.getValue("Stars Alpha");

    // CAMERA SETUP
    
	// set the initial camera direction
	ofVec3f camDirection(0,0,1);
    // find the center of the plane
	ofVec3f center(cam.getWidth()/2.f,cam.getHeight()/2.f, 0);
    // rotate the camera based on the `starsCamPan` GUI value
	ofVec3f camDirectionRotated = camDirection.getRotated(starsCamPan, ofVec3f(1,0,0));
    // set the camera position vector
	ofVec3f camPosition = center + camDirectionRotated * starsCamZoom;
    // set the camera position
	starsCam.setPosition(camPosition);
    // point the camea at the center of th plane
	starsCam.lookAt(center);
    // rotate camera around the "North Star" based on the timeline value
//    starsCam.rotate(timeline.getValue("Stars Rotation"), (camPosition - northStar));

    starsFbo.begin();
    ofClear(0,0,0,0);
    
    // stars camera
    starsCam.begin();
    
    // set colors
    ofSetColor(color);
    ofFill();
    
    // arrange by depth rather than order added
    ofEnableDepthTest();
    
    ofPushMatrix();
    
        if(orbitStars) {
            ofTranslate(northStar);
            ofRotateZ(timeline.getValue("Stars Rotation"));
        }
        ofPushMatrix();
            if(orbitStars) {
                ofTranslate(-northStar);
            }
    
            // draw our actual stars if there are any
            if(stars.size() > 0) {
                
                // here we are basing the number of stars on the alpha value,
                // so the higher the alpha the more stars will be drawn
                // @todo: reverse the ordering on this so it goes largest to smallest
                for(int i = 0; i< ceil(stars.size()*alpha); i++) {
                    // calculate radius based on star "quality" (order) and star radius max/min
                    starRadius = (((maxRadius-minRadius)/stars.size())*i)+minRadius;
                    // draw the star
                    ofDrawSphere(stars[i], starRadius);
                }
            }

            // draw randomly generated stars
            for(int j=0; j < randomStars.size()*alpha; j++) {
                ofDrawSphere(randomStars[j], minRadius);
            }
        ofPopMatrix();
    
    ofPopMatrix();
    
    // make a graceful exit
    ofDisableDepthTest();
    starsCam.end();
    starsFbo.end();
}


//--------------------------------------------------------------
void Vespers::keyPressed(int key){
    bool showTimeline;
	switch(key) {
		case 'd': sequenceMode = !sequenceMode; break;
        case 't' :
            showTimeline = timeline.toggleShow();
            drawGui = !showTimeline;
            if(!showTimeline) { ofGetWindowPtr()->showCursor(); }
            break;
		case 'f': isFullScreen = !isFullScreen; ofToggleFullscreen(); break;
        case 'z' :
            useSampleImage = !useSampleImage;
            break;
        case 's': gui.saveToFile("settings.xml"); ofLogVerbose() << "Saved config"; break;
        case 'l': gui.loadFromFile("settings.xml"); ofLogVerbose() << "Loaded config"; break;
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
	int transX = x;
	int transY = y;

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
