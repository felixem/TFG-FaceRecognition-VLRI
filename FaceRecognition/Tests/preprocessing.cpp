#include "preprocessing.h"
#include "stdafx.h"

#include <vector>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include <windows.h>

using namespace std;
using namespace cv;

//Distancia entre vectores
float dist(std::vector<float> &a, std::vector<float> &b)
{
	float r = 0;
	for (int i = 0; i<(signed)a.size(); i++)
		r += (a[i] - b[i])*(a[i] - b[i]);
	return r;
}

//Obtener el descriptor SIFT
void getSIFTdescripter(std::vector<float> &descripter, std::vector<std::vector<float> > &m,
	std::vector<std::vector<float> > &theta, int x, int y, int windowSize, int histDim, int bucketsDim,
	std::vector<std::vector<float> > &Gaussian)
{
	for (int i = 0; i<(signed)descripter.size(); i++)
		descripter[i] = 0;

	int histDimWidth = 2 * windowSize / histDim;
	float degPerBin = 360.0f / bucketsDim;

	// weight magnitudes by Gaussian with sigma equal to half window
	vector<float> mtimesGRow(2 * windowSize);
	vector<vector<float> > mtimesG(2 * windowSize, mtimesGRow);
	for (int i = 0; i<2 * windowSize; i++)
	{
		for (int j = 0; j<2 * windowSize; j++)
		{
			int xx = x + i - (windowSize - 1), yy = y + j - (windowSize - 1);
			mtimesG[i][j] = m[xx][yy] * Gaussian[i][j];
		}
	}

	// calculate descripter
	// using trilinear interpolation
	int histBin[2], histX[2], histY[2];
	float dX[2], dY[2], dBin[2];
	for (int i = 0; i<2 * windowSize; i++)
	{
		for (int j = 0; j<2 * windowSize; j++)
		{
			histX[0] = i / histDim; histX[1] = i / histDim;
			histY[0] = j / histDim; histY[1] = j / histDim;
			dX[1] = 0;
			dY[1] = 0;

			int iModHD = i % histDim;
			int jModHD = j % histDim;
			int histDimD2 = histDim / 2;

			if (iModHD >= histDimD2 && i < 2 * windowSize - histDimD2)
			{
				histX[1] = histX[0] + 1;
				dX[1] = (iModHD + 0.5f - histDimD2) / histDim;
			}
			if (iModHD < histDimD2 && i >= histDimD2)
			{
				histX[1] = histX[0] - 1;
				dX[1] = (histDimD2 + 0.5f - iModHD) / histDim;
			}
			if (jModHD >= histDimD2 && j < 2 * windowSize - histDimD2)
			{
				histY[1] = histY[0] + 1;
				dY[1] = (jModHD + 0.5f - histDimD2) / histDim;
			}
			if (jModHD < histDimD2 && j >= histDimD2)
			{
				histY[1] = histY[0] - 1;
				dY[1] = (histDimD2 + 0.5f - jModHD) / histDim;
			}

			dX[0] = 1.0f - dX[1];
			dY[0] = 1.0f - dY[1];

			float histAngle = theta[x + i - (windowSize - 1)][y + j - (windowSize - 1)];

			histBin[0] = (int)(histAngle / degPerBin);
			histBin[1] = (histBin[0] + 1) % bucketsDim;
			dBin[1] = (histAngle - histBin[0] * degPerBin) / degPerBin;
			dBin[0] = 1.0f - dBin[1];

			for (int histBinIndex = 0; histBinIndex<2; histBinIndex++)
			{
				for (int histXIndex = 0; histXIndex<2; histXIndex++)
				{
					for (int histYIndex = 0; histYIndex<2; histYIndex++)
					{
						int histNum = histX[histXIndex] * histDimWidth + histY[histYIndex];
						int bin = histBin[histBinIndex];
						descripter[histNum*bucketsDim + bin] += (mtimesG[i][j] * dX[histXIndex] * dY[histYIndex] * dBin[histBinIndex]);
					}
				}
			}
		}
	}

	// normalize
	// threshold values at .2, renormalize
	float sum = 0;
	for (int i = 0; i<(signed)descripter.size(); i++)
		sum += descripter[i];

	if (sum < .0000001f)
	{
		float dn = 1.0f / (signed)descripter.size();
		for (int i = 0; i<(signed)descripter.size(); i++)
			descripter[i] = 0;
		return;
	}

	for (int i = 0; i<(signed)descripter.size(); i++)
	{
		descripter[i] /= sum;
		if (descripter[i] > .2f)
			descripter[i] = .2f;
	}
	sum = 0;
	for (int i = 0; i<(signed)descripter.size(); i++)
		sum += descripter[i];
	for (int i = 0; i<(signed)descripter.size(); i++)
		descripter[i] /= sum;
}

