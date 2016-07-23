// IntroMenu.cpp: archivo de implementación
//

#include "stdafx.h"
#include "VisionGUI.h"
#include "IntroMenu.h"
#include "afxdialogex.h"
#include "TrainingDialog.h"
#include "VisionGUIDlg.h"


// Cuadro de diálogo de IntroMenu

IMPLEMENT_DYNAMIC(IntroMenu, CDialog)

IntroMenu::IntroMenu(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_INTROMENU, pParent)
{

}

IntroMenu::~IntroMenu()
{
}

void IntroMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(IntroMenu, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MODO_ENTRENAMIENTO, &IntroMenu::OnBnClickedButtonModoEntrenamiento)
	ON_BN_CLICKED(IDC_BUTTON_MODO_RECONOCIMIENTO, &IntroMenu::OnBnClickedButtonModoReconocimiento)
END_MESSAGE_MAP()


// Controladores de mensajes de IntroMenu

//Modo de entrenamiento
void IntroMenu::OnBnClickedButtonModoEntrenamiento()
{
	//Mostrar diálogo de reconocimiento
	TrainingDialog dlg;
	this->ShowWindow(SW_HIDE);
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
}

//Modo de reconocimiento
void IntroMenu::OnBnClickedButtonModoReconocimiento()
{
	//Mostrar diálogo de reconocimiento
	CVisionGUIDlg dlg;
	this->ShowWindow(SW_HIDE);
	dlg.DoModal();
	this->ShowWindow(SW_SHOW);
}
