#include "stdafx.h"
#include "FisherFacesRecognizer.h"

#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

namespace tfg
{
	//Constructor
	FisherFacesRecognizer::FisherFacesRecognizer()
	{
		this->model = cv::face::createFisherFaceRecognizer();
	}

	FisherFacesRecognizer::~FisherFacesRecognizer()
	{
	}

	//Entrenar modelo a partir de un vector de imágenes y de labels
	void FisherFacesRecognizer::train(const std::vector<cv::Mat> &images, const std::vector<int> &labels)
	{
		//Entrenar modelo
		this->model->train(images, labels);
	}

	//Predecir clase de la imagen
	int FisherFacesRecognizer::predict(const cv::Mat& img, double &confidence)
	{
		int label;
		//Predecir clase junto con su valor de confianza
		model->predict(img, label, confidence);

		return label;
	}

	//Almacenar fisherfaces en fichero
	void FisherFacesRecognizer::saveFisherFaces(const int height, const std::string &directory, const std::string& baseName)
	{
		//Obtener los eigenvectores
		cv::Mat W = model->getEigenVectors();
		//Obtener los eigenvalues
		cv::Mat eigenvalues = model->getEigenValues();

		//Recorrer las 16 primeras fisherfaces 
		for (int i = 0; i < std::min(16, W.cols); i++) {
			std::string msg = cv::format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
			std::cout << msg << std::endl;
			// get eigenvector #i
			cv::Mat ev = W.col(i).clone();
			// Reshape to original size & normalize to [0...255] for imshow.
			cv::Mat grayscale = norm_0_255(ev.reshape(1, height));
			// Show the image & apply a Jet colormap for better sensing.
			cv::Mat cgrayscale;
			cv::applyColorMap(grayscale, cgrayscale, cv::COLORMAP_BONE);
			//Almacenar eigenfaces
			cv::imwrite(cv::format(("%s/" + baseName + "_%d.png").c_str(), directory.c_str(), i), norm_0_255(cgrayscale));
		}
	}

	//Mostrar eigenfaces
	void FisherFacesRecognizer::showFisherFaces(const int height)
	{
		//Obtener los eigenvectores
		cv::Mat W = model->getEigenVectors();
		//Obtener los eigenvalues
		cv::Mat eigenvalues = model->getEigenValues();

		//Recorrer las 16 primeras fisherfaces 
		for (int i = 0; i < std::min(16, W.cols); i++) {
			std::string msg = cv::format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
			std::cout << msg << std::endl;
			// get eigenvector #i
			cv::Mat ev = W.col(i).clone();
			// Reshape to original size & normalize to [0...255] for imshow.
			cv::Mat grayscale = norm_0_255(ev.reshape(1, height));
			// Show the image & apply a Jet colormap for better sensing.
			cv::Mat cgrayscale;
			cv::applyColorMap(grayscale, cgrayscale, cv::COLORMAP_BONE);
			//Mostrar eigenfaces
			cv::imshow(cv::format("fisherface_%d", i), cgrayscale);
			//Espera necesaria
			cv::waitKey(0);
		}
	}

	//Guardar reconstrucción por fases de la imagen
	void FisherFacesRecognizer::saveReconstruction(const cv::Mat& img, const std::string &directory, const std::string& baseName)
	{
		//Obtener los eigenvectores
		cv::Mat W = model->getEigenVectors();
		//Obtener los eigenvalues
		cv::Mat eigenvalues = model->getEigenValues();
		// Get the sample mean from the training data
		cv::Mat mean = model->getMean();

		// Display or save the image reconstruction at some predefined steps:
		for (int num_component = 0; num_component < std::min(16, W.cols); num_component++) {
			// Slice the Fisherface from the model:
			cv::Mat ev = W.col(num_component);
			cv::Mat projection = cv::LDA::subspaceProject(ev, mean, img.reshape(1, 1));
			cv::Mat reconstruction = cv::LDA::subspaceReconstruct(ev, mean, projection);
			// Normalize the result:
			reconstruction = norm_0_255(reconstruction.reshape(1, img.rows));
			// Salvar reconstrucción
			imwrite(cv::format(("%s/" + baseName + "_%d.png").c_str(), num_component), reconstruction);
		}
	}

	//Mostrar reconstrucción por fases de la imagen
	void FisherFacesRecognizer::showReconstruction(const cv::Mat& img)
	{
		//Obtener los eigenvectores
		cv::Mat W = model->getEigenVectors();
		//Obtener los eigenvalues
		cv::Mat eigenvalues = model->getEigenValues();
		// Get the sample mean from the training data
		cv::Mat mean = model->getMean();

		// Display or save the image reconstruction at some predefined steps:
		for (int num_component = 0; num_component < std::min(16, W.cols); num_component++) {
			// Slice the Fisherface from the model:
			cv::Mat ev = W.col(num_component);
			cv::Mat projection = cv::LDA::subspaceProject(ev, mean, img.reshape(1, 1));
			cv::Mat reconstruction = cv::LDA::subspaceReconstruct(ev, mean, projection);
			// Normalize the result:
			reconstruction = norm_0_255(reconstruction.reshape(1, img.rows));
			// Salvar reconstrucción
			imshow(cv::format("fisherface_reconstruction_%d", num_component), reconstruction);
		}
	}

	//Normalizar imagen
	cv::Mat FisherFacesRecognizer::norm_0_255(cv::InputArray _src)
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
