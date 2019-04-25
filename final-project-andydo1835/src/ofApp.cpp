#include "ofApp.h"
#include <stdlib.h>

//--------------------------------------------------------------
void ofApp::setup(){

	int bufferSize = 512;
	delay.setDelay(8192);
	ofSoundStreamSettings streamSettings;
		streamSettings.numInputChannels = 2;
		streamSettings.numOutputChannels = 2;
		streamSettings.sampleRate = 44100;
		streamSettings.bufferSize = bufferSize;
		streamSettings.numBuffers = 4;
	player_stream.setup(streamSettings);
	player_stream.setOutput(player_output);
	player_stream.setInput(input);

	song_stream.setup(streamSettings);
	song_stream.setOutput(song_output);
	song_stream.setInput(input);

	ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac)");
	if (result.bSuccess) {
		std::cout << "Path: " << result.getPath() << std::endl;
		song.load(result.getPath());
		player.load(result.getPath());
		std::cout << "Load Successful!" << std::endl;
		song.play();
		song.setVolume(0);
	}

	song.connectTo(song_output).connectTo(delay);
	player.connectTo(player_output);

	rms_values.push_back(0.0f);

	ofBackground(0, 0, 0);

	for (int i = 0; i < 4; i++) {
		ofRectangle key(256 * i, 64, 256, 96);
		keys.push_back(key);
	}
	ofSetColor(256, 256, 256);
	key_pressed.resize(keys.size(), false);
}

//--------------------------------------------------------------
void ofApp::update(){
	count++;
	if (count == 700 / scroll_speed * -1) {
		player.play();
		started = true;
	}

	// add the current rms value
	rms = song_output.getBuffer().getRMSAmplitude();
	rms_values.push_back(rms);

	// we can "smooth" the rms value by using feedback.
	// this is a simple example of using the previous value 
	// with a weight (0.9) and the current value (0.1)

	//rms_values.push_back(0.9 * rms_values.back() + 0.1 * rms);

	// we only keep a maximum of 25 values of the rms readings
	if (rms_values.size() > 25) {
		// if we have added more than 25, then we delete the first one
		rms_values.erase(rms_values.begin(), rms_values.begin() + 1);
	}

	if (!player.isPlaying() && started) {
		ofExit();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	std::cout << score << std::endl;
	for (int i = 0; i < keys.size(); i++) {
		ofNoFill();
		ofDrawRectangle(keys[i]);
	}
	ofFill();

	// calculate the average of the rms values
	float average_rms = 0.0f;
	for (int i = 0; i < rms_values.size(); i++) {
		average_rms = average_rms + rms_values[i];
	}
	average_rms = average_rms / rms_values.size();

	// calculate the variance of the rms values
	float var_rms = 0.0f;
	for (int i = 0; i < rms_values.size(); i++) {
		var_rms = var_rms + abs(rms_values[i] - average_rms);
	}
	var_rms = var_rms / rms_values.size();

	// now we see if the current value is outside the mean + variance
	// basic statistics tells us a normally distributed function
	// has a mean and a variance where 97% of the data is explained by
	// 3 standard deviations.  we use this principle here in detecting 
	// the the current rms reading is outside this probability
	if (rms_values.back() > (average_rms + 4.0*var_rms)) {
		int column = rand() % 4;
		bool intersect = false;
		ofRectangle* myRect = new ofRectangle(256 * column, 768, 256, 32);
		for (int i = 0; i < notes.size(); i++) {
			if (myRect->intersects(*notes[i])) {
				delete myRect;
				intersect = true;
				break;
			}
		}
		if (!intersect) {
			notes.push_back(myRect);
		}
	}

	for (int i = 0; i < notes.size(); i++) {
		if (notes[i]->getTop() <= 0) {
			delete notes[i];
			notes.erase(notes.begin() + i);
			score--;
		} else {
			notes[i]->translateY(scroll_speed);
			ofDrawRectangle(*notes[i]);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'w':
		if (!key_pressed[0]) {
			checkHit(0);
			key_pressed[0] = true;
		}
		break;
	case 'e':
		if (!key_pressed[1]) {
			checkHit(1);
			key_pressed[1] = true;
		}
		break;
	case 'i':
		if (!key_pressed[2]) {
			checkHit(2);
			key_pressed[2] = true;
		}
		break;
	case 'o':
		if (!key_pressed[3]) {
			checkHit(3);
			key_pressed[3] = true;
		}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case 'w':
		key_pressed[0] = false;
		break;
	case 'e':
		key_pressed[1] = false;
		break;
	case 'i':
		key_pressed[2] = false;
		break;
	case 'o':
		key_pressed[3] = false;		
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

bool ofApp::checkHit(int pos) {
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i]->intersects(keys[pos])) {
			delete notes[i];
			notes.erase(notes.begin() + i);
			score++;
			return true;
		}
	}
	score--;
	return false;
}