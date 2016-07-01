#pragma once
#include <iostream>

namespace tfg
{
	//Calculadora de estadísticas
	class StatisticsCalculator
	{
		//Método para volcar resultado sobre stream de salida
		friend std::ostream& operator<<(std::ostream &os, const StatisticsCalculator& calculator);
	public:
		//Constructor a partir del nombre
		StatisticsCalculator(const std::string& nombre = "");
		//Constructor de copia
		StatisticsCalculator(const StatisticsCalculator& calculator);
		//Operador asignación
		StatisticsCalculator& operator=(const StatisticsCalculator& calculator);
		//Destructor
		~StatisticsCalculator();
		//Añadir muestra
		void addMuestra(const int prediction, const int correctLabel, const double &confidence);
		//Realizar impresión en texto desestructurado sobre la salida estándar
		void imprimirResultados() const;
		//Volcar cabecera sobre el stream de salida
		void imprimirCabecera(std::ostream &os) const;
		//Establecer nombre del algoritmo
		void setNombreAlgoritmo(const std::string &nombre);
		//Establecer tiempo
		void setTime(const double& time);
		//Establecer umbral usado
		void setUmbral(const double& umbral);
		//Realizar cálculos internos al terminar de añadir muestras
		void realizarCalculos();
		//Reiniciar calculadora
		void reiniciar();

		//Índices estadísticos internos
		double fnr, fcpr, fpr;
		//Valores de confianza
		double minConfidence, maxConfidence;

	private:
		//Valores de matriz de confusión
		int verdaderosPositivos, verdaderosNegativos,
			positivosConfundidos, falsosPositivos, falsosNegativos;
		//Confianza para valores de confusión
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

