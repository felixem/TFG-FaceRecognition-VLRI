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
#include <windows.h>

using namespace std;
using namespace cv;
using namespace tfg;

//Entrenar bases de aprendizaje
int trainFaces(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc != 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvDatabases outputFolder]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string csvBases = argv[1];
	std::string outputFolder = argv[2];

	//Direcciones de las bases de imágenes
	std::vector<std::string> nombresBases;
	//Obtener nombres de las bases
	readDatabasesFile(csvBases, nombresBases);

	//Por cada base, cargar las imágenes y realizar el aprendizaje con cada modelo
	for (unsigned int i = 0; i < nombresBases.size(); ++i)
	{
		// Vector de imágenes y labels.
		vector<Mat> images;
		vector<int> labels;
		//Cargar imágenes
		try
		{
			FaceCSVReader::loadGrayImagesFromCSV(nombresBases[i], images, labels);
		}
		catch (cv::Exception& e) {
			cerr << "Error al abrir el fichero \"" << nombresBases[i] << "\". Razón: " << e.msg << endl;
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
		//Buscar las caras de todas las imágenes
		for (unsigned int i = 0; i < images.size(); ++i)
		{
			cv::Mat face;
			//Extraer cara principal
			faceDetector.extractMainFace(images[i], face, 32, 32, 64, 64);
			//Añadir cara
			croppedFaces.push_back(face);
		}

		//Proceso de entrenamiento para eigen
		EigenFacesRecognizer modelEigen;
		//Entrenar el modelo
		std::cout << "Entrenando modelo Eigen con " << croppedFaces.size() << " imagenes" << std::endl;
		modelEigen.train(croppedFaces, labels);
		std::cout << "Modelo entrenado" << std::endl;
		std::string outputEigen = outputFolder + "/eigenfaces" + std::to_string(i) + ".yml";
		modelEigen.save(outputEigen);
		std::cout << "Modelo almacenado en " << outputEigen << std::endl;

		//Proceso de entrenamiento para Fisher
		FisherFacesRecognizer modelFisher;
		//Entrenar el modelo
		std::cout << "Entrenando modelo Fisher con " << croppedFaces.size() << " imagenes" << std::endl;
		modelFisher.train(croppedFaces, labels);
		std::cout << "Modelo entrenado" << std::endl;
		std::string outputFisher = outputFolder + "/fisherfaces" + std::to_string(i) + ".yml";
		modelFisher.save(outputFisher);
		std::cout << "Modelo almacenado en " << outputFisher << std::endl;

		//Proceso de entrenamiento para LBP
		LBPRecognizer modelLBP;
		//Entrenar el modelo
		std::cout << "Entrenando modelo LBP con " << croppedFaces.size() << " imagenes" << std::endl;
		modelLBP.train(croppedFaces, labels);
		std::cout << "Modelo entrenado" << std::endl;
		std::string outputLBP = outputFolder + "/lbp" + std::to_string(i) + ".yml";
		modelLBP.save(outputLBP);
		std::cout << "Modelo almacenado en " << outputLBP << std::endl;

	}

	//Esperar a teclado
	waitKey(0);

	return 0;
}

