#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"
#include "FaceCSVReader.h"
#include "HaarLikeFaces.h"
#include "HaarLikeFaceDetector.h"

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
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = croppedFaces[croppedFaces.size() - 1];
	int testLabel = labels[labels.size() - 1];
	croppedFaces.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	EigenFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Eigen con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample,confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Mostrar o volcar en fichero el resultado
	if (outputFolder == "")
	{
		//Mostrar eigenfaces
		model.showEigenFaces(height);
		//Mostrar reconstrucción
		model.showReconstruction(testSample);
	}
	else
	{
		//Guardar eigenfaces
		model.saveEigenFaces(height, outputFolder, "eigenfaces");
		//Guardar reconstrucción
		model.saveReconstruction(testSample, outputFolder, "eigen-reconstruction");
	}

	//Esperar a teclado
	waitKey(0);

	return 0;
}

//Main para reconocimiento de caras con Fisherfaces con LR
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
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = croppedFaces[croppedFaces.size() - 1];
	int testLabel = labels[labels.size() - 1];
	croppedFaces.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	FisherFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Fisher con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Comprobar si deben mostrarse o almacenarse las reconstrucciones
	if (outputFolder == "")
	{
		//Mostrar fisherfaces
		model.showFisherFaces(height);
		//Mostrar reconstrucción
		model.showReconstruction(testSample);
	}
	else
	{
		//Mostrar fisherfaces
		model.saveFisherFaces(height,outputFolder,"fisherface");
		//Mostrar reconstrucción
		model.saveReconstruction(testSample,outputFolder,"fisher-reconstruction");
	}

	//Esperar a teclado
	waitKey(0);
	return 0;
}

//Main para reconocimiento de caras con LBP con LR
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
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la última imagen del dataset
	Mat testSample = croppedFaces[croppedFaces.size() - 1];
	int testLabel = labels[labels.size() - 1];
	croppedFaces.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Crear reconocedor
	LBPRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo LBP con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(testSample, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Esperar a teclado
	waitKey(0);
	return 0;
}