
// VisionGUIDlg.cpp: archivo de implementaci�n
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "VisionGUIDlg.h"
#include "afxdialogex.h"
#include <chrono>
#include <thread>
#include "CamaraDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_HEIGHT 465
#define IMG_WIDTH 560
#define FACE_HEIGHT 128
#define FACE_WIDTH 92
#define FACES_X_ROW 10

//Definir mensaje propio
#define WM_MY_MESSAGE (WM_USER+1000)

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
LRESULT CVisionGUIDlg::updateDataCall(WPARAM wpD, LPARAM lpD)
{
	//Opci�n de mensaje
	switch (lpD)
	{
	case 0:
		//Actualizar datos
		break;
	case 1:
		//Borrar informaci�n sobre hilo de procesamiento
		this->hiloProc = NULL;
		delete this->infoHiloProc;
		this->infoHiloProc = NULL;
		break;
	}

	//Actualizar datos
	UpdateData(FALSE);
	return LRESULT();
}


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
	DDX_Control(pDX, IDC_EDIT_ANCHURA_RECO, anchuraReconocimientoStr);
	DDX_Control(pDX, IDC_EDIT_ALTURA_RECO, AlturaReconocimientoStr);
	DDX_Control(pDX, IDC_EDIT_VECINOS_DETEC, vecinosDetectStr);
}

BEGIN_MESSAGE_MAP(CVisionGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(PROCESSIMG_BUTTON, &CVisionGUIDlg::OnProcesarClickedButton)
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
	ON_BN_CLICKED(SHOW_FACES_NOT_RECOGNIZED_BUTTON2, &CVisionGUIDlg::OnBnClickedFacesNotRecognizedButton)
	ON_BN_CLICKED(IDC_BUTTON_CARGAR_ARCHIVO_IMAGEN, &CVisionGUIDlg::OnBnClickedButtonCargarArchivoImagen)
	ON_MESSAGE(WM_MY_MESSAGE, updateDataCall)
	ON_BN_CLICKED(IDC_BUTTON_PAUSAR, &CVisionGUIDlg::OnBnClickedButtonPausar)
	ON_BN_CLICKED(IDC_BUTTON_TERMINAR_PROC, &CVisionGUIDlg::OnBnClickedButtonTerminarProc)
	ON_EN_CHANGE(IDC_EDIT_ANCHURA_RECO, &CVisionGUIDlg::OnEnChangeEditAnchuraRecog)
	ON_EN_CHANGE(IDC_EDIT_ALTURA_RECO, &CVisionGUIDlg::OnEnChangeEditAlturaRecog)
	ON_EN_CHANGE(IDC_EDIT_VECINOS_DETEC, &CVisionGUIDlg::OnEnChangeEditVecinosDetec)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CAMERA, &CVisionGUIDlg::OnBnClickedButtonLoadCamera)
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
	//Inicializar selecci�n de comboboxes
	comboboxRecognizer.SetCurSel(indexReconocimiento);
	comboboxUpsampler.SetCurSel(indexUpsampling);
	//Generar detector, reconocedor y upsampler de caras
	faceRecognizer.setFaceDetector(new tfg::HaarLikeFaceDetector(ficFaceDetector));
	faceRecognizer.setFaceRecognizer(generateRecognizer(comboboxRecognizer.GetCurSel()));
	faceRecognizer.setUpsampler(generateUpsampler(comboboxUpsampler.GetCurSel()));

	//Inicializar texto de los textboxes
	escalaString.SetWindowText(std::to_string(escalaDeteccion).c_str());
	vecinosDetectStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
	anchuraMinString.SetWindowText(std::to_string(anchuraMinFace).c_str());
	alturaMinString.SetWindowText(std::to_string(alturaMinFace).c_str());
	anchuraMaxString.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	AlturaMaxString.SetWindowText(std::to_string(alturaMaxFace).c_str());
	umbralReconocimientoString.SetWindowText(std::to_string(umbralReconocimiento).c_str());
	anchuraReconocimientoStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
	AlturaReconocimientoStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());

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

