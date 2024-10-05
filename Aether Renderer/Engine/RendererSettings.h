#pragma once
struct RendererSettings {
	//Gamma Correction
	bool gammaCorrection = true;
	float gamma = 2.2f;
	bool HDR = true;
	bool toneMapping = true;
	float exposure = 0.3f;
};
const RendererSettings DefaultRendererSettings;