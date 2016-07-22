
// VisionGUIDlg.h: archivo de encabezado
//

#pragma once
#include "afxwin.h"

#include "ImageDownsampler.h"
#include "HaarLikeFaceDetector.h"
#include "SimpleImageUpsampler.h"
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"
#include "SimpleImageUpsampler.h"
#include "CompleteFaceRecognizer.h"

//Modo de procesamiento
enum MODO_PROCESAMIENTO_RECOG { ARCHIVO_RECOG, CAMARA_RECOG };

// Cuadro de diálogo de CVisionGUIDlg
class CVisionGUIDlg : public CDialog
{
	//Estructura para pasar a la función de control
	typedef struct THREADSTRUCT
	{
		//Referencia a la interfaz
		CVisionGUIDlg*    _this;
		//Mensaje de pausa
		bool pausa;
		//Mensaje de terminar
		bool terminar;
		//Número de intentos hasta timeout y espera entre intentos
		int numIntentosUntilTimeout;
		int esperaEntreIntentos;
		//Modo de procesamiento
		MODO_PROCESAMIENTO_RECOG modoProc;
		
	} THREADSTRUCT;

// Construcción
public:
	CVisionGUIDlg(CWnd* pParent = NULL);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISIONGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV


// Implementación
protected:
	HICON m_hIcon;

	// Funciones de asignación de mensajes generadas
	virtual BOOL OnInitDialog();
	//Finalización del diálogo
	void EndDialog(int nResult);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//Función de procesamiento de vídeo
	static UINT procesarMedia(LPVOID param);
	//Función de procesamiento de imagen
	static UINT procesarImagen(LPVOID param);

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
	//Número de vecinos para detección de caras
	int numVecinosCaras = 3;
	//Anchura y altura para reconocimiento de cara
	int anchuraReconocimiento = 64, alturaReconocimiento = 64;
	//Umbral de reconocimiento
	float umbralReconocimiento = 0;
	//Integración del reconocedor de caras
	tfg::CompleteFaceRecognizer faceRecognizer;
	//Modo de procesamiento actual
	MODO_PROCESAMIENTO_RECOG modo = ARCHIVO_RECOG;
	//Hilo de procesamiento actual
	CWinThread* hiloProc = NULL;
	//Estructura de comunicación de hilo de procesamiento
	THREADSTRUCT * infoHiloProc = NULL;
	//Opciones de reconocimiento facial y de upsampling
	int indexReconocimiento = 0, indexUpsampling = 0;
	//Número de intentos hasta timeout y espera entre intentos
	int numIntentosUntilTimeout = 200, esperaEntreIntentos = 50;

public:

	//Respuesta a mensaje de pintado
	LRESULT updateDataCall(WPARAM wpD, LPARAM lpD);

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

	afx_msg void OnProcesarClickedButton();
	afx_msg void OnMostrarCarasReconocidasClickedFacesButton();
	afx_msg void OnClickedButtonOcultarCaras();
	afx_msg void OnEnChangeEditEscala();
	CEdit escalaString;
	CEdit anchuraMinString;
	CEdit alturaMinString;
	afx_msg void OnEnChangeEditMinWidthFace();
	afx_msg void OnEnChangeEditMinHeightFace();
	CEdit anchuraMaxString;
	CEdit AlturaMaxString;
	afx_msg void OnEnChangeEditMaxWidthFace();
	afx_msg void OnEnChangeEditMaxHeightFace();
	CComboBox comboboxRecognizer;
	CComboBox comboboxUpsampler;
	afx_msg void OnCbnSelchangeComboRecognizer();
	afx_msg void OnCbnSelchangeComboUpsampler();
	afx_msg void OnBnClickedButtonLoadmodel();
	CEdit umbralReconocimientoString;
	afx_msg void OnEnChangeEditUmbral();
	afx_msg void OnBnClickedFacesNotRecognizedButton();
	afx_msg void OnBnClickedButtonCargarArchivoImagen();
	afx_msg void OnBnClickedButtonPausar();
	afx_msg void OnBnClickedButtonTerminarProc();
	CEdit anchuraReconocimientoStr;
	CEdit AlturaReconocimientoStr;
	afx_msg void OnEnChangeEditAnchuraRecog();
	afx_msg void OnEnChangeEditAlturaRecog();
	CEdit vecinosDetectStr;
	afx_msg void OnEnChangeEditVecinosDetec();
	afx_msg void OnBnClickedButtonLoadCamera();
};
