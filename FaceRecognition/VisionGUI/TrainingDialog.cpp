// TrainingDialog.cpp: archivo de implementación
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "TrainingDialog.h"
#include "afxdialogex.h"
#include <chrono>
#include <thread>
#include "CamaraDialog.h"

#define IMG_HEIGHT 465
#define IMG_WIDTH 560

//Definir mensaje propio
#define WM_MY_MESSAGE (WM_USER+1000)

// Cuadro de diálogo de TrainingDialog

IMPLEMENT_DYNAMIC(TrainingDialog, CDialog)

TrainingDialog::TrainingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TRAININGDIALOG, pParent)
{

}

TrainingDialog::~TrainingDialog()
{
}

void TrainingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ESCALA_TRAIN, EscalaStr);
	DDX_Control(pDX, IDC_EDIT_MIN_WIDTH_FACE_TRAIN, AnchuraMinDetectStr);
	DDX_Control(pDX, IDC_EDIT_MIN_HEIGHT_FACE_TRAIN, AlturaMinDetectStr);
	DDX_Control(pDX, IDC_EDIT_VECINOS_DETEC_TRAIN, VecinosMinStr);
	DDX_Control(pDX, IDC_EDIT_MAX_WIDTH_FACE_TRAIN, AnchuraMaxDetectStr);
	DDX_Control(pDX, IDC_EDIT_MAX_HEIGHT_FACE_TRAIN, AlturaMaxDetectStr);
	DDX_Control(pDX, IDC_EDIT_ANCHURA_RECO_TRAIN, AnchuraCaraRecogStr);
	DDX_Control(pDX, IDC_EDIT_ALTURA_RECO_TRAIN, AlturaCaraRecogStr);
	DDX_Control(pDX, IDC_COMBO_RECOGNIZER_TRAIN, comboboxReconocimiento);
	DDX_Control(pDX, IDC_COMBO_UPSAMPLER_TRAIN, comboboxUpsampling);
	DDX_Control(pDX, IDC_COMBO_IDS_APRENDIDAS, comboboxIdsAprendidas);
	DDX_Control(pDX, IDC_COMBO_CARAS_DETECTADAS, comboboxCarasDetectadas);
	DDX_Control(pDX, IDC_COMBO_CARA_ID, comboboxCarasId);
}


BEGIN_MESSAGE_MAP(TrainingDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CARGAR_ARCHIVO_IMAGEN_TRAIN, &TrainingDialog::OnBnClickedButtonCargarArchivoImagenTrain)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_CAMERA_TRAIN, &TrainingDialog::OnBnClickedButtonLoadCameraTrain)
	ON_BN_CLICKED(PROCESSIMG_BUTTON, &TrainingDialog::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_PAUSAR, &TrainingDialog::OnBnClickedButtonPausar)
	ON_BN_CLICKED(IDC_BUTTON_TERMINAR_PROC, &TrainingDialog::OnBnClickedButtonTerminarProc)
	ON_EN_CHANGE(IDC_EDIT_ESCALA_TRAIN, &TrainingDialog::OnEnChangeEditEscalaTrain)
	ON_EN_CHANGE(IDC_EDIT_MIN_WIDTH_FACE_TRAIN, &TrainingDialog::OnEnChangeEditMinWidthFaceTrain)
	ON_EN_CHANGE(IDC_EDIT_MIN_HEIGHT_FACE_TRAIN, &TrainingDialog::OnEnChangeEditMinHeightFaceTrain)
	ON_EN_CHANGE(IDC_EDIT_VECINOS_DETEC_TRAIN, &TrainingDialog::OnEnChangeEditVecinosDetecTrain)
	ON_EN_CHANGE(IDC_EDIT_MAX_WIDTH_FACE_TRAIN, &TrainingDialog::OnEnChangeEditMaxWidthFaceTrain)
	ON_EN_CHANGE(IDC_EDIT_MAX_HEIGHT_FACE_TRAIN, &TrainingDialog::OnEnChangeEditMaxHeightFaceTrain)
	ON_CBN_SELCHANGE(IDC_COMBO_RECOGNIZER_TRAIN, &TrainingDialog::OnCbnSelchangeComboRecognizerTrain)
	ON_CBN_SELCHANGE(IDC_COMBO_UPSAMPLER_TRAIN, &TrainingDialog::OnCbnSelchangeComboUpsamplerTrain)
	ON_EN_CHANGE(IDC_EDIT_ANCHURA_RECO_TRAIN, &TrainingDialog::OnEnChangeEditAnchuraRecoTrain)
	ON_EN_CHANGE(IDC_EDIT_ALTURA_RECO_TRAIN, &TrainingDialog::OnEnChangeEditAlturaRecoTrain)
	ON_BN_CLICKED(IDC_BUTTON_APRENDER, &TrainingDialog::OnBnClickedButtonAprender)
	ON_CBN_SELCHANGE(IDC_COMBO_IDS_APRENDIDAS, &TrainingDialog::OnCbnSelchangeComboIdsAprendidas)
	ON_BN_CLICKED(IDC_BUTTON_ELIMINAR_ID, &TrainingDialog::OnBnClickedButtonEliminarId)
	ON_BN_CLICKED(IDC_BUTTON_ELIMINAR_CARA, &TrainingDialog::OnBnClickedButtonEliminarCara)
	ON_BN_CLICKED(IDC_BUTTON_CREAR_ID, &TrainingDialog::OnBnClickedButtonCrearId)
	ON_CBN_SELCHANGE(IDC_COMBO_CARAS_DETECTADAS, &TrainingDialog::OnCbnSelchangeComboCarasDetectadas)
	ON_BN_CLICKED(IDC_BUTTON_MOSTRAR_DETECCION, &TrainingDialog::OnBnClickedButtonMostrarDeteccion)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CARA, &TrainingDialog::OnBnClickedButtonAddCara)
	ON_BN_CLICKED(IDC_BUTTON_OCULTAR_CARAS, &TrainingDialog::OnBnClickedButtonOcultarCaras)
	ON_MESSAGE(WM_MY_MESSAGE, updateDataCall)
	ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
	ON_BN_CLICKED(IDC_BUTTON_MOSTRAR_ID_CARA, &TrainingDialog::OnBnClickedButtonMostrarIdCara)
	ON_CBN_SELCHANGE(IDC_COMBO_CARA_ID, &TrainingDialog::OnCbnSelchangeComboCaraId)
	ON_BN_CLICKED(IDC_BUTTON_GUARDAR_CARAS, &TrainingDialog::OnBnClickedButtonGuardarCaras)
