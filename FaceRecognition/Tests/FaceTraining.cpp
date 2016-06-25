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
#include "IFaceRecognizer.h"
#include "ImageUpsampler.h"
#include "WaveletSpatialSuperResolution.h"
#include <windows.h>

using namespace std;
using namespace cv;
using namespace tfg;

//Main para test de reconocimiento de caras
int testFaces(int argc, char**argv)
{
	//Comprobar parámetros
	if (argc != 4)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " testCSV modelType ymlDir]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv
	std::string csvFaces = argv[1];
	//Tipo de modelo
	int modelType = atoi(argv[2]);
	//Dirección del modelo entreado
	std::string trainedModelDir = argv[3];

	std::cout << "Cargando modelo de reconocimiento" << std::endl;
	//Cargar modelo de reconocimiento según parámetros
	IFaceRecognizer* recognizer = NULL;
	switch (modelType)
	{
		//Eigenfaces
	case 0: recognizer = new EigenFacesRecognizer();
		recognizer->load(trainedModelDir);
		break;
		//Fisher
	case 1: recognizer = new FisherFacesRecognizer();
		recognizer->load(trainedModelDir);
		break;
		//LBP
	case 2: recognizer = new LBPRecognizer();
		recognizer->load(trainedModelDir);
		break;
		//Fallo al cargar
	default:
		std::cerr << "Error: tipo de modelo incorrecto " << std::endl;
		getchar();
		return -1;
	}
	std::cout << "Modelo de reconocimiento cargado" << std::endl;

	//Cargar imágenes de test
	vector<Mat> images;
	vector<int> labels;
	//Cargar imágenes
	try
	{
		FaceCSVReader::loadCroppedGrayImagesFromCSV(csvFaces, images, labels);
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

	//Almacenar valores de acierto y de fallo
	int numAciertos = 0, numFallos = 0;
	//Confianza para aciertos y fallos
	double confianzaAciertos = 0, confianzaFallos = 0;
	//Realizar test para cada imagen del conjunto
	std::cout << "Realizando test para "<<images.size()<<" imagenes" << std::endl;
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		//Realizar predicción
		int correctLabel = labels[i];
		double confidence;
		int prediction = recognizer->predict(images[i],confidence);
		//Comprobar si se ha acertado o fallado
		if (prediction == correctLabel)
		{
			confianzaAciertos += confidence;
			numAciertos++;
		}
		else
		{
			confianzaFallos += confidence;
			numFallos++;
		}
	}
	std::cout << "Test finalizado" << std::endl;

	//Mostrar resultados
	std::cout << "Tecnica de aprendizaje: " << recognizer->getName() << std::endl;
	std::cout << "Conjunto de test: " << images.size() << std::endl;
	std::cout << "Aciertos: " << numAciertos << std::endl;
	std::cout << "Fallos: " << numFallos << std::endl;
	std::cout << "Porcentaje de acierto: " << (double)numAciertos/((double)images.size())*100 <<"%" << std::endl;
	std::cout << "Confianza media aciertos: " << confianzaAciertos / (double)numAciertos << std::endl;
	std::cout << "Confianza media fallos: " << confianzaFallos / (double)numFallos << std::endl;

	//Preparar downsampling
	std::vector<int> downSamplingSizes;
	downSamplingSizes.push_back(32);
	downSamplingSizes.push_back(16);
	downSamplingSizes.push_back(8);
	downSamplingSizes.push_back(4);
	ImageDownsampler downSampler;
	int numAlgoritmosDownsampling = 6;

	//Realizar bucle de test con downsampling
	for (unsigned int indexTam = 0; indexTam < downSamplingSizes.size(); ++indexTam)
	{
		//Realizar downsampling
		int size = downSamplingSizes[indexTam];
		std::cout << "---------------------------" << std::endl;
		std::cout << "Upsampling desde " << size << "x" << size << std::endl;
		//Tests para los distintos algoritmos
		for (unsigned int i = 0; i < numAlgoritmosDownsampling; ++i)
		{
			//Algoritmo de upsampling
			ImageUpsampler* upsampler = generateUpsampler(i);
			//Almacenar valores de acierto y de fallo
			int numAciertos = 0, numFallos = 0;
			//Confianza para aciertos y fallos
			double confianzaAciertos = 0, confianzaFallos = 0;

			//Realizar test para cada imagen del conjunto
			std::cout << "Realizando test para " << upsampler->getName()<< std::endl;
			for (unsigned int j = 0; j < images.size(); ++j)
			{
				//Generar imagen downsampleada
				cv::Mat downSampled;
				downSampler.downSampleWithAllNoises(images[j], downSampled, size, size, cv::InterpolationFlags::INTER_AREA);
				//Aplicar upsampling
				cv::Mat upSampled;
				upsampler->upSample(downSampled, upSampled, 64, 64);
				//Realizar predicción
				int correctLabel = labels[j];
				double confidence;
				int prediction = recognizer->predict(upSampled, confidence);
				//Comprobar si se ha acertado o fallado
				if (prediction == correctLabel)
				{
					confianzaAciertos += confidence;
					numAciertos++;
				}
				else
				{
					confianzaFallos += confidence;
					numFallos++;
				}
			}
			std::cout << "Aciertos: " << numAciertos << std::endl;
			std::cout << "Fallos: " << numFallos << std::endl;
			std::cout << "Porcentaje de acierto: " << (double)numAciertos / ((double)images.size()) * 100 << "%" << std::endl;
			std::cout << "Confianza media aciertos: " << confianzaAciertos / (double)numAciertos << std::endl;
			std::cout << "Confianza media fallos: " << confianzaFallos / (double)numFallos << std::endl << std::endl;
			//Liberar upsampler
			delete upsampler;
		}
	}

	//Limpiar reconocedor
	delete recognizer;

	//Esperar a teclado
	std::cout << "Pulse una tecla para terminar: " << std::endl;
	getchar();
	return 0;
}

