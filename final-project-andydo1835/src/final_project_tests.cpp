//#define CATCH_CONFIG_MAIN
//#include "catch.hpp"
//#include "audio_onset_detector.h"
//
//TEST_CASE("Detect Audio Onset") {
//	audio_onset_detector onset_detect;
//	for (int i = 0; i < 24; i++) {
//		onset_detect.updateRMS(0.0);
//	}
//	onset_detect.updateRMS(100.0);
//	REQUIRE(onset_detect.detectAudioOnset(3.0));
//
//	for (int i = 0; i < 24; i++) {
//		onset_detect.updateRMS(100.0);
//	}
//
//	REQUIRE(!onset_detect.detectAudioOnset(3.0));
//}