//M�todo para convertir una imagen de opencv en una imagen para mostrar
void CVisionGUIDlg::prepareImgToShow(const cv::Mat &img, CBitmap& output)
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
void CVisionGUIDlg::OnProcesarClickedButton()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL)
	{
		//Comprobar si estaba en pausa el hilo
		if (this->infoHiloProc->pausa)
		{
			//Reanudar procesamiento
			this->infoHiloProc->pausa = false;
			//Destruir las ventanas de caras
			this->closeFaceWindows();
			return;
		}
		else
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			return;
		}
	}

	//Acci�n seg�n tipo de procesamiento
	switch (modo)
	{
	//Archivo de v�deo/imagen
	case ARCHIVO:
		//Llamar a hilo de procesamiento de v�deo
		infoHiloProc = new THREADSTRUCT;
		infoHiloProc->_this = this;
		infoHiloProc->terminar = false;
		infoHiloProc->pausa = false;
		infoHiloProc->numIntentosUntilTimeout = 0;
		infoHiloProc->esperaEntreIntentos = 0;
		infoHiloProc->modoProc = ARCHIVO;
		hiloProc = AfxBeginThread(procesarMedia, infoHiloProc);
		break;
	//C�mara
	case CAMARA:
		//Llamar a hilo de procesamiento de v�deo
		infoHiloProc = new THREADSTRUCT;
		infoHiloProc->_this = this;
		infoHiloProc->terminar = false;
		infoHiloProc->pausa = false;
		infoHiloProc->numIntentosUntilTimeout = numIntentosUntilTimeout;
		infoHiloProc->esperaEntreIntentos = esperaEntreIntentos;
		infoHiloProc->modoProc = CAMARA;
		hiloProc = AfxBeginThread(procesarMedia, infoHiloProc);
		break;
	}
}

//Funci�n de procesamiento de imagen
UINT CVisionGUIDlg::procesarImagen(LPVOID param)
{
	//Cargar estructura
	THREADSTRUCT*    ts = (THREADSTRUCT*)param;
	//Obtener objeto
	CVisionGUIDlg* interfaz = ts->_this;

	//Comprobar si hay alguna imagen cargada
	if (interfaz->imgCargada.empty())
		return -1;

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)interfaz->GetDlgItem(IMG_CONTROL);

	//Imagen resultado
	cv::Mat imgFinal;
	//Limpiar caras a color
	interfaz->colourFoundFaces.clear();
	//Caras grises detectadas
	std::vector<cv::Mat> grayFoundFaces;

	try
	{
		//Detectar caras
		interfaz->faceRecognizer.recognizeFaces(interfaz->imgCargada, interfaz->colourFoundFaces, imgFinal,
			interfaz->anchuraReconocimiento, interfaz->alturaReconocimiento,
			interfaz->escalaDeteccion, interfaz->anchuraMinFace, interfaz->alturaMinFace,
			interfaz->anchuraMaxFace, interfaz->alturaMaxFace, interfaz->numVecinosCaras);
	}
	catch (...)
	{
		//Renovar detector de caras
		interfaz->faceRecognizer.setFaceDetector(new tfg::HaarLikeFaceDetector(interfaz->ficFaceDetector));
		//La imagen final es la original cargada
		imgFinal = interfaz->imgCargada.clone();
		//Mostrar imagen
		interfaz->showImage(imgFinal, pictureControl);
		//Mandar mensaje a la interfaz
		::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 0);
		throw;
	}

	//Mostrar imagen
	interfaz->showImage(imgFinal, pictureControl);
	//Mandar mensaje a la interfaz
	::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 0);
	//UpdateData(FALSE);
	return 1;
}

