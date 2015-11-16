//
//  VespersDetectParticipant.cpp
//  vespers
//
//  Created by Andrew Dayton on 11/15/15.
//
//

#include "VespersDetectParticipant.h"


void VespersDetectParticipant::setup() {
	background.setLearningTime(900);
	background.setThresholdValue(10);
}

void VespersDetectParticipant::update(ofVideoGrabber &frame) {
//    cropped.cropFrom(frame, region.x, region.y, region.getWidth(), region.getHeight());

    cv::Mat roi;
//    cv::Mat frameMat;
    
//    frameMat = ofxCv::toCv(frame);
    roi = ofxCv::toCv(frame)(region);
    
    background.update(roi, thresholded);
    thresholded.update();
}


void VespersDetectParticipant::draw(int x, int y, int w, int h) {
    thresholded.draw(x, y, w, h);
}


void VespersDetectParticipant::setRegion(ofRectangle r) {
    region = cv::Rect(r.x, r.y, r.width, r.height);
}


void VespersDetectParticipant::resetBackground() {
    background.reset();
}

float VespersDetectParticipant::getPresence() {
    return background.getPresence();
}