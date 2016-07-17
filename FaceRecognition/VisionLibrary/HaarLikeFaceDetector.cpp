#include "stdafx.h"
#include "HaarLikeFaceDetector.h"
#include <iostream>
#include "ImageDownsampler.h"

using namespace cv;

namespace tfg
{
	//Nombre de detector por defecto
	const std::string HaarLikeFaceDetector::FICHEROFACEDETECTION = "sources/haarlike/haarcascade_frontalface_alt.xml";

	//Constructor por defecto
	HaarLikeFaceDetector::HaarLikeFaceDetector()
	{
		// Cargar el fichero de detecci�n en cascada
		if (!face_cascade.load(FICHEROFACEDETECTION))
		{
			std::cerr << "--(!)Error loading " + FICHEROFACEDETECTION + " file\n";
		}
	}

	//Constructor a partir del nombre de fichero del detector
	HaarLikeFaceDetector::HaarLikeFaceDetector(std::string faceCascadeFile)
	{
		// Cargar el fichero de detecci�n en cascada
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

	//Detectar caras
	bool HaarLikeFaceDetector::detectFaces(const cv::Mat & input, std::vector<cv::Mat>& foundFaces, cv::Mat & output,
		float scale, int minWidth, int minHeight, int maxWidth, int maxHeight)
	{
		//Caras detectadas
		std::vector<Rect> faces;
		//Im�genes intermedias
		Mat frame_gray;

		//Detectar caras
		try
		{
			this->detectLocatedFaces(input, faces, frame_gray, scale, minWidth, minHeight, maxWidth, maxHeight);
		}
		catch (...)
		{
			throw std::exception("Memoria insuficiente");
		}

		//Iterar sobre las caras encontradas
		for (size_t ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
		{
			//Establecer regiones de inter�s
			const cv::Rect& roi_c = faces[ic];
			//Obtener cara en escala de grises
			Mat gray = frame_gray(roi_c);

			//A�adir cara en gris a la lista de caras
			foundFaces.push_back(gray);
		}

		//Se�alar caras en la imagen de salida
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			const Rect &face = faces[i];
			//Se�alar la cara en la imagen de salida
			Point pt1(face.x, face.y);
			Point pt2((face.x + face.height), (face.y + face.width));
			rectangle(output, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
		}

		//Devolver si se ha encontrado una cara
		return faces.size() != 0;
	}

	//Detectar caras en una imagen devolviendo las caras a color tambi�n
	bool HaarLikeFaceDetector::detectFaces(const cv::Mat & input, std::vector<cv::Mat>& grayFoundFaces, std::vector<cv::Mat>& colourFoundFaces, cv::Mat & output,
		float scale, int minWidth, int minHeight, int maxWidth, int maxHeight)
	{
		//Caras detectadas
		std::vector<Rect> faces;
		//Im�genes intermedias
		Mat frame_gray;

		//Detectar caras
		this->detectLocatedFaces(input, faces, frame_gray, scale, minWidth, minHeight, maxWidth, maxHeight);

		//Iterar sobre las caras encontradas
		for (size_t ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
		{
			//Establecer regiones de inter�s
			const cv::Rect& roi_c = faces[ic];

			//Obtener la cara en su tono original
			Mat crop = input(roi_c);
			//Devolver cara en su tono original
			colourFoundFaces.push_back(crop);

			//Convertir imagen a escala de grises
			Mat gray = frame_gray(roi_c);
			//A�adir cara en gris a la lista de caras
			grayFoundFaces.push_back(gray);
		}

		//Se�alar caras en la imagen de salida
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			const Rect &face = faces[i];
			//Se�alar la cara en la imagen de salida
			Point pt1(face.x, face.y);
			Point pt2((face.x + face.height), (face.y + face.width));
			rectangle(output, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
		}

		//Devolver si se ha encontrado una cara
		return faces.size() != 0;
	}

	//Detectar caras en una imagen devolviendo los rect�ngulos y la imagen en escala de grises
	bool HaarLikeFaceDetector::detectLocatedFaces(const cv::Mat & input, std::vector<cv::Rect>& rectFaces, cv::Mat & grayInput,
		float scale, int minWidth, int minHeight, int maxWidth, int maxHeight)
	{
		//Comprobar si es rgb
		if (input.channels() == 3)
		{
			//Transformar la imagen de entrada en escala de grises
			cvtColor(input, grayInput, COLOR_BGR2GRAY);
		}
		else
		{
			//La imagen es tal cual
			grayInput = input;
		}

		//Ecualizar el histograma
		equalizeHist(grayInput, grayInput);

		//Detectar las caras
		face_cascade.detectMultiScale(grayInput, rectFaces, scale, 3, 0 | CASCADE_SCALE_IMAGE, Size(minWidth, minHeight), Size(maxWidth, maxHeight));

		//Devolver si se ha encontrado alguna cara
		return rectFaces.size();
	}

	//Extraer la cara principal de la imagen
	void HaarLikeFaceDetector::extractMainFace(const cv::Mat &img, cv::Mat &face, int minWidth, int minHeight, int finalWidth, int finalHeight)
	{
		//Extraer �nicamente la cara
		std::vector<cv::Mat> foundFaces;
		this->detectFaces(img, foundFaces, cv::Mat(), 1.05, minWidth, minHeight);

		//Encontrar la cara m�s grande encontrada
		cv::Mat maxFace;
		//Comprobar si se han encontrado caras
		if (foundFaces.size() == 0)
			maxFace = img;
		else
		{
			//Asignar primera cara
			maxFace = foundFaces[0];
			double maxFaceDim = maxFace.size().height * maxFace.size().width;
			//Recorrer las caras buscando la m�s grande
			for (unsigned int i = 1; i < foundFaces.size(); ++i)
			{
				//Obtener nueva cara y obtener su dimensi�n
				const cv::Mat& face = foundFaces[i];
				double faceDim = face.size().height * face.size().width;

				//Comprobar si la nueva cara es m�s grande
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
}
