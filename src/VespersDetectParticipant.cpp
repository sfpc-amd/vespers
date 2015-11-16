//
//  VespersDetectParticipant.cpp
//  vespers
//
//  Created by Andrew Dayton on 11/15/15.
//
//

#include "VespersDetectParticipant.h"


void VespersDetectParticipant::setup(ofRectangle r, float thresh) {

    setRegion(r);
    setPresenceThreshold(thresh);

	background.setLearningTime(900);
	background.setThresholdValue(10);
    
    resetInProgress = false;
    
    ofAddListener(timer.TIMER_COMPLETE, this, &VespersDetectParticipant::resetBackgroundHandler);
}

void VespersDetectParticipant::update(ofVideoGrabber &frame) {
//    cropped.cropFrom(frame, region.x, region.y, region.getWidth(), region.getHeight());

    cv::Mat roi;
    cv::Mat frameMat;
    
    frameMat = ofxCv::toCv(frame);
    roi = frameMat(region);
    
    background.update(roi, thresholded);
    thresholded.update();
}


void VespersDetectParticipant::draw(int x, int y, int w, int h) {

    ofPushStyle();
        ofEnableAlphaBlending();
        if(resetInProgress) {
            ofSetColor(255, 0, 0);
        } else if(getPresence()) {
            ofSetColor(0, 255, 0);
        } else {
            ofSetColor(255);
        }
        thresholded.draw(x, y, w, h);
        timer.draw(x, y+h);
    ofPopStyle();

}


void VespersDetectParticipant::setRegion(ofRectangle r) {
    region = cv::Rect(r.x, r.y, r.getWidth(), r.getHeight());
}


void VespersDetectParticipant::setPresenceThreshold(float thresh) {
    pThresh = thresh;
}

void VespersDetectParticipant::resetBackground() {
    background.reset();
}

void VespersDetectParticipant::resetBackground(int delay, ofRectangle r) {

    resetInProgress = true;
    setRegion(r);

    timer.setup(delay);

    ofLogNotice("Starting background reset timer for "+ofToString(delay)+"ms");
    // don't loop
    timer.start(false);
    
}

bool VespersDetectParticipant::getPresence() {
    return background.getPresence() >= pThresh;
}

void VespersDetectParticipant::resetBackgroundHandler(int &args) {
    ofLogNotice("Background reset timer complete, resetting bg...");
    resetBackground();
    resetInProgress = false;
}

