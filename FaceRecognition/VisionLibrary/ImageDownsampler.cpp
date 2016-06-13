#include "stdafx.h"
#include "ImageDownsampler.h"
#include <iostream>

namespace tfg
{
	//Constructor
	ImageDownsampler::ImageDownsampler()
	{
	}

	//Destructor
	ImageDownsampler::~ImageDownsampler()
	{
	}

	//Aplicar downsampling sin ruido adicional
	void ImageDownsampler::downSampleWithNoNoise(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo)
	{
		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(img, output, size, 0, 0, tipo);
	}

	//Aplicar downsampling con ruido gaussiano
	void ImageDownsampler::downSampleWithGaussianNoise(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double Mean, double StdDev)
	{
		//Crear imagen auxiliar
		cv::Mat imgAux;
		//Crear tamaño
		cv::Size size(width, height);

		//Aplicar ruido
		this->addGaussianNoise(img, imgAux, Mean, StdDev);
		//Crear nueva imagen
		cv::resize(imgAux, output, size, 0, 0, tipo);
	}

	//Aplicar downsampling con ruido de sal y pimienta
	void ImageDownsampler::downSampleWithSaltAndPepper(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double probability)
	{
		//Crear imagen auxiliar
		cv::Mat imgAux;
		//Crear tamaño
		cv::Size size(width, height);

		//Aplicar ruido
		this->addSaltAndPepper(img, imgAux, probability);
		//Crear nueva imagen
		cv::resize(imgAux, output, size, 0, 0, tipo);
	}

	//Aplicar downsampling con todos los ruidos
	void ImageDownsampler::downSampleWithAllNoises(const cv::Mat &img, cv::Mat &output, int height, int width, cv::InterpolationFlags tipo, double Mean, double StdDev, double probability)
	{
		//Imagen auxiliar para ruido gaussiano
		cv::Mat imgGauss;
		//Aplicar ruido gaussiano
		this->addGaussianNoise(img, imgGauss, Mean, StdDev);
		//Crear imagen auxiliar
		cv::Mat imgAux;
		//Aplicar ruido gaussiano
		this->addSaltAndPepper(imgGauss, imgAux, probability);

		//Crear tamaño
		cv::Size size(width, height);
		//Crear nueva imagen
		cv::resize(imgAux, output, size, 0, 0, tipo);
	}

	//Añadir ruido gaussiano
	void ImageDownsampler::addGaussianNoise(const cv::Mat &mSrc, cv::Mat &mDst, double Mean, double StdDev)
	{
		//Generar ruido gaussiano aleatorio
		cv::Mat mGaussian_noise = cv::Mat(mSrc.size(), mSrc.type());
		randn(mGaussian_noise, cv::Scalar::all(Mean), cv::Scalar::all(StdDev));

		//Aplicar ruido gaussiano
		addWeighted(mSrc, 1.0, mGaussian_noise, 1.0, 0.0, mDst);
	}

	//Añadir ruido salt and pepper
	void ImageDownsampler::addSaltAndPepper(const cv::Mat &mSrc, cv::Mat &mDst, double probability)
	{
		//Generar matriz de ruido
		cv::Mat noise = cv::Mat(mSrc.size(), mSrc.type());
		randu(noise, 0, 256);

		//Calcular los umbrales de sal y pimienta
		int umbralSal = 128-(256 * probability);
		int umbralPimienta = -128+(256 * probability);

		//Generar copia de la matriz original
		mDst = mSrc.clone();
		//Recorrer la matriz para aplicar el ruido de sal y pimienta
		for (int i = 0; i < mSrc.rows; i++) {
			for (int j = 0; j < mSrc.cols; j++) {
				//Comprobar ruido de pimienta
				if ((int)noise.at<char>(i, j) <= umbralPimienta) {
					mDst.at<char>(i, j) = 0;
				}
				//Comprobar ruido de sal
				else if ((int)noise.at<char>(i, j) >= umbralSal) {
					mDst.at<char>(i, j) = 255;
				}
			}
		}
	}
}
