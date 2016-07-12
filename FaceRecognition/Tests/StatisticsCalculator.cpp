#include "stdafx.h"
#include "StatisticsCalculator.h"


namespace tfg
{	
	//Constructor a partir del nombre
	StatisticsCalculator::StatisticsCalculator(const std::string& nombre) : verdaderosPositivos(0), verdaderosNegativos(0),
		positivosConfundidos(0), falsosPositivos(0), falsosNegativos(0), confVerdaderosPositivos(0),
		confVerdaderosNegativos(0), confPositivosConfundidos(0), confFalsosPositivos(0), confFalsosNegativos(0),
		nombreAlgoritmo(nombre), fnr(0), fcpr(0), fpr(0), minConfidence(std::numeric_limits<double>::max()), maxConfidence(0), umbral(0)
	{
	}

	//Destructor
	StatisticsCalculator::~StatisticsCalculator()
	{
	}

	//Constructor de copia
	StatisticsCalculator::StatisticsCalculator(const StatisticsCalculator& calculator) : verdaderosPositivos(calculator.verdaderosPositivos), verdaderosNegativos(calculator.verdaderosNegativos),
		positivosConfundidos(calculator.positivosConfundidos), falsosPositivos(calculator.falsosPositivos), falsosNegativos(calculator.falsosNegativos), confVerdaderosPositivos(calculator.confVerdaderosPositivos),
		confVerdaderosNegativos(calculator.confVerdaderosNegativos), confPositivosConfundidos(calculator.confPositivosConfundidos), confFalsosPositivos(calculator.confFalsosPositivos), confFalsosNegativos(calculator.confFalsosNegativos),
		nombreAlgoritmo(calculator.nombreAlgoritmo), fnr(calculator.fnr), fcpr(calculator.fcpr), fpr(calculator.fpr),
		minConfidence(calculator.minConfidence), maxConfidence(calculator.maxConfidence), umbral(calculator.umbral)
	{

	}

	//Operador asignación
	StatisticsCalculator& StatisticsCalculator::operator=(const StatisticsCalculator& calculator)
	{
		this->verdaderosPositivos = calculator.verdaderosPositivos;
		this->verdaderosNegativos = calculator.verdaderosNegativos;
		this->positivosConfundidos = calculator.positivosConfundidos;
		this->falsosPositivos = calculator.falsosPositivos;
		this->falsosNegativos = calculator.falsosNegativos;
		this->confVerdaderosPositivos = calculator.confVerdaderosPositivos;
		this->confVerdaderosNegativos = calculator.confVerdaderosNegativos;
		this->confPositivosConfundidos = calculator.confPositivosConfundidos;
		this->confFalsosPositivos = calculator.confFalsosPositivos;
		this->confFalsosNegativos = calculator.confFalsosNegativos;
		this->nombreAlgoritmo = calculator.nombreAlgoritmo;
		this->fnr = calculator.fnr;
		this->fcpr = calculator.fcpr;
		this->fpr = calculator.fpr;
		this->minConfidence = calculator.minConfidence;
		this->maxConfidence = calculator.maxConfidence;
		this->umbral = umbral;

		return *this;
	}

	//Añadir muestra
	void StatisticsCalculator::addMuestra(const int prediction, const int correctLabel, const double &confidence)
	{
		//Comprobar valor de confianza
		if (confidence > maxConfidence)
			maxConfidence = confidence;
		else if (confidence < minConfidence)
			minConfidence = confidence;

		//Comprobar si la muestra real es una imagen de la bd o no
		if (correctLabel != -1)
		{
			//Verdadero positivo
			if (prediction == correctLabel)
			{
				confVerdaderosPositivos += confidence;
				verdaderosPositivos++;
			}
			//Falso positivo (o bien se ha equivocado de clase o bien la clase no pertenece a la bd)
			else if (prediction == -1)
			{
				confFalsosNegativos += confidence;
				falsosNegativos++;
			}
			else
			{
				//Positivo cambiado de clase
				confPositivosConfundidos += confidence;
				positivosConfundidos++;
			}
		}
		else
		{
			//Verdadero negativo
			if (prediction == correctLabel)
			{
				confVerdaderosNegativos += confidence;
				verdaderosNegativos++;
			}
			//Falso negativo
			else
			{
				confFalsosPositivos += confidence;
				falsosPositivos++;
			}
		}
	}

