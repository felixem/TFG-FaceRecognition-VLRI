// Tests.cpp: define el punto de entrada de la aplicación de consola.
//
#include "stdafx.h"
#include <iostream>
#include "HaarLikeFaces.h"
#include "ReconocimientoCaras.h"

//Main de entrada para los tests
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Faltan argumentos [" << argv[0] << " opcion]" << std::endl;
		getchar();
		return -1;
	}

	//Leer opción
	int opcion = std::atoi(argv[1]);
	//Ejecutar main adecuado
	switch (opcion)
	{
		//Detección de caras
		case 0: mainHaarLike();
			break;
		//Reconocimiento de caras con Eigenfaces
		case 1: mainEigenFaces();
			break;
		//Reconocimiento de caras con Fisherfaces
		case 2: mainFisherFaces();
			break;
		//Reconocimiento de caras con LBPfaces
		case 3: mainLBPFaces();
			break;
		default:
			std::cerr << "Error: opción incorrecta" << std::endl;
	}
}