END_MESSAGE_MAP()


// Controladores de mensajes de TrainingDialog

//Inicialización
BOOL TrainingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Inicialización
	//Inicializar selección de comboboxes
	comboboxReconocimiento.SetCurSel(indexReconocimiento);
	comboboxUpsampling.SetCurSel(indexUpsampling);
	//Generar detector, reconocedor y upsampler de caras
	faceDetector = new tfg::HaarLikeFaceDetector(ficFaceDetector);
	faceRecognizer = generateRecognizer(comboboxReconocimiento.GetCurSel());
	upsampler = generateUpsampler(comboboxUpsampling.GetCurSel());

	//Inicializar texto de los textboxes
	EscalaStr.SetWindowText(std::to_string(escalaDeteccion).c_str());
	VecinosMinStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
	AnchuraMinDetectStr.SetWindowText(std::to_string(anchuraMinFace).c_str());
	AlturaMinDetectStr.SetWindowText(std::to_string(alturaMinFace).c_str());
	AnchuraMaxDetectStr.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	AlturaMaxDetectStr.SetWindowText(std::to_string(alturaMaxFace).c_str());
	AnchuraCaraRecogStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
	AlturaCaraRecogStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());

	return TRUE;
}

//Salir de la ventana
LRESULT TrainingDialog::OnSysCommand(UINT wParam, LPARAM lParam)
{
	UINT nID = wParam;
	//Comprobar tipo de mensaje
	if (nID == SC_CLOSE)
	{
		//Comprobar si hay algún hilo procesando
		if (hiloProc != NULL)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Detenga el procesamiento en curso antes de finalizar"), MB_OK | MB_ICONSTOP);
			return -1;
		}

		//Gestiones de cierre
		this->EndDialog(0);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}

	return LRESULT();
}

//Método para convertir una imagen de opencv en una imagen para mostrar
void TrainingDialog::prepareImgToShow(const cv::Mat &img, CBitmap& output)
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

