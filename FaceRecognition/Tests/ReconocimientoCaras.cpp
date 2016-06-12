#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"
#include "FaceCSVReader.h"

using namespace std;
using namespace cv;
using namespace tfg;

//Main para reconocimiento de caras con Eigenfaces
int mainEigenFaces(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvDir outputFolder]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string csvFaces = argv[1];
	std::string outputFolder = "";
	if (argc == 3)
		outputFolder = argv[2];

	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;
	//Cargar imágenes
	try
	{
		FaceCSVReader::loadImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Razón: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las imágenes
	if (images.size() <= 1) {
		string error_message = "¡Añade más imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	// Altura de la primera imagen:
	int height = images[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	EigenFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Eigen con " << images.size() << " imagenes" << std::endl;
	model.train(images, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample,confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Mostrar eigenfaces
	model.showEigenFaces(height);
	//Mostrar reconstrucción
	model.showReconstruction(testSample);
	//Esperar a teclado
	getchar();
	return 0;
}

//Main para reconocimiento de caras con Fisherfaces
int mainFisherFaces(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvDir outputFolder]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string csvFaces = argv[1];
	std::string outputFolder = "";
	if (argc == 3)
		outputFolder = argv[2];

	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;
	//Cargar imágenes
	try
	{
		FaceCSVReader::loadImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Razón: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las imágenes
	if (images.size() <= 1) {
		string error_message = "¡Añade más imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	// Altura de la primera imagen:
	int height = images[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	FisherFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Fisher con " << images.size() << " imagenes" << std::endl;
	model.train(images, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Mostrar fisherfaces
	model.showFisherFaces(height);
	//Mostrar reconstrucción
	model.showReconstruction(testSample);
	//Esperar a teclado
	getchar();
	return 0;
}

//Main para reconocimiento de caras con LBP
int mainLBPFaces(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc != 2)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvDir ]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv
	std::string csvFaces = argv[1];

	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;
	//Cargar imágenes
	try
	{
		FaceCSVReader::loadImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Razón: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las imágenes
	if (images.size() <= 1) {
		string error_message = "¡Añade más imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	// Altura de la primera imagen:
	int height = images[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	LBPRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo LBP con " << images.size() << " imagenes" << std::endl;
	model.train(images, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Esperar a teclado
	getchar();
	return 0;
}