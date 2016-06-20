#pragma once
#include "stdafx.h"
#include <string>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace tfg
{
	//Clase utilizada para detectar caras
	class HaarLikeFaceDetector
	{
	public:

		//Constructor por defecto
		HaarLikeFaceDetector();
		//Constructor a partir del nombre de fichero del detector
		HaarLikeFaceDetector(std::string faceCascadeFile);
		//Constructor de copia
		HaarLikeFaceDetector(const HaarLikeFaceDetector & detector);
		//Operador asignaci�n
		HaarLikeFaceDetector& operator=(const HaarLikeFaceDetector& detector);
		//Destructor
		~HaarLikeFaceDetector();

		//Detectar caras en una imagen
		bool detectFaces(const cv::Mat & input, std::vector<cv::Mat>& foundFaces, cv::Mat & output,
			float scale = 1.01, int minWidth = 16, int minHeight = 16, int maxWidth = 0, int maxHeight = 0);

		//Extraer la cara principal de la imagen
		void extractMainFace(const cv::Mat &img, cv::Mat &face, int minWidth, int minHeight, int finalWidth, int finalHeight);

	private:
		//Nombre de fichero de clasificaci�n por defecto
		static const std::string FICHEROFACEDETECTION;
		//Clasificador en cascada
		cv::CascadeClassifier face_cascade;
	};
}