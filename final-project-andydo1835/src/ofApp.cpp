#include "ofApp.h"
#include <stdlib.h>

void ofApp::setup(){

	int bufferSize = 512;
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
		song.load(result.getPath());
		player.load(result.getPath());
		song.play();
		song.setVolume(0);
	}

	song.connectTo(song_output);
	player.connectTo(player_output);

	rms_values.push_back(0.0f);

	ofBackground(ofColor::black);

	for (int i = 0; i < NUM_KEYS; i++) {
		ofRectangle key(ofGetWindowWidth() / NUM_KEYS * i, NOTE_HEIGHT * 2,		// (x, y) coordinates
						ofGetWindowWidth() / NUM_KEYS, NOTE_HEIGHT * 3);	    // (l, h) dimensions
		keys.push_back(key);
	}
	ofSetColor(ofColor::white);
	key_pressed.resize(keys.size() + 1, false);

	gui.setup();
	gui.add(scroll_speed.setup("scroll speed", DEFAULT_SCROLL_SPEED, MIN_SCROLL_SPEED, MAX_SCROLL_SPEED));
	gui.add(difficulty.setup("difficulty", DEFAULT_DIFFICULTY, MIN_DIFFICULTY, MAX_DIFFICULTY));

	keybindings.insert(pair<char, int>('w', 0));
	keybindings.insert(pair<char, int>('e', 1));
	keybindings.insert(pair<char, int>('i', 2));
	keybindings.insert(pair<char, int>('o', 3));
	keybindings.insert(pair<char, int>('p', 4));
}

void ofApp::update(){
	song.setPaused(paused);
	player.setPaused(paused);
	if (paused) {

	} else {
		updateRMS();
		detectAudioOnset();
		trackGameState();
	}
}

void ofApp::draw(){
	std::cout << score << std::endl;
	if (paused) {
		gui.draw();
	} else {
		drawKeys();
		drawNotes();
	}
}

void ofApp::keyPressed(int key){
	if (keybindings.count(key) == 0) {
		return;
	}

	int pressed_index = keybindings[key];

	if (!key_pressed[pressed_index]) {
		if (pressed_index < NUM_KEYS) {
			checkHit(pressed_index);
		} else {
			if (paused) {
				paused = false;
			} else {
				paused = true;
			}
		}
		key_pressed[pressed_index] = true;
	}
}

void ofApp::keyReleased(int key){
	if (keybindings.count(key) == 0) {
		return;
	}

	key_pressed[ keybindings[key] ] = false;
}

bool ofApp::checkHit(int pos) {
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].intersects(keys[pos])) {
			notes.erase(notes.begin() + i);
			combo++;
			score += combo;
			return true;
		}
	}
	combo = 0;
	return false;
}

void ofApp::makeNote() {
	int column = 0;
	while (column == last_column) {
		column = rand() % NUM_KEYS;
	}
	last_column = column;
	bool intersect = false;
	ofRectangle note = ofRectangle(ofGetWindowWidth() / NUM_KEYS * column,			// x coordinate
								   ofGetWindowHeight(),								// y coordinate
								   ofGetWindowWidth() / NUM_KEYS, NOTE_HEIGHT);	    // (l, h) dimensions
	for (int i = 0; i < notes.size(); i++) {
		// if it intersects another rectangle, don't draw it
		if (note.intersects(notes[i])) {
			intersect = true;
			break;
		}
	}
	if (!intersect) {
		notes.push_back(note);
	}
}

bool ofApp::checkStart() {
	// here, we make the assumption that the song creates a note
	// to start, which is usually true unless it begins extremely quietly
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].getTop() <= NOTE_HEIGHT * 5) { // NOTE_HEIGHT * 5 = bottom of the row of keys
			started = true;
			player.play();
		}
	}
	return started;
}

void ofApp::drawNotes() {
	for (int i = 0; i < notes.size(); i++) {
		notes[i].translateY(scroll_speed * -1);
		ofDrawRectangle(notes[i]);

		// if the player has missed a note
		if (notes[i].getTop() <= 0) {
			score--;
			notes.erase(notes.begin() + i);
			combo = 0;
		}
	}
}

void ofApp::drawKeys() {
	for (int i = 0; i < keys.size(); i++) {
		ofNoFill();
		ofDrawRectangle(keys[i]);
	}
	ofFill();
}

void ofApp::updateRMS() {
	// add the current rms value
	rms = song_output.getBuffer().getRMSAmplitude();
	rms_values.push_back(rms);

	if (rms_values.size() > RMS_BUFFER) {
		// if we have added more than rms_buffer rms values, then we delete the first one
		rms_values.erase(rms_values.begin(), rms_values.begin() + 1);
	}
}

bool ofApp::trackGameState() {
	if (!player.isPlaying() && started) {
		ofExit();
	}

	if (!started) {
		checkStart();
	}
	return started;
}

bool ofApp::detectAudioOnset() {
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

	/*
	* if the current rms value is "difficulty" variances
	* away from the average, we can be so sure that there is a
	* significant change in the rms values, therefore we've
	* detected an audio onset
	*/
	if (rms_values.back() > (average_rms + (6.0 - difficulty) * var_rms)) {
		makeNote();
		return true;
	}
	return false;
}