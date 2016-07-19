// CamaraDialog.cpp: archivo de implementación
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "CamaraDialog.h"
#include "afxdialogex.h"


// Cuadro de diálogo de CamaraDialog

IMPLEMENT_DYNAMIC(CamaraDialog, CDialog)

CamaraDialog::CamaraDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CAMARADIALOG, pParent)
{

}

CamaraDialog::~CamaraDialog()
{
}

void CamaraDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, UrlCamaraIpStr);
	DDX_Control(pDX, IDC_CAMARA_LOCAL, comboboxCameraLocal);
}


BEGIN_MESSAGE_MAP(CamaraDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IPCAMERA, &CamaraDialog::OnBnClickedButtonLoadIpcamera)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_LOCAL_CAMERA, &CamaraDialog::OnBnClickedButtonLoadLocalCamera)
END_MESSAGE_MAP()


// Controladores de mensajes de CamaraDialog

//Seleccionada cámara ip
void CamaraDialog::OnBnClickedButtonLoadIpcamera()
{
	//Marcar como cámara seleccionada
	this->cameraSelected = true;

	//Convertir cstring a string
	CString valor;
	GetDlgItemText(IDC_CAMARA_IP, valor);
	// Convertir cstring a formato string normal
	CT2CA pszConvertedAnsiString(valor);
	// construct a std::string using the LPCSTR input
	std::string urlCamaraIp(pszConvertedAnsiString);

	//Cargar cámara ip
	capture.open(urlCamaraIp);

	//Cerrar diálogo
	this->EndDialog(1);
}

//Seleccionada cámara local
void CamaraDialog::OnBnClickedButtonLoadLocalCamera()
{
	//Marcar como cámara seleccionada
	this->cameraSelected = true;

	//Obtener índice de la cámara
	int index = comboboxCameraLocal.GetCurSel();

	//Inicializar captura con el índice
	capture.open(index);

	//Cerrar diálogo
	this->EndDialog(1);
}

//Comprobar si se ha seleccionado cámara
bool CamaraDialog::isCameraSelected() const
{
	return this->cameraSelected;
}

//Obtener captura de vídeo
cv::VideoCapture& CamaraDialog::getVideoCapture()
{
	return this->capture;
}
