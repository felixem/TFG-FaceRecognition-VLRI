//
//  super_resolution.cpp
//  SuperResolution
//
//  Created by Hana_Chang on 2015/5/14.
//  Copyright (c) 2015年 Andrea.C. All rights reserved.
//

#include "stdafx.h"
#include "WaveletSpatialSuperResolution.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using std::vector;

namespace tfg
{
	//Constructor del algoritmo
	WaveletSpatialSRUpsampler::WaveletSpatialSRUpsampler(int total_iteration) {
		this->scaling_factor = 1;
		this->total_iteration = total_iteration;
	}

	//Aplicar algoritmo
	void WaveletSpatialSRUpsampler::upSample(const cv::Mat &img, cv::Mat &output, int height, int width) {
		//Copiar valores
		Mat low_resolution_image64;
		img.convertTo(low_resolution_image64, CV_64FC1);
		low_resolution_image64.copyTo(this->low_resolution_image);
		this->outputHeight = height;
		this->outputWidth = width;

		//Comenzar upsampling
		if (low_resolution_image.channels() == 1) {
			// ----- upsamling scheme -----
			Mat h0Img = usScheme(low_resolution_image);

			// ----- gaussian filter -----
			Mat gauImg;
			GaussianBlur(h0Img, gauImg, Size(3, 3), 0.5, 0.5, BORDER_CONSTANT);
			Mat gauImg64;
			gauImg.convertTo(gauImg64, CV_64FC1);

			Mat result64 = reconstructIter(0, gauImg64, h0Img);
			result64.convertTo(output, CV_8UC1);
		}
		else {
			// ---- rgb2ycbcr ----
			vector<Mat> channels(3);
			split(low_resolution_image, channels);
			Mat B = channels[0];
			Mat G = channels[1];
			Mat R = channels[2];

			Mat Y = 0.299*R + 0.587*G + 0.114*B;
			Mat U = -0.168736*R - 0.331264*G + 0.5*B;
			Mat V = 0.5*R - 0.418688*G - 0.081312*B;

			// ----- upsamling scheme -----
			Mat h0Img = usScheme(Y);

			// ----- gaussian filter -----
			Mat gauImg;
			GaussianBlur(h0Img, gauImg, Size(3, 3), 0.5, 0.5, BORDER_CONSTANT);
			Mat gauImg64;
			gauImg.convertTo(gauImg64, CV_64FC1);
			low_resolution_image = Y;
			Mat resultY = reconstructIter(0, gauImg64, h0Img);

			// ---- ycbcr2rgb ----
			Mat upU, upV;
			resize(U, upU, resultY.size(), 0, 0, INTER_LINEAR);
			resize(V, upV, resultY.size(), 0, 0, INTER_LINEAR);

			Mat resultR = resultY - 0 * upU + 1.4020*upV;
			Mat resultG = resultY - 0.3441*upU - 0.7141*upV;
			Mat resultB = resultY + 1.772*upU + 0 * upV;

			Mat result64;
			channels[0] = resultB;
			channels[1] = resultG;
			channels[2] = resultR;

			merge(channels, result64);
			result64.convertTo(output, CV_8UC1);
		}
	}

	//Realizar pasos del algoritmo
	Mat WaveletSpatialSRUpsampler::usScheme(const Mat& img) {

		Mat upImg = Mat::zeros(this->outputHeight, this->outputWidth, CV_64FC1);
		Interpolator ip(img, this->outputWidth, this->outputHeight);
		upImg = ip.BicubicInterpolate();


		WaveletCdf97 wc(upImg, 1);
		Mat dwtImg = wc.Run();

		Mat recoverImg = Mat::zeros((int)ceil(dwtImg.rows*0.5), (int)ceil(dwtImg.cols*0.5), CV_64FC1);
		resize(img, recoverImg, recoverImg.size(), 0, 0, INTER_AREA);

		for (int i = 0; i < recoverImg.rows; i++) {
			for (int j = 0; j < recoverImg.cols; j++) {
				dwtImg.at<double>(i, j) = recoverImg.at<double>(i, j)*scaling_factor;
			}
		}

		WaveletCdf97 wc1(dwtImg, -1);
		Mat result = wc1.Run();

		return result;
	}

