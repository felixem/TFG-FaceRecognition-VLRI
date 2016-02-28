#include "stdafx.h"
#include "HaarLikeFaceDetector.h"
#include <iostream>

using namespace cv;

//Nombre de detector por defecto
const std::string HaarLikeFaceDetector::FICHEROFACEDETECTION = "sources/haarlike/haarcascade_frontalface_alt.xml";

//Constructor por defecto
HaarLikeFaceDetector::HaarLikeFaceDetector()
{
	// Cargar el fichero de detecci�n en cascada
	if (!face_cascade.load(FICHEROFACEDETECTION)) 
	{
		std::cerr<<"--(!)Error loading "+FICHEROFACEDETECTION+" file\n";
	}
}

//Constructor a partir del nombre de fichero del detector
HaarLikeFaceDetector::HaarLikeFaceDetector(std::string faceCascadeFile)
{
	// Cargar el fichero de detecci�n en cascada
	if (!face_cascade.load(faceCascadeFile))
	{
		std::cerr << "--(!)Error loading "+faceCascadeFile+" file\n";
	}
}

//Constructor de copia
HaarLikeFaceDetector::HaarLikeFaceDetector(const HaarLikeFaceDetector & detector)
{
	this->face_cascade = detector.face_cascade;
}

//Operador asignaci�n
HaarLikeFaceDetector& HaarLikeFaceDetector::operator=(const HaarLikeFaceDetector& detector)
{
	this->face_cascade = detector.face_cascade;
	return *this;
}

//Destructor
HaarLikeFaceDetector::~HaarLikeFaceDetector()
{

}

bool HaarLikeFaceDetector::detectFaces(const cv::Mat & input, std::vector<cv::Mat>& foundFaces, cv::Mat & output)
{
	//Caras detectadas
	std::vector<Rect> faces;
	//Im�genes intermedias
	Mat frame_gray;
	//Transformar la imagen de entrada en escala de grises
	cvtColor(input, frame_gray, COLOR_BGR2GRAY);
	//Ecualizar el histograma
	equalizeHist(frame_gray, frame_gray);
	//Copiar la imagen original para se�alar las caras
	output = input;

	//Detectar las caras
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	//Iterar sobre las caras encontradas
	for (size_t ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
	{
		//Establecer regiones de inter�s
		cv::Rect roi_c;
		roi_c.x = faces[ic].x;
		roi_c.y = faces[ic].y;
		roi_c.width = (faces[ic].width);
		roi_c.height = (faces[ic].height);

		//Obtener la cara en su tono original
		Mat crop = input(roi_c);
		//Convertir imagen a escala de grises
		Mat gray;
		cvtColor(crop, gray, CV_BGR2GRAY);

		//A�adir cara en gris a la lista de caras
		foundFaces.push_back(gray);

		//Se�alar la cara en la imagen de salida
		Point pt1(roi_c.x, roi_c.y);
		Point pt2((roi_c.x + roi_c.height), (roi_c.y + roi_c.width));
		rectangle(output, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
	}

	//Devolver si se ha encontrado una cara
	return faces.size() != 0;
}
