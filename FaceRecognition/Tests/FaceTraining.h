#pragma once
#include "ImageUpsampler.h"
#include <string>

//Main para test de reconocimiento de caras
int testFaces(int argc, char**argv);
//Entrenar bases de aprendizaje
int trainFaces(int argc, char** argv);
//Separar base de datos
int separeDatabase(int argc, char**argv);
//Leer fichero que contiene bases de imágenes
void readDatabasesFile(const std::string &file, std::vector<std::string> &databases);
//Crear directorio
bool createDir(const std::string& dirName_in);
//Generar upsampler
tfg::ImageUpsampler* generateUpsampler(int opcion);