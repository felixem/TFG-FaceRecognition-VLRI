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
#include "WaveletSpatialSuperResolution.h"
#include "FaceTraining.h"

using namespace std;
using namespace cv;
using namespace tfg;

//Main para mostrar el funcionamiento de los algoritmos de super-resolution
int mainSuperResolution(int argc, char**argv)
{
	//Comprobar par�metros
	if (argc != 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvDir outputFolder]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string csvFaces = argv[1];
	std::string outputFolder = argv[2];

	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Raz�n: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las im�genes
	if (images.size() <= 1) {
		string error_message = "�A�ade m�s imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	//Extraer la cara �nicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las im�genes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 16, 16, 128, 128);
		//A�adir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Eliminar la �ltima imagen del dataset
	Mat testSample = croppedFaces[croppedFaces.size() - 1];
	int testLabel = labels[labels.size() - 1];
	croppedFaces.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	imwrite(outputFolder + "/" + "imgRetirada.jpg", testSample);
	waitKey(20);

	//Algoritmos de upsampling
	unsigned int numAlgoritmosUpsampling = 5;

	//Preparar downsampling
	std::vector<int> downSamplingSizes;
	downSamplingSizes.push_back(32);
	downSamplingSizes.push_back(16);
	downSamplingSizes.push_back(8);
	downSamplingSizes.push_back(4);

	//Tama�o final
	int finalWidth = testSample.size().width;
	int finalHeight = testSample.size().height;

	//Aplicar downsample con ruido
	ImageDownsampler downSampler;
	//Realizar bucle de test con downsampling
	for (unsigned int indexTam = 0; indexTam < downSamplingSizes.size(); ++indexTam)
	{
		//Realizar downsampling
		int size = downSamplingSizes[indexTam];
		cv::Mat downSampledFace;
		downSampler.downSampleWithNoNoise(testSample, downSampledFace, size, size, cv::InterpolationFlags::INTER_NEAREST);
		//Mostrar imagen downsampled
		imshow("Imagen downsampleada con ruido " + std::to_string(size) + "x" + std::to_string(size), downSampledFace);
		imwrite(outputFolder + "/" + "imgRuido"+ std::to_string(size) + "x" + std::to_string(size)+".jpg", downSampledFace);
		waitKey(20);

		//Generar upsampling
		for (unsigned int j = 0; j < numAlgoritmosUpsampling; ++j)
		{
			//Generar upsampler
			tfg::ImageUpsampler* upsampler = generateUpsampler(j);
			cv::Mat upSampledFace;
			upsampler->upSample(downSampledFace, upSampledFace, finalHeight, finalWidth);
			//Mostrar imagen upsampled
			imshow("Imagen upsampled con " + upsampler->getName(), upSampledFace);
			imwrite(outputFolder + "/" + "imgUpsampled" + std::to_string(size) + "x" + std::to_string(size) + "-" + upsampler->getName() + ".jpg", upSampledFace);
			waitKey(20);
			delete upsampler;
		}

	}

	return 0;
}

//Main para reconocimiento de caras con Eigenfaces con LR
int mainLREigenFaces(int argc, char**argv)
{
	//Comprobar par�metros
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
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Raz�n: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las im�genes
	if (images.size() <= 1) {
		string error_message = "�A�ade m�s imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	//Extraer la cara �nicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las im�genes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 16, 16, 64, 64);
		//A�adir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la �ltima imagen del dataset
	Mat testSample = croppedFaces[croppedFaces.size() - 1];
	int testLabel = labels[labels.size() - 1];
	croppedFaces.pop_back();
	labels.pop_back();

	//Mostrar imagen retirada
	imshow("Imagen retirada", testSample);
	waitKey(1000);

	//Algoritmos de upsampling
	unsigned int numAlgoritmosUpsampling = 5;

	//Preparar downsampling
	std::vector<int> downSamplingSizes;
	downSamplingSizes.push_back(32);
	downSamplingSizes.push_back(16);
	downSamplingSizes.push_back(8);
	downSamplingSizes.push_back(4);

	//Aplicar downsample con ruido
	ImageDownsampler downSampler;
	//Realizar bucle de test con downsampling
	for (unsigned int indexTam = 0; indexTam < downSamplingSizes.size(); ++indexTam)
	{
		//Realizar downsampling
		int size = downSamplingSizes[indexTam];
		cv::Mat downSampledFace;
		downSampler.downSampleWithAllNoises(testSample, downSampledFace, size, size, cv::InterpolationFlags::INTER_NEAREST);
		//Mostrar imagen downsampled
		imshow("Imagen downsampleada con ruido "+std::to_string(size)+"x"+std::to_string(size), downSampledFace);
		waitKey(0);

		//Generar upsampling
		for (unsigned int j = 0; j < numAlgoritmosUpsampling; ++j)
		{
			//Generar upsampler
			tfg::ImageUpsampler* upsampler = generateUpsampler(j);
			cv::Mat upSampledFace;
			upsampler->upSample(downSampledFace, upSampledFace, testSample.rows, testSample.cols);
			//Mostrar imagen upsampled
			imshow("Imagen upsampled con "+upsampler->getName(), upSampledFace);
			waitKey(0);
			delete upsampler;
		}

	}

	//Downsampling
	cv::Mat downSampledFace;
	downSampler.downSampleWithAllNoises(testSample, downSampledFace, testSample.rows/4, testSample.cols/4, cv::InterpolationFlags::INTER_NEAREST);
	//Imagen tras upsampling
	SimpleImageUpsampler upsampler(cv::InterpolationFlags::INTER_CUBIC);
	cv::Mat upSampledFace;
	upsampler.upSample(downSampledFace, upSampledFace, testSample.rows, testSample.cols);
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

	//Mostrar o volcar en fichero el resultado
	if (outputFolder == "")
	{
		//Mostrar eigenfaces
		model.showEigenFaces(height);
		//Mostrar reconstrucci�n
		model.showReconstruction(testSample);
	}
	else
	{
		//Guardar eigenfaces
		model.saveEigenFaces(height, outputFolder, "eigenfaces");
		//Guardar reconstrucci�n
		model.saveReconstruction(testSample, outputFolder, "eigen-reconstruction");
	}

	//Esperar a teclado
	waitKey(0);
	return 0;
}

//Main para reconocimiento de caras con Fisherfaces
int mainLRFisherFaces(int argc, char**argv)
{
	//Comprobar par�metros
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
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Raz�n: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las im�genes
	if (images.size() <= 1) {
		string error_message = "�A�ade m�s imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	//Extraer la cara �nicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las im�genes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//A�adir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la �ltima imagen del dataset
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
	SimpleImageUpsampler upsampler(cv::InterpolationFlags::INTER_CUBIC);
	cv::Mat upSampledFace;
	upsampler.upSample(downSampledFace, upSampledFace, testSample.rows, testSample.cols);
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
	//Mostrar reconstrucci�n
	model.showReconstruction(testSample);
	//Esperar a teclado
	waitKey(0);
	return 0;
}

//Main para reconocimiento de caras con LBP
int mainLRLBPFaces(int argc, char**argv)
{
	//Comprobar par�metros
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
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadGrayImagesFromCSV(csvFaces, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error al abrir el fichero \"" << csvFaces << "\". Raz�n: " << e.msg << endl;
		getchar();
		return -1;
	}
	//Comprobar que se han cargado las im�genes
	if (images.size() <= 1) {
		string error_message = "�A�ade m�s imagenes al dataset!";
		CV_Error(CV_StsError, error_message);
		getchar();
		return -1;
	}

	//Extraer la cara �nicamente
	std::vector<cv::Mat> croppedFaces;
	HaarLikeFaceDetector faceDetector;
	std::cout << "Detectando caras" << std::endl;
	//Buscae las caras de todas las im�genes
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		cv::Mat face;
		//Extraer cara principal
		faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
		//A�adir cara
		croppedFaces.push_back(face);
	}
	std::cout << "Caras detectadas" << std::endl;

	// Altura de la primera imagen:
	int height = croppedFaces[0].rows;
	// Eliminar la �ltima imagen del dataset
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
	cv::Mat upSampledFace;
	/*SimpleImageUpsampler upsampler;
	upsampler.applyBicubicFilter(downSampledFace, upSampledFace, testSample.rows, testSample.cols);*/

	//Realizar upsampling con WaveletSpatial
	WaveletSpatialSRUpsampler upsampler(2);
	upsampler.upSample(downSampledFace, upSampledFace, testSample.cols, testSample.rows);

	//Realizar upsampling con EigenTransformation
	/*EigenTransformationUpsampler upsampler;
	//Eliminar �ltimo elemento (el de test)
	images.pop_back();
	//Entrenando transformador
	std::cout << "Entrenando transformador" << std::endl;
	//Entrenar transformador
	upsampler.train(images, testSample.cols / 4, testSample.rows / 4, 64, 64);
	//Generar imagen subida de resoluci�n
	upsampler.upSample(downSampledFace, upSampledFace, 20);
	std::cout << "Transformador entrenado" << std::endl;*/

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