
// VisionGUIDlg.cpp: archivo de implementaci�n
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "VisionGUIDlg.h"
#include "afxdialogex.h"
#include "ImageDownsampler.h"
#include "HaarLikeFaceDetector.h"
#include "SimpleImageUpsampler.h"
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_HEIGHT 400
#define IMG_WIDTH 500
#define FACE_HEIGHT 128
#define FACE_WIDTH 92
#define FACES_X_ROW 10

//M�todo para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShow(const cv::Mat &img, CBitmap& output);
//Destruir caras encontradas
void closeFaceWindows();
//Convertir CString en String
std::string cStringToString(const CString& str);

//Imagen cargada en memoria
cv::Mat imgCargada;
//Nombre base de las ventanas
const std::string windowBaseName = "Cara ";
//Nombre del fichero de detecci�n de caras
const std::string ficFaceDetector = "sources/haarlike/haarcascade_frontalface_alt.xml";
//Detector de caras
tfg::HaarLikeFaceDetector faceDetector(ficFaceDetector);
//Upsampler
tfg::SimpleImageUpsampler upSampler(cv::InterpolationFlags::INTER_LANCZOS4);
//Reconocedor de caras
tfg::IFaceRecognizer *faceRecognizer;
//Caras encontradas
std::vector<cv::Mat> colourFoundFaces;
//Escala de detecci�n
float escalaDeteccion = 1.05f;
//Anchura y altura m�nima de detecci�n cara
int anchuraMinFace = 16, anchuraMaxFace = 0, alturaMinFace = 16, alturaMaxFace = 0;

// Cuadro de di�logo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Datos del cuadro de di�logo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementaci�n
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cuadro de di�logo de CVisionGUIDlg



CVisionGUIDlg::CVisionGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VISIONGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisionGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ESCALA, escalaString);
	DDX_Control(pDX, IDC_EDIT_MIN_WIDTH_FACE, anchuraMinString);
	DDX_Control(pDX, IDC_EDIT_MIN_HEIGHT_FACE, alturaMinString);
	DDX_Control(pDX, IDC_EDIT_MAX_WIDTH_FACE, anchuraMaxString);
	DDX_Control(pDX, IDC_EDIT_MAX_HEIGHT_FACE, AlturaMaxString);
}

BEGIN_MESSAGE_MAP(CVisionGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(LOADIMG_BUTTON, &CVisionGUIDlg::OnLoadImageClickedButton)
	ON_BN_CLICKED(PROCESSIMG_BUTTON, &CVisionGUIDlg::OnProcesarImagenClickedButton)
	ON_BN_CLICKED(SHOW_FACES_BUTTON, &CVisionGUIDlg::OnMostrarCarasClickedFacesButton)
	ON_BN_CLICKED(IDC_BUTTON_OCULTAR_CARAS, &CVisionGUIDlg::OnClickedButtonOcultarCaras)
	ON_EN_CHANGE(IDC_EDIT_ESCALA, &CVisionGUIDlg::OnEnChangeEditEscala)
	ON_EN_CHANGE(IDC_EDIT_MIN_WIDTH_FACE, &CVisionGUIDlg::OnEnChangeEditMinWidthFace)
	ON_EN_CHANGE(IDC_EDIT_MIN_HEIGHT_FACE, &CVisionGUIDlg::OnEnChangeEditMinHeightFace)
	ON_EN_CHANGE(IDC_EDIT_MAX_WIDTH_FACE, &CVisionGUIDlg::OnEnChangeEditMaxWidthFace)
	ON_EN_CHANGE(IDC_EDIT_MAX_HEIGHT_FACE, &CVisionGUIDlg::OnEnChangeEditMaxHeightFace)
END_MESSAGE_MAP()


// Controladores de mensaje de CVisionGUIDlg

BOOL CVisionGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Agregar el elemento de men� "Acerca de..." al men� del sistema.

	// IDM_ABOUTBOX debe estar en el intervalo de comandos del sistema.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Establecer el icono para este cuadro de di�logo.  El marco de trabajo realiza esta operaci�n
	//  autom�ticamente cuando la ventana principal de la aplicaci�n no es un cuadro de di�logo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono peque�o

	// TODO: agregar aqu� inicializaci�n adicional
	//Inicializar texto de los textboxes
	escalaString.SetWindowText(std::to_string(escalaDeteccion).c_str());
	anchuraMinString.SetWindowText(std::to_string(anchuraMinFace).c_str());
	alturaMinString.SetWindowText(std::to_string(alturaMinFace).c_str());
	anchuraMaxString.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	AlturaMaxString.SetWindowText(std::to_string(alturaMaxFace).c_str());

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}

void CVisionGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Si agrega un bot�n Minimizar al cuadro de di�logo, necesitar� el siguiente c�digo
//  para dibujar el icono.  Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operaci�n la realiza autom�ticamente el marco de trabajo.

void CVisionGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Contexto de dispositivo para dibujo

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrar icono en el rect�ngulo de cliente
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dibujar el icono
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// El sistema llama a esta funci�n para obtener el cursor que se muestra mientras el usuario arrastra
//  la ventana minimizada.
HCURSOR CVisionGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Clicado en el bot�n de carga de imagen
void CVisionGUIDlg::OnLoadImageClickedButton()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	int result = dlg.DoModal();
	//Comprobar que se ha cargado la imagen
	if (result == IDOK)
	{
		//Destruir las ventanas de caras
		closeFaceWindows();

		//Obtener path y panel de la imagen
		std::string pathStr = cStringToString(dlg.GetPathName());

		//Cargar imagen
		imgCargada = cv::imread(pathStr);

		//Limpiar las caras detectadas
		colourFoundFaces.clear();
		//Preparar imagen para mostrar
		CBitmap bitmap;
		//Convertir imagen
		prepareImgToShow(imgCargada, bitmap);

		//Mostrar imagen
		CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
		pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
		pictureControl->SetBitmap(bitmap);
		UpdateData(FALSE);
	}
}

//M�todo para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShow(const cv::Mat &img, CBitmap& output)
{
	//Imagen final para mostrado
	cv::Mat imgFinal;

	//Crear downsampler
	tfg::ImageDownsampler downsampler;
	//Bajar de resoluci�n la imagen si es necesario
	if (img.size().width > IMG_WIDTH || img.size().height > IMG_HEIGHT)
	{
		//Ajustar tama�os
		int width = std::min(img.size().width, IMG_WIDTH);
		int height = std::min(img.size().height, IMG_HEIGHT);
		//Realizar downsampling
		cv::Mat downsampled;
		downsampler.downSampleWithNoNoise(img, downsampled, height, width, cv::InterpolationFlags::INTER_AREA);
		//Asignar a la imagen original
		imgFinal = downsampled.clone();
	}
	else
	{
		imgFinal = img.clone();
	}

	//Conversi�n a CImage
	CImage m_pImg;
	int nBPP = imgFinal.channels() * 8;
	m_pImg.Create(imgFinal.cols, imgFinal.rows, nBPP);

	//Paleta de colores
	if (nBPP == 8)
	{
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		m_pImg.SetColorTable(0, 256, pRGB);
	}

	uchar* psrc = imgFinal.data;
	uchar* pdst = (uchar*)m_pImg.GetBits();
	int imgPitch = m_pImg.GetPitch();
	for (int y = 0; y < imgFinal.rows; y++)
	{
		memcpy(pdst, psrc, imgFinal.cols*imgFinal.channels());//mat->step is incorrect for those images created by roi (sub-images!)
		psrc += imgFinal.step;
		pdst += imgPitch;
	}

	//Preparar imagen para mostrar
	output.Attach(m_pImg.Detach());

	//Liberar objeto
	delete m_pImg;
}

