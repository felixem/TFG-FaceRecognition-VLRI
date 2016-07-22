#pragma once


// Cuadro de di�logo de IntroMenu

class IntroMenu : public CDialog
{
	DECLARE_DYNAMIC(IntroMenu)

public:
	IntroMenu(CWnd* pParent = NULL);   // Constructor est�ndar
	virtual ~IntroMenu();

// Datos del cuadro de di�logo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTROMENU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonModoEntrenamiento();
	afx_msg void OnBnClickedButtonModoReconocimiento();
};
