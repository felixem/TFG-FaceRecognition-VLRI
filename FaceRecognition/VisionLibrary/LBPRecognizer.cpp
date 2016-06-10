#include "stdafx.h"
#include "LBPRecognizer.h"

namespace tfg
{
	//Constructor
	LBPRecognizer::LBPRecognizer()
	{
		model = cv::face::createLBPHFaceRecognizer();
	}


	LBPRecognizer::~LBPRecognizer()
	{
	}

	//Entrenar modelo a partir de un vector de im�genes y de labels
	void LBPRecognizer::train(const std::vector<cv::Mat> &images, const std::vector<int> &labels)
	{
		//Entrenar modelo
		this->model->train(images, labels);
	}

	//Predecir clase de la imagen
	int LBPRecognizer::predict(const cv::Mat& img, double &confidence)
	{
		int label;
		//Predecir clase junto con su valor de confianza
		model->predict(img, label, confidence);

		return label;
	}
}
