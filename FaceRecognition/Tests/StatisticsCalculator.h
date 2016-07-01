#pragma once
#include <iostream>

namespace tfg
{
	//Calculadora de estad�sticas
	class StatisticsCalculator
	{
		//M�todo para volcar resultado sobre stream de salida
		friend std::ostream& operator<<(std::ostream &os, const StatisticsCalculator& calculator);
	public:
		//Constructor a partir del nombre
		StatisticsCalculator(const std::string& nombre = "");
		//Constructor de copia
		StatisticsCalculator(const StatisticsCalculator& calculator);
		//Operador asignaci�n
		StatisticsCalculator& operator=(const StatisticsCalculator& calculator);
		//Destructor
		~StatisticsCalculator();
		//A�adir muestra
		void addMuestra(const int prediction, const int correctLabel, const double &confidence);
		//Realizar impresi�n en texto desestructurado sobre la salida est�ndar
		void imprimirResultados() const;
		//Volcar cabecera sobre el stream de salida
		void imprimirCabecera(std::ostream &os) const;
		//Establecer nombre del algoritmo
		void setNombreAlgoritmo(const std::string &nombre);
		//Establecer tiempo
		void setTime(const double& time);
		//Establecer umbral usado
		void setUmbral(const double& umbral);
		//Realizar c�lculos internos al terminar de a�adir muestras
		void realizarCalculos();
		//Reiniciar calculadora
		void reiniciar();

		//�ndices estad�sticos internos
		double fnr, fcpr, fpr;
		//Valores de confianza
		double minConfidence, maxConfidence;

	private:
		//Valores de matriz de confusi�n
		int verdaderosPositivos, verdaderosNegativos,
			positivosConfundidos, falsosPositivos, falsosNegativos;
		//Confianza para valores de confusi�n
		double confVerdaderosPositivos, confVerdaderosNegativos,
			confPositivosConfundidos, confFalsosPositivos, confFalsosNegativos;
		//Nombre del algoritmo
		std::string nombreAlgoritmo;
		//Tiempo medio
		double time;
		//Umbral del algoritmo
		double umbral;
	};
}

