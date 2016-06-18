#include "stdafx.h"
#include "EigenTransformationUpsampler.h"
#include "HaarLikeFaceDetector.h"
#include "ImageDownsampler.h"
#include "SimpleImageUpsampler.h"

namespace tfg
{
	//Constructor
	EigenTransformationUpsampler::EigenTransformationUpsampler()
	{
		this->highModel = cv::face::createEigenFaceRecognizer();
		this->lowModel = cv::face::createEigenFaceRecognizer();
	}

	//Destructor
	EigenTransformationUpsampler::~EigenTransformationUpsampler()
	{
	}

	//Entrenar modelo a partir del conjunto de im�genes de alta resoluci�n de entrenamiento y las dimensiones de
	//las im�genes de alta y baja resoluci�n
	void EigenTransformationUpsampler::train(const std::vector<cv::Mat>&images, int lowResWidth,
		int lowResHeight, int highResWidth, int highResHeight)
	{
		//Copiar valores
		this->lowResWidth = lowResWidth;
		this->lowResHeight = lowResHeight;
		this->highResWidth = highResWidth;
		this->highResHeight = highResHeight;

		//Detectar las caras de las im�genes de alta resoluci�n
		std::vector<cv::Mat> croppedFaces;
		HaarLikeFaceDetector faceDetector;
		//Buscar las caras de todas las im�genes
		for (unsigned int i = 0; i < images.size(); ++i)
		{
			cv::Mat face;
			//Extraer cara principal
			faceDetector.extractMainFace(images[i], face, 32, 32, highResWidth, highResHeight);
			//A�adir cara
			croppedFaces.push_back(face);
		}

		//Realizar entrenamiento del modelo de alta resoluci�n con las caras cortadas
		std::vector<int> fakeClasses(croppedFaces.size());
		this->highModel->train(croppedFaces, fakeClasses);

		//Realizar downsampling para el modelo de baja resoluci�n
		ImageDownsampler downSampler;		
		std::vector<cv::Mat> downSampledImgs;
		for (unsigned int i = 0; i < croppedFaces.size(); ++i)
		{
			//Realizar downsampling con ruido gaussiano
			cv::Mat downSampledFace;
			downSampler.downSampleWithGaussianNoise(croppedFaces[i], downSampledFace, lowResHeight, lowResWidth, cv::InterpolationFlags::INTER_CUBIC);
			downSampledImgs.push_back(downSampledFace);
		}

		//Realizar aprendizaje del modelo de baja resoluci�n
		this->lowModel->train(downSampledImgs, fakeClasses);

	}

	//Generar imagen de alta resoluci�n a partir de la imagen de baja resoluci�n
	void EigenTransformationUpsampler::upSample(const cv::Mat& img, cv::Mat& output, const int kFacesToUse)
	{
		//Transformar la imagen de baja resoluci�n al tama�o de las im�genes del modelo de baja resoluci�n
		cv::Mat lowImg;
		if (lowImg.size().height != this->lowResHeight || lowImg.size().width != this->lowResWidth)
		{
			SimpleImageUpsampler upSampler;
			upSampler.applyBilinearFilter(img, lowImg, this->lowResHeight, this->lowResWidth);
		}
		else
		{
			//Copia directa al compartir tama�o
			lowImg = img;
		}

		//Obtener los eigenvectores del espacio de baja resoluci�n
		cv::Mat lowW = lowModel->getEigenVectors();
		//Obtener la imagen media del espacio de baja resoluci�n
		cv::Mat lowMean = lowModel->getMean();
		// Proyectar la imagen sobre el espacio de baja resoluci�n
		cv::Mat lowEvs = cv::Mat(lowW, cv::Range::all(), cv::Range(0, kFacesToUse));
		cv::Mat projection = cv::LDA::subspaceProject(lowEvs, lowMean, img.reshape(1, 1));
		
		//Obtener los eigenvectores del espacio de alta resoluci�n
		cv::Mat highW = highModel->getEigenVectors();
		//Obtener la imagen media del espacio de alta resoluci�n
		cv::Mat highMean = highModel->getMean();
		//Realizar reconstrucci�n sobre el espacio de alta resoluci�n
		cv::Mat highEvs = cv::Mat(highW, cv::Range::all(), cv::Range(0, kFacesToUse));
		output = cv::LDA::subspaceReconstruct(highEvs, highMean, projection);
		//Normalizar el resultado
		output = norm_0_255(output.reshape(1, highResHeight));
	}

	//Normalizar imagen
	cv::Mat EigenTransformationUpsampler::norm_0_255(cv::InputArray _src)
	{
		cv::Mat src = _src.getMat();
		// Create and return normalized image:
		cv::Mat dst;
		switch (src.channels()) {
		case 1:
			cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			break;
		case 3:
			cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);
			break;
		default:
			src.copyTo(dst);
			break;
		}
		return dst;
	}
}