//Funci�n de procesamiento de imagen desde archivo
UINT CVisionGUIDlg::procesarMedia(LPVOID param)
{
	//Cargar estructura
	THREADSTRUCT*    ts = (THREADSTRUCT*)param;
	//Obtener objeto
	CVisionGUIDlg* interfaz = ts->_this;

	//Comprobar si hay alg�n v�deo cargado
	if (!interfaz->videoCaptura.isOpened())
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No hay cargada ninguna fuente de v�deo"), MB_OK | MB_ICONERROR);
		//Informar sobre fin del hilo
		::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 1);
		return -1;
	}

	//Destruir las ventanas de caras
	interfaz->closeFaceWindows();

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)interfaz->GetDlgItem(IMG_CONTROL);
	//Mostrar imagen
	interfaz->showImage(interfaz->imgCargada, pictureControl);
	//Mandar mensaje a la interfaz
	::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 0);

	//Frame de lectura
	cv::Mat frame;
	//Intentos y timeout de lectura
	int numIntentosLectura = ts->numIntentosUntilTimeout;
	int esperaEntreIntentos = ts->esperaEntreIntentos;
	//Forzar salida
	bool salir = false;

	//Recorrer el v�deo
	while (true)
	{
		try
		{
			//Procesar imagen cargada
			CVisionGUIDlg::procesarImagen(param);
		}
		catch (std::exception &ex)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T(ex.what()), MB_OK | MB_ICONSTOP);
			//Informar sobre fin del hilo
			::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 1);
			return -1;
		}

		//Bucle de lectura de frame para permitir recuperar se�al
		bool frameLeido = false;
		int intentos = numIntentosLectura;
		do
		{
			//Lectura de frame
			interfaz->videoCaptura >> frame;

			//Comprobar si debe pausarse el hilo
			while (ts->pausa)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				intentos = numIntentosLectura;
			}

			//Comprobar si debe terminar el procesamiento
			if (ts->terminar)
				salir = true;

			//Comprobar si se ha le�do un frame
			if (frame.empty())
			{
				if (esperaEntreIntentos > 0)
				{
					//Realizar espera
					std::this_thread::sleep_for(std::chrono::milliseconds(esperaEntreIntentos));
				}
				//Decrementar intentos
				intentos--;
			}
			else
			{
				//Frame le�do
				frameLeido = true;
				//Asignar frame a la imagen cargada
				interfaz->imgCargada = frame;
			}
		} while (!frameLeido && intentos > 0 && !salir);

		//Comprobar si se eligi� salir
		if (salir || !frameLeido)
			break;
	}

	//Reiniciar captura al principio si es desde archivo
	if (ts->modoProc == ARCHIVO)
	{
		//Posicionarse en primer frame
		interfaz->videoCaptura.set(CV_CAP_PROP_POS_FRAMES, 0);
	}
	else
	{
		//Liberar captura
		interfaz->videoCaptura.release();
	}

	//Informar sobre fin del hilo
	::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 1);

	//Mostrar mensaje para informar de la salida
	AfxMessageBox(_T("Termin� el procesamiento"), MB_OK | MB_ICONINFORMATION);

	return 1;
}

//Bot�n para mostrar caras
void CVisionGUIDlg::OnMostrarCarasReconocidasClickedFacesButton()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Ocultar las caras
	closeFaceWindows();

	//Indicador de que se ha mostrado alguna cara
	bool caraMostrada = false;
	//Llevar cuenta de n�mero de caras
	int numCaras = 0;

	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resoluci�n al tama�o indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		//Recuperar cara
		const tfg::Face &face = colourFoundFaces[i];
		//Comprobar si es una cara reconocida
		if (face.clase == -1)
			continue;

		//Indicar que se ha mostrado una cara
		caraMostrada = true;

		//Ajustar cara al tama�o indicado
		cv::Mat finalFace;
		faceRecognizer.upSample(face.img, finalFace, FACE_HEIGHT, FACE_WIDTH);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Id=" + std::to_string(face.clase), cvPoint(FACE_WIDTH / 2 - 15, 10),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 255, 0), 1, CV_AA);
		//Mostrar texto sobre cara
		cv::putText(finalFace, "Conf=" + std::to_string(face.confidence), cvPoint(5, FACE_HEIGHT - 5),
			cv::FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0, 255, 0), 1, CV_AA);
		//Mostrar cara
		std::string windowName = "Cara " + std::to_string(i) + " Id: " + std::to_string(face.clase);
		cv::imshow(windowName, finalFace);
		//Mover ventana
		cv::moveWindow(windowName, numCaras%FACES_X_ROW*(FACE_WIDTH + 85), numCaras / FACES_X_ROW * (FACE_HEIGHT + 47));
		cv::waitKey(20);

		//Aumentar n�mero de caras
		numCaras++;
	}

	//Si no se ha mostrado ninguna cara, mostrar mensaje
	if (!caraMostrada)
		AfxMessageBox(_T("No hay ninguna imagen para mostrar"), MB_OK | MB_ICONINFORMATION);
}

//Destruir caras encontradas
void CVisionGUIDlg::closeFaceWindows()
{
	//Limpiar las ventanas de las caras encontradas
	cv::destroyAllWindows();
}

