#pragma once

#include "opencv2/core/core.hpp"

namespace tfg
{
	//Clase para aumentar la resoluci�n de una imagen de manera simple
	class SimpleImageUpsampler
	{
	public:
		//Constructor
		SimpleImageUpsampler();
		//Destructor
		~SimpleImageUpsampler();

		//Modificar tama�o usando interpolaci�n por vecino m�s cercano
		void applyNearestNeightbornFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tama�o usando interpolaci�n bilineal
		void applyBilinearFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tama�o usando interpolaci�n por relaci�n de �rea
		void applyAreaFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tama�o usando interpolaci�n bic�bica 4x4
		void applyBicubicFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tama�o usando interpolaci�n Lanzcos de 8x8
		void applyLanczosFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
	};
}