// Cuadro de diálogo de CVisionGUIDlg
LRESULT TrainingDialog::updateDataCall(WPARAM wpD, LPARAM lpD)
{
	//Opción de mensaje
	switch (lpD)
	{
	case 0:
	{
		//Actualizar datos sobre caras detectadas
		updateDetectedFacesCombobox();
		//Apuntar a la primera cara, si es posible
		if (this->detectedFaces.size() > 0)
			this->comboboxCarasDetectadas.SetCurSel(0);
		break;
	}
	case 1:
		//Borrar información sobre hilo de procesamiento
		this->hiloProc = NULL;
		delete this->infoHiloProc;
		this->infoHiloProc = NULL;
		break;
	}

	//Actualizar datos
	UpdateData(FALSE);
	return LRESULT();
}

//Función de procesamiento de imagen
UINT TrainingDialog::procesarImagen(LPVOID param)
{
	//Cargar estructura
	THREADSTRUCT*    ts = (THREADSTRUCT*)param;
	//Obtener objeto
	TrainingDialog* interfaz = ts->_this;

	//Comprobar si hay alguna imagen cargada
	if (interfaz->imgCargada.empty())
		return -1;

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)interfaz->GetDlgItem(IMG_ENTRENAMIENTO_PREVIEW);

	//Imagen resultado
	cv::Mat imgFinal;
	//Limpiar caras a color
	interfaz->detectedFaces.clear();

	try
	{
		//Detectar caras
		interfaz->faceDetector->detectFaces(interfaz->imgCargada, interfaz->detectedFaces, imgFinal,
			interfaz->escalaDeteccion, interfaz->anchuraMinFace, interfaz->alturaMinFace,
			interfaz->anchuraMaxFace, interfaz->alturaMaxFace, interfaz->numVecinosCaras);
	}
	catch (...)
	{
		//Renovar detector de caras
		delete interfaz->faceDetector;
		interfaz->faceDetector = new tfg::HaarLikeFaceDetector(interfaz->ficFaceDetector);
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

//Función de procesamiento de imagen desde archivo
UINT TrainingDialog::procesarMedia(LPVOID param)
{
	//Cargar estructura
	THREADSTRUCT*    ts = (THREADSTRUCT*)param;
	//Obtener objeto
	TrainingDialog* interfaz = ts->_this;

	//Comprobar si hay algún vídeo cargado
	if (!interfaz->videoCaptura.isOpened())
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No hay cargada ninguna fuente de vídeo"), MB_OK | MB_ICONERROR);
		//Informar sobre fin del hilo
		::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 1);
		return -1;
	}

	//Destruir las ventanas de caras
	interfaz->closeFaceWindows();

	//Mostrar imagen original
	CStatic* pictureControl = (CStatic *)interfaz->GetDlgItem(IMG_ENTRENAMIENTO_PREVIEW);
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

	//Recorrer el vídeo
	while (true)
	{
		try
		{
			//Procesar imagen cargada
			TrainingDialog::procesarImagen(param);
		}
		catch (std::exception &ex)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T(ex.what()), MB_OK | MB_ICONSTOP);
			//Informar sobre fin del hilo
			::SendMessage(*interfaz, WM_MY_MESSAGE, 0, 1);
			return -1;
		}

		//Bucle de lectura de frame para permitir recuperar señal
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

			//Comprobar si se ha leído un frame
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
				//Frame leído
				frameLeido = true;
				//Asignar frame a la imagen cargada
				interfaz->imgCargada = frame;
			}
		} while (!frameLeido && intentos > 0 && !salir);

		//Comprobar si se eligió salir
		if (salir || !frameLeido)
			break;
	}

	//Reiniciar captura al principio si es desde archivo
	if (ts->modoProc == ARCHIVO_TRAIN)
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
	AfxMessageBox(_T("Terminó el procesamiento"), MB_OK | MB_ICONINFORMATION);

	return 1;
}

//Destruir caras encontradas
void TrainingDialog::closeFaceWindows()
{
	//Limpiar las ventanas de las caras encontradas
	cv::destroyAllWindows();
}

//Generar algoritmo de reconocimiento según índice seleccionado
tfg::IFaceRecognizer* TrainingDialog::generateRecognizer(int id)
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
tfg::ImageUpsampler* TrainingDialog::generateUpsampler(int id)
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

//Convertir CString en String
std::string TrainingDialog::cStringToString(const CString& str)
{
	// Convertir cstring a formato string normal
	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	return std::string(pszConvertedAnsiString);
}

//Mostrar imagen en pictureControl
void TrainingDialog::showImage(const cv::Mat& img, CStatic* pictureControl)
{
	//Preparar imagen para mostrar
	CBitmap bitmap;
	//Convertir imagen
	prepareImgToShow(img, bitmap);

	//Mostrar imagen
	pictureControl->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	pictureControl->SetBitmap(bitmap);
}


