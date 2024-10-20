#pragma once
struct RendererSettings {
	bool zPrePass = true;
	int screenFBODepthStencilType = 2;
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

	//Bloom
	bool bloom = false;
	BloomTypes bloomType = BloomTypes::gaussianBlur;
	int amount = 10;

	//SSAO
	bool SSAO = true;
	bool SSAOOnly = false;
	int kernelSize = 64;
	float sampleRad = 0.5f;
	float power = 2;
	float bias = 0.03f;

	//Shadow map
	bool shadowMapping = true;
	bool softShadow = true;
	glm::ivec2 shadowResolution = glm::ivec2(1920, 1080);
	float shadowbias = 0.0005f;
	float minBias = 0.0003f;
};
const RendererSettings DefaultRendererSettings;