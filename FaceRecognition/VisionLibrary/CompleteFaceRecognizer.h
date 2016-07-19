#pragma once

#include "HaarLikeFaceDetector.h"
#include "IFaceRecognizer.h"
#include "ImageUpsampler.h"
#include "Face.h"

namespace tfg
{
	//Clase utilizada para gestionar todo el proceso de detección y reconocimiento de caras
	class CompleteFaceRecognizer
	{
	public:
		//Constructor a partir de los elementos principales
		CompleteFaceRecognizer(HaarLikeFaceDetector* faceDetector = NULL, 
			IFaceRecognizer* faceRecognizer = NULL, ImageUpsampler* upsampler = NULL);
		//Destructor
		~CompleteFaceRecognizer();

		//Reconocer caras en una imagen
		bool recognizeFaces(const cv::Mat& img, std::vector<Face> &faces, cv::Mat & output, int recognizeWidth = 64, int recognizeHeight = 64,
			float scale = 1.01, int minWidth = 16, int minHeight = 16, int maxWidth = 0, int maxHeight = 0, int numVecinos = 3);

		//Método de upsampling
		void upSample(const cv::Mat& img, cv::Mat &output, int height, int width);
		//Poner umbral de detección (valores por encima del umbral se considerarán desconocidos)
		void setUmbral(double umbral);
		//Cargar desde fichero el modelo
		void load(const std::string& fichero);

		//Setters
		void setFaceDetector(HaarLikeFaceDetector* faceDetector);
		void setFaceRecognizer(IFaceRecognizer* faceRecognizer);
		void setUpsampler(ImageUpsampler* upsampler);

	private:
		//Detector de caras
		HaarLikeFaceDetector* faceDetector;
		//Reconocedor de caras
		IFaceRecognizer* faceRecognizer;
		//Upsampler
		ImageUpsampler* upsampler;
	};
}

