#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"

namespace tfg
{
	//Clase utilizada para realizar upsampling mediante la t�cnica de EigenTransformation
	class EigenTransformationUpsampler
	{
	public:
		//Constructor
		EigenTransformationUpsampler();
		//Destructor
		~EigenTransformationUpsampler();

		//Entrenar modelo a partir del conjunto de im�genes de alta resoluci�n de entrenamiento y las dimensiones de
		//las im�genes de alta y baja resoluci�n
		void train(const std::vector<cv::Mat>&imgs, int lowResWidth, int lowResHeight, int highResWidth, int highResHeight);

		//Generar imagen de alta resoluci�n a partir de la imagen de baja resoluci�n
		void upSample(const cv::Mat& img, cv::Mat& output, const int kFacesToUse = 50);

	private:
		//Altura y anchura de la imagen de baja resoluci�n
		int lowResWidth;
		int lowResHeight;
		//Altura y anchura de la imagen de alta resoluci�n
		int highResWidth;
		int highResHeight;
		//Modelo de Eigenfaces para baja resoluci�n
		cv::Ptr<cv::face::BasicFaceRecognizer> lowModel;
		//Modelo de Eigenfaces para alta resoluci�n
		cv::Ptr<cv::face::BasicFaceRecognizer> highModel;

		//Normalizar imagen
		cv::Mat norm_0_255(cv::InputArray _src);
	};
}

