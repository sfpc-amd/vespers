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
//    current.cropFrom(cur, region.x, region.y, region.getWidth(), region.getHeight());
    background.update(frame, thresholded);
    thresholded.update();
}


void VespersDetectParticipant::draw(int x, int y, int w, int h) {
    thresholded.draw(x, y, w, h);
}


void VespersDetectParticipant::setRegion(ofRectangle r) {
    region = r;
}


void VespersDetectParticipant::resetBackground() {
    background.reset();
}

float VespersDetectParticipant::getPresence() {
    return background.getPresence();
}