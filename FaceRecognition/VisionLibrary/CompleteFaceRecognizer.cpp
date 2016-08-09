#include "stdafx.h"
#include "CompleteFaceRecognizer.h"

using namespace cv;

namespace tfg
{
	//Constructor a partir de los elementos principales
	CompleteFaceRecognizer::CompleteFaceRecognizer(HaarLikeFaceDetector* faceDetector,
		IFaceRecognizer* faceRecognizer, ImageUpsampler* upsampler) :
		faceDetector(faceDetector), faceRecognizer(faceRecognizer), upsampler(upsampler)
	{

	}

	//Destructor
	CompleteFaceRecognizer::~CompleteFaceRecognizer()
	{
		//Liberar memoria
		if (faceDetector != NULL)
		{
			delete faceDetector;
			faceDetector = NULL;
		}
		if (faceRecognizer != NULL)
		{
			delete faceRecognizer;
			faceRecognizer = NULL;
		}
		if (upsampler != NULL)
		{
			delete upsampler;
			upsampler = NULL;
		}
	}

	//Reconocer caras en una imagen
	bool CompleteFaceRecognizer::recognizeFaces(const cv::Mat& img, std::vector<Face> &faces, cv::Mat & output, int recognizeWidth, int recognizeHeight,
		float scale, int minWidth, int minHeight, int maxWidth, int maxHeight, int numVecinos)
	{
		//Rectángulos de las caras encontradas y la imagen en escala de grises
		std::vector<cv::Rect> foundRectFaces;
		cv::Mat grayImg;

		//Encontrar caras
		this->faceDetector->detectLocatedFaces(img, foundRectFaces, grayImg, scale, minWidth, minHeight, maxWidth, maxHeight, numVecinos);

		//Preparar imagen final
		output = img.clone();
		//Para cada cara encontrada, reconocerla, añadirla a la lista de caras y dibujarla sobre la imagen final
		for (unsigned int i = 0; i < foundRectFaces.size(); ++i)
		{
			const cv::Rect& rectFace = foundRectFaces[i];
			//Recortar cara
			cv::Mat grayFace = grayImg(rectFace);
			//Realizar ecualización
			cv::equalizeHist(grayFace, grayFace);

			//Aplicar upsampling
			cv::Mat upsampledFace;
			this->upsampler->upSample(grayFace, upsampledFace, recognizeHeight, recognizeWidth);
			//Reconocer cara
			double confidence;
			int clase = this->faceRecognizer->predict(upsampledFace,confidence);
			//Almacenar cara detectada en su formato original
			faces.push_back(Face(img(rectFace), clase, confidence));

			//Dibujar detección sobre la imagen según sea detección positiva o negativa
			if (clase == -1)
			{
				//Señalar la cara en rojo en la imagen de salida
				Point pt1(rectFace.x, rectFace.y);
				Point pt2((rectFace.x + rectFace.height), (rectFace.y + rectFace.width));
				rectangle(output, pt1, pt2, Scalar(0, 0, 255), 2, 8, 0);
			}
			else
			{
				//Señalar la cara en verde en la imagen de salida
				Point pt1(rectFace.x, rectFace.y);
				Point pt2((rectFace.x + rectFace.height), (rectFace.y + rectFace.width));
				rectangle(output, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
			}
		}

		//Devolver si se ha encontrado alguna cara
		return faces.size() > 0;
	}

	//Método de upsampling
	void CompleteFaceRecognizer::upSample(const cv::Mat& img, cv::Mat &output, int height, int width)
	{
		this->upsampler->upSample(img, output, height, width);
	}

	//Poner umbral de detección (valores por encima del umbral se considerarán desconocidos)
	void CompleteFaceRecognizer::setUmbral(double umbral)
	{
		this->faceRecognizer->setUmbral(umbral);
	}

	//Cargar desde fichero el modelo
	void CompleteFaceRecognizer::load(const std::string& fichero)
	{
		this->faceRecognizer->load(fichero);
	}

	//Setter del detector de caras
	void CompleteFaceRecognizer::setFaceDetector(HaarLikeFaceDetector* faceDetector)
	{
		//Liberar memoria
		if (this->faceDetector != NULL)
			delete this->faceDetector;

		this->faceDetector = faceDetector;
	}

	//Setter del reconocedor de caras
	void CompleteFaceRecognizer::setFaceRecognizer(IFaceRecognizer* faceRecognizer)
	{
		//Liberar memoria
		if (this->faceRecognizer != NULL)
			delete this->faceRecognizer;

		this->faceRecognizer = faceRecognizer;
	}

	//Setter del upsampler de caras
	void CompleteFaceRecognizer::setUpsampler(ImageUpsampler* upsampler)
	{
		//Liberar memoria
		if (upsampler != NULL)
			delete this->upsampler;

		this->upsampler = upsampler;
	}
}
