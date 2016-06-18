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

