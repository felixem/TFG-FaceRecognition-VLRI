#include "stdafx.h"
#include "FaceCSVReader.h"

#include <sstream>
#include <fstream>
#include <iostream>

namespace tfg
{
	//Constructor por defecto
	FaceCSVReader::FaceCSVReader()
	{
	}

	//Destructor
	FaceCSVReader::~FaceCSVReader()
	{
	}

	//Leer imágenes desde el csv
	void FaceCSVReader::loadImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, 
		std::vector<int>& labels, char separator)
	{
		//Abrir fichero
		std::ifstream file(filename.c_str(), std::ifstream::in);
		if (!file) {
			std::string error_message = "No valid input file was given, please check the given filename.";
			CV_Error(CV_StsBadArg, error_message);
		}
		std::string line, path, classlabel;
		//Cargar imágenes y etiquetas línea a línea
		while (getline(file, line)) {
			std::stringstream liness(line);
			getline(liness, path, separator);
			getline(liness, classlabel);
			if (!path.empty() && !classlabel.empty()) {
				//Cargar imagen
				cv::Mat img = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
				//Comprobar que se haya cargado bien
				if (img.empty())
				{
					std::cerr << "Error al cargar " << line << std::endl;
				}
				else
				{
					//Añadir imagen
					images.push_back(img);
					labels.push_back(atoi(classlabel.c_str()));
				}
			}
		}
	}
}
