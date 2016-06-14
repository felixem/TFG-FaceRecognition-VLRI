#include "stdafx.h"
#include "HaarLikeFaceDetector.h"
#include <iostream>

using namespace cv;

namespace tfg
{
	//Nombre de detector por defecto
	const std::string HaarLikeFaceDetector::FICHEROFACEDETECTION = "sources/haarlike/haarcascade_frontalface_alt.xml";

	//Constructor por defecto
	HaarLikeFaceDetector::HaarLikeFaceDetector()
	{
		// Cargar el fichero de detección en cascada
		if (!face_cascade.load(FICHEROFACEDETECTION))
		{
			std::cerr << "--(!)Error loading " + FICHEROFACEDETECTION + " file\n";
		}
	}

	//Constructor a partir del nombre de fichero del detector
	HaarLikeFaceDetector::HaarLikeFaceDetector(std::string faceCascadeFile)
	{
		// Cargar el fichero de detección en cascada
		if (!face_cascade.load(faceCascadeFile))
		{
			std::cerr << "--(!)Error loading " + faceCascadeFile + " file\n";
		}
	}

	//Constructor de copia
	HaarLikeFaceDetector::HaarLikeFaceDetector(const HaarLikeFaceDetector & detector)
	{
		this->face_cascade = detector.face_cascade;
	}

	//Operador asignación
	HaarLikeFaceDetector& HaarLikeFaceDetector::operator=(const HaarLikeFaceDetector& detector)
	{
		this->face_cascade = detector.face_cascade;
		return *this;
	}

	//Destructor
	HaarLikeFaceDetector::~HaarLikeFaceDetector()
	{

	}

	bool HaarLikeFaceDetector::detectFaces(const cv::Mat & input, std::vector<cv::Mat>& foundFaces, cv::Mat & output,
		float scale, int minWidth, int minHeight)
	{
		//Caras detectadas
		std::vector<Rect> faces;
		//Imágenes intermedias
		Mat frame_gray;

		//Comprobar si es rgb
		if (input.channels() == 3)
		{
			//Transformar la imagen de entrada en escala de grises
			cvtColor(input, frame_gray, COLOR_BGR2GRAY);
		}
		else
		{
			//La imagen es tal cual
			frame_gray = input;
		}

		//Ecualizar el histograma
		equalizeHist(frame_gray, frame_gray);
		//Copiar la imagen original para señalar las caras
		output = input.clone();

		//Detectar las caras
		face_cascade.detectMultiScale(frame_gray, faces, scale, 3, 0 | CASCADE_SCALE_IMAGE, Size(minWidth, minHeight));

		//Iterar sobre las caras encontradas
		for (size_t ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
		{
			//Establecer regiones de interés
			const cv::Rect& roi_c = faces[ic];

			//Obtener la cara en su tono original
			Mat crop = input(roi_c);
			//Convertir imagen a escala de grises
			Mat gray;
			//Comprobar si es rgb
			if (crop.channels() == 3)
			{
				cvtColor(crop, gray, CV_BGR2GRAY);
			}
			else
			{
				gray = crop;
			}

			//Añadir cara en gris a la lista de caras
			foundFaces.push_back(gray);
		}

		//Señalar caras en la imagen de salida
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			const Rect &face = faces[i];
			//Señalar la cara en la imagen de salida
			Point pt1(face.x, face.y);
			Point pt2((face.x + face.height), (face.y + face.width));
			rectangle(output, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
		}

		//Devolver si se ha encontrado una cara
		return faces.size() != 0;
	}
}