//Cargar archivo de imagen
void TrainingDialog::OnBnClickedButtonCargarArchivoImagenTrain()
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
	//Comprobar que se ha cargado el archivo
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
			AfxMessageBox(_T("Error al cargar el archivo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Modo de procesamiento de imagen
		modo = ARCHIVO_TRAIN;
		//Leer primer frame del vídeo
		cv::Mat frame;
		bool leido = videoCaptura.read(frame);

		//Comprobar si se ha leido
		if (!leido)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("Error leer frame del archivo"), MB_OK | MB_ICONSTOP);
			return;
		}

		//Asignar primer frame a la imagen cargada
		imgCargada = frame;
		//Limpiar las caras detectadas
		detectedFaces.clear();

		//Mostrar imagen
		CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_ENTRENAMIENTO_PREVIEW);
		//Mostrar imagen
		showImage(imgCargada, pictureControl);
		UpdateData(FALSE);
	}
}

//Carga de imagen en cámara
void TrainingDialog::OnBnClickedButtonLoadCameraTrain()
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

	//Cargar diálogo de captura
	CamaraDialog dialogoCamara;
	dialogoCamara.DoModal();

	//Comprobar si se ha cargado alguna cámara
	if (!dialogoCamara.isCameraSelected())
	{
		//Avisar de que no se ha seleccionado ninguna cámara
		AfxMessageBox(_T("No se ha seleccionado ninguna cámara"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	//Cargar vídeo
	videoCaptura = dialogoCamara.getVideoCapture();

	//Comprobar cargado del vídeo
	if (!videoCaptura.isOpened())
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Error al cargar la cámara"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Modo de procesamiento de imagen
	modo = CAMARA_TRAIN;
	//Leer primer frame de la cámara
	cv::Mat frame;

	//Bucle de intento de lectura
	bool leido = false;
	//Número de intentos
	int numIntentos = this->numIntentosUntilTimeout;
	do
	{
		//Intentar captar frame
		leido = videoCaptura.read(frame);
		//Disminuir número de intentos
		numIntentos--;
		//Pequeña espera
		std::this_thread::sleep_for(std::chrono::milliseconds(this->esperaEntreIntentos));

	} while (!leido && numIntentos > 0);

	//Comprobar si se ha leido
	if (!leido)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Error al leer frame de la cámara"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Asignar primer frame a la imagen cargada
	imgCargada = frame;
	//Limpiar las caras detectadas
	detectedFaces.clear();

	//Mostrar imagen
	CStatic* pictureControl = (CStatic *)GetDlgItem(IMG_ENTRENAMIENTO_PREVIEW);
	//Mostrar imagen
	showImage(imgCargada, pictureControl);
	UpdateData(FALSE);
}

//Procesar fuente de imagen
void TrainingDialog::OnBnClickedButtonProcess()
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

	//Acción según tipo de procesamiento
	switch (modo)
	{
		//Archivo de vídeo/imagen
	case ARCHIVO_TRAIN:
		//Llamar a hilo de procesamiento de vídeo
		infoHiloProc = new THREADSTRUCT;
		infoHiloProc->_this = this;
		infoHiloProc->terminar = false;
		infoHiloProc->pausa = false;
		infoHiloProc->numIntentosUntilTimeout = 0;
		infoHiloProc->esperaEntreIntentos = 0;
		infoHiloProc->modoProc = ARCHIVO_TRAIN;
		hiloProc = AfxBeginThread(procesarMedia, infoHiloProc);
		break;
		//Cámara
	case CAMARA_TRAIN:
		//Llamar a hilo de procesamiento de vídeo
		infoHiloProc = new THREADSTRUCT;
		infoHiloProc->_this = this;
		infoHiloProc->terminar = false;
		infoHiloProc->pausa = false;
		infoHiloProc->numIntentosUntilTimeout = numIntentosUntilTimeout;
		infoHiloProc->esperaEntreIntentos = esperaEntreIntentos;
		infoHiloProc->modoProc = CAMARA_TRAIN;
		hiloProc = AfxBeginThread(procesarMedia, infoHiloProc);
		break;
	}
}

//Finalización del diálogo
void TrainingDialog::EndDialog(int nResult)
{
	//Ocultar caras
	this->closeFaceWindows();

	//Liberar memoria dinámica
	if (faceDetector != NULL)
		delete faceDetector;
	if (faceRecognizer != NULL)
		delete faceRecognizer;
	if (upsampler != NULL)
		delete upsampler;
	//Convertir a null
	faceDetector = NULL;
	faceRecognizer = NULL;
	upsampler = NULL;

	//Finalizar diálogo
	CDialog::EndDialog(nResult);
}

//Pausar procesamiento
void TrainingDialog::OnBnClickedButtonPausar()
{
	//Comprobar si existe un hilo procesando
	if (this->hiloProc != NULL)
	{
		//Comprobar si ya estaba pausado
		if (this->infoHiloProc->pausa)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T("El procesamiento ya está pausado"), MB_OK | MB_ICONINFORMATION);
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
		AfxMessageBox(_T("No hay ningún procesamiento en curso"), MB_OK | MB_ICONINFORMATION);
	}
}

