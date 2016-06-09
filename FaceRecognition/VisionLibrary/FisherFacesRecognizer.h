#pragma once

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"

namespace tfg
{
	//Clase para reconocer caras seg�n el m�todo de Fisher
	class FisherFacesRecognizer
	{
	public:
		FisherFacesRecognizer();
		~FisherFacesRecognizer();

		//Entrenar modelo a partir de un vector de im�genes y de labels
		void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels);
		//Predecir clase de la imagen
		int predict(const cv::Mat& img, double &confidence);

		//Almacenar fisherfaces en fichero
		void saveFisherFaces(const int height, const std::string &directory, const std::string& baseName);
		//Mostrar fisherfaces
		void showFisherFaces(const int height);
		//Guardar reconstrucci�n por fases de la imagen
		void saveReconstruction(const cv::Mat& img, const std::string &directory, const std::string& baseName);
		//Mostrar reconstrucci�n por fases de la imagen
		void showReconstruction(const cv::Mat& img);

	private:
		//Modelo de reconocimiento
		cv::Ptr<cv::face::BasicFaceRecognizer> model;

		//Normalizar imagen
		cv::Mat norm_0_255(cv::InputArray _src);
	};
}

