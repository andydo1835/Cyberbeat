#include "ofApp.h"
#include <stdlib.h>

void ofApp::setup(){
	ofBackground(ofColor::white);
	setupAudio();
	setupGameplay();
}

void ofApp::update(){
	song.setPaused(paused);
	player.setPaused(paused);
	if (!paused) {
		updateRMS();
		detectAudioOnset();
		trackGameState();
	}
}

void ofApp::draw(){
	if (paused) {
		gui.draw();
	} else {
		drawKeys();
		drawNotes();
		drawHitLighting();
		drawText();
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
		if (notes[i].first.intersects(keys[pos].first)) {
			notes.erase(notes.begin() + i);
			combo++;
			score += combo;
			hit_lighting_alpha[pos] = 255;
			return true;
		}
	}
	combo = 0;
	miss_text_alpha = 255;
	return false;
}

void ofApp::makeNote() {
	int column = 0;
	while (column == last_column) {
		column = rand() % NUM_KEYS;
	}
	last_column = column;
	bool intersect = false;
	ofRectangle note = ofRectangle(ofGetWindowWidth() / NUM_KEYS * column + (NOTE_PADDING / 2),	// x coordinate
								   ofGetWindowHeight(),											// y coordinate
								   ofGetWindowWidth() / NUM_KEYS - NOTE_PADDING, NOTE_HEIGHT);	// (l, h) dimensions

	for (int i = 0; i < notes.size(); i++) {
		// if it intersects another rectangle, don't draw it
		if (note.intersects(notes[i].first)) {
			intersect = true;
			break;
		}
	}
	if (!intersect) {
		if (column == 0 || column == 3) {
			notes.push_back(pair<ofRectangle, ofColor>(note, ofColor::red));
		} else {
			notes.push_back(pair<ofRectangle, ofColor>(note, ofColor::blue));
		}
	}
}

bool ofApp::checkStart() {
	// here, we make the assumption that the song creates a note
	// to start, which is usually true unless it begins extremely
	// quietly and the difficulty is preemptively set to low
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].first.getTop() <= NOTE_HEIGHT * 5) { // NOTE_HEIGHT * 5 = bottom of the row of keys
			started = true;
			player.play();
		}
	}
	return started;
}

void ofApp::drawNotes() {
	for (int i = 0; i < notes.size(); i++) {
		notes[i].first.translateY(scroll_speed * -1);
		ofSetColor(notes[i].second);
		ofDrawRectangle(notes[i].first);

		// if the player has missed a note
		if (notes[i].first.getBottom() <= 0) {
			miss_text_alpha = 255;
			notes.erase(notes.begin() + i);
			combo = 0;
		}
	}
}

void ofApp::drawKeys() {
	for (int i = 0; i < keys.size(); i++) {
		ofNoFill();
		ofSetColor(keys[i].second);
		ofDrawRectangle(keys[i].first);
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

bool ofApp::setupAudio() {
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
	} else {
		return false;
	}

	song.connectTo(song_output);
	player.connectTo(player_output);

	return true;
}

void ofApp::setupGameplay() {
	for (int i = 0; i < NUM_KEYS; i++) {
		ofRectangle key(ofGetWindowWidth() / NUM_KEYS * i + (NOTE_PADDING / 2) , NOTE_HEIGHT * 2,	// (x, y) coordinates
						ofGetWindowWidth() / NUM_KEYS - NOTE_PADDING, NOTE_HEIGHT * 3);				// (l, h) dimensions
		if (i == 0 || i == 3) {
			keys.push_back(pair<ofRectangle, ofColor>(key, ofColor::red));
		} else {
			keys.push_back(pair<ofRectangle, ofColor>(key, ofColor::blue));
		}
	}

	key_pressed.resize(keys.size() + 1, false);
	hit_lighting_alpha.resize(keys.size(), 0);
	miss_text_alpha = 0;

	gui.setup();
	gui.add(scroll_speed.setup("scroll speed", DEFAULT_SCROLL_SPEED, MIN_SCROLL_SPEED, MAX_SCROLL_SPEED));
	gui.add(difficulty.setup("difficulty", DEFAULT_DIFFICULTY, MIN_DIFFICULTY, MAX_DIFFICULTY));

	verdana24A.load("verdana.ttf", 24, false);
	verdana24A.setLineHeight(28.0f);
	verdana24A.setLetterSpacing(1.036);

	keybindings.insert(pair<char, int>('w', 0));
	keybindings.insert(pair<char, int>('e', 1));
	keybindings.insert(pair<char, int>('i', 2));
	keybindings.insert(pair<char, int>('o', 3));
	keybindings.insert(pair<char, int>('p', 4));

	rms_values.push_back(0.0f);
}

void ofApp::drawHitLighting() {
	ofEnableAlphaBlending();
	for (int i = 0; i < hit_lighting_alpha.size(); i++) {
		ofSetColor(keys[i].second, hit_lighting_alpha[i]);
		ofDrawRectangle(keys[i].first);
		hit_lighting_alpha[i] -= FADE_SPEED;
	}
	ofDisableAlphaBlending();
}

void ofApp::drawText() {
	ofSetColor(ofColor::black);
	verdana24A.drawString("Score: " + std::to_string(score), ofGetWindowWidth() / 4 * 3, NOTE_PADDING * 2);
	verdana24A.drawString("Combo: " + std::to_string(combo), NOTE_PADDING, NOTE_PADDING * 2);
	ofSetColor(ofColor::red, miss_text_alpha);
	ofEnableAlphaBlending();
	verdana24A.drawString("MISS", ofGetWindowWidth() / 2 - NOTE_PADDING * 3, ofGetWindowHeight() / 2);
	miss_text_alpha -= FADE_SPEED;
	ofDisableAlphaBlending();
}