//Terminar procesamiento
void TrainingDialog::OnBnClickedButtonTerminarProc()
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
		AfxMessageBox(_T("No hay ningún procesamiento en curso"), MB_OK | MB_ICONINFORMATION);
	}
}

//Editar escala de detección
void TrainingDialog::OnEnChangeEditEscalaTrain()
{
	//Obtener el texto de escala
	CString escala;
	GetDlgItemText(IDC_EDIT_ESCALA_TRAIN, escala);
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
			EscalaStr.SetWindowText(std::to_string(escalaDeteccion).c_str());
		}
		return;
	}

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
		EscalaStr.SetWindowText(std::to_string(escalaDeteccion).c_str());
	}
}

//Editar anchura mínima de detección
void TrainingDialog::OnEnChangeEditMinWidthFaceTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MIN_WIDTH_FACE_TRAIN, valor);
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
			AnchuraMinDetectStr.SetWindowText(std::to_string(anchuraMinFace).c_str());
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
		AnchuraMinDetectStr.SetWindowText(std::to_string(anchuraMinFace).c_str());
	}
}

//Editar altura mínima de detección de caras
void TrainingDialog::OnEnChangeEditMinHeightFaceTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MIN_HEIGHT_FACE_TRAIN, valor);
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
			AlturaMinDetectStr.SetWindowText(std::to_string(alturaMinFace).c_str());
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
		AlturaMinDetectStr.SetWindowText(std::to_string(alturaMinFace).c_str());
	}
}

//Editar número de vecinos de detección
void TrainingDialog::OnEnChangeEditVecinosDetecTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_VECINOS_DETEC_TRAIN, valor);
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
			VecinosMinStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
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
		VecinosMinStr.SetWindowText(std::to_string(numVecinosCaras).c_str());
	}
}

//Editar anchura máxima de la cara detectada
void TrainingDialog::OnEnChangeEditMaxWidthFaceTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MAX_WIDTH_FACE_TRAIN, valor);
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
			AnchuraMaxDetectStr.SetWindowText(std::to_string(anchuraMaxFace).c_str());
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
		AnchuraMaxDetectStr.SetWindowText(std::to_string(anchuraMaxFace).c_str());
	}
}

//Altura máxima de detección de cara
void TrainingDialog::OnEnChangeEditMaxHeightFaceTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_MAX_HEIGHT_FACE_TRAIN, valor);
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
			AlturaMaxDetectStr.SetWindowText(std::to_string(alturaMaxFace).c_str());
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
		AlturaMaxDetectStr.SetWindowText(std::to_string(alturaMaxFace).c_str());
	}
}

//Cambiar reconocedor de caras
void TrainingDialog::OnCbnSelchangeComboRecognizerTrain()
{
	//Comprobar si ha habido cambios
	if (indexReconocimiento == comboboxReconocimiento.GetCurSel())
		return;

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		comboboxReconocimiento.SetCurSel(indexReconocimiento);
		return;
	}

	//Crear nuevo reconocedor
	indexReconocimiento = comboboxReconocimiento.GetCurSel();
	//Limpiar reconocedor de caras y asignar el nuevo
	delete faceRecognizer;
	faceRecognizer = generateRecognizer(indexReconocimiento);
}

//Cambiar upsampler
void TrainingDialog::OnCbnSelchangeComboUpsamplerTrain()
{
	//Comprobar si ha habido cambios
	if (indexUpsampling == comboboxUpsampling.GetCurSel())
		return;

	//Comprobar procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		comboboxUpsampling.SetCurSel(indexUpsampling);
		return;
	}
	//Crear nuevo upsampler y liberar el anterior
	indexUpsampling = comboboxUpsampling.GetCurSel();
	delete upsampler;
	upsampler = generateUpsampler(indexUpsampling);
}

