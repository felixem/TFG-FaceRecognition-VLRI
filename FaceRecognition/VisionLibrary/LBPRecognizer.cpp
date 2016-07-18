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

	//Entrenar modelo a partir de un vector de imágenes y de labels
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
		
		//Comprobar el umbral
		if (confidence <= valorUmbral)
			return label;
		else
			return -1;
	}

	//Limpìar modelo
	void LBPRecognizer::clear()
	{
		this->model->clear();
	}

	//Poner umbral de detección (valores por encima del umbral se considerarán desconocidos)
	void LBPRecognizer::setUmbral(double umbral)
	{
		this->valorUmbral = umbral;
	}

	//Nombre del reconocedor
	std::string LBPRecognizer::getName() const
	{
		return "LBPH";
	}

	//Cargar desde fichero el modelo
	void LBPRecognizer::load(const std::string& fichero)
	{
		this->model->load(fichero);
	}

	//Guardar en fichero el modelo
	void LBPRecognizer::save(const std::string& fichero)
	{
		this->model->save(fichero);
	}
}
