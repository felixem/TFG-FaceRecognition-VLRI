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
#include "StatisticsCalculator.h"
#include <windows.h>
#include <limits>       // std::numeric_limits

using namespace std;
using namespace cv;
using namespace tfg;

//Main para test de reconocimiento de caras en situaciones reales
int testRealFaces(int argc, char**argv)
{
	//Comprobar par�metros
	if (argc != 5)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " testCSV modelType ymlDir ficResultados]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv
	std::string csvFaces = argv[1];
	//Tipo de modelo
	int modelType = atoi(argv[2]);
	//Direcci�n del modelo entreado
	std::string trainedModelDir = argv[3];

	std::cout << "Cargando modelo de reconocimiento" << std::endl;
	//Cargar modelo de reconocimiento seg�n par�metros
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

	//Cargar im�genes de test
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

	//Fichero de salida de resultados
	std::ofstream ficSalida(argv[4]);
	//Crear calculadora de estad�sticas
	StatisticsCalculator calculator;
	calculator.imprimirCabecera(ficSalida);

	//Detector de caras
	HaarLikeFaceDetector faceDetector;
	unsigned int numAlgoritmosUpsampling = 6;

	//Realizar detecci�n de caras
	std::cout << "Detectando caras" << std::endl;
	std::vector<std::vector<cv::Mat>> totalFacesFound;
	for (unsigned int i = 0; i < images.size(); ++i)
	{
		//Detectar caras
		std::vector<cv::Mat> foundFaces;
		cv::Mat detectedFaces;
		faceDetector.detectFaces(images[i], foundFaces, detectedFaces, 1.1f, 8, 8);
		//A�adir cara al conjunto
		totalFacesFound.push_back(foundFaces);
		//Mostrar caras detectadas
#ifdef DEBUG
		imshow("Caras detectadas", detectedFaces);
		waitKey(0);
#endif
	}
	std::cout << "Caras detectadas" << std::endl;

	//Tests para los distintos algoritmos
	for (unsigned int i = 0; i < numAlgoritmosUpsampling; ++i)
	{
		//Algoritmo de upsampling
		ImageUpsampler* upsampler = generateUpsampler(i);
		//Crear calculadora de estad�sticas
		calculator.reiniciar();

		//Medida del tiempo en segundos
		double time = (double)getTickCount();
		//Realizar test para cada imagen del conjunto
		std::cout << "Realizando test para " <<recognizer->getName()<<" - "<< upsampler->getName() << std::endl;
		for (unsigned int j = 0; j < totalFacesFound.size(); ++j)
		{
			//Detectar las caras de la imagen
			const std::vector<cv::Mat> &foundFaces = totalFacesFound[j];

			//Recorrer las caras encontradas
			for (unsigned int z = 0; z < foundFaces.size(); ++z)
			{
				//Aplicar upsampling
				cv::Mat upSampled;
				upsampler->upSample(foundFaces[z], upSampled, 64, 64);

#ifdef DEBUG
				imshow("Caras detectada", foundFaces[z]);
				imshow("Caras upsampled", upSampled);
				waitKey(0);
#endif

				//Realizar predicci�n
				int correctLabel = labels[j];
				double confidence;
				int prediction = recognizer->predict(upSampled, confidence);
				//A�adir muestra a la calculadora
				calculator.addMuestra(prediction, correctLabel, confidence);
			}
		}

		//Calcular tiempo medio final
		time = ((double)getTickCount() - time) / getTickFrequency() / (double)images.size();
		//Finalizar c�lculos
		calculator.realizarCalculos();
		//A�adir nombre y tiempo a la calculadora
		calculator.setNombreAlgoritmo(recognizer->getName());
		calculator.setTime(time);

		//Mostrar resultados
		calculator.imprimirResultados();
		std::cout << std::endl;
		//Volcado de resultados sobre fichero
		ficSalida << calculator;

		//Liberar upsampler
		delete upsampler;
	}

	//Limpiar reconocedor
	delete recognizer;
	return 0;
}

