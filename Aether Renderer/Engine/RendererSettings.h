#pragma once
struct RendererSettings {
	//MultiSampling
	bool multiSampling = false;
	int samples = 4;
	//Gamma Correction
	bool gammaCorrection = true;
	float gamma = 2.2f;
	//HDR
	bool HDR = true;
	bool toneMapping = true;
	float exposure = 0.3f;
};
const RendererSettings DefaultRendererSettings;