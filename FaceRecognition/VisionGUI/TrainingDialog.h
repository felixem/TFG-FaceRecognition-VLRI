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

// Cuadro de di�logo de TrainingDialog
class TrainingDialog : public CDialog
{
	DECLARE_DYNAMIC(TrainingDialog)

	//Estructura para pasar a la funci�n de control
	typedef struct THREADSTRUCT
	{
		//Referencia a la interfaz
		TrainingDialog*    _this;
		//Mensaje de pausa
		bool pausa;
		//Mensaje de terminar
		bool terminar;
		//N�mero de intentos hasta timeout y espera entre intentos
		int numIntentosUntilTimeout;
		int esperaEntreIntentos;
		//Modo de procesamiento
		MODO_PROCESAMIENTO_TRAIN modoProc;

	} THREADSTRUCT;

public:
	TrainingDialog(CWnd* pParent = NULL);   // Constructor est�ndar
	virtual ~TrainingDialog();
	//Iniciar di�logo
	BOOL OnInitDialog();

// Datos del cuadro de di�logo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAININGDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

	//Funci�n de procesamiento de v�deo
	static UINT procesarMedia(LPVOID param);
	//Funci�n de procesamiento de imagen
	static UINT procesarImagen(LPVOID param);

	//Finalizaci�n del di�logo
	void EndDialog(int nResult);

	//Imagen cargada en memoria
	cv::Mat imgCargada;
	//Videocaptura en memoria
	cv::VideoCapture videoCaptura;
	//Nombre del fichero de detecci�n de caras
	const std::string ficFaceDetector = "sources/haarlike/haarcascade_frontalface_alt.xml";
	//Caras encontradas
	std::vector<cv::Mat> detectedFaces;
	//Escala de detecci�n
	float escalaDeteccion = 1.05f;
	//Anchura y altura m�nima de detecci�n cara
	int anchuraMinFace = 16, anchuraMaxFace = 0, alturaMinFace = 16, alturaMaxFace = 0;
	//N�mero de vecinos para detecci�n de caras
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
	//Estructura de comunicaci�n de hilo de procesamiento
	THREADSTRUCT * infoHiloProc = NULL;
	//Opciones de reconocimiento facial y de upsampling
	int indexReconocimiento = 0, indexUpsampling = 0;
	//N�mero de intentos hasta timeout y espera entre intentos
	int numIntentosUntilTimeout = 200, esperaEntreIntentos = 50;
	//Colecci�n de caras asociadas a una id
	std::vector<std::vector<cv::Mat>> recognizedFaces;

	//Actualizar caras reconocidas para una id
	void updateRecognizedFacesCombobox();

	DECLARE_MESSAGE_MAP()
public:
	//Respuesta a mensaje de pintado
	LRESULT updateDataCall(WPARAM wpD, LPARAM lpD);
	//M�todo para convertir una imagen de opencv en una imagen para mostrar
	void prepareImgToShow(const cv::Mat &img, CBitmap& output);
	//Destruir caras encontradas
	void closeFaceWindows();
	//Convertir CString en String
	std::string cStringToString(const CString& str);
	//Mostrar imagen en pictureControl
	void showImage(const cv::Mat& img, CStatic* pictureControl);
	//Generar algoritmo de reconocimiento seg�n �ndice seleccionado
	tfg::IFaceRecognizer* generateRecognizer(int id);
	//Generar algoritmo de upsampling seg�n �ndice seleccionado
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
