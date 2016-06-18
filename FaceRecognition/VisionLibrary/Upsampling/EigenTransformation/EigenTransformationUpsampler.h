#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/face.hpp"

namespace tfg
{
	//Clase utilizada para realizar upsampling mediante la técnica de EigenTransformation
	class EigenTransformationUpsampler
	{
	public:
		//Constructor
		EigenTransformationUpsampler();
		//Destructor
		~EigenTransformationUpsampler();

		//Entrenar modelo a partir del conjunto de imágenes de alta resolución de entrenamiento y las dimensiones de
		//las imágenes de alta y baja resolución
		void train(const std::vector<cv::Mat>&imgs, int lowResWidth, int lowResHeight, int highResWidth, int highResHeight);

		//Generar imagen de alta resolución a partir de la imagen de baja resolución
		void upSample(const cv::Mat& img, cv::Mat& output, const int kFacesToUse = 50);

	private:
		//Altura y anchura de la imagen de baja resolución
		int lowResWidth;
		int lowResHeight;
		//Altura y anchura de la imagen de alta resolución
		int highResWidth;
		int highResHeight;
		//Modelo de Eigenfaces para baja resolución
		cv::Ptr<cv::face::BasicFaceRecognizer> lowModel;
		//Modelo de Eigenfaces para alta resolución
		cv::Ptr<cv::face::BasicFaceRecognizer> highModel;

		//Normalizar imagen
		cv::Mat norm_0_255(cv::InputArray _src);
	};
}

