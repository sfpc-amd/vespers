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

class VespersDetectParticipant {

    public:
        void setup();
        void update(ofVideoGrabber &frame);
        void draw(int x, int y, int w, int h);
        void resetBackground();
        void setRegion(ofRectangle r);
        float getPresence();
    
        ofImage thresholded;
        cv::Rect region;
    
        ofxCv::RunningBackground background;
        

};