//Ocultar caras
void CVisionGUIDlg::OnClickedButtonOcultarCaras()
{
	//Cerrar ventanas de caras
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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(escalaDeteccion) != escalaStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			escalaString.SetWindowText(std::to_string(escalaDeteccion).c_str());
		}
		return;
	}

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
std::string CVisionGUIDlg::cStringToString(const CString& str)
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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(anchuraMinFace) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			anchuraMinString.SetWindowText(std::to_string(anchuraMinFace).c_str());
		}
		return;
	}

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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(alturaMinFace) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			alturaMinString.SetWindowText(std::to_string(alturaMinFace).c_str());
		}
		return;
	}

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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(anchuraMaxFace) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			anchuraMaxString.SetWindowText(std::to_string(anchuraMaxFace).c_str());
		}
		return;
	}

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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(alturaMaxFace) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			AlturaMaxString.SetWindowText(std::to_string(alturaMaxFace).c_str());
		}
		return;
	}

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
void CVisionGUIDlg::showImage(const cv::Mat& img, CStatic* pictureControl)
{
	//Preparar imagen para mostrar
	CBitmap bitmap;
	//Convertir imagen
	prepareImgToShow(img, bitmap);

	//Mostrar imagen
	pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	pictureControl->SetBitmap(bitmap);
}

//Generar algoritmo de reconocimiento seg�n �ndice seleccionado
tfg::IFaceRecognizer* CVisionGUIDlg::generateRecognizer(int id)
{
	//Reconocedor
	tfg::IFaceRecognizer* reconocedor = NULL;

	//Devolver reconocedor seg�n id seleccionado
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

//Generar algoritmo de upsampling seg�n �ndice seleccionado
tfg::ImageUpsampler* CVisionGUIDlg::generateUpsampler(int id)
{
	//Devolver upsampler seg�n id seleccionado
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

//Cambio de selecci�n de algoritmo de reconocimiento
void CVisionGUIDlg::OnCbnSelchangeComboRecognizer()
{
	//Comprobar si ha habido cambios
	if (indexReconocimiento == comboboxRecognizer.GetCurSel())
		return;

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		comboboxRecognizer.SetCurSel(indexReconocimiento);
		return;
	}

	//Crear nuevo reconocedor
	indexReconocimiento = comboboxRecognizer.GetCurSel();
	faceRecognizer.setFaceRecognizer(generateRecognizer(indexReconocimiento));
}

//Cambio de selecci�n de upsampler
void CVisionGUIDlg::OnCbnSelchangeComboUpsampler()
{
	//Comprobar si ha habido cambios
	if (indexUpsampling == comboboxUpsampler.GetCurSel())
		return;

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		comboboxUpsampler.SetCurSel(indexUpsampling);
		return;
	}
	//Crear nuevo upsampler
	indexUpsampling = comboboxUpsampler.GetCurSel();
	faceRecognizer.setUpsampler(generateUpsampler(indexUpsampling));
}

//Cargado de modelo
void CVisionGUIDlg::OnBnClickedButtonLoadmodel()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

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

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Reestablecer el texto
		if (umbralStr != std::to_string(umbralReconocimiento))
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			umbralReconocimientoString.SetWindowText(std::to_string(umbralReconocimiento).c_str());
		}
		return;
	}

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

//Bot�n para mostrar caras no reconocidas
void CVisionGUIDlg::OnBnClickedFacesNotRecognizedButton()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Ocultar las caras
	closeFaceWindows();
	//Variable para monitorizar si se ha mostrado alguna cara
	bool caraMostrada = false;
	//Variable que cuenta las caras mostradas
	int numCaras = 0;

	//Abrir ventanas de opencv con las caras encontradas realizando cambio de resoluci�n al tama�o indicado
	for (unsigned int i = 0; i < colourFoundFaces.size(); ++i)
	{
		//Recuperar cara
		const tfg::Face &face = colourFoundFaces[i];
		//Comprobar si es una cara reconocida
		if (face.clase != -1)
			continue;

		//Indicar que se ha mostrado una cara
		caraMostrada = true;

		//Ajustar cara al tama�o indicado
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

		//Aumentar n�mero de caras
		numCaras++;
	}

	//Si no se ha mostrado ninguna cara, mostrar mensaje
	if (!caraMostrada)
		AfxMessageBox(_T("No hay ninguna imagen para mostrar"), MB_OK | MB_ICONINFORMATION);
}

