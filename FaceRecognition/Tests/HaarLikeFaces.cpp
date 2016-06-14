// Tests.cpp: define el punto de entrada de la aplicación de consola.
//
#include "stdafx.h"
#include "HaarLikeFaceDetector.h"
#include "HaarLikeFaces.h"
#include <stdio.h>
#include <iostream>
#include "ImageDownsampler.h"

using namespace std;
using namespace cv;
using namespace tfg;

// Function Headers
void detectAndDisplay(Mat frame);

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string outputFolder = "output/";
string window_name = "Capture - Face detection";

// Function main
int mainHaarLike(int argc, char** argv)
{
	//Comprobar parámetros
	if (argc != 3)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " imgDir outputFolder]" << std::endl;
		getchar();
		return -1;
	}

	//String de fichero csv y carpeta de salida
	std::string imgFaces = argv[1];
	std::string outputFolder = argv[2];

	// Read the image file
	Mat frame = imread(imgFaces);

	// Apply the classifier to the frame
	if (!frame.empty()) {
		detectAndDisplay(frame);
	}
	else {
		printf(" --(!) No captured frame -- Break!\n");
		getchar();
		return -1;
	}

	//Evitar cierre
	waitKey(0);

	return 0;
}

// Function detectAndDisplay
void detectAndDisplay(Mat frame)
{
	//Detector de caras
	HaarLikeFaceDetector faceDetector;
	//Vector de imágenes con las caras encontradas
	std::vector<Mat> faces;
	//Imagen final con las caras señaladas
	Mat outputFrame;

	//Detectar las caras
	if (faceDetector.detectFaces(frame, faces, outputFrame))
	{
		//Recorrer todas las caras
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			//Crear el nombre de fichero
			std::string filename = "";
			stringstream ssfn;
			ssfn << outputFolder << i << ".png";
			filename = ssfn.str();
			//Escribir en el fichero
			imwrite(filename, faces[i]);
		}
	}
	else
	{
		printf(" --(!) No captured faces -- Break!");
		return;
	}

	//Mostrar imagen original
	imshow("original", frame);
	//Mostrar imagen detectada
	imshow("Caras detectadas", outputFrame);
	//Esperar a tecla
	waitKey(0);
}

//Extraer la cara principal de la imagen
void extractMainFace(HaarLikeFaceDetector &faceDetector, const cv::Mat &img, cv::Mat &face, int minWidth, int minHeight, int finalWidth, int finalHeight)
{
	//Extraer únicamente la cara
	std::vector<cv::Mat> foundFaces;
	faceDetector.detectFaces(img, foundFaces, cv::Mat(), 1.1, minWidth, minHeight);

	//Encontrar la cara más grande encontrada
	cv::Mat maxFace;
	//Comprobar si se han encontrado caras
	if (foundFaces.size() == 0)
		maxFace = img;
	else
	{
		//Asignar primera cara
		maxFace = foundFaces[0];
		double maxFaceDim = maxFace.size().height * maxFace.size().width;
		//Recorrer las caras buscando la más grande
		for (unsigned int i = 1; i < foundFaces.size(); ++i)
		{
			//Obtener nueva cara y obtener su dimensión
			const cv::Mat& face = foundFaces[i];
			double faceDim = face.size().height * face.size().width;

			//Comprobar si la nueva cara es más grande
			if (faceDim > maxFaceDim)
			{
				maxFace = face;
				maxFaceDim = faceDim;
			}
		}
	}

	//Ajustar cara a las dimensiones requeridas
	ImageDownsampler downSampler;
	downSampler.downSampleWithNoNoise(maxFace, face, finalHeight, finalWidth, cv::InterpolationFlags::INTER_CUBIC);
}

