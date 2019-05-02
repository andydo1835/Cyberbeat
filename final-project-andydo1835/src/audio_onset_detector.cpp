#include "audio_onset_detector.h"

void audio_onset_detector::updateRMS(float rms) {
	rms_values.push_back(rms);

	if (rms_values.size() > RMS_BUFFER) {
		// if we have added more than rms_buffer rms values, then we delete the first one
		rms_values.erase(rms_values.begin(), rms_values.begin() + 1);
	}
}

bool audio_onset_detector::detectAudioOnset(float difficulty) {
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
	return rms_values.back() > (average_rms + (6.0 - difficulty) * var_rms);
}