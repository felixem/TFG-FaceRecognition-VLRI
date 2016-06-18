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

	//Entrenar modelo a partir del conjunto de imágenes de alta resolución de entrenamiento y las dimensiones de
	//las imágenes de alta y baja resolución
	void EigenTransformationUpsampler::train(const std::vector<cv::Mat>&images, int lowResWidth,
		int lowResHeight, int highResWidth, int highResHeight)
	{
		//Copiar valores
		this->lowResWidth = lowResWidth;
		this->lowResHeight = lowResHeight;
		this->highResWidth = highResWidth;
		this->highResHeight = highResHeight;

		//Detectar las caras de las imágenes de alta resolución
		std::vector<cv::Mat> croppedFaces;
		HaarLikeFaceDetector faceDetector;
		//Buscar las caras de todas las imágenes
		for (unsigned int i = 0; i < images.size(); ++i)
		{
			cv::Mat face;
			//Extraer cara principal
			faceDetector.extractMainFace(images[i], face, 32, 32, highResWidth, highResHeight);
			//Añadir cara
			croppedFaces.push_back(face);
		}

		//Realizar entrenamiento del modelo de alta resolución con las caras cortadas
		std::vector<int> fakeClasses(croppedFaces.size());
		this->highModel->train(croppedFaces, fakeClasses);

		//Realizar downsampling para el modelo de baja resolución
		ImageDownsampler downSampler;		
		std::vector<cv::Mat> downSampledImgs;
		for (unsigned int i = 0; i < croppedFaces.size(); ++i)
		{
			//Realizar downsampling con ruido gaussiano
			cv::Mat downSampledFace;
			downSampler.downSampleWithGaussianNoise(croppedFaces[i], downSampledFace, lowResHeight, lowResWidth, cv::InterpolationFlags::INTER_CUBIC);
			downSampledImgs.push_back(downSampledFace);
		}

		//Realizar aprendizaje del modelo de baja resolución
		this->lowModel->train(downSampledImgs, fakeClasses);

	}

	//Generar imagen de alta resolución a partir de la imagen de baja resolución
	void EigenTransformationUpsampler::upSample(const cv::Mat& img, cv::Mat& output, const int kFacesToUse)
	{
		//Transformar la imagen de baja resolución al tamaño de las imágenes del modelo de baja resolución
		cv::Mat lowImg;
		if (lowImg.size().height != this->lowResHeight || lowImg.size().width != this->lowResWidth)
		{
			SimpleImageUpsampler upSampler;
			upSampler.applyBilinearFilter(img, lowImg, this->lowResHeight, this->lowResWidth);
		}
		else
		{
			//Copia directa al compartir tamaño
			lowImg = img;
		}

		//Obtener los eigenvectores del espacio de baja resolución
		cv::Mat lowW = lowModel->getEigenVectors();
		//Obtener la imagen media del espacio de baja resolución
		cv::Mat lowMean = lowModel->getMean();
		// Proyectar la imagen sobre el espacio de baja resolución
		cv::Mat lowEvs = cv::Mat(lowW, cv::Range::all(), cv::Range(0, kFacesToUse));
		cv::Mat projection = cv::LDA::subspaceProject(lowEvs, lowMean, img.reshape(1, 1));
		
		//Obtener los eigenvectores del espacio de alta resolución
		cv::Mat highW = highModel->getEigenVectors();
		//Obtener la imagen media del espacio de alta resolución
		cv::Mat highMean = highModel->getMean();
		//Realizar reconstrucción sobre el espacio de alta resolución
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
