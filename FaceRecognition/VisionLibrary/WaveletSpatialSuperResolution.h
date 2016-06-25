//
//  super_resolution.h
//  SuperResolution
//
//  Created by Hana_Chang on 2015/5/14.
//  Copyright (c) 2015年 Andrea.C. All rights reserved.
//

#ifndef SUPER_RESOLUTION_H_
#define SUPER_RESOLUTION_H_
#include <opencv2/opencv.hpp>

#include "ImageUpsampler.h"

namespace tfg
{
	//Clase utilizada para aplicar el algoritmo de superresolution de
	//A Fast Algorithm for Single Image Super Resolution in Both Wavelet and Spatial Domain
	class WaveletSpatialSRUpsampler : public ImageUpsampler {
	private:
		//Variables internas
		int scaling_factor;
		int outputWidth;
		int outputHeight;
		int total_iteration;		
		cv::Mat low_resolution_image;

		//Métodos internos
		cv::Mat usScheme(const cv::Mat& img);
		cv::Mat reconstructIter(int current_iteration, const cv::Mat& src, const cv::Mat& hImg);

	public:
		//Constructor
		WaveletSpatialSRUpsampler(int total_iteration);
		//Realizar upsampling
		void upSample(const cv::Mat &low_resolution_image, cv::Mat &output, int height, int width);
		//Nombre del reconocedor
		std::string getName() const;
	};

	//Clase utilizada para realizar transformaciones
	class WaveletCdf97 {
	private:
		friend class WaveletSpatialSRUpsampler;
		struct WaveletParam {
			cv::Mat lift_filter;
			std::vector<double> extrapolate_odd;
			double scale_factor;
			int enable;
			WaveletParam() {
				enable = 0;
			};

		};

		cv::Mat wavelet_img;
		int nlevel;
		WaveletParam wp;

		void ForwardTransform();
		void InverseTransform();
		void TransferAlongRow(const int N1, const int N2, const int M1, const int M2, const bool flag);
		void TransferAlongCol(const int N1, const int N2, const int M1, const int M2, const bool flag);
		cv::Mat filter(const cv::Mat& B, const cv::Mat& X, const cv::Mat& Zi); // b is a column vector

	public:
		WaveletCdf97(cv::Mat& img, const int nlevel, WaveletParam w = WaveletParam());
		cv::Mat Run();
	};

	//Tipo de punto necesario
	struct Point5d {
		double x;
		double y;
		double xw;
		double yw;
		double color;
	};

	//Interpolator interno del algoritmo
	class Interpolator {
	public:
		Interpolator(const cv::Mat &src, int width, int height);
		cv::Mat BicubicInterpolate();
	private:
		cv::Mat src;
		int width, height;
		std::vector<Point5d> neighbors;

		double GetColor(const cv::Point_<double>& p);
	};
}


#endif /* SUPER_RESOLUTION_H_ */