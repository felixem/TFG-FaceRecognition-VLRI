#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace tfg
{
	//Clase utilizada para cargar imágenes de una base de datos csv
	class FaceCSVReader
	{
	public:
		//Constructor
		FaceCSVReader();
		//Destructor
		virtual ~FaceCSVReader();

		//Leer imágenes desde el csv
		static void loadImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');
	};
}

