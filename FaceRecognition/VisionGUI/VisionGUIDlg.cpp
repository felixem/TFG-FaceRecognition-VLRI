
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
#include "SimpleImageUpsampler.h"
#include "CompleteFaceRecognizer.h"
#include <future>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_HEIGHT 400
#define IMG_WIDTH 500
#define FACE_HEIGHT 128
#define FACE_WIDTH 92
#define FACES_X_ROW 10

//Modo de procesamiento
enum MODO_PROCESAMIENTO {IMAGEN, VIDEO, CAMARA};

//Método para convertir una imagen de opencv en una imagen para mostrar
void prepareImgToShow(const cv::Mat &img, CBitmap& output);
//Destruir caras encontradas
void closeFaceWindows();
//Convertir CString en String
std::string cStringToString(const CString& str);
//Mostrar imagen en pictureControl
void showImage(const cv::Mat& img, CStatic* pictureControl);
//Generar algoritmo de reconocimiento según índice seleccionado
tfg::IFaceRecognizer* generateRecognizer(int id);
//Generar algoritmo de upsampling según índice seleccionado
tfg::ImageUpsampler* generateUpsampler(int id);

//Imagen cargada en memoria
cv::Mat imgCargada;
//Videocaptura en memoria
cv::VideoCapture videoCaptura;
//Nombre del fichero de detección de caras
const std::string ficFaceDetector = "sources/haarlike/haarcascade_frontalface_alt.xml";

//Caras encontradas
std::vector<tfg::Face> colourFoundFaces;
//Escala de detección
float escalaDeteccion = 1.05f;
//Anchura y altura mínima de detección cara
int anchuraMinFace = 16, anchuraMaxFace = 0, alturaMinFace = 16, alturaMaxFace = 0;
//Anchura y altura para reconocimiento de cara
int anchuraReconocimiento = 64, alturaReconocimiento = 64;
//Umbral de reconocimiento
float umbralReconocimiento = std::numeric_limits<float>::max();
//Integración del reconocedor de caras
tfg::CompleteFaceRecognizer faceRecognizer;

//Modo de procesamiento actual
MODO_PROCESAMIENTO modo = IMAGEN;

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
	DDX_Control(pDX, IDC_EDIT_ESCALA, escalaString);
	DDX_Control(pDX, IDC_EDIT_MIN_WIDTH_FACE, anchuraMinString);
	DDX_Control(pDX, IDC_EDIT_MIN_HEIGHT_FACE, alturaMinString);
	DDX_Control(pDX, IDC_EDIT_MAX_WIDTH_FACE, anchuraMaxString);
	DDX_Control(pDX, IDC_EDIT_MAX_HEIGHT_FACE, AlturaMaxString);
	DDX_Control(pDX, IDC_COMBO_RECOGNIZER, comboboxRecognizer);
	DDX_Control(pDX, IDC_COMBO_UPSAMPLER, comboboxUpsampler);
	DDX_Control(pDX, IDC_EDIT_UMBRAL, umbralReconocimientoString);
}

