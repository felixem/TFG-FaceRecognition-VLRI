
// VisionGUIDlg.h: archivo de encabezado
//

#pragma once


// Cuadro de di�logo de CVisionGUIDlg
class CVisionGUIDlg : public CDialog
{
// Construcci�n
public:
	CVisionGUIDlg(CWnd* pParent = NULL);	// Constructor est�ndar

// Datos del cuadro de di�logo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISIONGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV


// Implementaci�n
protected:
	HICON m_hIcon;

	// Funciones de asignaci�n de mensajes generadas
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadImageClickedButton();
	afx_msg void OnProcesarImagenClickedButton();
	afx_msg void OnMostrarCarasClickedFacesButton();
	afx_msg void OnClickedButtonOcultarCaras();
};
