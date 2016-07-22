#pragma once

#include "ImageDownsampler.h"
#include "HaarLikeFaceDetector.h"
#include "SimpleImageUpsampler.h"
#include "EigenFacesRecognizer.h"
#include "FisherFacesRecognizer.h"
#include "LBPRecognizer.h"
#include "SimpleImageUpsampler.h"
#include "HaarLikeFaceDetector.h"
#include "Face.h"
#include "afxwin.h"


//Modo de procesamiento
enum MODO_PROCESAMIENTO_TRAIN { ARCHIVO_TRAIN, CAMARA_TRAIN };

// Cuadro de diálogo de TrainingDialog
class TrainingDialog : public CDialog
{
	DECLARE_DYNAMIC(TrainingDialog)

	//Estructura para pasar a la función de control
	typedef struct THREADSTRUCT
	{
		//Referencia a la interfaz
		TrainingDialog*    _this;
		//Mensaje de pausa
		bool pausa;
		//Mensaje de terminar
		bool terminar;
		//Número de intentos hasta timeout y espera entre intentos
		int numIntentosUntilTimeout;
		int esperaEntreIntentos;
		//Modo de procesamiento
		MODO_PROCESAMIENTO_TRAIN modoProc;

	} THREADSTRUCT;

public:
	TrainingDialog(CWnd* pParent = NULL);   // Constructor estándar
	virtual ~TrainingDialog();
	//Iniciar diálogo
	BOOL OnInitDialog();

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAININGDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

	//Función de procesamiento de vídeo
	static UINT procesarMedia(LPVOID param);
	//Función de procesamiento de imagen
	static UINT procesarImagen(LPVOID param);

	//Finalización del diálogo
	void EndDialog(int nResult);

	//Imagen cargada en memoria
	cv::Mat imgCargada;
	//Videocaptura en memoria
	cv::VideoCapture videoCaptura;
	//Nombre del fichero de detección de caras
	const std::string ficFaceDetector = "sources/haarlike/haarcascade_frontalface_alt.xml";
	//Caras encontradas
	std::vector<cv::Mat> detectedFaces;
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
	//Detector de caras
	tfg::HaarLikeFaceDetector* faceDetector;
	//Reconocedor de caras
	tfg::IFaceRecognizer* faceRecognizer;
	//Upsampler
	tfg::ImageUpsampler* upsampler;
	//Modo de procesamiento actual
	MODO_PROCESAMIENTO_TRAIN modo = ARCHIVO_TRAIN;
	//Hilo de procesamiento actual
	CWinThread* hiloProc = NULL;
	//Estructura de comunicación de hilo de procesamiento
	THREADSTRUCT * infoHiloProc = NULL;
	//Opciones de reconocimiento facial y de upsampling
	int indexReconocimiento = 0, indexUpsampling = 0;
	//Número de intentos hasta timeout y espera entre intentos
	int numIntentosUntilTimeout = 200, esperaEntreIntentos = 50;
	//Colección de caras asociadas a una id
	std::vector<std::vector<cv::Mat>> recognizedFaces;

	//Actualizar caras reconocidas para una id
	void updateRecognizedFacesCombobox();

	DECLARE_MESSAGE_MAP()
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

	//Respuestas a eventos
	afx_msg void OnBnClickedButtonCargarArchivoImagenTrain();
	afx_msg void OnBnClickedButtonLoadCameraTrain();
	afx_msg void OnBnClickedButtonProcess();
	afx_msg void OnBnClickedButtonPausar();
	afx_msg void OnBnClickedButtonTerminarProc();
	CEdit EscalaStr;
	CEdit AnchuraMinDetectStr;
	CEdit AlturaMinDetectStr;
	CEdit VecinosMinStr;
	CEdit AnchuraMaxDetectStr;
	CEdit AlturaMaxDetectStr;
	CEdit AnchuraCaraRecogStr;
	CEdit AlturaCaraRecogStr;
	afx_msg void OnEnChangeEditEscalaTrain();
	afx_msg void OnEnChangeEditMinWidthFaceTrain();
	afx_msg void OnEnChangeEditMinHeightFaceTrain();
	afx_msg void OnEnChangeEditVecinosDetecTrain();
	afx_msg void OnEnChangeEditMaxWidthFaceTrain();
	afx_msg void OnEnChangeEditMaxHeightFaceTrain();
	afx_msg void OnCbnSelchangeComboRecognizerTrain();
	afx_msg void OnCbnSelchangeComboUpsamplerTrain();
	afx_msg void OnEnChangeEditAnchuraRecoTrain();
	afx_msg void OnEnChangeEditAlturaRecoTrain();
	afx_msg void OnBnClickedButtonAprender();
	afx_msg void OnCbnSelchangeComboIdsAprendidas();
	afx_msg void OnBnClickedButtonEliminarId();
	afx_msg void OnBnClickedButtonEliminarCara();
	afx_msg void OnBnClickedButtonCrearId();
	afx_msg void OnCbnSelchangeComboCarasDetectadas();
	afx_msg void OnBnClickedButtonMostrarDeteccion();
	afx_msg void OnBnClickedButtonAddCara();
	afx_msg void OnBnClickedButtonOcultarCaras();
	CComboBox comboboxReconocimiento;
	CComboBox comboboxUpsampling;
	CComboBox comboboxIdsAprendidas;
	CComboBox comboboxCarasDetectadas;
	CComboBox comboboxCarasId;
	afx_msg void OnBnClickedButtonMostrarIdCara();
	afx_msg void OnCbnSelchangeComboCaraId();
};