BEGIN_MESSAGE_MAP(CVisionGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(LOADIMG_BUTTON, &CVisionGUIDlg::OnLoadImageClickedButton)
	ON_BN_CLICKED(PROCESSIMG_BUTTON, &CVisionGUIDlg::OnProcesarImagenClickedButton)
	ON_BN_CLICKED(SHOW_FACES_RECOGNIZED_BUTTON, &CVisionGUIDlg::OnMostrarCarasReconocidasClickedFacesButton)
	ON_BN_CLICKED(IDC_BUTTON_OCULTAR_CARAS, &CVisionGUIDlg::OnClickedButtonOcultarCaras)
	ON_EN_CHANGE(IDC_EDIT_ESCALA, &CVisionGUIDlg::OnEnChangeEditEscala)
	ON_EN_CHANGE(IDC_EDIT_MIN_WIDTH_FACE, &CVisionGUIDlg::OnEnChangeEditMinWidthFace)
	ON_EN_CHANGE(IDC_EDIT_MIN_HEIGHT_FACE, &CVisionGUIDlg::OnEnChangeEditMinHeightFace)
	ON_EN_CHANGE(IDC_EDIT_MAX_WIDTH_FACE, &CVisionGUIDlg::OnEnChangeEditMaxWidthFace)
	ON_EN_CHANGE(IDC_EDIT_MAX_HEIGHT_FACE, &CVisionGUIDlg::OnEnChangeEditMaxHeightFace)
	ON_CBN_SELCHANGE(IDC_COMBO_RECOGNIZER, &CVisionGUIDlg::OnCbnSelchangeComboRecognizer)
	ON_CBN_SELCHANGE(IDC_COMBO_UPSAMPLER, &CVisionGUIDlg::OnCbnSelchangeComboUpsampler)
	ON_BN_CLICKED(IDC_BUTTON_LOADMODEL, &CVisionGUIDlg::OnBnClickedButtonLoadmodel)
	ON_EN_CHANGE(IDC_EDIT_UMBRAL, &CVisionGUIDlg::OnEnChangeEditUmbral)
	ON_BN_CLICKED(SHOW_FACES_NOT_RECOGNIZED_BUTTON2, &CVisionGUIDlg::OnBnClickedFacesNotRecognizedButton2)
	ON_BN_CLICKED(IDC_BUTTON_CARGAR_VIDEO, &CVisionGUIDlg::OnBnClickedButtonCargarVideo)
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
	//Inicializar selección de comboboxes
	comboboxRecognizer.SetCurSel(0);
	comboboxUpsampler.SetCurSel(0);
	//Generar detector, reconocedor y upsampler de caras
	faceRecognizer.setFaceDetector(new tfg::HaarLikeFaceDetector(ficFaceDetector));
	faceRecognizer.setFaceRecognizer(generateRecognizer(comboboxRecognizer.GetCurSel()));
	faceRecognizer.setUpsampler(generateUpsampler(comboboxUpsampler.GetCurSel()));

	//Inicializar texto de los textboxes
	escalaString.SetWindowText(std::to_string(escalaDeteccion).c_str());
	anchuraMinString.SetWindowText(std::to_string(anchuraMinFace).c_str());
	alturaMinString.SetWindowText(std::to_string(alturaMinFace).c_str());
	anchuraMaxString.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	AlturaMaxString.SetWindowText(std::to_string(alturaMaxFace).c_str());
	umbralReconocimientoString.SetWindowText(std::to_string(umbralReconocimiento).c_str());

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
		std::string pathStr = cStringToString(dlg.GetPathName());

		//Cargar imagen
		imgCargada = cv::imread(pathStr);
		//Comprobar cargado de la imagen
		if(imgCargada.empty())
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error al cargar la imagen"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Modo de procesamiento de imagen
		modo = IMAGEN;
		//Limpiar las caras detectadas
		colourFoundFaces.clear();

		//Mostrar imagen
		CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
		//Mostrar imagen
		showImage(imgCargada, pictureControl);
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
	//Acción según tipo de procesamiento
	switch (modo)
	{
		case IMAGEN:
			procesarImagen();
			break;
		case VIDEO:
			procesarVideo();
			break;
	}
}

//Función de procesamiento de imagen
void CVisionGUIDlg::procesarImagen()
{
	//Comprobar si hay alguna imagen cargada
	if (imgCargada.empty())
		return;

	//Destruir las ventanas de caras
	closeFaceWindows();

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
	//Mostrar imagen
	showImage(imgCargada, pictureControl);
	UpdateData(FALSE);

	//Imagen resultado
	cv::Mat imgFinal;
	//Limpiar caras a color
	colourFoundFaces.clear();
	//Caras grises detectadas
	std::vector<cv::Mat> grayFoundFaces;

	try
	{
		//Detectar caras
		faceRecognizer.recognizeFaces(imgCargada, colourFoundFaces, imgFinal, anchuraReconocimiento, alturaReconocimiento,
			escalaDeteccion, anchuraMinFace, alturaMinFace, anchuraMaxFace, alturaMaxFace);
	}
	catch (std::exception &ex)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T(ex.what()), MB_OK | MB_ICONSTOP);
		//Renovar detector de caras
		faceRecognizer.setFaceDetector(new tfg::HaarLikeFaceDetector(ficFaceDetector));
		//La imagen final es la original cargada
		imgFinal = imgCargada.clone();
	}

	//Mostrar imagen
	showImage(imgFinal, pictureControl);
	UpdateData(FALSE);

}

//Función de procesamiento de vídeo
void CVisionGUIDlg::procesarVideo()
{
	//Comprobar si hay algún vídeo cargado
	if (!videoCaptura.isOpened())
		return;

	//Destruir las ventanas de caras
	closeFaceWindows();

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
	//Mostrar imagen
	showImage(imgCargada, pictureControl);
	UpdateData(FALSE);

	//Frame de lectura
	cv::Mat frame;
	//Recorrer el vídeo
	while (true)
	{
		//Procesar imagen cargada
		procesarImagen();
		//Leer frame
		videoCaptura >> frame;
		//Comprobar si se ha leído un frame correctamente
		if (frame.empty())
			break;
		//Asignar frame a la imagen cargada
		imgCargada = frame;
	}

	//Mostrar mensaje para informar de la salida
	AfxMessageBox(_T("Terminó el procesamiento exitosamente"), MB_OK | MB_ICONINFORMATION);
}

