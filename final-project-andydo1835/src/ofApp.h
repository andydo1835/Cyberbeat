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
constexpr char DEFAULT_KEYBINDINGS[] = "weiop";
constexpr int NOTE_HEIGHT = 32;
constexpr int NOTE_PADDING = 16;
constexpr int RMS_BUFFER = 25;
constexpr int FADE_SPEED = 15;

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
		bool setupAudio();
		void setupGameplay();
		void drawHitLighting();
		void drawText();

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
		ofTrueTypeFont verdana24A;
		ofTrueTypeFont verdana48A;

		// gameplay variables
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
