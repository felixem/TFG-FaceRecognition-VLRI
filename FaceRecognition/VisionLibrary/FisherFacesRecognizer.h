#pragma once

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"
#include "IFaceRecognizer.h"

namespace tfg
{
	//Clase para reconocer caras según el método de Fisher
	class FisherFacesRecognizer : public IFaceRecognizer
	{
	public:
		FisherFacesRecognizer();
		~FisherFacesRecognizer();

		//Entrenar modelo a partir de un vector de imágenes y de labels
		void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels);
		//Predecir clase de la imagen
		int predict(const cv::Mat& img, double &confidence);

		//Limpiar modelo
		void clear();
		//Nombre del reconocedor
		std::string getName() const;

		//Cargar desde fichero el modelo
		void load(const std::string& fichero);
		//Guardar en fichero el modelo
		void save(const std::string& fichero);

		//Almacenar fisherfaces en fichero
		void saveFisherFaces(const int height, const std::string &directory, const std::string& baseName);
		//Mostrar fisherfaces
		void showFisherFaces(const int height);
		//Guardar reconstrucción por fases de la imagen
		void saveReconstruction(const cv::Mat& img, const std::string &directory, const std::string& baseName);
		//Mostrar reconstrucción por fases de la imagen
		void showReconstruction(const cv::Mat& img);

	private:
		//Modelo de reconocimiento
		cv::Ptr<cv::face::BasicFaceRecognizer> model;

		//Normalizar imagen
		cv::Mat norm_0_255(cv::InputArray _src);
	};
}

