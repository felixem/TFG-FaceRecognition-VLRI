#pragma once
#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"
#include "IFaceRecognizer.h"

namespace tfg
{
	//Clase utilizada para reconocer caras mediante el método de aprendizaje basado eigenfaces
	class EigenFacesRecognizer : public IFaceRecognizer
	{
	public:
		//Constructor por defecto
		EigenFacesRecognizer();
		//Destructor
		virtual ~EigenFacesRecognizer();
		//Entrenar modelo a partir de un vector de imágenes y de labels
		void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels);
		//Predecir clase de la imagen
		int predict(const cv::Mat& img, double& confidence);

		//Limpiar modelo
		void clear();
		//Nombre del reconocedor
		std::string getName() const;

		//Poner umbral de detección (valores por encima del umbral se considerarán desconocidos)
		void setUmbral(double umbral);

		//Cargar desde fichero el modelo
		void load(const std::string& fichero);
		//Guardar en fichero el modelo
		void save(const std::string& fichero);
		
		//Almacenar eigenfaces en fichero
		void saveEigenFaces(const int height, const std::string &directory, const std::string& baseName);
		//Mostrar eigenfaces
		void showEigenFaces(const int height);
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