	//Realizar impresión en texto desestructurado sobre la salida estándar
	void StatisticsCalculator::imprimirResultados() const
	{
		//Imprimir resultados
		std::cout << "Algoritmo: " << this->nombreAlgoritmo.c_str() << std::endl;
		std::cout << "Umbral usado: " << this->umbral << std::endl;
		std::cout << "Verdaderos positivos: " << verdaderosPositivos << std::endl;
		std::cout << "Positivos confundidos: " << positivosConfundidos << std::endl;
		std::cout << "Falsos positivos: " << falsosPositivos << std::endl;
		std::cout << "Verdaderos negativos: " << verdaderosNegativos << std::endl;
		std::cout << "Falsos negativos: " << falsosNegativos << std::endl;
		std::cout << "FNR (False negatives ratio): " << fnr * 100 << "%" << std::endl;
		std::cout << "FCPR (False class-positives ratio): " << fcpr * 100 << "%" << std::endl;
		std::cout << "FPR (False positives ratio): " << fpr * 100 << "%" << std::endl;
		std::cout << "Tiempo medio: " << time << std::endl;
	}

	//Volcar cabecera sobre el stream de salida
	void StatisticsCalculator::imprimirCabecera(std::ostream &os) const
	{
		os << "Algoritmo;Umbral usado;Verdaderos positivos;Positivos confundidos;Falsos positivos;Verdaderos negativos;Falsos negativos;"
			<< "FNR;FCPR;FPR;Tiempo medio" << std::endl;
	}

	//Método para volcar resultado sobre stream de salida
	std::ostream& operator<<(std::ostream &os, const StatisticsCalculator& calculator)
	{
		//Volcado de resultados
		os << calculator.nombreAlgoritmo.c_str() << ";" << calculator.umbral << "; " << calculator.verdaderosPositivos << "; " << calculator.positivosConfundidos << "; "
			<< calculator.falsosPositivos << ";" << calculator.verdaderosNegativos << ";" << calculator.falsosNegativos
			<< ";" << calculator.fnr << ";" << calculator.fcpr << "; " << calculator.fpr << "; " << calculator.time << std::endl;

		return os;
	}

	//Establecer nombre del algoritmo
	void StatisticsCalculator::setNombreAlgoritmo(const std::string &nombre)
	{
		this->nombreAlgoritmo = nombre;
	}

	//Establecer tiempo
	void StatisticsCalculator::setTime(const double& time)
	{
		this->time = time;
	}

	//Establecer umbral usado
	void StatisticsCalculator::setUmbral(const double& umbral)
	{
		this->umbral = umbral;
	}

	//Realizar cálculos internos
	void StatisticsCalculator::realizarCalculos()
	{
		//Calcular índices estadísticos
		fnr = (double)falsosNegativos / ((double)verdaderosPositivos + (double) positivosConfundidos + (double)falsosNegativos);
		fcpr = (double)positivosConfundidos / ((double)verdaderosPositivos + (double)positivosConfundidos + (double)falsosNegativos);
		fpr = (double)falsosPositivos / ((double)verdaderosNegativos + (double)falsosPositivos);

		//Comprobar desborde de fcpr
		if (isnan(fcpr))
			fcpr = 0;
	}

	//Reiniciar calculadora
	void StatisticsCalculator::reiniciar()
	{
		this->verdaderosPositivos = 0;
		this->verdaderosNegativos = 0;
		this->positivosConfundidos = 0;
		this->falsosPositivos = 0;
		this->falsosNegativos = 0;
		this->confVerdaderosPositivos = 0;
		this->confVerdaderosNegativos = 0;
		this->confPositivosConfundidos = 0;
		this->confFalsosPositivos = 0;
		this->confFalsosNegativos = 0;
		this->nombreAlgoritmo = "";
		this->fnr = 0;
		this->fcpr = 0;
		this->fpr = 0;
		this->minConfidence = std::numeric_limits<double>::max();
		this->maxConfidence = 0;
		this->umbral = 0;
	}
}