//Cargado de imagen desde archivo
void CVisionGUIDlg::OnBnClickedButtonCargarArchivoImagen()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	int result = dlg.DoModal();
	//Comprobar que se ha cargado el v�deo
	if (result == IDOK)
	{
		//Destruir las ventanas de caras
		closeFaceWindows();

		//Obtener path y panel de la imagen
		std::string pathStr = cStringToString(dlg.GetPathName());

		//Cargar v�deo
		videoCaptura = cv::VideoCapture(pathStr);

		//Comprobar cargado del v�deo
		if (!videoCaptura.isOpened())
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error al cargar el v�deo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Modo de procesamiento de imagen
		modo = ARCHIVO;
		//Leer primer frame del v�deo
		cv::Mat frame;
		bool leido = videoCaptura.read(frame);

		//Comprobar si se ha leido
		if (!leido)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error leer frame del v�deo"), MB_OK | MB_ICONSTOP);
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

//Bot�n de pausa de procesamiento
void CVisionGUIDlg::OnBnClickedButtonPausar()
{
	//Comprobar si existe un hilo procesando
	if (this->hiloProc != NULL)
	{
		//Comprobar si ya estaba pausado
		if (this->infoHiloProc->pausa)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("El procesamiento ya est� pausado"), MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			//Pausar
			this->infoHiloProc->pausa = true;
		}
	}
	else
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No hay ning�n procesamiento en curso"), MB_OK | MB_ICONINFORMATION);
	}
}

//Bot�n de finalizaci�n del reconocimiento
void CVisionGUIDlg::OnBnClickedButtonTerminarProc()
{
	//Comprobar si existe un hilo procesando
	if (this->hiloProc != NULL)
	{
		this->infoHiloProc->terminar = true;
		this->infoHiloProc->pausa = false;
	}
	else
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No hay ning�n procesamiento en curso"), MB_OK | MB_ICONINFORMATION);
	}
}

//Cambio en la anchura del reconocimiento
void CVisionGUIDlg::OnEnChangeEditAnchuraRecog()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_ANCHURA_RECO, valor);
	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(anchuraReconocimiento) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			anchuraReconocimientoStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
		}
		return;
	}

	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		anchuraReconocimiento = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		anchuraReconocimientoStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
	}
}

//Cambio en la altura del reconocimiento
void CVisionGUIDlg::OnEnChangeEditAlturaRecog()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_ALTURA_RECO, valor);
	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(alturaReconocimiento) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			AlturaReconocimientoStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());
		}
		return;
	}

	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		alturaReconocimiento = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		AlturaReconocimientoStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());
	}
}

//Cambios en el n�mero de vecinos para la detecci�n
void CVisionGUIDlg::OnEnChangeEditVecinosDetec()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_VECINOS_DETEC, valor);
	//Convertir a string convencional
	std::string valorStr = cStringToString(valor);

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		if (std::to_string(numVecinosCaras) != valorStr)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
			//Reestablecer el texto
			vecinosDetectStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
		}
		return;
	}

	//Intentar convertir a entero
	int nuevoValor;

	try
	{
		//Convertir nuevo valor
		nuevoValor = std::stoi(valorStr);
		//Pasarla a la variable original
		numVecinosCaras = nuevoValor;

	}
	catch (...)
	{
		//Reestablecer el texto
		vecinosDetectStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
	}
}

//Cargar c�mara
void CVisionGUIDlg::OnBnClickedButtonLoadCamera()
{
	//Comprobar procesamiento en curso
	if (hiloProc != NULL)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Liberar captura actual
	videoCaptura.release();

	//Cargar di�logo de captura
	CamaraDialog dialogoCamara;
	dialogoCamara.DoModal();

	//Comprobar si se ha cargado alguna c�mara
	if (!dialogoCamara.isCameraSelected())
	{
		//Avisar de que no se ha seleccionado ninguna c�mara
		AfxMessageBox(_T("No se ha seleccionado ninguna c�mara"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	//Cargar v�deo
	videoCaptura = dialogoCamara.getVideoCapture();

	//Comprobar cargado del v�deo
	if (!videoCaptura.isOpened())
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Error al cargar la c�mara"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Modo de procesamiento de imagen
	modo = CAMARA;
	//Leer primer frame de la c�mara
	cv::Mat frame;

	//Bucle de intento de lectura
	bool leido = false;
	//N�mero de intentos
	int numIntentos = this->numIntentosUntilTimeout;
	do
	{
		//Intentar captar frame
		leido = videoCaptura.read(frame);
		//Disminuir n�mero de intentos
		numIntentos--;
		//Peque�a espera
		std::this_thread::sleep_for(std::chrono::milliseconds(this->esperaEntreIntentos));
		
	} while (!leido && numIntentos>0);

	//Comprobar si se ha leido
	if (!leido)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Error al leer frame de la c�mara"), MB_OK | MB_ICONSTOP);
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
