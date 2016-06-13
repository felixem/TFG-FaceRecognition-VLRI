#pragma once

#include "opencv2/core/core.hpp"

namespace tfg
{
	//Clase para aumentar la resolución de una imagen de manera simple
	class SimpleImageUpsampler
	{
	public:
		//Constructor
		SimpleImageUpsampler();
		//Destructor
		~SimpleImageUpsampler();

		//Modificar tamaño usando interpolación por vecino más cercano
		void applyNearestNeightbornFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tamaño usando interpolación bilineal
		void applyBilinearFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tamaño usando interpolación por relación de área
		void applyAreaFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tamaño usando interpolación bicúbica 4x4
		void applyBicubicFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Modificar tamaño usando interpolación Lanzcos de 8x8
		void applyLanczosFilter(const cv::Mat& img, cv::Mat &output, int height, int width);
	};
}
