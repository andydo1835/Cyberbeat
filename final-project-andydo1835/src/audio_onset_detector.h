#pragma once

#include <vector>

using std::vector;

constexpr int RMS_BUFFER = 25;

class audio_onset_detector {
	private:
		vector<float> rms_values;

	public:
		bool detectAudioOnset(float difficulty);
		void updateRMS(float rms);
};