//Calcular gaussiana
void computeGaussian(vector<vector<float> > &Gaussian, int windowSize)
{
	for (int i = 0; i<2 * windowSize; i++)
	{
		vector<float> grow(2 * windowSize);
		for (int j = 0; j<2 * windowSize; j++)
		{
			float ii = i - (windowSize - 0.5f), jj = j - (windowSize - 0.5f);
			grow[j] = exp(-(ii*ii + jj*jj) / (2 * windowSize*windowSize));
		}
		Gaussian.push_back(grow);
	}
}

//Calcular vecindad
float computeLogLikelihood(vector<vector<float> > &distField, vector<vector<float> > &fids, int numFeatureClusters)
{
	float l = 0;
	for (int j = 0; j<(signed)fids.size(); j++)
	{
		for (int i = 0; i<numFeatureClusters; i++)
			l += fids[j][i] * (float)log(distField[j][i]);
	}
	return l;
}

void getNewFeatsInvT(vector<vector<float> > &newFIDs, vector<vector<vector<float> > > &originalFeats,
	vector<float> &vparams, float centerX, float centerY, vector<pair<int, int> > &randPxls)
{
	int numFeats = newFIDs[0].size();
	vector<float> uniformDist(numFeats, 1.0f / numFeats);

	float postM[2][3] = { { 1,0,centerX },{ 0,1,centerY } };
	float preM[3][3] = { { 1,0,-centerX },{ 0,1,-centerY },{ 0,0,1 } };

	float tM[3][3] = { { 1, 0, vparams[0] },{ 0, 1, vparams[1] },{ 0,0,1 } };
	float rM[3][3] = { { cos(vparams[2]), -sin(vparams[2]), 0 },{ sin(vparams[2]), cos(vparams[2]), 0 },{ 0, 0, 1 } };
	float sM[3][3] = { { exp(vparams[3]), 0, 0 },{ 0, exp(vparams[3]), 0 },{ 0, 0, 1 } };

	CvMat tCVM, rCVM, sCVM, hxCVM, hyCVM, *xform, postCVM, preCVM;
	tCVM = cvMat(3, 3, CV_32FC1, tM);
	rCVM = cvMat(3, 3, CV_32FC1, rM);
	sCVM = cvMat(3, 3, CV_32FC1, sM);

	postCVM = cvMat(2, 3, CV_32FC1, postM);
	preCVM = cvMat(3, 3, CV_32FC1, preM);

	xform = cvCreateMat(2, 3, CV_32FC1);
	cvMatMul(&postCVM, &tCVM, xform);
	cvMatMul(xform, &rCVM, xform);
	cvMatMul(xform, &sCVM, xform);
	cvMatMul(xform, &preCVM, xform);

	int height = (signed)originalFeats.size();
	int width = (signed)originalFeats[0].size();

	for (int i = 0; i<(signed)newFIDs.size(); i++)
	{
		int j = randPxls[i].first, k = randPxls[i].second;
		int nx = (int)(xform->data.fl[0] * k + xform->data.fl[1] * j + xform->data.fl[2] + 0.5f);
		int ny = (int)(xform->data.fl[3] * k + xform->data.fl[4] * j + xform->data.fl[5] + 0.5f);
		if (!(ny >= 0 && ny < height && nx >= 0 && nx < width))
			newFIDs[i] = uniformDist;
		else
			newFIDs[i] = originalFeats[ny][nx];
	}

	cvReleaseMat(&xform);
}

//Comprobar si un directorio existe
bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}