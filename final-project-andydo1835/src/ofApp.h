#pragma once

#include "ofMain.h"
#include "ofxSoundPlayerObject.h"
#include "DigitalDelay.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		bool checkHit(int pos);

		ofSoundStream song_stream;
		ofSoundStream player_stream;
		ofxSoundOutput song_output;
		ofxSoundOutput player_output;
		ofxSoundInput input;
		ofxSoundPlayerObject song;
		ofxSoundPlayerObject player;
		DigitalDelay delay;

		float rms;
		vector<float> rms_values;

		bool started = false;
		vector<ofRectangle*> notes;
		vector<ofRectangle> keys;
		int scroll_speed = -10;

		int count = 0;
		int score = 0;
		vector<bool> key_pressed;
};
