// CamaraDialog.cpp: archivo de implementaci�n
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "CamaraDialog.h"
#include "afxdialogex.h"


// Cuadro de di�logo de CamaraDialog

IMPLEMENT_DYNAMIC(CamaraDialog, CDialog)

CamaraDialog::CamaraDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CAMARADIALOG, pParent)
{
}

BOOL CamaraDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Inicializaci�n
	//Cargar c�maras locales
	cv::VideoCapture video;
	for (int i = 0; ; ++i)
	{
		//Abrir c�mara local
		video.open(i);
		//Comprobar si se ha abierto
		if (video.isOpened())
		{
			//A�adir entrada a la lista
			comboboxCameraLocal.AddString(std::to_string(i).c_str());
			//Liberar c�mara
			video.release();
		}
		else
			break;
	}

	//Establecer primer �ndice como seleccionado
	if (comboboxCameraLocal.GetCount() > 0)
		comboboxCameraLocal.SetCurSel(0);

	return TRUE;
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

//Seleccionada c�mara ip
void CamaraDialog::OnBnClickedButtonLoadIpcamera()
{
	//Marcar como c�mara seleccionada
	this->cameraSelected = true;

	//Convertir cstring a string
	CString valor;
	GetDlgItemText(IDC_CAMARA_IP, valor);
	// Convertir cstring a formato string normal
	CT2CA pszConvertedAnsiString(valor);
	// construct a std::string using the LPCSTR input
	std::string urlCamaraIp(pszConvertedAnsiString);

	//Mostrar mensaje de selecci�n
	AfxMessageBox(_T("Se ha seleccionado conexi�n a c�mara ip"), MB_OK | MB_ICONINFORMATION);

	//Cargar c�mara ip
	capture.open(urlCamaraIp);

	//Cerrar di�logo
	this->EndDialog(1);
}

//Seleccionada c�mara local
void CamaraDialog::OnBnClickedButtonLoadLocalCamera()
{
	//Marcar como c�mara seleccionada
	this->cameraSelected = true;

	//Obtener �ndice de la c�mara
	int index = comboboxCameraLocal.GetCurSel();

	//Mostrar mensaje de selecci�n
	AfxMessageBox(_T("Se ha seleccionado conexi�n a c�mara local"), MB_OK | MB_ICONINFORMATION);

	//Inicializar captura con el �ndice
	capture.open(index);

	//Cerrar di�logo
	this->EndDialog(1);
}

//Comprobar si se ha seleccionado c�mara
bool CamaraDialog::isCameraSelected() const
{
	return this->cameraSelected;
}

//Obtener captura de v�deo
cv::VideoCapture& CamaraDialog::getVideoCapture()
{
	return this->capture;
}
