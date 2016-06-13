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

	//Modificar tama�o usando interpolaci�n por vecino m�s cercano
	void SimpleImageUpsampler::applyNearestNeightbornFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tama�o
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_NEAREST);
	}

	//Modificar tama�o usando interpolaci�n bilineal
	void SimpleImageUpsampler::applyBilinearFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tama�o
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_LINEAR);
	}

	//Modificar tama�o usando interpolaci�n por relaci�n de �rea
	void SimpleImageUpsampler::applyAreaFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tama�o
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_AREA);
	}

	//Modificar tama�o usando interpolaci�n bic�bica 4x4
	void SimpleImageUpsampler::applyBicubicFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tama�o
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_CUBIC);
	}

	//Modificar tama�o usando interpolaci�n Lanzcos de 8x8
	void SimpleImageUpsampler::applyLanczosFilter(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tama�o
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, cv::INTER_LANCZOS4);
	}

}
