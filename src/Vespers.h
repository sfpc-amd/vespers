#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ofxTimeline.h"
#include "ofxStereoCamera.h"

#include "VespersCv.h"
#include "VespersDetectParticipant.h"


class Vespers : public ofBaseApp {

	public:
		void setup();
		void update();
		void updateTransition();

		void draw();
        void drawSequence();
        void drawTransition();

        void setAfterImage();
        void receivedBang(ofxTLBangEventArgs& bang);
        void drawHud(int x, int y);
    
		void findStars();
		void drawStars(
			ofColor color
			, float minRadius
			, float maxRadius
		);
    
        VespersDetectParticipant detect;


		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        // dimensions
        int camWidth;
        int camHeight;
        int procWidth;
        int procHeight;
        int guiWidth;
		int configWindowWidth;
		int configWindowHeight;
		int sequenceWindowWidth;
		int sequenceWindowHeight;

        // toggles
		bool isFullScreen;
        bool drawGui;
        bool sequenceMode;
        bool useSampleImage;
    
        // transition stuff
        float triangleZ;
        float moireSpacing;
        float moireAmount;
        float textOpacity;
        float shapeOpacity;

        ofxStereoCamera<ofCamera> stereoCam;

		// a grabber for our camera
		ofVideoGrabber grabber;
		// base image for processing
		ofImage base;
		// processed greyscale image
		ofImage gray;
        // afterimage
        ofImage afterImage;
        // sample image
        ofImage sampleImage;
        // bg image to use for bg subtractions
        ofImage bgImage;
    
    
		// canvas for final image
		ofRectangle canvas;

		// this will be the placeable "central" star
		ofPoint northStar;

		// store our stars
		vector<ofPoint> stars;
        vector<ofPoint> randomStars;

		// shader stuff
		ofFbo mainFbo;
        ofFbo starsFbo;
    


		ofxAutoReloadedShader camShader;
		ofxAutoReloadedShader starShader;
        ofxAutoReloadedShader afterImageShader;
        ofxAutoReloadedShader transitionShader;

        // GUI
    
		ofxPanel gui;
		ofxLabel prepLabel;

		// image prep
		ofxToggle useBilateralFilter;
		ofxFloatSlider thresh;
		ofxIntSlider dilateIterations;
		ofxIntSlider erodeIterations;
		ofxToggle dilateErodeInvert;

		// stars
		ofxLabel starsLabel;
        ofxToggle alwaysUpdateStars;
        ofxFloatSlider maxStarRadius;
		ofxFloatSlider minStarRadius;
		ofxIntSlider maxStars;
		ofxFloatSlider qualityLevel;
		ofxFloatSlider minDistance;
		ofxFloatSlider blockSize;
        ofxIntSlider maxRandomStars;
        ofxToggle orbitStars;
        ofxFloatSlider starsCamPan;
        ofxFloatSlider starsCamZoom;

        // detection
		ofxLabel detectLabel;
        ofxFloatSlider detectThreshold;
        ofxToggle useDetection;
        ofxIntSlider detectRegionWidth;
        ofxIntSlider detectRegionHeight;
        ofxIntSlider resetBackgroundDelay;



        // timeline
        ofxTimeline timeline;
    
        // camera for our stars
        ofCamera starsCam;
    
    private:
};
