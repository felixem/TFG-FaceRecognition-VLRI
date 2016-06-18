#pragma once
#include <vector>

//Mains especiales
int congealRealMain(int argc, char* argv[]);
int funnelRealMain(int argc, char *argv[]);

//Comprobar si un directorio existe
bool dirExists(const std::string& dirName_in);

//Métodos comunes
//Distancia entre vectores
float dist(std::vector<float> &a, std::vector<float> &b);
//Descriptor SIFT
void getSIFTdescripter(std::vector<float> &descripter, std::vector<std::vector<float> > &m, 
	std::vector<std::vector<float> > &theta, int x, int y, int windowSize, int histDim, int bucketsDim,
	std::vector<std::vector<float> > &Gaussian);
//Calcular gaussiana
void computeGaussian(std::vector<std::vector<float> > &Gaussian, int windowSize);
//Calcular vecindad
float computeLogLikelihood(std::vector<std::vector<float> > &logDistField, 
	std::vector<std::vector<float> > &fids, int numFeatureClusters);
//Calcular
void getNewFeatsInvT(std::vector<std::vector<float> > &newFIDs, std::vector<std::vector<std::vector<float> > > &originalFeats,
	std::vector<float> &vparams, float centerX, float centerY, std::vector<std::pair<int, int> > &randLocs);