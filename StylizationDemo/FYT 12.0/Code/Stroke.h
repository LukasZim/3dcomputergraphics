#ifndef STROKE_H
#define STROKE_H
#include "Stylization.h"
extern Stylization* stylization;

class Stroke{
public:
	Stroke()
	{
		gradientThreshold  = 0.08f;
		angleThreshold     = 0.06f;
		strokeLength       = stylization->WindowWidth * 0.01f; //10
		strokeWidth        = strokeLength * 0.5f; //4
		ProbabilityLargeThreshold  = 0.13f;
		ProbabilityMediumThreshold = 0.4f;
		ProbabilitySmallThreshold  = 0.7f;
		GradientLargeThreshold     = 0.1f;
		GradientMediumThreshold	   = 0.3f;
		SizeMediumThreshold        = 0.6f;
		SizeSmallThreshold         = 0.4f;
		AlphaLargeThreshold        = 0.5f;
		AlphaMediumThreshold       = 0.65f;
		AlphaSmallThreshold        = 0.8f;
	}

	float                               gradientThreshold;
	float                               angleThreshold;
	float                               strokeLength; //10
	float                               strokeWidth; //4
	float								ProbabilityLargeThreshold;
	float								ProbabilityMediumThreshold;
	float								ProbabilitySmallThreshold;
	float								GradientLargeThreshold;
	float								GradientMediumThreshold;
	float								SizeMediumThreshold;
	float								SizeSmallThreshold;
	float								AlphaLargeThreshold;
	float								AlphaMediumThreshold;
	float								AlphaSmallThreshold;
};

#endif