//Botón para mostrar caras
void CVisionGUIDlg::OnMostrarCarasReconocidasClickedFacesButton()
{
	//Ocultar las caras
	closeFaceWindows();

	//Indicador de que se ha mostrado alguna cara
	bool caraMostrada = false;
	//Llevar cuenta de número de caras
	int numCaras = 0;

	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resolución al tamaño indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		//Recuperar cara
		const tfg::Face &face = colourFoundFaces[i];
		//Comprobar si es una cara reconocida
		if (face.clase == -1)
			continue;

		//Indicar que se ha mostrado una cara
		caraMostrada = true;

		//Ajustar cara al tamaño indicado
		cv::Mat finalFace;
		faceRecognizer.upSample(face.img, finalFace, FACE_HEIGHT, FACE_WIDTH);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Id="+std::to_string(face.clase), cvPoint(FACE_WIDTH/2 - 15, 10),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 255, 0), 1, CV_AA);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Conf=" + std::to_string(face.confidence), cvPoint(5, FACE_HEIGHT-5),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 255, 0), 1, CV_AA);
		//Mostrar cara
		std::string windowName = "Cara " + std::to_string(i) + " Id: " + std::to_string(face.clase);
		cv::imshow(windowName, finalFace);
		//Mover ventana
		cv::moveWindow(windowName, numCaras%FACES_X_ROW*(FACE_WIDTH+85), numCaras / FACES_X_ROW * (FACE_HEIGHT+47));
		cv::waitKey(20);

		//Aumentar número de caras
		numCaras++;
	}

	//Si no se ha mostrado ninguna cara, mostrar mensaje
	if(!caraMostrada)
		AfxMessageBox(_T("No hay ninguna imagen para mostrar"), MB_OK | MB_ICONINFORMATION);
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
		//Pasarla a la variable de escala de detección
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

//Cambio de anchura mínima de cara
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

//Cambio de altura mínima de cara
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

//Cambio de anchura máxima de cara
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

//Cambio de altura máxima de cara
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

//Mostrar imagen en pictureControl
void showImage(const cv::Mat& img, CStatic* pictureControl)
{
	//Preparar imagen para mostrar
	CBitmap bitmap;
	//Convertir imagen
	prepareImgToShow(img, bitmap);

	//Mostrar imagen
	pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	pictureControl->SetBitmap(bitmap);
}

//Generar algoritmo de reconocimiento según índice seleccionado
tfg::IFaceRecognizer* generateRecognizer(int id)
{
	//Reconocedor
	tfg::IFaceRecognizer* reconocedor = NULL;

	//Devolver reconocedor según id seleccionado
	switch (id)
	{
		case 0: reconocedor = new tfg::EigenFacesRecognizer();
				break;
		case 1: reconocedor = new tfg::FisherFacesRecognizer();
				break;
		case 2: reconocedor = new tfg::LBPRecognizer();
				break;
		default:
			return NULL;
	}

	//Establecer umbral al reconocedor
	reconocedor->setUmbral(umbralReconocimiento);
	return reconocedor;
}

//Generar algoritmo de upsampling según índice seleccionado
tfg::ImageUpsampler* generateUpsampler(int id)
{
	//Devolver upsampler según id seleccionado
	switch (id)
	{
		case 0: return new tfg::SimpleImageUpsampler(cv::InterpolationFlags::INTER_NEAREST);
		case 1: return new tfg::SimpleImageUpsampler(cv::InterpolationFlags::INTER_AREA);
		case 2: return new tfg::SimpleImageUpsampler(cv::InterpolationFlags::INTER_LINEAR);
		case 3: return new tfg::SimpleImageUpsampler(cv::InterpolationFlags::INTER_CUBIC);
		case 4: return new tfg::SimpleImageUpsampler(cv::InterpolationFlags::INTER_LANCZOS4);
		default:
			return NULL;
	}
}

//Cambio de selección de algoritmo de reconocimiento
void CVisionGUIDlg::OnCbnSelchangeComboRecognizer()
{
	//Crear nuevo reconocedor
	faceRecognizer.setFaceRecognizer(generateRecognizer(comboboxRecognizer.GetCurSel()));
}

//Cambio de selección de upsampler
void CVisionGUIDlg::OnCbnSelchangeComboUpsampler()
{
	//Crear nuevo upsampler
	faceRecognizer.setUpsampler(generateUpsampler(comboboxUpsampler.GetCurSel()));
}

