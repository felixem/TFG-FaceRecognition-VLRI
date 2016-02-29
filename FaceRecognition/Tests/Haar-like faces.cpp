// Tests.cpp: define el punto de entrada de la aplicación de consola.
//

#include "HaarLikeFaceDetector.h"
#include <stdio.h>

using namespace std;
using namespace cv;

// Function Headers
void detectAndDisplay(Mat frame);

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string outputFolder = "output/";
string window_name = "Capture - Face detection";

// Function main
int main(void)
{
	// Read the image file
	Mat frame = imread("sources/prueba.jpg");

	// Apply the classifier to the frame
	if (!frame.empty()) {
		detectAndDisplay(frame);
	}
	else {
		printf(" --(!) No captured frame -- Break!\n");
		system("pause");
		return -1;
	}

	//Bucle infinito para evitar cierre
	waitKey(30);
	getchar();

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
}