	//Reconstruir iteración
	Mat WaveletSpatialSRUpsampler::reconstructIter(int current_iteration, const Mat& src, const Mat& hImg) {
		Mat downImg = Mat::zeros(low_resolution_image.rows, low_resolution_image.cols, CV_64FC1);

		resize(src, downImg, downImg.size(), 0, 0, INTER_NEAREST);
		Mat downImg64;
		downImg.convertTo(downImg64, CV_64FC1);

		Mat reconError = Mat::zeros(downImg64.rows, downImg64.cols, CV_64FC1);

		reconError = low_resolution_image - downImg64;
		Mat errImg = usScheme(reconError);

		// back-projecting the error
		Mat dst = errImg + hImg;
		if (current_iteration < total_iteration) {
			return reconstructIter(++current_iteration, dst, dst);
		}
		else {
			return dst;
		}
	}

	//Constructor del wavelet
	WaveletCdf97::WaveletCdf97(Mat& img, const int nlevel, WaveletParam w) {
		// input image initialization
		this->wavelet_img = img;
		this->nlevel = nlevel;

		// Wavelet cdf 9/7 parameters setting
		// 1. lift filter
		if (w.enable == 0) {
			double lift_filter_arr[8] = { -1.5861343420693648, -0.0529801185718856, 0.8829110755411875, 0.4435068520511142, -1.5861343420693648, -0.0529801185718856, 0.8829110755411875, 0.4435068520511142 };
			w.scale_factor = 1.1496043988602418;

			double S1 = lift_filter_arr[0];
			double S2 = lift_filter_arr[1];
			double S3 = lift_filter_arr[2];
			Mat tmp(2, 4, CV_64FC1, lift_filter_arr);

			w.lift_filter = tmp;

			// 2. extrapolate
			w.extrapolate_odd.push_back(-2 * S1*S2*S3 / (1 + 2 * S2*S3));
			w.extrapolate_odd.push_back(-2 * S2*S3 / (1 + 2 * S2*S3));
			w.extrapolate_odd.push_back(-2 * (S1 + S3 + 3 * S1*S2*S3) / (1 + 2 * S2*S3));
			this->wp.scale_factor = w.scale_factor;
			this->wp.extrapolate_odd = w.extrapolate_odd;
			w.lift_filter.copyTo(wp.lift_filter);

		}
		else {
			this->wp.extrapolate_odd = w.extrapolate_odd;
			w.lift_filter.copyTo(this->wp.lift_filter);
			this->wp.scale_factor = w.scale_factor;
			this->wp = w;
		}
	}

	//Aplicar transformación necesaria
	Mat WaveletCdf97::Run() {
		if (nlevel >= 0) {
			ForwardTransform();
		}
		else {
			InverseTransform();
		}

		return wavelet_img;
	}

	//Transformación forward
	void WaveletCdf97::ForwardTransform() {
		int N1 = wavelet_img.rows;
		int N2 = wavelet_img.cols;
		int M1 = 0;
		int M2 = 0;

		//---- forward transform ----
		if (nlevel >= 0) {
			for (int k = 0; k < nlevel; k++) {
				M1 = (int)ceil((double)(N1 / 2.0));
				M2 = (int)ceil((double)(N2 / 2.0));
				// transform along columns
				if (N1 > 1) {
					TransferAlongCol(N1, N2, M1, M2, 0);
				}
				// transform along rows
				if (N2 > 1) {
					TransferAlongRow(N1, N2, M1, M2, 0);
				}
				N1 = M1;
				N2 = M2;
			}
		}
	}

	//Transformación inversa
	void WaveletCdf97::InverseTransform() {
		int N1 = wavelet_img.rows;
		int N2 = wavelet_img.cols;
		int M1 = 0;
		int M2 = 0;
		for (int k = 1 + nlevel; k < 1; k++) {
			M1 = (int)ceil(N1*pow(2.0, (double)k));
			M2 = (int)ceil(N2*pow(2.0, (double)k));
			// transform along rows
			if (M2 > 1) {
				TransferAlongRow(N1, N2, M1, M2, 1);
			}
			// transform along cols
			if (M1 > 1) {
				TransferAlongCol(N1, N2, M1, M2, 1);
			}
		}
	}

