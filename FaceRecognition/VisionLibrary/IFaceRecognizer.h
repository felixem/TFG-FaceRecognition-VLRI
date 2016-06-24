#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"

namespace tfg
{
	//Interfaz para reconocedor de clasas
	class IFaceRecognizer
	{
	public:
		IFaceRecognizer();
		virtual ~IFaceRecognizer() {}

		//Entrenar modelo a partir de un vector de imágenes y de labels
		virtual void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels) = 0;
		//Predecir clase de la imagen
		virtual int predict(const cv::Mat& img, double& confidence) = 0;

		//Limpiar modelo
		virtual void clear() = 0;
		//Nombre del reconocedor
		virtual std::string getName() const = 0;

		//Cargar desde fichero el modelo
		virtual void load(const std::string& fichero) = 0;
		//Guardar en fichero el modelo
		virtual void save(const std::string& fichero) = 0;
	};
}

