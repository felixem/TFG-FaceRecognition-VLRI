#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace tfg
{
	//Clase utilizada para cargar im�genes de una base de datos csv
	class FaceCSVReader
	{
	public:
		//Constructor
		FaceCSVReader();
		//Destructor
		virtual ~FaceCSVReader();

		//Leer im�genes desde el csv y convertir a escala de grises
		static void loadGrayImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');
		//Leer im�genes desde el csv y convertir a escala de grises y recortar
		static void loadCroppedGrayImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');
		//Leer im�genes desde el csv
		static void loadActualImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');
	};
}

