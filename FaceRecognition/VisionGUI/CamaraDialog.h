#pragma once
#include "afxwin.h"
#include <opencv2/opencv.hpp>


// Cuadro de diálogo de CamaraDialog

class CamaraDialog : public CDialog
{
	DECLARE_DYNAMIC(CamaraDialog)

public:
	CamaraDialog(CWnd* pParent = NULL);   // Constructor estándar
	virtual ~CamaraDialog();

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMARADIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

	//Captura de vídeo
	cv::VideoCapture capture;
	//Se ha seleccionado cámara
	bool cameraSelected = false;

	DECLARE_MESSAGE_MAP()
public:

	//Comprobar si se ha seleccionado cámara
	bool isCameraSelected() const;
	//Obtener captura de vídeo
	cv::VideoCapture& getVideoCapture();

	CEdit UrlCamaraIpStr;
	afx_msg void OnBnClickedButtonLoadIpcamera();
	afx_msg void OnBnClickedButtonLoadLocalCamera();
	CComboBox comboboxCameraLocal;
};
