#include "ofApp.h"
#include <stdlib.h>

void ofApp::setup(){
	ofBackground(ofColor::white);
	setupSoundObjects();
	setupGameplay();
}

void ofApp::update(){
	song.setPaused(paused);
	player.setPaused(paused);
	if (!paused) {
		onset_detect.updateRMS(song_output.getBuffer().getRMSAmplitude());
		if (onset_detect.detectAudioOnset(difficulty)) {
			makeNote();
		}
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
	// if the key has not been registered in keybindings
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

bool ofApp::setupSoundObjects() {
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
	if (!result.bSuccess) {
		return false;
	}

	loadAudioFile(result.getPath());
	song.connectTo(song_output);
	player.connectTo(player_output);

	return true;
}

void ofApp::loadAudioFile(string path) {
	song.load(path);
	player.load(path);
	song.play();
	song.setVolume(0);
}

void ofApp::setupGameplay() {
	for (int i = 0; i < NUM_KEYS; i++) {
		ofRectangle key(ofGetWindowWidth() / NUM_KEYS * i + (NOTE_PADDING / 2) , KEY_BEGIN_Y,	// (x, y) coordinates
						ofGetWindowWidth() / NUM_KEYS - NOTE_PADDING, KEY_HEIGHT);				// (l, h) dimensions
		if (i == 0 || i == NUM_KEYS - 1) {
			keys.push_back(pair<ofRectangle, ofColor>(key, ofColor::red));
		} else {
			keys.push_back(pair<ofRectangle, ofColor>(key, ofColor::blue));
		}
	}
	for (int i = 0; i < NUM_KEYS + 1; i++) {
		keybindings.insert(pair<char, int>(DEFAULT_KEYBINDINGS[i], i));
	}

	key_pressed.resize(keys.size() + 1, false);
	hit_lighting_alpha.resize(keys.size(), 0);
	miss_text_alpha = 0;

	gui.setup();
	gui.add(scroll_speed.setup("scroll speed", DEFAULT_SCROLL_SPEED, MIN_SCROLL_SPEED, MAX_SCROLL_SPEED));
	gui.add(difficulty.setup("difficulty", DEFAULT_DIFFICULTY, MIN_DIFFICULTY, MAX_DIFFICULTY));

	verdana24A.load("verdana.ttf", 24, false); // set font size to 24 and aliasing to false
	verdana24A.setLineHeight(28.0f);		   // set to font size + 4 to leave space for 4 pixels above letters
	verdana48A.load("verdana.ttf", 48, false);
	verdana48A.setLineHeight(52.0f);

	onset_detect.updateRMS(0.0f);
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

bool ofApp::checkStart() {
	// here, we make the assumption that the song creates a note
	// to start, which is usually true unless it begins extremely
	// quietly and the difficulty is preemptively set to low
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].first.getTop() <= KEY_BEGIN_Y + KEY_HEIGHT) {
			started = true;
			player.play();
		}
	}
	return started;
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
		// otherwise it will make the illusion of two notes as one
		if (note.intersects(notes[i].first)) {
			intersect = true;
			break;
		}
	}
	if (!intersect) {
		if (column == 0 || column == keys.size() - 1) {
			notes.push_back(pair<ofRectangle, ofColor>(note, ofColor::red));
		} else {
			notes.push_back(pair<ofRectangle, ofColor>(note, ofColor::blue));
		}
	}
}

void ofApp::drawNotes() {
	for (int i = 0; i < notes.size(); i++) {
		notes[i].first.translateY(scroll_speed * -1);
		ofSetColor(notes[i].second);
		ofDrawRectangle(notes[i].first);

		// if the player has missed a note
		if (notes[i].first.getBottom() <= 0) {
			miss_text_alpha = MAX_ALPHA;
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
	verdana24A.drawString("Score: " + std::to_string(score), ofGetWindowWidth() * 0.75, TEXT_BEGIN_Y);
	verdana24A.drawString("Combo: " + std::to_string(combo), NOTE_PADDING, TEXT_BEGIN_Y);

	ofSetColor(ofColor::red, miss_text_alpha);
	ofEnableAlphaBlending();
	verdana24A.drawString("MISS", ofGetWindowWidth() / 2 - MISS_TEXT_LEFT_SHIFT, ofGetWindowHeight() / 2);
	miss_text_alpha -= FADE_SPEED;
	ofDisableAlphaBlending();

	// draw keybinds on keys
	for (int i = 0; i < keys.size(); i++) {
		if (i == 0 || i == keys.size() - 1) {
			ofSetColor(ofColor::red);
		} else {
			ofSetColor(ofColor::blue);
		}
		verdana48A.drawString(string(1, DEFAULT_KEYBINDINGS[i]), keys[i].first.getCenter().x - NOTE_PADDING,
																 keys[i].first.getCenter().y + NOTE_PADDING);
	}
}

bool ofApp::checkHit(int pos) {
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].first.intersects(keys[pos].first)) {
			notes.erase(notes.begin() + i);
			combo++;
			score += combo;
			hit_lighting_alpha[pos] = MAX_ALPHA;
			return true;
		}
	}
	combo = 0;
	miss_text_alpha = MAX_ALPHA;
	return false;
}
