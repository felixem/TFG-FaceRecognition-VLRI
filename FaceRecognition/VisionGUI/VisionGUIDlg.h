
// VisionGUIDlg.h: archivo de encabezado
//

#pragma once
#include "afxwin.h"


// Cuadro de diálogo de CVisionGUIDlg
class CVisionGUIDlg : public CDialog
{
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
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadImageClickedButton();
	afx_msg void OnProcesarImagenClickedButton();
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
	afx_msg void OnBnClickedFacesNotRecognizedButton2();
};
