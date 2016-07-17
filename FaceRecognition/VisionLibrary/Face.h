#pragma once

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//Clase utilizada para almacenar la información sobre una posible cara encontrada
namespace tfg
{
	class Face
	{
	public:
		//Constructor a partir de la imagen, la clase y el valor de confianza
		Face(const cv::Mat& img, const int clase, const double confidence);
		//Destructor
		~Face();

		//Imagen
		cv::Mat img;
		//Clase reconocida
		int clase;
		//Valor de confianza
		double confidence;

	};
}

