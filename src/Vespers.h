#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ofxTimeline.h"

#include "VespersCv.h"

class Vespers : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
    
        void receivedBang(ofxTLBangEventArgs& bang);
        void drawHud(int x, int y);
    
		void findStars();
		void drawStars(
			ofColor color
			, float minRadius
			, float maxRadius
		);
    

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		int configWindowWidth;
		int configWindowHeight;
		int sequenceWindowWidth;
		int sequenceWindowHeight;

		bool isFullScreen;
        bool drawGui;
    
		// a grabber for our camera
		ofVideoGrabber cam;

		// size of our image
		int camWidth;
		int camHeight;
		int procWidth;
		int procHeight;
		int guiWidth;

		// base image for processing
		ofImage base;
		// processed greyscale image
		ofImage gray;


		// canvas for final image
		ofRectangle canvas;

		// this will be the placeable "central" star
		ofPoint northStar;

		// store our stars
		vector<ofPoint> stars;

		// shader stuff
		ofFbo mainFbo;
        ofFbo starsFbo;

		ofxAutoReloadedShader camShader;
		ofxAutoReloadedShader starShader;

		int period;
		float start_t;
		float t;

		// gui stuff
		ofxPanel gui;
		ofxLabel prepLabel;

		// image prep
		ofxToggle useBilateralFilter;
		// ofxIntSlider bfDiameter;
		int bfDiameter;
		// ofxFloatSlider bfSigmaColor;
		float bfSigmaColor;
		// ofxFloatSlider bfSigmaSpace;
		float bfSigmaSpace;
		// ofxToggle useNormalize;
		bool useNormalize;
		// ofxToggle useManualThreshold;
		ofxFloatSlider thresh;
		// ofxToggle useAutoThreshold;
		// ofxToggle useDilate;
		ofxIntSlider dilateIterations;
		// ofxToggle useErode;
		ofxIntSlider erodeIterations;
		ofxToggle dilateErodeInvert;

		// stars
		ofxLabel starsLabel;
		ofxToggle showStars;
		ofxFloatSlider maxStarRadius;
		ofxFloatSlider minStarRadius;
		ofxIntSlider maxStars;
		ofxFloatSlider qualityLevel;
		ofxFloatSlider minDistance;
		ofxFloatSlider blockSize;
		ofxToggle drawStarsAsPoints;

		// contours
		ofxLabel contoursLabel;
		ofxToggle showContours;

		// sequence mode
		ofxLabel sequenceLabel;
		ofxToggle sequenceMode;

        ofxTimeline timeline;
};