//Cargado de modelo
void CVisionGUIDlg::OnBnClickedButtonLoadmodel()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	int result = dlg.DoModal();
	//Comprobar que se ha cargado la imagen
	if (result == IDOK)
	{
		//Obtener path del fichero
		std::string pathStr = cStringToString(dlg.GetPathName());

		//Cargar modelo de reconocimiento
		try
		{
			faceRecognizer.load(pathStr);
			faceRecognizer.setUmbral(umbralReconocimiento);
		}
		catch (...)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error al cargar el modelo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Mostrar mensaje de tarea realizada
		AfxMessageBox(_T("Modelo cargado correctamente"), MB_OK | MB_ICONINFORMATION);
	}
}

//Cambio del valor de umbral
void CVisionGUIDlg::OnEnChangeEditUmbral()
{
	//Obtener el texto valor
	CString umbral;
	GetDlgItemText(IDC_EDIT_UMBRAL, umbral);

	//Convertir a string convencional
	std::string umbralStr = cStringToString(umbral);
	//Intentar convertir a float
	float nuevoUmbral;

	try
	{
		//Convertir valor nuevo
		nuevoUmbral = std::stof(umbralStr);
		//Pasarla a la variable de umbral de reconocimiento
		umbralReconocimiento = nuevoUmbral;
		//Establecer umbral al reconocedor
		faceRecognizer.setUmbral(umbralReconocimiento);
	}
	catch (...)
	{
		//Reestablecer el texto
		umbralReconocimientoString.SetWindowText(std::to_string(umbralReconocimiento).c_str());
	}
}

//Botón para mostrar caras no reconocidas
void CVisionGUIDlg::OnBnClickedFacesNotRecognizedButton2()
{
	//Ocultar las caras
	closeFaceWindows();
	//Variable para monitorizar si se ha mostrado alguna cara
	bool caraMostrada = false;
	//Variable que cuenta las caras mostradas
	int numCaras = 0;

	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resolución al tamaño indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		//Recuperar cara
		const tfg::Face &face = colourFoundFaces[i];
		//Comprobar si es una cara reconocida
		if (face.clase != -1)
			continue;

		//Indicar que se ha mostrado una cara
		caraMostrada = true;

		//Ajustar cara al tamaño indicado
		cv::Mat finalFace;
		faceRecognizer.upSample(face.img, finalFace, FACE_HEIGHT, FACE_WIDTH);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Id=" + std::to_string(face.clase), cvPoint(FACE_WIDTH / 2 - 15, 10),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 0, 255), 1, CV_AA);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Conf=" + std::to_string(face.confidence), cvPoint(5, FACE_HEIGHT - 5),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 0, 255), 1, CV_AA);
		//Mostrar cara
		std::string windowName = "Cara " + std::to_string(i) + " Id: " + std::to_string(face.clase);
		cv::imshow(windowName, finalFace);
		//Mover ventana
		cv::moveWindow(windowName, numCaras%FACES_X_ROW*(FACE_WIDTH + 85), numCaras / FACES_X_ROW * (FACE_HEIGHT + 47));
		cv::waitKey(20);

		//Aumentar número de caras
		numCaras++;
	}

	//Si no se ha mostrado ninguna cara, mostrar mensaje
	if (!caraMostrada)
		AfxMessageBox(_T("No hay ninguna imagen para mostrar"), MB_OK | MB_ICONINFORMATION);
}

//Cargado de vídeo
void CVisionGUIDlg::OnBnClickedButtonCargarVideo()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	int result = dlg.DoModal();
	//Comprobar que se ha cargado el vídeo
	if (result == IDOK)
	{
		//Destruir las ventanas de caras
		closeFaceWindows();

		//Obtener path y panel de la imagen
		std::string pathStr = cStringToString(dlg.GetPathName());

		//Cargar vídeo
		videoCaptura = cv::VideoCapture(pathStr);

		//Comprobar cargado del vídeo
		if (!videoCaptura.isOpened())
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error al cargar el vídeo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Modo de procesamiento de imagen
		modo = VIDEO;
		//Leer primer frame del vídeo
		cv::Mat frame;
		bool leido = videoCaptura.read(frame);

		//Comprobar si se ha leido
		if (!leido)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error leer frame del vídeo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Asignar primer frame a la imagen cargada
		imgCargada = frame;
		//Limpiar las caras detectadas
		colourFoundFaces.clear();

		//Mostrar imagen
		CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_CONTROL);
		//Mostrar imagen
		showImage(imgCargada, pictureControl);
		UpdateData(FALSE);
	}
}
