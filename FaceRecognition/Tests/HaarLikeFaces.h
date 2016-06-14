#pragma once

#include <opencv\cv.h>
#include "HaarLikeFaceDetector.h"

// Function main
int mainHaarLike(int argc, char** argv);

//Extraer la cara principal de la imagen
void extractMainFace(tfg::HaarLikeFaceDetector &faceDetector, const cv::Mat &img, cv::Mat &face, int minWidth, int minHeight, int finalWidth, int finalHeight);