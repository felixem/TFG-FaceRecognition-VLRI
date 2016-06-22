#pragma once
#include <string>

//Entrenar bases de aprendizaje
int trainFaces(int argc, char** argv);
//Separar base de datos
int separeDatabase(int argc, char**argv);
//Leer fichero que contiene bases de imágenes
void readDatabasesFile(const std::string &file, std::vector<std::string> &databases);
//Crear directorio
bool createDir(const std::string& dirName_in);