//Main para test de reconocimiento de caras
int testFaces(int argc, char**argv)
{
	//Comprobar par�metros
	if (argc != 5)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " testCSV modelType ymlDir ficResultados]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv
	std::string csvFaces = argv[1];
	//Tipo de modelo
	int modelType = atoi(argv[2]);
	//Direcci�n del modelo entreado
	std::string trainedModelDir = argv[3];
	//Tramos para umbral
	unsigned int numTramosUmbral = 50;

	std::cout << "Cargando modelo de reconocimiento" << std::endl;
	//Cargar modelo de reconocimiento seg�n par�metros
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

	//Cargar im�genes de test
	vector<Mat> images;
	vector<int> labels;
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadCroppedGrayImagesFromCSV(csvFaces, images, labels);
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

	//Fichero de salida de resultados
	std::ofstream ficSalida(argv[4]);
	//Crear calculadora de estad�sticas
	StatisticsCalculator calculator;

	//Realizar test para cada imagen del conjunto
	std::cout << "Realizando test para "<<images.size()<<" imagenes" << std::endl;

	//Preparar downsampling
	std::vector<int> downSamplingSizes;
	downSamplingSizes.push_back(64);
	downSamplingSizes.push_back(32);
	downSamplingSizes.push_back(16);
	downSamplingSizes.push_back(8);
	downSamplingSizes.push_back(4);
	ImageDownsampler downSampler;
	unsigned int numAlgoritmosUpsampling = 5;

	//Realizar bucle de test con downsampling
	for (unsigned int indexTam = 0; indexTam < downSamplingSizes.size(); ++indexTam)
	{
		//Realizar downsampling
		int size = downSamplingSizes[indexTam];
		std::cout << "---------------------------" << std::endl;
		std::cout << "Upsampling desde " << size << "x" << size << std::endl;
		//Tests para los distintos algoritmos
		for (unsigned int i = 0; i < numAlgoritmosUpsampling; ++i)
		{
			//Algoritmo de upsampling
			ImageUpsampler* upsampler = generateUpsampler(i);
			//Crear calculadora de estad�sticas
			calculator.reiniciar();

			//Generar imag�nes downsampleadas
			std::vector<cv::Mat> upsampledImgs;
			for (unsigned int j = 0; j < images.size(); ++j)
			{
				//Generar imagen downsampleada
				cv::Mat downSampled;
				downSampler.downSampleWithSaltAndPepper(images[j], downSampled, size, size, cv::InterpolationFlags::INTER_AREA);
				//Aplicar upsampling
				cv::Mat upSampled;
				upsampler->upSample(downSampled, upSampled, 64, 64);
				upsampledImgs.push_back(upSampled);
			}

			//Realizar test para cada imagen del conjunto
			std::cout << "Realizando test para " << upsampler->getName()<< std::endl;
			//Inicializar umbral de predicci�n
			recognizer->setUmbral((std::numeric_limits<double>::max)());
			//Vector de distancias calculadas
			std::vector<double> confianzasCalculadas;
			//Vector de predicciones no negativas
			std::vector<int> prediccionesCalculadas;
			//Medida del tiempo en segundos
			double time = (double)getTickCount();
			//Estimaci�n inicial del umbral
			for (unsigned int j = 0; j < upsampledImgs.size(); ++j)
			{
				//Realizar predicci�n
				int correctLabel = labels[j];
				double confidence;
				int prediction = recognizer->predict(upsampledImgs[j], confidence);
				//A�adir muestra a la calculadora
				calculator.addMuestra(prediction, correctLabel, confidence);
				//A�adir distancia calculada
				confianzasCalculadas.push_back(confidence);
				//A�adir predicci�n
				prediccionesCalculadas.push_back(prediction);
			}
			//Calcular tiempo medio final
			time = ((double)getTickCount() - time) / getTickFrequency() / (double)images.size();

			//Calcular tramos para umbral
			double minUmbral = calculator.minConfidence - 1;
			double maxUmbral = calculator.maxConfidence;
			double incremento = (maxUmbral - minUmbral) / (double)numTramosUmbral;
			//Imprimir resultados
			calculator.imprimirCabecera(ficSalida);

			//Realizar tramos de test a partir del umbral
			for (unsigned int indexUmbral = 0; indexUmbral < numTramosUmbral + 2; ++indexUmbral)
			{
				//Reiniciar calculadora
				calculator.reiniciar();
				//Inicializar umbral de predicci�n
				double umbral = incremento*indexUmbral + minUmbral;

				//Realizar pruebas sobre las im�genes
				for (unsigned int j = 0; j < upsampledImgs.size(); ++j)
				{
					//Realizar predicci�n
					int correctLabel = labels[j];
					double confidence = confianzasCalculadas[j];
					int prediction = prediccionesCalculadas[j];

					//Si la confianza es mayor al umbral, detectar como negativa
					if (confidence > umbral)
						prediction = -1;

					//A�adir muestra a la calculadora
					calculator.addMuestra(prediction, correctLabel, confidence);
				}

				//Finalizar c�lculos
				calculator.realizarCalculos();
				//A�adir nombre, umbral y tiempo a la calculadora
				calculator.setNombreAlgoritmo(recognizer->getName()
					+to_string(size)+"x"+to_string(size)+" - "+upsampler->getName());
				calculator.setTime(time);
				calculator.setUmbral(umbral);

				//Mostrar resultados
				calculator.imprimirResultados();
				std::cout << std::endl;

				//Volcado de resultados sobre fichero
				ficSalida << calculator;
			}

			//Imprimir resultados
			ficSalida << std::endl;
			//Liberar upsampler
			delete upsampler;
		}
	}

	//Cerrar fichero
	ficSalida.close();

	//Limpiar reconocedor
	delete recognizer;
	return 0;
}