//Main separar en conjunto de test y de entrenamiento
int separeDatabase(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc != 6)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " csvBase testFolder trainFolder numFaces extension]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string csvFaces = argv[1];
	std::string testFolder = argv[2];
	std::string trainFolder = argv[3];
	int numTestFaces = atoi(argv[4]);
	std::string extension = argv[5];

	//Crear directorios de salida
	if (!createDir(testFolder))
	{
		std::cerr << "Error: no se pudo crear el directorio "<<testFolder << std::endl;
		getchar();
		return -1;
	}
	if (!createDir(trainFolder))
	{
		std::cerr << "Error: no se pudo crear el directorio " << trainFolder << std::endl;
		getchar();
		return -1;
	}

	//Cargar imágenes y etiquetas
	vector<Mat> images;
	vector<int> labels;
	//Cargar imágenes
	try
	{
		FaceCSVReader::loadActualImagesFromCSV(csvFaces, images, labels);
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

	//Label de inicio
	int labelInicio = labels[0];
	std::vector<int> indicesTraining;
	indicesTraining.push_back(0);

	//Recorrer la base de imágenes y elegir la cantidad
	for (unsigned int i = 1; i < images.size(); ++i)
	{
		//Obtener label de la imagen actual
		int labelActual = labels[i];
		//Comprobar si ha cambiado el label
		if (labelActual != labelInicio)
		{
			//Elegir índices de imagen para test
			std::vector<int> indicesTest;
			for (unsigned int j = 0; j < numTestFaces; ++j)
			{
				//Seleccionar índice al azar
				int indexSelected = rand() % (indicesTraining.size());
				//Añadir índice al test
				indicesTest.push_back(indicesTraining[indexSelected]);
				//Eliminar índice para training
				indicesTraining.erase(indicesTraining.begin() + indexSelected);
			}
			//Almacenar caras en sus respectivas carpetas
			std::string currentTestFolder = testFolder + "/" + to_string(labelActual);
			std::string currentTrainFolder = trainFolder + "/" + to_string(labelActual);
			createDir(currentTestFolder);
			createDir(currentTrainFolder);

			//Almacenar caras de test
			for (unsigned j = 0; j < indicesTest.size(); j++)
			{
				//Crear fichero
				imwrite(currentTestFolder + "/test" + to_string(j) + "." + extension,images[indicesTest[j]]);
			}
			//Almacenar caras de train
			for (unsigned j = 0; j < indicesTraining.size(); j++)
			{
				//Crear fichero
				imwrite(currentTrainFolder + "/train" + to_string(j) + "." + extension, images[indicesTraining[j]]);
			}

			//Limpiar indices de training y actualizar label de inicio
			indicesTraining.clear();
			labelInicio = labelActual;
		}

		//Añadir imagen al indice de entrenamiento
		indicesTraining.push_back(i);
	}

	//Terminar último grupo
	if (indicesTraining.size() > 0)
	{
		//Elegir índices de imagen para test
		std::vector<int> indicesTest;
		for (unsigned int j = 0; j < numTestFaces; ++j)
		{
			//Seleccionar índice al azar
			int indexSelected = rand() % (indicesTraining.size());
			//Añadir índice al test
			indicesTest.push_back(indicesTraining[indexSelected]);
			//Eliminar índice para training
			indicesTraining.erase(indicesTraining.begin() + indexSelected);
		}
		//Almacenar caras en sus respectivas carpetas
		std::string currentTestFolder = testFolder + "/" + to_string(labelInicio);
		std::string currentTrainFolder = trainFolder + "/" + to_string(labelInicio);
		createDir(currentTestFolder);
		createDir(currentTrainFolder);

		//Almacenar caras de test
		for (unsigned j = 0; j < indicesTest.size(); j++)
		{
			//Crear fichero
			imwrite(currentTestFolder + "/test" + to_string(j) + "." + extension, images[indicesTest[j]]);
		}
		//Almacenar caras de train
		for (unsigned j = 0; j < indicesTraining.size(); j++)
		{
			//Crear fichero
			imwrite(currentTrainFolder + "/train" + to_string(j) + "." + extension, images[indicesTraining[j]]);
		}
	}

	//Esperar a teclado
	waitKey(0);

	return 0;
}

//Leer fichero que contiene bases de imágenes
void readDatabasesFile(const std::string &filename, std::vector<string> &databases)
{
	//Abrir fichero
	std::ifstream file(filename.c_str(), std::ifstream::in);
	if (!file) {
		std::string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}

	//Mensaje de cargado
	std::cout << "Leyendo ficheros bases de datos " << filename << std::endl;
	std::string line;
	//Cargar nombres de las bases de datos línea a línea
	while (getline(file, line)) {
		databases.push_back(line);
	}

	//Mensaje de cargado
	std::cout << "Lectura completada" << std::endl;
}

//Crear directorio
bool createDir(const std::string& dirName_in)
{
	return (CreateDirectory(dirName_in.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError());
}