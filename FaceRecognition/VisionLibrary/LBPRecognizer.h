#pragma once

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"

namespace tfg
{
	//Clase utilizada para reconocer caras según el método Local Binary Patterns
	class LBPRecognizer
	{
	public:
		//Constructor
		LBPRecognizer();
		//Destructor
		~LBPRecognizer();

		//Entrenar modelo a partir de un vector de imágenes y de labels
		void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels);
		//Predecir clase de la imagen
		int predict(const cv::Mat& img, double &confidence);

	private:
		//Modelo de reconocimiento
		cv::Ptr<cv::face::FaceRecognizer> model;
	};
}