//Evento para procesar la imagen
void CVisionGUIDlg::OnProcesarImagenClickedButton()
{
	//Comprobar si hay alguna imagen cargada
	if (imgCargada.empty())
		return;

	//Destruir las ventanas de caras
	closeFaceWindows();

	//Imagen resultado
	cv::Mat imgFinal;
	//Limpiar caras a color
	colourFoundFaces.clear();
	//Caras grises detectadas
	std::vector<cv::Mat> grayFoundFaces;

	try
	{
		//Detectar caras
		faceDetector.detectFaces(imgCargada, grayFoundFaces, colourFoundFaces, imgFinal,
			escalaDeteccion, anchuraMinFace, alturaMinFace, anchuraMaxFace, alturaMaxFace);
	}
	catch (...)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Memoria Insuficiente"), MB_OK | MB_ICONSTOP);
		//Renovar detector de caras
		faceDetector = tfg::HaarLikeFaceDetector(ficFaceDetector);
		//La imagen final es la original cargada
		imgFinal = imgCargada.clone();
	}

	//Preparar imagen para mostrar
	CBitmap bitmap;
	//Convertir imagen
	prepareImgToShow(imgFinal, bitmap);

	//Mostrar imagen
	CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
	pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	pictureControl->SetBitmap(bitmap);
	UpdateData(FALSE);
}

//Bot�n para mostrar caras
void CVisionGUIDlg::OnMostrarCarasClickedFacesButton()
{
	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resoluci�n al tama�o indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		const cv::Mat &face = colourFoundFaces[i];
		//Ajustar cara al tama�o indicado
		cv::Mat finalFace;
		upSampler.upSample(face, finalFace, FACE_HEIGHT, FACE_WIDTH);
		//Mostrar cara
		std::string windowName = windowBaseName + std::to_string(i);
		cv::imshow(windowName, finalFace);
		cv::moveWindow(windowName, i%FACES_X_ROW*(FACE_WIDTH+85), i / FACES_X_ROW * (FACE_HEIGHT+47));
		cv::waitKey(20);
	}
}

//Destruir caras encontradas
void closeFaceWindows()
{
	//Limpiar las ventanas de las caras encontradas
	cv::destroyAllWindows();
}

//Ocultar caras
void CVisionGUIDlg::OnClickedButtonOcultarCaras()
{
	closeFaceWindows();
}

//Cambio de escala
void CVisionGUIDlg::OnEnChangeEditEscala()
{
	//Obtener el texto de escala
	CString escala;
	GetDlgItemText(IDC_EDIT_ESCALA, escala);

	//Convertir a string convencional
	std::string escalaStr = cStringToString(escala);
	//Intentar convertir a float
	float nuevaEscala;

	try
	{
		//Convertir nueva escala
		nuevaEscala = std::stof(escalaStr);
		//Pasarla a la variable de escala de detecci�n
		escalaDeteccion = nuevaEscala;

	}
	catch (...)
	{
		//Reestablecer el texto
		escalaString.SetWindowText(std::to_string(escalaDeteccion).c_str());
	}
}

//Convertir CString en String
std::string cStringToString(const CString& str)
{
	// Convertir cstring a formato string normal
	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	return std::string(pszConvertedAnsiString);
}

//Cambio de anchura m�nima de cara
void CVisionGUIDlg::OnEnChangeEditMinWidthFace()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MIN_WIDTH_FACE, valor);

	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);
	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		anchuraMinFace = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		anchuraMinString.SetWindowText(std::to_string(anchuraMinFace).c_str());
	}
}

//Cambio de altura m�nima de cara
void CVisionGUIDlg::OnEnChangeEditMinHeightFace()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MIN_HEIGHT_FACE, valor);

	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);
	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		alturaMinFace = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		alturaMinString.SetWindowText(std::to_string(alturaMinFace).c_str());
	}
}

//Cambio de anchura m�xima de cara
void CVisionGUIDlg::OnEnChangeEditMaxWidthFace()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MAX_WIDTH_FACE, valor);

	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);
	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		anchuraMaxFace = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		anchuraMaxString.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	}
}

//Cambio de altura m�xima de cara
void CVisionGUIDlg::OnEnChangeEditMaxHeightFace()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MAX_HEIGHT_FACE, valor);

	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);
	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		alturaMaxFace = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		AlturaMaxString.SetWindowText(std::to_string(alturaMaxFace).c_str());
	}
}
