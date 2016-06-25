#pragma once

#include "opencv2/core/core.hpp"
namespace tfg
{
	//Interfaz para aquellas clases que realizan upsampling
	class ImageUpsampler
	{
	public:
		ImageUpsampler();
		virtual ~ImageUpsampler() {}

		//Método de upsampling
		virtual void upSample(const cv::Mat& img, cv::Mat &output, int height, int width) = 0;
		//Nombre del reconocedor
		virtual std::string getName() const = 0;
	};
}

