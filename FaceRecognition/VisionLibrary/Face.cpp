#include "stdafx.h"
#include "Face.h"

namespace tfg
{
	//Constructor a partir de la imagen, la clase y el valor de confianza
	Face::Face(const cv::Mat& img, const int clase, const double confidence) :
		img(img), clase(clase), confidence(confidence)
	{

	}

	Face::~Face()
	{
	}
}