	//Transferencia a lo largo de columnas
	void WaveletCdf97::TransferAlongCol(const int N1, const int N2, const int M1, const int M2, const bool flag) {
		// for forward transform
		if (flag == 0) {
			Mat X0 = Mat::zeros((N1 - 1) / 2 + 1, N2, CV_64FC1);
			Mat X1 = Mat::zeros((N1 - 1) / 2 + 1, N2, CV_64FC1);
			vector<int> rightShiftIdx;
			for (int i = 1; i < M1; ++i) {
				rightShiftIdx.push_back(i);
			}

			rightShiftIdx.push_back(M1 - 1);


			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					X0.at<double>(i, j) = wavelet_img.at<double>(i * 2, j);
				}
			}

			// apply lifting stages
			Mat X0_rightShift = Mat::zeros(rightShiftIdx.size(), X0.cols, CV_64FC1); // X0(rightShift,:,:)
			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}

			if (N1 % 2 == 1) {
				Mat tmp = filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));
				for (int j = 0; j < X1.cols; j++) {
					for (int i = 0; i < X1.rows - 1; i++) {
						X1.at<double>(i, j) = wavelet_img.at<double>(i * 2 + 1, j);
					}
					X1.at<double>(X1.rows - 1, j) = X0.at<double>(M1 - 2, j)*wp.extrapolate_odd[0]
						+ wavelet_img.at<double>(N1 - 2, j)*wp.extrapolate_odd[1]
						+ X0.at<double>(M1 - 1, j)*wp.extrapolate_odd[2];
				}
				X1 = X1 + tmp;

			}
			else {

				Mat tmp = filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));
				for (int i = 0; i < X1.rows; i++) {
					for (int j = 0; j < X1.cols; j++) {
						X1.at<double>(i, j) = wavelet_img.at<double>(i * 2 + 1, j);
					}
				}
				X1 = X1 + tmp;

			}

			X0 = X0 + filter(wp.lift_filter.col(1), X1, X1.row(0)*wp.lift_filter.at<double>(0, 1));

			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}

			X1 = X1 + filter(wp.lift_filter.col(2), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 2));

			X0 = X0 + filter(wp.lift_filter.col(3), X1, X1.row(0)*wp.lift_filter.at<double>(0, 3));

			Mat new_X1;
			if (N1 % 2 == 1) {
				new_X1 = Mat::zeros(X1.rows - 1, X1.cols, CV_64FC1);
				for (int i = 0; i < new_X1.rows; i++) {
					for (int j = 0; j < new_X1.cols; j++) {
						new_X1.at<double>(i, j) = X1.at<double>(i, j);
					}
				}

			}
			else {
				new_X1 = X1;
			}

			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					wavelet_img.at<double>(i, j) = X0.at<double>(i, j)*wp.scale_factor;
				}
			}

			for (int i = 0; i < new_X1.rows; i++) {
				for (int j = 0; j < new_X1.cols; j++) {
					wavelet_img.at<double>(i + X0.rows, j) = new_X1.at<double>(i, j) / wp.scale_factor;
				}
			}

			// for inverse transform
		}
		else {
			int Q = (int)ceil((double)(M1 / 2.0));
			Mat X0 = Mat::zeros(Q, M2, CV_64FC1);
			Mat X1 = Mat::zeros(M1 - Q, M2, CV_64FC1);
			vector<int> rightShiftIdx;
			for (int i = 1; i < Q; ++i) {
				rightShiftIdx.push_back(i);
			} rightShiftIdx.push_back(Q - 1);

			for (int i = 0; i < X1.rows; i++) {
				for (int j = 0; j < X1.cols; j++) {
					X1.at<double>(i, j) = wavelet_img.at<double>(i + Q, j)*wp.scale_factor;
				}
			}

			Mat add_X1;
			if (M1 % 2 == 1) {
				add_X1 = Mat::zeros(X1.rows + 1, X1.cols, CV_64FC1);
				for (int i = 0; i < X1.rows; i++) {
					for (int j = 0; j < X1.cols; j++) {
						add_X1.at<double>(i, j) = X1.at<double>(i, j);
					}
				}

			}
			else {
				add_X1 = X1;
			}

			// Undo lifting stages
			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					X0.at<double>(i, j) = wavelet_img.at<double>(i, j) / wp.scale_factor;
				}
			}
			X0 = X0 - filter(wp.lift_filter.col(3), add_X1, add_X1.row(0)*wp.lift_filter.at<double>(0, 3));

			Mat X0_rightShift = Mat::zeros(rightShiftIdx.size(), X0.cols, CV_64FC1); // X0(rightShift,:,:)
			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}
			add_X1 = add_X1 - filter(wp.lift_filter.col(2), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 2));
			X0 = X0 - filter(wp.lift_filter.col(1), add_X1, add_X1.row(0)*wp.lift_filter.at<double>(0, 1));

			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}
			add_X1 = add_X1 - filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));

			Mat minus_X1;
			if (M1 % 2 == 1) {
				minus_X1 = Mat::zeros(add_X1.rows - 1, add_X1.cols, CV_64FC1);
				for (int i = 0; i < minus_X1.rows; i++) {
					for (int j = 0; j < minus_X1.cols; j++) {
						minus_X1.at<double>(i, j) = add_X1.at<double>(i, j);
					}
				}
			}
			else {
				minus_X1 = add_X1;
			}

			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					wavelet_img.at<double>(i * 2, j) = X0.at<double>(i, j);
				}
			}
			for (int i = 0; i < minus_X1.rows; i++) {
				for (int j = 0; j < minus_X1.cols; j++) {
					wavelet_img.at<double>(i * 2 + 1, j) = minus_X1.at<double>(i, j);
				}
			}
		}
	}

	//Transferencia a lo largo de filas
	void WaveletCdf97::TransferAlongRow(const int N1, const int N2, const int M1, const int M2, const bool flag) {

		// for forward transform
		if (flag == 0) {
			Mat X0 = Mat::zeros((N2 - 1) / 2 + 1, N1, CV_64FC1);
			Mat X1 = Mat::zeros((N2 - 1) / 2 + 1, N1, CV_64FC1);
			vector<int> rightShiftIdx;
			for (int i = 1; i < M2; ++i) {
				rightShiftIdx.push_back(i);
			} rightShiftIdx.push_back(M2 - 1);

			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					X0.at<double>(i, j) = wavelet_img.at<double>(j, i * 2);
				}
			}

			// apply lifting stages
			Mat X0_rightShift = Mat::zeros(rightShiftIdx.size(), X0.cols, CV_64FC1); // X0(rightShift,:,:)
			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}

			if (N2 % 2 == 1) {
				Mat tmp = wavelet_img(cv::Rect(N2 - 3, 0, 1, N1))*wp.extrapolate_odd[0]
					+ wavelet_img(cv::Rect(N2 - 2, 0, 1, N1))*wp.extrapolate_odd[1]
					+ wavelet_img(cv::Rect(N2 - 1, 0, 1, N1))*wp.extrapolate_odd[2];

				for (int j = 0; j < X1.cols; j++) {
					for (int i = 0; i < X1.rows - 1; i++) {
						X1.at<double>(i, j) = wavelet_img.at<double>(j, i * 2 + 1);
					}
					X1.at<double>(X1.rows - 1, j) = tmp.at<double>(j, 0);
				}
				X1 = X1 + filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));

			}
			else {
				for (int i = 0; i < X1.rows; i++) {
					for (int j = 0; j < X1.cols; j++) {
						X1.at<double>(i, j) = wavelet_img.at<double>(j, i * 2 + 1);
					}
				}
				X1 = X1 + filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));
			}

			X0 = X0 + filter(wp.lift_filter.col(1), X1, X1.row(0)*wp.lift_filter.at<double>(0, 1));

			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}
			X1 = X1 + filter(wp.lift_filter.col(2), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 2));

			X0 = X0 + filter(wp.lift_filter.col(3), X1, X1.row(0)*wp.lift_filter.at<double>(0, 3));

			Mat new_X1;
			if (N2 % 2 == 1) {
				new_X1 = Mat::zeros(X1.rows - 1, X1.cols, CV_64FC1);
				for (int i = 0; i < new_X1.rows; i++) {
					for (int j = 0; j < new_X1.cols; j++) {
						new_X1.at<double>(i, j) = X1.at<double>(i, j);
					}
				}

			}
			else {
				new_X1 = X1;
			}

			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					wavelet_img.at<double>(j, i) = X0.at<double>(i, j)*wp.scale_factor;
				}
			}


			for (int i = 0; i < new_X1.rows; i++) {
				for (int j = 0; j < new_X1.cols; j++) {
					wavelet_img.at<double>(j, i + X0.rows) = new_X1.at<double>(i, j) / wp.scale_factor;
				}
			}

			// for inverse transform
		}
		else {
			int Q = (int)ceil((double)(M2 / 2.0));
			Mat X0 = Mat::zeros(Q, M1, CV_64FC1);
			Mat X1 = Mat::zeros(M2 - Q, M1, CV_64FC1);
			vector<int> rightShiftIdx;
			for (int i = 1; i < Q; ++i) {
				rightShiftIdx.push_back(i);
			} rightShiftIdx.push_back(Q - 1);

			for (int i = 0; i < X1.rows; i++) {
				for (int j = 0; j < X1.cols; j++) {
					X1.at<double>(i, j) = wavelet_img.at<double>(j, i + Q)*wp.scale_factor;
				}
			}

			Mat add_X1;
			if (M2 % 2 == 1) {
				add_X1 = Mat::zeros(X1.rows + 1, X1.cols, CV_64FC1);
				for (int i = 0; i < X1.rows; i++) {
					for (int j = 0; j < X1.cols; j++) {
						add_X1.at<double>(i, j) = X1.at<double>(i, j);
					}
				}

			}
			else {
				add_X1 = X1;
			}

			// Undo lifting stages
			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					X0.at<double>(i, j) = wavelet_img.at<double>(j, i) / wp.scale_factor;
				}
			}
			X0 = X0 - filter(wp.lift_filter.col(3), add_X1, add_X1.row(0)*wp.lift_filter.at<double>(0, 3));

			Mat X0_rightShift = Mat::zeros(rightShiftIdx.size(), X0.cols, CV_64FC1); // X0(rightShift,:,:)
			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}
			add_X1 = add_X1 - filter(wp.lift_filter.col(2), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 2));
			X0 = X0 - filter(wp.lift_filter.col(1), add_X1, add_X1.row(0)*wp.lift_filter.at<double>(0, 1));

			for (int i = 0; i < X0_rightShift.rows; i++) {
				for (int j = 0; j < X0_rightShift.cols; j++) {
					X0_rightShift.at<double>(i, j) = X0.at<double>(rightShiftIdx[i], j);
				}
			}
			add_X1 = add_X1 - filter(wp.lift_filter.col(0), X0_rightShift, X0.row(0)*wp.lift_filter.at<double>(0, 0));

			Mat minus_X1;
			if (M2 % 2 == 1) {
				minus_X1 = Mat::zeros(add_X1.rows - 1, add_X1.cols, CV_64FC1);
				for (int i = 0; i < minus_X1.rows; i++) {
					for (int j = 0; j < minus_X1.cols; j++) {
						minus_X1.at<double>(i, j) = add_X1.at<double>(i, j);
					}
				}
			}
			else {
				minus_X1 = add_X1;
			}

			for (int i = 0; i < X0.rows; i++) {
				for (int j = 0; j < X0.cols; j++) {
					wavelet_img.at<double>(j, i * 2) = X0.at<double>(i, j);
				}
			}

			for (int i = 0; i < minus_X1.rows; i++) {
				for (int j = 0; j < minus_X1.cols; j++) {
					wavelet_img.at<double>(j, i * 2 + 1) = minus_X1.at<double>(i, j);
				}
			}
		}
	}

	//Filtrado
	Mat WaveletCdf97::filter(const Mat& B, const Mat& X, const Mat& Zi) {
		// b is a column vector
		Mat b;
		flip(B, b, 0);
		int padding = b.rows - 1;
		Mat expendX = Mat::zeros(X.rows + padding, X.cols, CV_64FC1);
		Mat Y = Mat::zeros(X.rows, X.cols, CV_64FC1);

		// fill the expend X matrix
		for (int i = 0; i < X.rows; i++) {
			for (int j = 0; j < X.cols; j++) {
				expendX.at<double>(i + padding, j) = X.at<double>(i, j);
			}
		}

		// convolution operation
		for (int c = 0; c < expendX.cols; ++c) {
			for (int r = 0; r < expendX.rows - padding; ++r) {
				for (int i = 0; i < b.rows; i++) {
					Y.at<double>(r, c) += b.at<double>(i, 0)*expendX.at<double>(r + i, c);
				}
			}
			// Zi is a row vector (initial delays)
			Y.at<double>(0, c) += Zi.at<double>(0, c);
		}

		return Y;
	}

	//Constructor del interpolador interno
	Interpolator::Interpolator(const Mat& src, int width, int height) {
		Mat src64;
		src.convertTo(src64, CV_64FC1);
		src64.copyTo(this->src);
		this->width = width;
		this->height = height;
	}

	//Realizar interpolación bicúbica
	Mat Interpolator::BicubicInterpolate() {
		Mat dst = Mat::zeros(height, width, CV_64FC1);
		Point2d p;

		for (int r = 0; r < dst.rows; r++) {
			for (int c = 0; c < dst.cols; c++) {
				p.x = c;
				p.y = r;
				dst.at<double>(r, c) = GetColor(p);
			}
		}
		return dst;
	}

	//Obtener color
	double Interpolator::GetColor(const Point2d& p) {
		double a = -0.5; // bicubic coefficient
		Point2d pivot;
		int x_start = 0;
		int y_start = 0;
		int   x_end = 0;
		int   y_end = 0;

		pivot.x = (src.cols - 1) / (width - 1)*p.x;
		pivot.y = (src.rows - 1) / (height - 1)*p.y;

		// if pivot.x is not an integer
		if ((floor(pivot.x) - ceil(pivot.x)) > DBL_MIN) {
			x_start = (int)floor(pivot.x) - 1;
			x_end = (int)ceil(pivot.x) + 1;
		}
		else {
			x_start = (int)pivot.x - 1;
			x_end = (int)pivot.x + 2;
		}

		// if pivot.y is not an integer
		if ((floor(pivot.y) - ceil(pivot.y)) > DBL_MIN) {
			y_start = (int)floor(pivot.y) - 1;
			y_end = (int)ceil(pivot.y) + 1;
		}
		else {
			y_start = (int)pivot.y - 1;
			y_end = (int)pivot.y + 2;
		}

		// get neighbors': corrdinate, weight, color
		Point5d neighbor;
		double x_diff, y_diff;
		for (int i = x_start; i <= x_end; i++) {
			for (int j = y_start; j <= y_end; j++) {
				neighbor.x = i;
				neighbor.y = j;
				x_diff = std::abs(pivot.x - neighbor.x);
				y_diff = std::abs(pivot.y - neighbor.y);

				// compute weight of x direction
				if (x_diff <= 1.0) {
					neighbor.xw = (a + 2)*pow(x_diff, 3) - (a + 3)*pow(x_diff, 2) + 1;
				}
				else if (1.0 < x_diff && x_diff < 2.0) {
					neighbor.xw = a*pow(x_diff, 3) - 5 * a*pow(x_diff, 2) + 8 * a*x_diff - 4 * a;
				}
				else {
					neighbor.xw = 0;
				}

				// compute weight of y direction
				if (y_diff <= 1.0) {
					neighbor.yw = (a + 2)*pow(y_diff, 3) - (a + 3)*pow(y_diff, 2) + 1;
				}
				else if (1.0 < y_diff && y_diff < 2.0) {
					neighbor.yw = a*pow(y_diff, 3) - 5 * a*pow(y_diff, 2) + 8 * a*y_diff - 4 * a;
				}
				else {
					neighbor.yw = 0;
				}

				double index_x = 0.0;
				double index_y = 0.0;
				// get color of each neighbor
				if (neighbor.x < 0) {
					index_x = -neighbor.x - 1;
				}
				else if (neighbor.x >= 0 && neighbor.x < src.cols - 1) {
					index_x = neighbor.x;
				}
				else {
					index_x = 2 * src.cols - 2 - neighbor.x;
				}

				if (neighbor.y < 0) {
					index_y = -neighbor.y - 1;
				}
				else if (neighbor.y >= 0 && neighbor.y < src.rows - 1) {
					index_y = neighbor.y;
				}
				else {
					index_y = 2 * src.rows - 2 - neighbor.y;
				}

				neighbor.color = src.at<double>(index_y, index_x);
				neighbors.push_back(neighbor);
			}
		}

		double color = 0.0;
		for (int i = 0; i < neighbors.size(); i++) {
			color += neighbors[i].xw*neighbors[i].yw*neighbors[i].color;
		}

		neighbors.clear();
		return color;
	}

	//Nombre del reconocedor
	std::string WaveletSpatialSRUpsampler::getName() const
	{
		return "Wavelet Spatial";
	}
}