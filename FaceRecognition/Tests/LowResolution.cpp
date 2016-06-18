#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"
#include "FaceCSVReader.h"
#include "SimpleImageUpsampler.h"
#include "ImageDownsampler.h"
#include "HaarLikeFaceDetector.h"
#include "HaarLikeFaces.h"
#include "EigenTransformationUpsampler.h"

using namespace std;
using namespace cv;
using namespace tfg;

//Main para reconocimiento de caras con Eigenfaces con LR
int mainLREigenFaces(int argc, char**argv)
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}

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

	//Aplicar downsample con ruido
	ImageDownsampler downSampler;
	cv::Mat downSampledFace;
	downSampler.downSampleWithAllNoises(testSample, downSampledFace, testSample.rows/4, testSample.cols/4, cv::InterpolationFlags::INTER_NEAREST);
	//Mostrar imagen downsampled
	imshow("Imagen downsampleada con ruido", downSampledFace);
	waitKey(1000);

	//Imagen tras upsampling
	SimpleImageUpsampler upsampler;
	cv::Mat upSampledFace;
	upsampler.applyBicubicFilter(downSampledFace, upSampledFace, testSample.rows, testSample.cols);
	//Mostrar imagen downsampled
	imshow("Imagen upsampled", upSampledFace);
	waitKey(1000);

	//Crear reconocedor
	EigenFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Eigen con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada tras upsampling
	double confidence;
	int predictedLabel = model.predict(upSampledFace, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Mostrar eigenfaces
	model.showEigenFaces(height);
	//Mostrar reconstrucción
	model.showReconstruction(upSampledFace);
	//Esperar a teclado
	waitKey(0);
	return 0;
}

//Main para reconocimiento de caras con Fisherfaces
int mainLRFisherFaces(int argc, char**argv)
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}

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

	//Aplicar downsample con ruido
	ImageDownsampler downSampler;
	cv::Mat downSampledFace;
	downSampler.downSampleWithAllNoises(testSample, downSampledFace, testSample.rows / 4, testSample.cols / 4, cv::InterpolationFlags::INTER_NEAREST);
	//Mostrar imagen downsampled
	imshow("Imagen downsampleada con ruido", downSampledFace);
	waitKey(1000);

	//Imagen tras upsampling
	SimpleImageUpsampler upsampler;
	cv::Mat upSampledFace;
	upsampler.applyBicubicFilter(downSampledFace, upSampledFace, testSample.rows, testSample.cols);
	//Mostrar imagen downsampled
	imshow("Imagen upsampled", upSampledFace);
	waitKey(1000);

	//Crear reconocedor
	FisherFacesRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo Fisher con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(upSampledFace, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Mostrar fisherfaces
	model.showFisherFaces(height);
	//Mostrar reconstrucción
	model.showReconstruction(testSample);
	//Esperar a teclado
	waitKey(0);
	return 0;
}

//Main para reconocimiento de caras con LBP
int mainLRLBPFaces(int argc, char**argv)
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

	//Extraer la cara únicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	//Buscae las caras de todas las imágenes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//Añadir cara
		croppedFaces.push_back(face);
	}

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

	//Aplicar downsample con ruido
	ImageDownsampler downSampler;
	cv::Mat downSampledFace;
	downSampler.downSampleWithNoNoise(testSample, downSampledFace, testSample.rows / 4, testSample.cols / 4, cv::InterpolationFlags::INTER_NEAREST);
	//Mostrar imagen downsampled
	imshow("Imagen downsampleada con ruido", downSampledFace);
	waitKey(1000);

	
	//Imagen tras upsampling
	cv::Mat upSampledFace;

	//Realizar upsampling con EigenTransformation
	EigenTransformationUpsampler upsampler;
	//Eliminar último elemento (el de test)
	images.pop_back();
	//Entrenando transformador
	std::cout << "Entrenando transformador" << std::endl;
	//Entrenar transformador
	upsampler.train(images, testSample.cols / 4, testSample.rows / 4, 64, 64);
	//Generar imagen subida de resolución
	upsampler.upSample(downSampledFace, upSampledFace, 20);
	std::cout << "Transformador entrenado" << std::endl;

	/*SimpleImageUpsampler upsampler;
	upsampler.applyBicubicFilter(downSampledFace, upSampledFace, testSample.rows, testSample.cols);*/

	//Mostrar imagen upsampled
	imshow("Imagen upsampled", upSampledFace);
	waitKey(1000);

	//Crear reconocedor
	LBPRecognizer model;

	//Entrenar el modelo
	std::cout << "Entrenando modelo LBP con " << croppedFaces.size() << " imagenes" << std::endl;
	model.train(croppedFaces, labels);
	std::cout << "Modelo entrenado" << std::endl;

	//Predecir la clase de la cara eliminada
	double confidence;
	int predictedLabel = model.predict(upSampledFace, confidence);

	//Mostrar clase predicha
	string result_message = format("Clase predicha = %d / Confidence: %f / Clase real = %d.", predictedLabel, confidence, testLabel);
	cout << result_message << endl;

	//Esperar a teclado
	waitKey(0);
	return 0;
}