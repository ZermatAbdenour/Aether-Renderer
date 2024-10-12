#pragma once
struct RendererSettings {
	//MultiSampling
	bool multiSampling = true;
	int samples = 4;
	//Gamma Correction
	bool gammaCorrection = true;
	float gamma = 2.2f;
	//HDR
	bool HDR = true;
	bool toneMapping = true;
	float exposure = 0.3f;
	bool autoExposure = true;
	float exposureMultiplier = 0.4;
	float adjustmentSpeed = 0.03f;

	bool bloom = true;
	int amount = 10;
};
const RendererSettings DefaultRendererSettings;