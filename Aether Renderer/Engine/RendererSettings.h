#pragma once
struct RendererSettings {
	bool zPrePass = true;
	int screenFBODepthStencilType = 1;
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

	enum BloomTypes {
		kernel,
		gaussianBlur
	};

	bool bloom = true;
	BloomTypes bloomType = BloomTypes::gaussianBlur;
	int amount = 10;

	bool SSAO = true;
	int kernelSize = 64;
};
const RendererSettings DefaultRendererSettings;