#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSoundPlayerObject.h"
#include "audio_onset_detector.h"
#include <map>

using std::map;
using std::pair;

// settings constans
constexpr float MIN_DIFFICULTY = 1.0;
constexpr float MAX_DIFFICULTY = 5.0;
constexpr float DEFAULT_DIFFICULTY = 3.0;
constexpr int MIN_SCROLL_SPEED = 5;
constexpr int MAX_SCROLL_SPEED = 30;
constexpr int DEFAULT_SCROLL_SPEED = 10;
constexpr char DEFAULT_KEYBINDINGS[] = "weiop";

// gameplay constants
constexpr int FADE_SPEED = 15;
constexpr int MAX_ALPHA = 255;
constexpr int NUM_KEYS = 4;

// constants for moving drawings
constexpr int NOTE_HEIGHT = 32;
constexpr int NOTE_PADDING = 16;
constexpr int KEY_HEIGHT = 96;
constexpr int KEY_BEGIN_Y = 64;
constexpr int TEXT_BEGIN_Y = 32;
constexpr int MISS_TEXT_LEFT_SHIFT = 48;

class ofApp : public ofBaseApp{

	public:
		// default openFrameworks functions
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);

		// gameplay functions
		bool setupSoundObjects();
		void loadAudioFile(string path);
		void setupGameplay();
		bool trackGameState();
		bool checkStart();
		void makeNote();
		void drawNotes();
		void drawKeys();
		void drawHitLighting();
		void drawText();
		bool checkHit(int pos);

		// sound objects
		ofSoundStream song_stream;
		ofSoundStream player_stream;
		ofxSoundOutput song_output;
		ofxSoundOutput player_output;
		ofxSoundInput input;
		ofxSoundPlayerObject song;
		ofxSoundPlayerObject player;

		// game setup / settings
		ofxPanel gui;
		ofxIntSlider scroll_speed;
		ofxFloatSlider difficulty;
		map<char, int> keybindings;
		ofTrueTypeFont verdana24A;
		ofTrueTypeFont verdana48A;

		// gameplay variables
		audio_onset_detector onset_detect;
		bool started = false;
		bool paused = false;
		vector<pair<ofRectangle, ofColor>> notes;
		vector<pair<ofRectangle, ofColor>> keys;
		int score = 0;
		int last_column = 0;
		int combo = 0;
		vector<bool> key_pressed;
		vector<int> hit_lighting_alpha;
		int miss_text_alpha;
};