//Generar upsampler
ImageUpsampler* generateUpsampler(int opcion)
{
	//Generar upsampler según opción
	switch (opcion)
	{
		case 0:
			//Vecino más cercano
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_NEAREST);
		case 1:
			//Área
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_AREA);
		case 2:
			//Bilineal
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_LINEAR);
		case 3:
			//Bicúbico
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_CUBIC);
		case 4:
			//Lanczos
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_LANCZOS4);
		case 5:
			//Wavelet
			return new WaveletSpatialSRUpsampler(2);
	}

	return NULL;
}

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

	//Crear directorios de salida
	if (!createDir(outputFolder))
	{
		std::cerr << "Error: no se pudo crear el directorio " << outputFolder << std::endl;
		getchar();
		return -1;
	}
	//Direcciones de las bases de imágenes
	std::vector<std::string> nombresBases;
	//Obtener nombres de las bases
	readDatabasesFile(csvBases, nombresBases);

	//Por cada base, cargar las imágenes y realizar el aprendizaje con cada modelo
	for (unsigned int i = 0; i < nombresBases.size(); ++i)
	{
		// Vector de imágenes y labels.
		vector<Mat> croppedFaces;
		vector<int> labels;
		//Cargar imágenes
		try
		{
			FaceCSVReader::loadCroppedGrayImagesFromCSV(nombresBases[i], croppedFaces, labels);
		}
		catch (cv::Exception& e) {
			cerr << "Error al abrir el fichero \"" << nombresBases[i] << "\". Razón: " << e.msg << endl;
			getchar();
			return -1;
		}

		//Comprobar que se han cargado las imágenes
		if (croppedFaces.size() <= 1) {
			string error_message = "¡Añade más imagenes al dataset!";
			CV_Error(CV_StsError, error_message);
			getchar();
			return -1;
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
		//Limpiar modelo
		modelEigen.clear();

		//Proceso de entrenamiento para Fisher
		FisherFacesRecognizer modelFisher;
		//Entrenar el modelo
		std::cout << "Entrenando modelo Fisher con " << croppedFaces.size() << " imagenes" << std::endl;
		modelFisher.train(croppedFaces, labels);
		std::cout << "Modelo entrenado" << std::endl;
		std::string outputFisher = outputFolder + "/fisherfaces" + std::to_string(i) + ".yml";
		modelFisher.save(outputFisher);
		std::cout << "Modelo almacenado en " << outputFisher << std::endl;
		//Limpiar modelo
		modelFisher.clear();

		//Proceso de entrenamiento para LBP
		LBPRecognizer modelLBP;
		//Entrenar el modelo
		std::cout << "Entrenando modelo LBP con " << croppedFaces.size() << " imagenes" << std::endl;
		modelLBP.train(croppedFaces, labels);
		std::cout << "Modelo entrenado" << std::endl;
		std::string outputLBP = outputFolder + "/lbp" + std::to_string(i) + ".yml";
		modelLBP.save(outputLBP);
		std::cout << "Modelo almacenado en " << outputLBP << std::endl;
		//Limpiar modelo
		modelLBP.clear();

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