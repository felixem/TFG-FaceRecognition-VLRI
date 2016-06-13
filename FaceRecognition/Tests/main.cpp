// Tests.cpp: define el punto de entrada de la aplicación de consola.
//
#include "stdafx.h"
#include <iostream>
#include "HaarLikeFaces.h"
#include "ReconocimientoCaras.h"
#include "LowResolution.h"

//Main de entrada para los tests
int main(int argc, char** argv)
{
	//Comprobar parámetros
	if (argc < 2)
	{
		std::cerr << "Error: Argumentos incorrectos [" << argv[0] << " opcion]" << std::endl;
		getchar();
		return -1;
	}

	//Leer opción
	int opcion = std::atoi(argv[1]);
	//Crear **char de argumentos quitando la opción elegida
	char** argsMetodo = new char*[argc];
	//Copiar argumentos
	argsMetodo[0] = argv[0];
	for (int i = 2; i < argc; ++i)
	{
		argsMetodo[i-1] = argv[i];
	}

	//Ejecutar main adecuado
	switch (opcion)
	{
		//Detección de caras
		case 0: mainHaarLike(argc-1,argsMetodo);
			break;
		//Reconocimiento de caras con Eigenfaces
		case 1: mainEigenFaces(argc-1, argsMetodo);
			break;
		//Reconocimiento de caras con Fisherfaces
		case 2: mainFisherFaces(argc-1, argsMetodo);
			break;
		//Reconocimiento de caras con LBPfaces
		case 3: mainLBPFaces(argc-1, argsMetodo);
			break;
		//Reconocimiento de caras con Eigenfaces con LR
		case 4: mainLREigenFaces(argc-1, argsMetodo);
			break;
		//Reconocimiento de caras con Fisherfaces con LR
		case 5: mainLRFisherFaces(argc - 1, argsMetodo);
			break;
		//Reconocimiento de caras con LBPfaces con LR
		case 6: mainLRLBPFaces(argc - 1, argsMetodo);
			break;
		default:
			std::cerr << "Error: opción incorrecta" << std::endl;
	}

	//Limpiar memoria
	delete[] argsMetodo;
}