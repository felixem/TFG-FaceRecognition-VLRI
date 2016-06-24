#include "stdafx.h"
#include "FaceCSVReader.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include "HaarLikeFaceDetector.h"

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

	//Leer im�genes desde el csv y convertir a escala de grises
	void FaceCSVReader::loadGrayImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images, 
		std::vector<int>& labels, char separator)
	{
		//Abrir fichero
		std::ifstream file(filename.c_str(), std::ifstream::in);
		if (!file) {
			std::string error_message = "Archivo "+filename+" no pudo ser abierto";
			CV_Error(CV_StsBadArg, error_message);
		}
		//Fichero corregido
		//std::ofstream outputFile((filename + "corregido").c_str());

		//Mensaje de cargado
		std::cout << "Cargando imagenes desde " << filename << std::endl;
		std::string line, path, classlabel;
		//Cargar im�genes y etiquetas l�nea a l�nea
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
					//outputFile << line << "\n";
					//A�adir imagen
					images.push_back(img);
					labels.push_back(atoi(classlabel.c_str()));
				}
			}
		}

		//Mensaje de cargado
		std::cout << "Carga completada y conversi�n a escala de grises" << std::endl;
		//Cerrar fichero
		//outputFile.close();
	}

	//Leer im�genes desde el csv y convertir a escala de grises
	void FaceCSVReader::loadCroppedGrayImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images,
		std::vector<int>& labels, char separator)
	{
		//Abrir fichero
		std::ifstream file(filename.c_str(), std::ifstream::in);
		if (!file) {
			std::string error_message = "Archivo " + filename + " no pudo ser abierto";
			CV_Error(CV_StsBadArg, error_message);
		}
		//Fichero corregido
		//std::ofstream outputFile((filename + "corregido").c_str());
		//Detector de caras
		HaarLikeFaceDetector faceDetector;

		//Mensaje de cargado
		std::cout << "Cargando imagenes y recortando caras desde " << filename << std::endl;
		std::string line, path, classlabel;
		//Cargar im�genes y etiquetas l�nea a l�nea
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
					//outputFile << line << "\n";
					cv::Mat face;
					//Extraer cara principal
					faceDetector.extractMainFace(img, face, 32, 32, 64, 64);
					//A�adir imagen con la cara detectada
					images.push_back(face);
					labels.push_back(atoi(classlabel.c_str()));
				}
			}
		}

		//Mensaje de cargado
		std::cout << "Carga completada y conversi�n a escala de grises" << std::endl;
		//Cerrar fichero
		//outputFile.close();
	}

	//Leer im�genes desde el csv
	void FaceCSVReader::loadActualImagesFromCSV(const std::string& filename, std::vector<cv::Mat>& images,
		std::vector<int>& labels, char separator)
	{
		//Abrir fichero
		std::ifstream file(filename.c_str(), std::ifstream::in);
		if (!file) {
			std::string error_message = "Archivo " + filename + " no pudo ser abierto";
			CV_Error(CV_StsBadArg, error_message);
		}

		//Mensaje de cargado
		std::cout << "Cargando imagenes desde " << filename << std::endl;
		std::string line, path, classlabel;
		//Cargar im�genes y etiquetas l�nea a l�nea
		while (getline(file, line)) {
			std::stringstream liness(line);
			getline(liness, path, separator);
			getline(liness, classlabel);
			if (!path.empty() && !classlabel.empty()) {
				//Cargar imagen
				cv::Mat img = cv::imread(path);
				//Comprobar que se haya cargado bien
				if (img.empty())
				{
					std::cerr << "Error al cargar " << line << std::endl;
				}
				else
				{
					//outputFile << line << "\n";
					//A�adir imagen
					images.push_back(img);
					labels.push_back(atoi(classlabel.c_str()));
				}
			}
		}

		//Mensaje de cargado
		std::cout << "Carga completada" << std::endl;
	}
}
