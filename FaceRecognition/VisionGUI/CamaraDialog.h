#pragma once
#include "afxwin.h"
#include <opencv2/opencv.hpp>


// Cuadro de di�logo de CamaraDialog

class CamaraDialog : public CDialog
{
	DECLARE_DYNAMIC(CamaraDialog)

public:
	CamaraDialog(CWnd* pParent = NULL);   // Constructor est�ndar
	virtual ~CamaraDialog();

// Datos del cuadro de di�logo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMARADIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

	//Captura de v�deo
	cv::VideoCapture capture;
	//Se ha seleccionado c�mara
	bool cameraSelected = false;

	DECLARE_MESSAGE_MAP()
public:

	//Comprobar si se ha seleccionado c�mara
	bool isCameraSelected() const;
	//Obtener captura de v�deo
	cv::VideoCapture& getVideoCapture();

	CEdit UrlCamaraIpStr;
	afx_msg void OnBnClickedButtonLoadIpcamera();
	afx_msg void OnBnClickedButtonLoadLocalCamera();
	CComboBox comboboxCameraLocal;
};
