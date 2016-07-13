
// VisionGUIDlg.cpp: archivo de implementación
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

//Método para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShow(const cv::Mat &img, CBitmap& output);
//Método para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShowV2(const cv::Mat &img, CBitmap& output);
//Destruir caras encontradas
void closeFaceWindows();

//Imagen cargada en memoria
cv::Mat imgCargada;
//Nombre base de las ventanas
const std::string windowBaseName = "Cara ";
//Detector de caras
tfg::HaarLikeFaceDetector faceDetector("sources/haarlike/haarcascade_frontalface_alt.xml");
//Upsampler
tfg::SimpleImageUpsampler upSampler(cv::InterpolationFlags::INTER_LANCZOS4);
//Reconocedor de caras
tfg::IFaceRecognizer *faceRecognizer;
//Caras encontradas
std::vector<cv::Mat> colourFoundFaces;

// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementación
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


// Cuadro de diálogo de CVisionGUIDlg



CVisionGUIDlg::CVisionGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VISIONGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisionGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVisionGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(LOADIMG_BUTTON, &CVisionGUIDlg::OnLoadImageClickedButton)
	ON_BN_CLICKED(PROCESSIMG_BUTTON, &CVisionGUIDlg::OnProcesarImagenClickedButton)
	ON_BN_CLICKED(SHOW_FACES_BUTTON, &CVisionGUIDlg::OnMostrarCarasClickedFacesButton)
	ON_BN_CLICKED(IDC_BUTTON_OCULTAR_CARAS, &CVisionGUIDlg::OnClickedButtonOcultarCaras)
END_MESSAGE_MAP()


// Controladores de mensaje de CVisionGUIDlg

BOOL CVisionGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Agregar el elemento de menú "Acerca de..." al menú del sistema.

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

	// Establecer el icono para este cuadro de diálogo.  El marco de trabajo realiza esta operación
	//  automáticamente cuando la ventana principal de la aplicación no es un cuadro de diálogo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono pequeño

	// TODO: agregar aquí inicialización adicional

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

// Si agrega un botón Minimizar al cuadro de diálogo, necesitará el siguiente código
//  para dibujar el icono.  Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operación la realiza automáticamente el marco de trabajo.

void CVisionGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Contexto de dispositivo para dibujo

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrar icono en el rectángulo de cliente
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

// El sistema llama a esta función para obtener el cursor que se muestra mientras el usuario arrastra
//  la ventana minimizada.
HCURSOR CVisionGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Clicado en el botón de carga de imagen
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
		CString path = dlg.GetPathName();
		// Convertir path a formato string normal
		CT2CA pszConvertedAnsiString(path);
		// construct a std::string using the LPCSTR input
		std::string pathStr(pszConvertedAnsiString);

		//Cargar imagen
		imgCargada = cv::imread(pathStr);

		//Limpiar las caras detectadas
		colourFoundFaces.clear();
		//Preparar imagen para mostrar
		CBitmap bitmap;
		//Convertir imagen
		prepareImgToShowV2(imgCargada, bitmap);

		//Mostrar imagen
		CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
		pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
		pictureControl->SetBitmap(bitmap);
		UpdateData(FALSE);
	}
}

//Método para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShow(const cv::Mat &img, CBitmap& output)
{
	//Imagen final para mostrado
	cv::Mat imgFinal;

	//Crear downsampler
	tfg::ImageDownsampler downsampler;
	//Bajar de resolución la imagen si es necesario
	if (img.size().width > IMG_WIDTH || img.size().height > IMG_HEIGHT)
	{
		//Ajustar tamaños
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

	//Rotar la imagen
	cv::flip(imgFinal, imgFinal, 0);

	//Convertir imagen a formato de salida
	CImage* m_pImg = new CImage();
	m_pImg->Create(imgFinal.size().width, imgFinal.size().height, 24);
	//Información de mapa de bits
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biWidth = imgFinal.size().width;
	bitInfo.bmiHeader.biHeight = imgFinal.size().height;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant =
		bitInfo.bmiHeader.biClrUsed =
		bitInfo.bmiHeader.biSizeImage =
		bitInfo.bmiHeader.biXPelsPerMeter =
		bitInfo.bmiHeader.biYPelsPerMeter = 0;
	//Pasar la información
	StretchDIBits(m_pImg->GetDC(), 0, 0,
		imgFinal.size().width, imgFinal.size().height, 0, 0,
		imgFinal.size().width, imgFinal.size().height,
		imgFinal.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	//Preparar imagen para mostrar
	m_pImg->ReleaseDC();
	output.Attach(m_pImg->Detach());

	//Liberar objeto
	delete m_pImg;
}

//Método para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShowV2(const cv::Mat &img, CBitmap& output)
{
	//Imagen final para mostrado
	cv::Mat imgFinal;

	//Crear downsampler
	tfg::ImageDownsampler downsampler;
	//Bajar de resolución la imagen si es necesario
	if (img.size().width > IMG_WIDTH || img.size().height > IMG_HEIGHT)
	{
		//Ajustar tamaños
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

	//Conversión a CImage
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
	//Detectar caras
	faceDetector.detectFaces(imgCargada, grayFoundFaces, colourFoundFaces, imgFinal, 1.05, 16, 16);

	//Preparar imagen para mostrar
	CBitmap bitmap;
	//Convertir imagen
	prepareImgToShowV2(imgFinal, bitmap);

	//Mostrar imagen
	CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
	pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	pictureControl->SetBitmap(bitmap);
	UpdateData(FALSE);
}

//Botón para mostrar caras
void CVisionGUIDlg::OnMostrarCarasClickedFacesButton()
{
	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resolución al tamaño indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		const cv::Mat &face = colourFoundFaces[i];
		//Ajustar cara al tamaño indicado
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
