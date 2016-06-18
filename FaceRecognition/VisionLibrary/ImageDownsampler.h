#pragma once

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace tfg
{
	//Clase utilizada para realizar downsampling
	class ImageDownsampler
	{
	public:
		ImageDownsampler();
		~ImageDownsampler();

		//Aplicar downsampling sin ruido
		void downSampleWithNoNoise(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo);
		//Aplicar downsampling con ruido gaussiano
		void downSampleWithGaussianNoise(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double Mean = 0.0, double StdDev = 10.0);
		//Aplicar downsampling con ruido de sal y pimienta
		void downSampleWithSaltAndPepper(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double probability = 0.01);
		//Aplicar downsampling con todos los ruidos
		void downSampleWithAllNoises(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double Mean = 0.0, double StdDev = 10.0, double probability = 0.01);

	private:
		//Añadir ruido gaussiano
		void addGaussianNoise(const cv::Mat &mSrc, cv::Mat &mDst, double Mean = 0.0, double StdDev = 10.0);
		//Añadir ruido de sal y pimienta
		void addSaltAndPepper(const cv::Mat &mSrc, cv::Mat &mDst, double probability = 0.01);
	
	};
}

