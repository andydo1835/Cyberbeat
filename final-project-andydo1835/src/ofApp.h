#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSoundPlayerObject.h"
#include <map>

using std::map;
using std::pair;

constexpr float MIN_DIFFICULTY = 1.0;
constexpr float MAX_DIFFICULTY = 5.0;
constexpr float DEFAULT_DIFFICULTY = 3.0;
constexpr int MIN_SCROLL_SPEED = 5;
constexpr int MAX_SCROLL_SPEED = 30;
constexpr int DEFAULT_SCROLL_SPEED = 10;

constexpr int NUM_KEYS = 4;
constexpr int NOTE_HEIGHT = 32;
constexpr int RMS_BUFFER = 25;

class ofApp : public ofBaseApp{

	public:

		// default openFrameworks functions
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);

		// gameplay functions
		bool checkHit(int pos);
		void makeNote();
		bool checkStart();
		void drawNotes();
		void drawKeys();
		void updateRMS();
		bool trackGameState();
		bool detectAudioOnset();

		// sound objects
		ofSoundStream song_stream;
		ofSoundStream player_stream;
		ofxSoundOutput song_output;
		ofxSoundOutput player_output;
		ofxSoundInput input;
		ofxSoundPlayerObject song;
		ofxSoundPlayerObject player;

		// audio onset detection
		float rms;
		vector<float> rms_values;

		// game setup / settings
		ofxPanel gui;
		ofxIntSlider scroll_speed;
		ofxFloatSlider difficulty;
		map<char, int> keybindings;

		// gameplay variables
		bool started = false;
		bool paused = false;
		vector<ofRectangle> notes;
		vector<ofRectangle> keys;
		int score = 0;
		int last_column = 0;
		int combo = 0;
		vector<bool> key_pressed;
};
