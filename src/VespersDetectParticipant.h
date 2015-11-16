//
//  VespersDetectParticipant.h
//  vespers
//
//  Created by Andrew Dayton on 11/15/15.
//
//
#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxSimpleTimer.h"

class VespersDetectParticipant {

    public:
        void setup(ofRectangle r, float thresh);
        void update(ofVideoGrabber &frame);
        void draw(int x, int y, int w, int h);
        void resetBackground();
        void resetBackground(int delay, ofRectangle r);
    
        void setRegion(ofRectangle roi);
        void setPresenceThreshold(float thresh);
    
        bool getPresence();
    
        float pThresh;
        ofImage thresholded;
        cv::Rect region;
        ofxSimpleTimer timer;
        bool resetInProgress;
        
        ofxCv::RunningBackground background;    
        void resetBackgroundHandler(int &args);


};
