#include "stdafx.h"
#include "SimpleImageUpsampler.h"
#include "opencv2/imgproc/imgproc.hpp"

namespace tfg {
	//Constructor
	SimpleImageUpsampler::SimpleImageUpsampler()
	{
	}

	//Destructor
	SimpleImageUpsampler::~SimpleImageUpsampler()
	{
	}

	//Modificar tamaño usando interpolación por vecino más cercano
	void SimpleImageUpsampler::applyNearestNeightbornFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_NEAREST);
	}

	//Modificar tamaño usando interpolación bilineal
	void SimpleImageUpsampler::applyBilinearFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_LINEAR);
	}

	//Modificar tamaño usando interpolación por relación de área
	void SimpleImageUpsampler::applyAreaFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_AREA);
	}

	//Modificar tamaño usando interpolación bicúbica 4x4
	void SimpleImageUpsampler::applyBicubicFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_CUBIC);
	}

	//Modificar tamaño usando interpolación Lanzcos de 8x8
	void SimpleImageUpsampler::applyLanczosFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_LANCZOS4);
	}

}