//Editar anchura de la cara para el reconocimiento
void TrainingDialog::OnEnChangeEditAnchuraRecoTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_ANCHURA_RECO_TRAIN, valor);
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
			AnchuraCaraRecogStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
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
		AnchuraCaraRecogStr.SetWindowText(std::to_string(anchuraReconocimiento).c_str());
	}
}

//Editar altura de la cara para el reconocimiento
void TrainingDialog::OnEnChangeEditAlturaRecoTrain()
{
	//Obtener el texto
	CString valor;
	GetDlgItemText(IDC_EDIT_ALTURA_RECO_TRAIN, valor);
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
			AlturaCaraRecogStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());
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
		AlturaCaraRecogStr.SetWindowText(std::to_string(alturaReconocimiento).c_str());
	}
}

//Aprender el modelo
void TrainingDialog::OnBnClickedButtonAprender()
{
	//Abrir diálogo de guardado de fichero
	CFileDialog dlg(FALSE, CString(".yml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString("Model Files (*.yml)|*.yml|"));
	int result = dlg.DoModal();
	//Comprobar que se ha seleccionado una ruta donde almacenar el aprendizaje
	if (result == IDOK)
	{
		//Preparar vectores de imágenes y labels
		std::vector<cv::Mat> faces;
		std::vector<int> labels;
		//Añadir imágenes y labels
		for (unsigned int i = 0; i < recognizedFaces.size(); ++i)
		{
			//Referencia de id actual
			const std::vector<cv::Mat>& vectorFace = recognizedFaces[i];
			for (unsigned int j = 0; j < vectorFace.size(); ++j)
			{
				//Añadir cara upsampleada y label asociado
				cv::Mat upsampledFace;
				upsampler->upSample(vectorFace[j], upsampledFace, alturaReconocimiento, anchuraReconocimiento);
				faces.push_back(upsampledFace);
				labels.push_back(i);
			}
		}

		try
		{
			//Entrenar reconocedor
			this->faceRecognizer->train(faces, labels);
			//Almacenar fichero
			this->faceRecognizer->save(cStringToString(dlg.GetPathName()));
			//Avisar de que se ha terminado el entrenamiento y el almacenamiento
			AfxMessageBox(_T("El entrenamiento se ha realizado correctamente"), MB_OK | MB_ICONINFORMATION);
		}
		catch (std::exception &ex)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T(ex.what()), MB_OK | MB_ICONSTOP);
		}
	}
}

//Cambiar id aprendida
void TrainingDialog::OnCbnSelchangeComboIdsAprendidas()
{
	//Cerrar ventanas
	this->closeFaceWindows();
	//Actualizar caras para la id
	this->updateRecognizedFacesCombobox();
	//Apuntar a la primera cara si la hubiera
	if (comboboxCarasId.GetCount() > 0)
		comboboxCarasId.SetCurSel(0);
}

