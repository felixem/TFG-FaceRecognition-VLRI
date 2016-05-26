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
		//Operador asignación
		HaarLikeFaceDetector& operator=(const HaarLikeFaceDetector& detector);
		//Destructor
		~HaarLikeFaceDetector();

		//Detectar caras en una imagen
		bool detectFaces(const cv::Mat &input, std::vector<cv::Mat> &foundFaces, cv::Mat &output);

	private:
		//Nombre de fichero de clasificación por defecto
		static const std::string FICHEROFACEDETECTION;
		//Clasificador en cascada
		cv::CascadeClassifier face_cascade;
	};
}