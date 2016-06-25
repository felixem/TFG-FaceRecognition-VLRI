#include "stdafx.h"
#include "SimpleImageUpsampler.h"
#include "opencv2/imgproc/imgproc.hpp"

namespace tfg {
	//Constructor
	SimpleImageUpsampler::SimpleImageUpsampler(cv::InterpolationFlags tipo)
	{
		this->tipo = tipo;
	}

	//Destructor
	SimpleImageUpsampler::~SimpleImageUpsampler()
	{
	}

	//Realizar upsampling
	void SimpleImageUpsampler::upSample(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, tipo);
	}

	//Nombre del reconocedor
	std::string SimpleImageUpsampler::getName() const
	{
		//Devolver nombre según tipo
		switch (this->tipo)
		{
			case cv::InterpolationFlags::INTER_AREA:
				return "Interpolacion Area";
			case cv::InterpolationFlags::INTER_CUBIC:
				return "Interpolacion Bicubica";
			case cv::InterpolationFlags::INTER_LANCZOS4:
				return "Interpolacion Lanczos";
			case cv::InterpolationFlags::INTER_LINEAR:
				return "Interpolacion Bilineal";
			case cv::InterpolationFlags::INTER_NEAREST:
				return "Interpolacion Nearest Neightbor";
		}

		return "Desconocido";
	}
}
