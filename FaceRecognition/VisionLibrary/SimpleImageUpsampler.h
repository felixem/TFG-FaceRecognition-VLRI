#pragma once

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ImageUpsampler.h"

namespace tfg
{
	//Clase para aumentar la resoluci�n de una imagen de manera simple
	class SimpleImageUpsampler : public ImageUpsampler
	{
	public:
		//Constructor a partir del tipo de upsampling
		SimpleImageUpsampler(cv::InterpolationFlags tipo);
		//Destructor
		~SimpleImageUpsampler();

		//M�todo de upsampling
		void upSample(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Nombre del reconocedor
		std::string getName() const;

	private:
		//Tipo de interpolaci�n
		cv::InterpolationFlags tipo;
	};
}
