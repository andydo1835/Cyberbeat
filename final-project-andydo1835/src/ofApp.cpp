#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	int bufferSize = 512;
	ofSoundStreamSettings streamSettings;
		streamSettings.numInputChannels = 2;
		streamSettings.numOutputChannels = 2;
		streamSettings.sampleRate = 44100;
		streamSettings.bufferSize = bufferSize;
		streamSettings.numBuffers = 4;
	stream.setup(streamSettings);
	stream.setOutput(output);
	stream.setInput(input);
	
	

	ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac)");
	if (result.bSuccess) {
		std::cout << "Path: " << result.getPath() << std::endl;
		song.load(result.getPath());
		std::cout << "Load Successful!" << std::endl;
		song.play();
	}

	song.connectTo(output);

	rms_values.push_back(0.0f);

}

//--------------------------------------------------------------
void ofApp::update(){
	// we can "smooth" the rms value by using feedback.
	// this is a simple example of using the previous value 
	// with a weight (0.9) and the current value (0.1)
	//rms_values.push_back(0.9 * rms_values.back() + 0.1 * rms);

	// add the current rms value
	rms = output.getBuffer().getRMSAmplitude();
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
}

//--------------------------------------------------------------
void ofApp::draw(){
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
	if (rms_values.back() > (average_rms + 2.5*var_rms)) {
		// draw a rectangle to denote the detection of an onset
		ofRect(0, 0, 256, 256);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