//Eliminar id del conjunto de entrenamiento
void TrainingDialog::OnBnClickedButtonEliminarId()
{
	//Comprobar si existe alguna id
	if (recognizedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna id para ser eliminada"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Cerrar ventanas
	this->closeFaceWindows();

	//Obtener índice
	int index = comboboxIdsAprendidas.GetCurSel();
	//Eliminar id
	this->recognizedFaces.erase(this->recognizedFaces.begin() + index);

	//Actualizar ids reconocidas
	this->updateIdsCombobox();
	//Apuntar al índice anterior
	if (index > 0)
		this->comboboxIdsAprendidas.SetCurSel(index - 1);
	else if(recognizedFaces.size() > 0)
		this->comboboxIdsAprendidas.SetCurSel(0);

	//Actualizar caras reconocidas para la id seleccionada
	this->updateRecognizedFacesCombobox();
	//Apuntar a la primera cara si la hubiera
	if (comboboxCarasId.GetCount() > 0)
		comboboxCarasId.SetCurSel(0);
}

//Actualizar ids reconocidas
void TrainingDialog::updateIdsCombobox()
{
	//Mostrar lista de caras del índice seleccionado, si lo hubiera
	comboboxIdsAprendidas.ResetContent();
	//Añadir lista de caras
	for (unsigned int i = 0; i < recognizedFaces.size(); ++i)
	{
		comboboxIdsAprendidas.AddString(_T(("Id " + std::to_string(i)).c_str()));
	}
}

//Actualizar caras reconocidas para una id
void TrainingDialog::updateRecognizedFacesCombobox()
{
	//Mostrar lista de caras del índice seleccionado, si lo hubiera
	comboboxCarasId.ResetContent();
	if (recognizedFaces.size() > 0)
	{
		//Referencia al vector correspondiente
		const std::vector<cv::Mat>& vectorFaces = recognizedFaces[comboboxIdsAprendidas.GetCurSel()];
		//Añadir lista de caras
		for (unsigned int i = 0; i < vectorFaces.size(); ++i)
		{
			comboboxCarasId.AddString(_T(("Cara " + std::to_string(i)).c_str()));
		}
	}
}

//Actualizar caras detectadas
void TrainingDialog::updateDetectedFacesCombobox()
{
	//Mostrar lista de caras detectadas
	comboboxCarasDetectadas.ResetContent();
	//Añadir lista de caras
	for (unsigned int i = 0; i < detectedFaces.size(); ++i)
	{
		comboboxCarasDetectadas.AddString(_T(("Cara " + std::to_string(i)).c_str()));
	}
}

//Eliminar cara para la identidad seleccionada
void TrainingDialog::OnBnClickedButtonEliminarCara()
{
	//Comprobar si existe alguna id
	if (recognizedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna id"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Comprobar si existe alguna cara para la id seleccionada
	int idPers = comboboxIdsAprendidas.GetCurSel();
	std::vector<cv::Mat> &vectorFaces = recognizedFaces[idPers];
	if (vectorFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna cara para la id seleccionada"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Cerrar ventanas
	this->closeFaceWindows();

	//Obtener id de la cara a eliminar
	int idCaras = comboboxCarasId.GetCurSel();
	//Eliminar cara y actualizar la lista de caras
	vectorFaces.erase(vectorFaces.begin() + idCaras);

	//Actualizar lista de caras
	updateRecognizedFacesCombobox();
	//Apuntar a la cara anterior
	if (idCaras > 0)
		comboboxCarasId.SetCurSel(idCaras - 1);
	else if (comboboxCarasId.GetCount() > 0)
		this->comboboxCarasId.SetCurSel(0);
}

//Crear nueva id
void TrainingDialog::OnBnClickedButtonCrearId()
{
	//Añadir vector
	this->recognizedFaces.push_back(std::vector<cv::Mat>());
	//Obtener próxima id
	int id = comboboxIdsAprendidas.GetCount();
	//Añadir string de identidad
	comboboxIdsAprendidas.AddString(_T(("Id " + std::to_string(id)).c_str()));
	//Seleccionar última id
	comboboxIdsAprendidas.SetCurSel(id);
	//Actualizar caras
	this->updateRecognizedFacesCombobox();
}

//Cambio en la selección de cara detectada
void TrainingDialog::OnCbnSelchangeComboCarasDetectadas()
{
	this->closeFaceWindows();
}

//Mostrar cara detectada seleccionada
void TrainingDialog::OnBnClickedButtonMostrarDeteccion()
{
	//Comprobar si hay procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Comprobar si hay alguna cara detectada
	if (detectedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna cara detectada"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Obtener id seleccionada de cara
	int id = comboboxCarasDetectadas.GetCurSel();

	//Ajustar resolución de la cara al tamaño del reconocimiento
	cv::Mat finalFace;
	upsampler->upSample(detectedFaces[id], finalFace, alturaReconocimiento, anchuraReconocimiento);
	//Mostrar cara
	std::string windowName = "Cara " + std::to_string(id);
	cv::imshow(windowName, finalFace);
	//Mover ventana
	cv::moveWindow(windowName, anchuraReconocimiento + 85, 0);
	cv::waitKey(20);
}

//Añadir cara seleccionada a la identidad seleccionada
void TrainingDialog::OnBnClickedButtonAddCara()
{
	//Comprobar si hay procesamiento en curso
	if (hiloProc != NULL && !infoHiloProc->pausa)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("Procesamiento en curso"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Comprobar si hay alguna cara detectada
	if (detectedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna cara detectada"), MB_OK | MB_ICONSTOP);
		return;
	}
	//Comprobar si existe alguna id
	if (recognizedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna id"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Añadir cara al vector de la identidad seleccionada
	int identidad = comboboxIdsAprendidas.GetCurSel();
	//Añadir cara para la identidad
	recognizedFaces[identidad].push_back(detectedFaces[comboboxCarasDetectadas.GetCurSel()]);

	//Añadir string de la cara
	//Obtener próxima id
	int id = comboboxCarasId.GetCount();
	//Añadir string de identidad
	comboboxCarasId.AddString(_T(("Cara " + std::to_string(id)).c_str()));
	//Seleccionar última id
	comboboxCarasId.SetCurSel(id);
}

//Ocultar las caras
void TrainingDialog::OnBnClickedButtonOcultarCaras()
{
	//Cerrar ventanas de caras
	closeFaceWindows();
}

//Mostrar cara reconocida seleccionada para la id
void TrainingDialog::OnBnClickedButtonMostrarIdCara()
{
	//Comprobar si existe alguna id
	if (recognizedFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna id"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Comprobar si existe alguna cara para la id seleccionada
	int idPers = comboboxIdsAprendidas.GetCurSel();
	std::vector<cv::Mat> &vectorFaces = recognizedFaces[idPers];
	if (vectorFaces.size() == 0)
	{
		//Mostrar mensaje de error
		AfxMessageBox(_T("No existe ninguna cara para la id seleccionada"), MB_OK | MB_ICONSTOP);
		return;
	}

	//Cerrar ventanas
	this->closeFaceWindows();
	//Obtener id de la cara elegida
	int idCara = comboboxCarasId.GetCurSel();

	//Ajustar resolución de la cara al tamaño del reconocimiento
	cv::Mat finalFace;
	upsampler->upSample(vectorFaces[idCara], finalFace, alturaReconocimiento, anchuraReconocimiento);
	//Mostrar cara
	std::string windowName = "Cara " + std::to_string(idCara);
	cv::imshow(windowName, finalFace);
	//Mover ventana
	cv::moveWindow(windowName, anchuraReconocimiento + 85, 0);
	cv::waitKey(20);
}

//Cambiada la selección de cara de la id
void TrainingDialog::OnCbnSelchangeComboCaraId()
{
	this->closeFaceWindows();
}

//Guardar caras en un directorio
void TrainingDialog::OnBnClickedButtonGuardarCaras()
{
	//Abrir diálogo de selección de directorio
	CFolderPickerDialog dlg;
	int result = dlg.DoModal();
	//Comprobar que se ha seleccionado una ruta donde almacenar las caras
	if (result == IDOK)
	{
		//Nombre del directorio base
		std::string directorioBase = cStringToString(dlg.GetPathName());
		//Si no existe el directorio base, crearlo
		if (!createDir(directorioBase))
		{
			//Mensaje de error
			AfxMessageBox(_T(("No se pudo crear el directorio "+directorioBase).c_str()), MB_OK | MB_ICONERROR);
			return;
		}

		//Formato de imágenes
		std::string formatoImg = ".jpg";
		try
		{
			//Crear directorios e imágenes
			for (unsigned int i = 0; i < recognizedFaces.size(); ++i)
			{
				//Nombre del directorio derivado
				std::string dirDerivado = directorioBase + "/" + std::to_string(i);
				//Si no existe el directorio base, crearlo
				if (!createDir(dirDerivado))
				{
					//Mensaje de error
					AfxMessageBox(_T(("No se pudo crear el directorio " + dirDerivado).c_str()), MB_OK | MB_ICONERROR);
					return;
				}

				//Referencia de id actual
				const std::vector<cv::Mat>& vectorFace = recognizedFaces[i];
				for (unsigned int j = 0; j < vectorFace.size(); ++j)
				{
					//Realizar upsampling de la cara
					cv::Mat upsampled;
					upsampler->upSample(vectorFace[j], upsampled, alturaReconocimiento, anchuraReconocimiento);
					//Crear imagen
					std::string imgPath = dirDerivado + "/" + std::to_string(j) + formatoImg;
					if (!cv::imwrite(imgPath, upsampled))
					{
						//Mensaje de error
						AfxMessageBox(_T(("No se pudo crear la imagen " + imgPath).c_str()), MB_OK | MB_ICONERROR);
						return;
					}
				}
			}

			//Mensaje de aceptación
			AfxMessageBox(_T("Se han volcado las imágenes correctamente"), MB_OK | MB_ICONINFORMATION);
		}
		catch (std::exception &ex)
		{
			//Mostrar mensaje de error
			AfxMessageBox(_T(ex.what()), MB_OK | MB_ICONSTOP);
		}
	}
}

//Crear directorio
bool TrainingDialog::createDir(const std::string& dirName_in)
{
	return (CreateDirectory(dirName_in.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError());
}