//Generar upsampler
ImageUpsampler* generateUpsampler(int opcion)
{
	//Generar upsampler seg�n opci�n
	switch (opcion)
	{
		case 0:
			//Vecino m�s cercano
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_NEAREST);
		case 1:
			//�rea
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_AREA);
		case 2:
			//Bilineal
			return new SimpleImageUpsampler(cv::InterpolationFlags::INTER_LINEAR);
		case 3:
			//Bic�bico
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
	//Comprobar par�metros
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
	//Direcciones de las bases de im�genes
	std::vector<std::string> nombresBases;
	//Obtener nombres de las bases
	readDatabasesFile(csvBases, nombresBases);

	//Por cada base, cargar las im�genes y realizar el aprendizaje con cada modelo
	for (unsigned int i = 0; i < nombresBases.size(); ++i)
	{
		// Vector de im�genes y labels.
		vector<Mat> croppedFaces;
		vector<int> labels;
		//Cargar im�genes
		try
		{
			FaceCSVReader::loadCroppedGrayImagesFromCSV(nombresBases[i], croppedFaces, labels);
		}
		catch (cv::Exception& e) {
			cerr << "Error al abrir el fichero \"" << nombresBases[i] << "\". Raz�n: " << e.msg << endl;
			getchar();
			return -1;
		}

		//Comprobar que se han cargado las im�genes
		if (croppedFaces.size() <= 1) {
			string error_message = "�A�ade m�s imagenes al dataset!";
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

//Main realizar tests a partir de un csv
int testMultiplesDatabases(int argc, char**argv)
{
	//Comprobar par�metros
	if (argc != 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << "csvTest opcionTest]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv
	std::string csvTest = argv[1];
	//Opci�n de test
	int opcionTest = atoi(argv[2]);
	//Leer csv de tests
	std::vector<std::string> bases;
	readDatabasesFile(csvTest, bases);
	
	//Lanzar cada uno de los tests
	for (unsigned int i = 0; i < bases.size(); ++i)
	{
		//Separar tokens
		std::vector<std::string> tokens;
		splitString(bases[i], " ", tokens);

		//Generar entrada de argumentos para entrenamiento
		int nuevoArgc = 1 + tokens.size();
		char ** nuevoArgv = new char*[nuevoArgc];
		nuevoArgv[0] = argv[0];
		//Pasar cada token al array
		for (unsigned int j = 0; j < tokens.size(); ++j)
		{
			nuevoArgv[j + 1] = (char*)tokens[j].c_str();
		}

		//Llamar a la funci�n de test
		switch (opcionTest)
		{
			//Tests con downsampling
			case 0: testFaces(nuevoArgc, nuevoArgv);
				break;
			//Tests reales
			case 1: testRealFaces(nuevoArgc, nuevoArgv);
				break;
			//Test erroneo
			default:
				std::cerr << "Error: opcion de test incorrecta" << std::endl;
				getchar();
				return -1;
		}
	}

	return 0;
}

//Main separar en conjunto de test y de entrenamiento
int separeDatabase(int argc, char**argv)
{
	//Comprobar par�metros
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
	unsigned int numTestFaces = atoi(argv[4]);
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

	//Cargar im�genes y etiquetas
	vector<Mat> images;
	vector<int> labels;
	//Cargar im�genes
	try
	{
		FaceCSVReader::loadActualImagesFromCSV(csvFaces, images, labels);
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

	//Label de inicio
	int labelInicio = labels[0];
	std::vector<int> indicesTraining;
	indicesTraining.push_back(0);

	//Recorrer la base de im�genes y elegir la cantidad
	for (unsigned int i = 1; i < images.size(); ++i)
	{
		//Obtener label de la imagen actual
		int labelActual = labels[i];
		//Comprobar si ha cambiado el label
		if (labelActual != labelInicio)
		{
			//Elegir �ndices de imagen para test
			std::vector<int> indicesTest;
			for (unsigned int j = 0; j < numTestFaces; ++j)
			{
				//Seleccionar �ndice al azar
				int indexSelected = rand() % (indicesTraining.size());
				//A�adir �ndice al test
				indicesTest.push_back(indicesTraining[indexSelected]);
				//Eliminar �ndice para training
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

		//A�adir imagen al indice de entrenamiento
		indicesTraining.push_back(i);
	}

	//Terminar �ltimo grupo
	if (indicesTraining.size() > 0)
	{
		//Elegir �ndices de imagen para test
		std::vector<int> indicesTest;
		for (unsigned int j = 0; j < numTestFaces; ++j)
		{
			//Seleccionar �ndice al azar
			int indexSelected = rand() % (indicesTraining.size());
			//A�adir �ndice al test
			indicesTest.push_back(indicesTraining[indexSelected]);
			//Eliminar �ndice para training
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

//Leer fichero que contiene bases de im�genes
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
	//Cargar nombres de las bases de datos l�nea a l�nea
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

//Realizar split de un string
void splitString(const string& str, const string& delim, std::vector<std::string> &tokens)
{
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
}

