#include "Setting.h"
#include "DefOrg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Sound.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include    <commctrl.h>
#include "Mouse.h"
#include "rxoFunction.h"

#define PI 3.14159265358979323846

#define NUMGRID		8
#define NUMGRIDA		11
#define MAXWAVE		100
#define maxx(a, b) ((a) > (b) ? (a) : (b))

extern HWND hDlgTrack;
extern HWND hDlgPlayer;
extern HWND hDlgEZCopy;


typedef struct{
	char name[20];
	char line;
	char dot;
}GRID;
GRID grid[NUMGRIDA] = {
	{"�����R�ݒ�",0,0},
	{"�S���F�S����",4,4},
	{"�S���F�R����",4,3},
	{"�R���F�S����",3,4},
	{"�R���F�R����",3,3},
	{"�S���F�U����",4,6},
	{"�R���F�U����",3,6},
	{"�S���F�Q����",4,2},
	{"�S���F�W����",4,8},
	{"�S���F12����",4,12},
	{"�T���F�S����",5,4},
};
//IDS_GRID_STRING��!��؂�Œǉ����邱�ƁB�����̕��������а�ł��B

int check_pipi[NUMGRID] ={
	IDC_CHECK_PIPI0,
	IDC_CHECK_PIPI1,
	IDC_CHECK_PIPI2,
	IDC_CHECK_PIPI3,
	IDC_CHECK_PIPI4,
	IDC_CHECK_PIPI5,
	IDC_CHECK_PIPI6,
	IDC_CHECK_PIPI7,
};

int txt_Pan[]={
	IDC_PANPOT1, IDC_PANPOT2 , IDC_PANPOT3, IDC_PANPOT4, IDC_PANPOT5, IDC_PANPOT6, IDC_PANPOT7, IDC_PANPOT8, 
	IDC_PANPOT9, IDC_PANPOT10, IDC_PANPOT11, IDC_PANPOT12, IDC_PANPOT13, IDC_PANPOT14, IDC_PANPOT15, IDC_PANPOT16
};

int txt_Vol[]={
	IDC_VOLUME1, IDC_VOLUME2, IDC_VOLUME3, IDC_VOLUME4, IDC_VOLUME5, IDC_VOLUME6, IDC_VOLUME7, IDC_VOLUME8,
	IDC_VOLUME9, IDC_VOLUME10, IDC_VOLUME11, IDC_VOLUME12, IDC_VOLUME13, IDC_VOLUME14, IDC_VOLUME15, IDC_VOLUME16
};

int Preset1_Pan[]={
	6, 6, 5, 4, 8, 9, 3, 7,
	6, 7, 5, 5, 6, 6, 8, 4
};
int Preset1_Vol[]={
	180, 210, 170, 150, 150, 190, 120, 140,
	180, 190, 225, 230, 195, 200, 180, 190
};


//char *strTrack[] ={"�P", "�Q", "�R", "�S", "�T", "�U", "�V", "�W",  "�p", "�v", "�d", "�q", "�s", "�x", "�t", "�h"};	// 2014.10.19 D
char *strTrack[MAXTRACK];	// 2014.10.19 A
//char *strNNNTrack[] ={"[�P]", "[�Q]", "[�R]", "[�S]", "[�T]", "[�U]", "[�V]", "[�W]",  "[�p]", "[�v]", "[�d]", "[�q]", "[�s]", "[�x]", "[�t]", "[�h]"};	// 2014.10.19 D
char *strNNNTrack[MAXTRACK];	// 2014.10.19 A

int SamplePlayHeight = 36; //�T���v������炷����

int SaveWithInitVolFile;	//�ȃf�[�^�Ɓc�Z�[�u���邩�B

extern HINSTANCE hInst;//�C���X�^���X�n���h��
extern void ClearEZC_Message(); //EZ���b�Z�[�W�Ɣ͈͂�����

double GetNormDistRand(){
	double x, y;
	double s, t;
	double r1, r2;

	// ��l���z�ɏ]���[������ x, y �����
	do { x = (double)rand()/(double)RAND_MAX; } while (x == 0.0); // x �� 0 �ɂȂ�̂������
	y = (double)rand()/(double)RAND_MAX;

	// s, t �̌v�Z
	s = sqrt(-2.0 * log(x)); // C, Java �ł� log �����R�ΐ�
	t = 2.0 * PI * y;

	// �W�����K���z�ɏ]���[������ r1, r2
	r1 = s * cos(t);  r2 = s * sin(t);
	return r2;
}

void SetText(HWND hdwnd, int ID_TEXTBOX, char *str)
{
	SetDlgItemText(hdwnd, ID_TEXTBOX , str);
}

void SetText(HWND hdwnd, int ID_TEXTBOX, int iValue)
{
	char str[128] = {NULL};
	itoa(iValue,str,10);
	SetDlgItemText(hdwnd, ID_TEXTBOX , str);
}

int GetText(HWND hdwnd, int ID_TEXTBOX)
{
	char str[32] = {NULL};
	GetDlgItemText(hdwnd,ID_TEXTBOX,str,31);
	int r;
	r = atoi(str);
	return r;
}

void EnableDialogWindow(int iValue = TRUE)
{
	EnableWindow(hDlgPlayer,iValue);
	EnableWindow(hDlgTrack,iValue);
	EnableWindow(hDlgEZCopy,iValue);

}


//�_�C�A���O���e�̏�����
void InitSettingDialog(HWND hdwnd)
{
	char str[128] = {NULL};
	long i,a;
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
//	//�ҏW�g���b�N���̏�����//////////////////
//	itoa(org_data.track,str,10);
//	SetDlgItemText(hdwnd,IDD_SETTRACK,str);
	//�Đ��E�G�C�g�̏�����//////////////////
	itoa(mi.wait,str,10);
	SetDlgItemText(hdwnd,IDD_SETWAIT,str);
	itoa((60000 / (mi.wait * mi.dot)),str,10);
	SetDlgItemText(hdwnd, IDC_BPM, str);
	//��د�ނ̏�����
	TCHAR *q, *p;
	p = MessageString[IDS_GRID_STRING];
	for(i = 0; i < NUMGRIDA; i++){
		for(q = grid[i].name; *p != 0; p++, q++)*q = *p; //����strcpy
		*q = 0; p++; //�|�C���^��'\0'�̎���
	}

	//�\���O���b�h�̏�����//////////////////
	for(i = 0; i < NUMGRIDA; i++)//���X�g�{�b�N�X�̏�����
		SendDlgItemMessage(hdwnd,IDD_LB1,LB_ADDSTRING,0,(LPARAM)grid[i].name);
	for(i = 0; i < NUMGRIDA; i++)//���ݎg�p���̃O���b�h������
		if(mi.line == grid[i].line && mi.dot == grid[i].dot)break;
	if(i == NUMGRIDA)i = 0;//������΂O��ݒ�
	
	if(i > 0){
		a = grid[i].line;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT1,str);
		a = grid[i].dot;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT2,str);
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), FALSE); //���R�ݒ�e�L�X�g������
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), FALSE);
	}else{
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), TRUE); //���R�ݒ�e�L�X�g�L����
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), TRUE);
		a = mi.line;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT1,str);
		a = mi.dot;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT2,str);
	}

	SendDlgItemMessage(hdwnd,IDD_LB1,LB_SETCURSEL,i,0);
	//���s�[�g�͈͂̏�����//////////////////
	a = mi.repeat_x / (mi.dot * mi.line);
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_REP_MEAS,str);
	a = mi.end_x / (mi.dot * mi.line);
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_END_MEAS,str);
	//�̏�����//////////////////
	a = mi.tdata[0].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ0,str);
	a = mi.tdata[1].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ1,str);
	a = mi.tdata[2].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ2,str);
	a = mi.tdata[3].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ3,str);
	a = mi.tdata[4].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ4,str);
	a = mi.tdata[5].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ5,str);
	a = mi.tdata[6].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ6,str);
	a = mi.tdata[7].freq;
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_SETFREQ7,str);

	for(i = 0; i < NUMGRID; i++){//pipi�̏�����
		if( mi.tdata[i].pipi )
			CheckDlgButton( hdwnd, check_pipi[i], 1 );
	}
	//MessageBox(NULL, "���b�Z�[�W���[�v�𔲂��܂���", "OK", MB_OK);
}

//�E�G�C�g�̐ݒ�
BOOL SetWait(HWND hdwnd, MUSICINFO *mi)
{
	char str[128] = {NULL};
	long a;
	GetDlgItemText(hdwnd,IDD_SETWAIT,str,7);
	a = atol(str);
	if(a > 2000 || a < 1){
		//MessageBox(hdwnd,"1�`2000�ɂ��Ă�������","ERROR(�E�G�C�g)",MB_OK);	// 2014.10.19 D
		msgbox(hdwnd,IDS_VALUESET12000,IDS_ERROR_WAIT,MB_OK);	// 2014.10.19 A
		return FALSE;
	}
	mi->wait = (unsigned short)a;
	return TRUE;
}

//�\���O���b�h�̐ݒ�
BOOL SetGrid(HWND hdwnd,MUSICINFO *mi)
{
	int i;
	//�I���O���b�h�̐ݒ�///////////////////////
	i = SendDlgItemMessage(hdwnd, IDD_LB1,LB_GETCURSEL,0,0);//�C���f�b�N�X�𓾂�
//	EndDialog(hdwnd,0);
	if(i>0){
		mi->dot = grid[i].dot;
		mi->line = grid[i].line;
	}else{
		char str[128] = {NULL};
		long a,b;
		GetDlgItemText(hdwnd,IDD_GRIDEDIT1,str,3);
		a = atol(str);
		if(a<=0 || a>=128){
			//MessageBox(hdwnd,"���q�̐��l�����������ł��B","ERROR(���q)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_WARNING_HYOUSHI,IDS_ERROR_HYOUSHI,MB_OK);	// 2014.10.19 A
			return FALSE;
		}
		GetDlgItemText(hdwnd,IDD_GRIDEDIT2,str,3);
		b = atol(str);
		if(b<=0 || b>=128){
			//MessageBox(hdwnd,"�����̐��l�����������ł��B","ERROR(����)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_WARNING_BUNKATSU,IDS_ERROR_BUNKATSU,MB_OK);	// 2014.10.19 A
			return FALSE;
		}

		mi->line = (unsigned char)a;
		mi->dot = (unsigned char)b;

	}
	return TRUE;
}
//���s�[�g�͈͂̐ݒ�
BOOL SetRepeat(HWND hdwnd, MUSICINFO *mi)
{
	char str[128] = {NULL};
	long a,b;
	GetDlgItemText(hdwnd,IDD_REP_MEAS,str,7);
	a = atol(str);
	mi->repeat_x = (unsigned short)a*mi->line*mi->dot;
	GetDlgItemText(hdwnd,IDD_END_MEAS,str,7);
	b = atol(str);
	mi->end_x = (unsigned short)b*mi->line*mi->dot;
	if(mi->end_x <= mi->repeat_x){
		//MessageBox(hdwnd,"�����܁������ �ɐݒ肵�Ă�������","ERROR(���s�[�g�͈�)",MB_OK);	// 2014.10.19 D
		msgbox(hdwnd,IDS_WARNING_FROM_TO,IDS_ERROR_REPERT,MB_OK);	// 2014.10.19 A
		return FALSE;
	}
	return TRUE;
}
//�ǉ����g���̐ݒ�
int freqbox[MAXTRACK] = {
	IDD_SETFREQ0,
	IDD_SETFREQ1,
	IDD_SETFREQ2,
	IDD_SETFREQ3,
	IDD_SETFREQ4,
	IDD_SETFREQ5,
	IDD_SETFREQ6,
	IDD_SETFREQ7,

	IDD_SETFREQ8,
	IDD_SETFREQ9,
	IDD_SETFREQ10,
	IDD_SETFREQ11,
	IDD_SETFREQ12,
	IDD_SETFREQ13,
	IDD_SETFREQ14,
	IDD_SETFREQ15,
};
BOOL SetTrackFreq(HWND hdwnd, MUSICINFO *mi)
{
	char str[128] = {NULL};
	unsigned short a;	
	for(int i = 0; i < MAXMELODY; i++){
		GetDlgItemText(hdwnd,freqbox[i],str,7);
		a = (unsigned short)atol(str);
//�폜 2014.05.02
//		if(a < 100 || a > 1900){
//			MessageBox(hdwnd,"100�`1900�ɐݒ�","ERROR(���g��)",MB_OK);
//			return FALSE;
//		}
		if(a < 100 || a > 1900){
			//MessageBox(hdwnd,"100�`1900�ɐݒ肵���ق��������ł���B","���m�点(���g��)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET1001900,IDS_INFO_TITLE_FREQ,MB_OK);	// 2014.10.19 A
//			return FALSE;
		}
		mi->tdata[i].freq = a;
	}
	return TRUE;
}
BOOL SetPipiCheck(HWND hdwnd, MUSICINFO *mi)
{
	for(int i = 0; i < MAXMELODY; i++){
		
		if( IsDlgButtonChecked( hdwnd, check_pipi[i] ) )
			mi->tdata[i].pipi = 1;
		else mi->tdata[i].pipi = 0;
	}
	return TRUE;
}

//�Ȃ̐ݒ�
BOOL CALLBACK DialogSetting(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	int iBPM, iWAIT;
	char str[128] = {NULL};
	RECT rect = {64,0,WWidth,WHeight};//�X�V����̈�
	MUSICINFO mi,mg;
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		InitSettingDialog(hdwnd);
		EnableDialogWindow(FALSE);
		return 1;
	case WM_CTLCOLOREDIT: //�e�L�X�g���ҏW���ꂽ�B
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_BTN1:
			GetDlgItemText(hdwnd, IDC_BPM, str, 128);
			iBPM = atoi(str);
			if(iBPM > 0){
				SetGrid(hdwnd, &mg);
				iWAIT = 60000 / iBPM / mg.dot;
				itoa(iWAIT,str,10);
				SetDlgItemText(hdwnd, IDD_SETWAIT, str);
			}
			return 1;
		case IDC_BTN2:
			GetDlgItemText(hdwnd, IDD_SETWAIT, str, 128);
			iWAIT = atoi(str);
			if(iWAIT > 0){
				SetGrid(hdwnd, &mg);
				iBPM = 60000 / (iWAIT * mg.dot);
				itoa(iBPM,str,10);
				SetDlgItemText(hdwnd, IDC_BPM, str);
			}
			return 1;
		case IDD_LB1:
			if(HIWORD(wParam) == LBN_SELCHANGE){ //ؽ��ޯ���ł̑I��ύX
				i = SendDlgItemMessage(hdwnd, IDD_LB1,LB_GETCURSEL,0,0);//�C���f�b�N�X�𓾂�
				if(i == 0){
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), TRUE);
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), TRUE);
				}else{
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), FALSE);
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), FALSE);
				}
			}
			break;
		case IDD_REP_MEAS: case IDD_END_MEAS: case IDD_SETWAIT: case IDC_BPM:
		case IDD_GRIDEDIT1: case IDD_GRIDEDIT2:
		case IDD_SETFREQ0: case IDD_SETFREQ1: case IDD_SETFREQ2: case IDD_SETFREQ3: case IDD_SETFREQ4: case IDD_SETFREQ5: case IDD_SETFREQ6: case IDD_SETFREQ7:
			if(HIWORD(wParam) == EN_SETFOCUS)PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1); //�t�H�[�J�X���Ƀe�L�X�g��S�I������
			return -1;
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			return 1;
		case IDOK:
			org_data.GetMusicInfo( &mi );
			if(!SetWait(hdwnd, &mi))return 1;
			if(!SetGrid(hdwnd,&mi))return 1;
			if(!SetRepeat(hdwnd, &mi))return 1;
			if(!SetTrackFreq(hdwnd, &mi))return 1;
			SetPipiCheck( hdwnd, &mi );

			//�v���C���[�ɕ\��
			itoa(mi.wait,str,10);
			SetDlgItemText(hDlgTrack,IDE_VIEWWAIT,str);
			//�p�����[�^��ݒ�
			org_data.SetMusicInfo(&mi,SETGRID|SETWAIT|SETREPEAT|SETFREQ|SETPIPI);
			//�g�`�̍��Ȃ���
			for(j = 0; j < MAXMELODY; j++)
				MakeOrganyaWave(j,mi.tdata[j].wave_no,mi.tdata[j].pipi);
			//�ĕ`��
			org_data.PutMusic();
			RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			ClearEZC_Message(); //EZ���b�Z�[�W�Ɣ͈͂�����
			return 1;
		}
	}
	return 0;
}
char *dram_name[] = {
	"Bass01",
	"Bass02",
	"Snare01",
	"Snare02",
	"Tom01",

	"HiClose",
	"HiOpen",
	"Crash",
	"Per01",
	"Per02",

	"Bass03",
	"Tom02",
	"Bass04", //�V�K�ǉ�
	"Bass05",
	"Snare03",

	"Snare04",
	"HiClose02",
	"HiOpen02",
	"HiClose03",
	"HiOpen03",

	"Crash02",
	"RevSym01",
	"Ride01",
	"Tom03",
	"Tom04",

	"OrcDrm01",	
	"Bell",
	"Cat" ,
	"Bass06",		//����ɒǉ�
	"Bass07",

	"Snare05",
	"Snare06",
	"Snare07",
	"Tom05",
	"HiOpen04",

	"HiClose04",
	"Clap01",
	"Pesi01",
	"Quick01",
	"Bass08" ,		//���肸�ɒǉ�	// 2011.10.17 

	"Snare08",
	"HiClose05",
};

//�A���t�@�x�b�g���ϊ���`
unsigned char Wave_no_to_List_no[]={
	0,1,10,12,13,28,29,39,26,27,36,7,20,5,16,18,35,41,6,17,19,34,25,8,9,37,38,21,22,2,3,14,15,30,31,32,40,4,11,23,24,33
};
unsigned char List_no_to_Wave_no[]={
	0,1,29,30,37,13,18,11,23,24,2,38,3,4,31,32,14,19,15,20,12,27,28,39,40,22,8,9,5,6,33,34,35,41,21,16,10,25,26,7,36,17
};


void Sl_Reset(HWND hdwnd)
{
	int i;
	SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_SETRANGEMIN , FALSE, 0);
	SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_SETRANGEMAX , FALSE, 95);
	SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_CLEARTICS , FALSE, 0);
	for(i=1; i<8; i++){
		SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_SETTIC , 0, i*12);

	}
	SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_SETTICFREQ  , TRUE, 12);
	SendDlgItemMessage(hdwnd , IDC_SLIDER1 , TBM_SETPOS , TRUE, SamplePlayHeight);

}

void ChangeListBoxSize(HWND hdwnd, int iMeloDrumMode){
	int j;
	if(iMeloDrumMode==1){
		for(j = 0; j < MAXMELODY; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*j+(j>3?9:0), 54, 77, 484, SWP_NOACTIVATE | SWP_NOZORDER );
		}
		for(j = MAXMELODY; j < MAXTRACK; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*(j-MAXMELODY)+((j-MAXMELODY)>3?9:0), 54+484+8, 77, 40, SWP_NOACTIVATE | SWP_NOZORDER );
		}
	}else if(iMeloDrumMode==2){
		for(j = 0; j < MAXMELODY; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*j+(j>3?9:0), 54, 77, 40, SWP_NOACTIVATE | SWP_NOZORDER );
		}
		for(j = MAXMELODY; j < MAXTRACK; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*(j-MAXMELODY)+((j-MAXMELODY)>3?9:0), 54+40+8, 77, 484, SWP_NOACTIVATE | SWP_NOZORDER );
		}
	}else{
		for(j = 0; j < MAXMELODY; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*j+(j>3?9:0), 54, 77, 240, SWP_NOACTIVATE | SWP_NOZORDER );
		}
		for(j = MAXMELODY; j < MAXTRACK; j++){
			HWND haDlg = GetDlgItem(hdwnd, freqbox[j]);
			SetWindowPos(haDlg, HWND_TOP, 7+77*(j-MAXMELODY)+((j-MAXMELODY)>3?9:0), 54+240+8, 77, 284, SWP_NOACTIVATE | SWP_NOZORDER );
		}
	}

}

BOOL CALLBACK DialogWave(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	int i,j;
	long mouse_x;
	long mouse_y;
	long mx, my, n;
	char str[10];
	TCHAR *p;
	static MUSICINFO mi;
	static int iLastLBox = 0;
	static int iMeloDrumMode = 0;
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		//strTrack, strNNNTrack�ɕ������������B
		for(p = MessageString[IDS_STRING112], i=0; i<MAXTRACK; i++){
			strTrack[i] = (char *)p;
			for(; *p != 0; p++); //������I�[�܂Ń|�C���^�ړ�
			p++; //���̎��̕����Ƀ|�C���^�ړ�
		}
		for(p = MessageString[IDS_STRING113], i=0; i<MAXTRACK; i++){
			strNNNTrack[i] = (char *)p;
			for(; *p != 0; p++); //������I�[�܂Ń|�C���^�ړ�
			p++; //���̎��̕����Ƀ|�C���^�ړ�
		}
		org_data.GetMusicInfo(&mi);
		//FREQ & PIPI
		for(j = 0; j < MAXMELODY; j++){
			i = mi.tdata[j].freq; itoa(i,str,10);
			SetDlgItemText(hdwnd,IDD_SETFREQx0+j,str);
			CheckDlgButton( hdwnd, IDC_CHECK_PIPIx0+j, (mi.tdata[j].pipi ? 1 : 0) );
		}
		//�����f�B���X�g�{�b�N�X�̏�����
		for(j = 0; j < MAXMELODY; j++){
			SendDlgItemMessage(hdwnd,freqbox[j],LB_RESETCONTENT,0,0);//������
			for(i = 0; i < MAXWAVE; i++){
				if(i == mi.tdata[j].wave_no){
					sprintf(str,MessageString[IDS_STRING117],i);	// 2010.09.30 D "Wave-%02d*"
				}else{
					sprintf(str,MessageString[IDS_STRING118],i);	// 2010.09.30 D "Wave-%02d"
				}
				//sprintf(str," %02d",i);	// 2010.09.30 A
				SendDlgItemMessage(hdwnd,freqbox[j],LB_ADDSTRING,0,(LPARAM)str);//(LPARAM)wave_name[i].name);
			}
			//�ŏ��̃A�C�e����I��
			SendDlgItemMessage(hdwnd,freqbox[j],LB_SETCURSEL,mi.tdata[j].wave_no,0);
			SendDlgItemMessage(hdwnd,freqbox[j],LB_SETTOPINDEX, maxx(0,mi.tdata[j].wave_no-9) ,0);
		}
		//�h�������X�g�{�b�N�X�̏�����
		for(j = MAXMELODY; j < MAXTRACK; j++){
			SendDlgItemMessage(hdwnd,freqbox[j],LB_RESETCONTENT,0,0);//������
			for(i = 0; i < NUMDRAMITEM; i++){
				//SendDlgItemMessage(hdwnd,freqbox[j],LB_ADDSTRING,0,(LPARAM)dram_name[i]);//(LPARAM)wave_name[i].name);
				SendDlgItemMessage(hdwnd,freqbox[j],LB_ADDSTRING,0,(LPARAM)dram_name[Wave_no_to_List_no[i]]);//(LPARAM)wave_name[i].name);
			}
			//�ŏ��̃A�C�e����I��
			SendDlgItemMessage(hdwnd,freqbox[j],LB_SETCURSEL,List_no_to_Wave_no[mi.tdata[j].wave_no],0);
			SendDlgItemMessage(hdwnd,freqbox[j],LB_SETTOPINDEX, maxx(0,List_no_to_Wave_no[mi.tdata[j].wave_no]-9) ,0);
		}
		Sl_Reset(hdwnd); //�X���C�_�[������
		EnableDialogWindow(FALSE);
		iLastLBox = 0;
		for(j=0;j<MAXTRACK;j++){
			if(j == iLastLBox)SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strNNNTrack[iLastLBox]);
			else SetDlgItemText(hdwnd, j + IDC_LABEL_TRACK_1, strTrack[j]);
		}
		ChangeListBoxSize(hdwnd, iMeloDrumMode);
		return 1;
	case WM_COMMAND:
		if((LOWORD(wParam) >= IDD_SETFREQx0 && LOWORD(wParam) <= IDD_SETFREQx7) && (HIWORD(wParam) == EN_SETFOCUS)){	// 2014.10.19 
			PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1); //�t�H�[�J�X���Ƀe�L�X�g��S�I������
			return -1;
		}
		if( (HWND)lParam == GetDlgItem( hdwnd, VS_VERSION_INFO ) || (HWND)lParam == GetDlgItem( hdwnd, IDCANCEL ) ){

			switch(LOWORD(wParam)){
			case IDOK:
				Rxo_StopAllSoundNow();
				n = 0;
				for(j = 0; j < MAXMELODY; j++){
					GetDlgItemText(hdwnd,IDD_SETFREQx0+j,str,7); i = atol(str); mi.tdata[j].freq = (i>0xFFFF) ? 0xFFFF : i;
					n |= (i < 100 || i >1999) ? 1 : 0;
					mi.tdata[j].pipi = (IsDlgButtonChecked( hdwnd, IDC_CHECK_PIPIx0+j )) ? 1 : 0;
				}
				//if (n) MessageBox(hdwnd,"100�`1900�ɐݒ肵���ق��������ł���B","���m�点(���g��)",MB_OK);	// 2014.10.19 D
				if (n) msgbox(hdwnd,IDS_VALUESET1001900,IDS_INFO_TITLE_FREQ,MB_OK);	// 2014.10.19 A
				org_data.SetMusicInfo(&mi,SETFREQ|SETPIPI|SETWAVE);
				EndDialog(hdwnd,0);
				EnableDialogWindow(TRUE);
				return 1;
			case IDCANCEL:
				Rxo_StopAllSoundNow();
				org_data.GetMusicInfo(&mi);
				for(j = 0; j < MAXMELODY; j++){
					MakeOrganyaWave(j, mi.tdata[j].wave_no ,mi.tdata[j].pipi);
				}
				for(j = MAXMELODY; j < MAXTRACK; j++){
					InitDramObject(dram_name[ mi.tdata[j].wave_no ], j-MAXMELODY);
				}
				EndDialog(hdwnd,0);
				EnableDialogWindow(TRUE);
				return 1;
			}
		}else if( (HWND)lParam == GetDlgItem( hdwnd, VS_SLIDER_RESET ) ){ //���Z�b�g�{�^��
			//FREQ & PIPI
			//for(j = 0; j < MAXMELODY; j++){
			//	i = mi.tdata[j].freq; itoa(i,str,10);
			//	SetDlgItemText(hdwnd,IDD_SETFREQx0+j,str);
			//	CheckDlgButton( hdwnd, IDC_CHECK_PIPIx0+j, (mi.tdata[j].pipi ? 1 : 0) );
			//}
			SamplePlayHeight = 36;
			Sl_Reset(hdwnd); //�X���C�_�[������
			//SetDlgItemText(hdwnd, VS_VERSION_INFO, "abc");
			iMeloDrumMode = 0;
			SendMessage(hdwnd, WM_INITDIALOG, 0, 0);
			for(j = 0; j < MAXMELODY; j++)MakeOrganyaWave(j, mi.tdata[j].wave_no ,mi.tdata[j].pipi);
			for(j = MAXMELODY; j < MAXTRACK; j++){InitDramObject(dram_name[ mi.tdata[j].wave_no ], j-MAXMELODY);}
			SetDlgItemText(hdwnd, IDC_NOTE, "Note");

			return 1;
		}else if( (HWND)lParam == GetDlgItem( hdwnd, IDC_DRUM ) ){
			iMeloDrumMode = (iMeloDrumMode + 1)%3;
			ChangeListBoxSize(hdwnd, iMeloDrumMode);
			return 1;
		}

		for(j = 0; j < MAXMELODY; j++){
			if(freqbox[j] == LOWORD(wParam)){
				if(HIWORD(wParam) == LBN_SELCHANGE){
					SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strTrack[iLastLBox]);
					iLastLBox = j;
					SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strNNNTrack[iLastLBox]);
					Rxo_StopAllSoundNow();
					i = SendDlgItemMessage(hdwnd, freqbox[j],LB_GETCURSEL,0,0);//�C���f�b�N�X�𓾂�
					MakeOrganyaWave(j,i,mi.tdata[j].pipi);
					PlayOrganKey(SamplePlayHeight,j,1000,240);
					//org_data.GetMusicInfo(&mi);
					mi.tdata[j].wave_no = i;
					//org_data.SetMusicInfo(&mi,SETWAVE);
				}else if(HIWORD(wParam) == LBN_SETFOCUS){
					if(j != iLastLBox){
						SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strTrack[iLastLBox]);
						iLastLBox = j;
						SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strNNNTrack[iLastLBox]);
					}
				}
			}else{
				//SetDlgItemText(hdwnd, j + IDC_LABEL_TRACK_1, strTrack[j]);
			}
		}
		for(j = MAXMELODY; j < MAXTRACK; j++){
			if(freqbox[j] == LOWORD(wParam)){
				if(HIWORD(wParam) == LBN_SELCHANGE){
					SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strTrack[iLastLBox]);
					iLastLBox = j;
					SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strNNNTrack[iLastLBox]);
					Rxo_StopAllSoundNow();
					i = SendDlgItemMessage(hdwnd, freqbox[j],LB_GETCURSEL,0,0);//�C���f�b�N�X�𓾂�
					i = Wave_no_to_List_no[i]; //�A���t�@�x�b�g��(List��)����Wave���ɕϊ�����B����d�v�B
					InitDramObject(dram_name[i],j-MAXMELODY);
					PlayOrganKey(SamplePlayHeight,j,1000,240);
					//org_data.GetMusicInfo(&mi);
					mi.tdata[j].wave_no = i;
					//org_data.SetMusicInfo(&mi,SETWAVE);
					iLastLBox = j;
				}else if(HIWORD(wParam) == LBN_SETFOCUS){
					if(j != iLastLBox){
						SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strTrack[iLastLBox]);
						iLastLBox = j;
						SetDlgItemText(hdwnd, iLastLBox + IDC_LABEL_TRACK_1, strNNNTrack[iLastLBox]);
					}
				}
			}
		}
		return 1;
	case WM_LBUTTONDOWN:
		//mouse_data.GetMousePosition(&mouse_x,&mouse_y);
		mouse_x = LOWORD(lParam);  mouse_y = HIWORD(lParam);
		if(mouse_y > 35 && mouse_y < 54 && mouse_x <646){
			if(iMeloDrumMode!=1)iMeloDrumMode = 1; else iMeloDrumMode = 0;
			ChangeListBoxSize(hdwnd, iMeloDrumMode);
		}else if(mouse_y > 587 && mouse_y < 587+19 && mouse_x <646){
			if(iMeloDrumMode!=2)iMeloDrumMode = 2; else iMeloDrumMode = 0;
			ChangeListBoxSize(hdwnd, iMeloDrumMode);
		}else if(mouse_y>73){

			mx = (mouse_x - 646+4 -((mouse_x>646+36*5)?4:0) )/36;        my = (mouse_y - 89 +16)/48;
			n = mx   + my * 10;
			
			if(mx <= 9 && mx >=0 && my >= 0 && my <= 9 && n>=0 && n<100 && iLastLBox>=0 && iLastLBox<MAXMELODY){
				SendDlgItemMessage(hdwnd,freqbox[iLastLBox], LB_SETTOPINDEX, maxx(0,n-9) ,0);
				SendDlgItemMessage(hdwnd,freqbox[iLastLBox], LB_SETCURSEL, n, 0);
				MakeOrganyaWave(iLastLBox,(unsigned char)n,mi.tdata[iLastLBox].pipi);
				//PlayOrganKey(SamplePlayHeight,iLastLBox,1000,240);
				Rxo_StopAllSoundNow();
				Rxo_PlayKey(SamplePlayHeight,iLastLBox,1000,1);
				//org_data.GetMusicInfo(&mi);
				mi.tdata[iLastLBox].wave_no = (unsigned char)n;
				//org_data.SetMusicInfo(&mi,SETWAVE);

			}
		}
		return 1;
	case WM_LBUTTONUP:
		Rxo_StopAllSoundNow();
		return 1;
	case WM_HSCROLL:
		if( (HWND)lParam == GetDlgItem( hdwnd, IDC_SLIDER1 ) ){
			char *strTone[]={"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
			//PlayOrganKey(36,0,1000);
			SamplePlayHeight = SendDlgItemMessage(hdwnd, IDC_SLIDER1, TBM_GETPOS ,0,0);
			SetDlgItemText(hdwnd, IDC_NOTE, strTone[SamplePlayHeight%12]);
		}
		return 1;
	}
	return 0;
}
int usebox[MAXTRACK] = {
	IDE_USE0,
	IDE_USE1,
	IDE_USE2,
	IDE_USE3,
	IDE_USE4,
	IDE_USE5,
	IDE_USE6,
	IDE_USE7,

	IDE_USE8,
	IDE_USE9,
	IDE_USE10,
	IDE_USE11,
	IDE_USE12,
	IDE_USE13,
	IDE_USE14,
	IDE_USE15,
};
int leftbox[MAXTRACK] = {
	IDE_LEFT0,
	IDE_LEFT1,
	IDE_LEFT2,
	IDE_LEFT3,
	IDE_LEFT4,
	IDE_LEFT5,
	IDE_LEFT6,
	IDE_LEFT7,

	IDE_LEFT8,
	IDE_LEFT9,
	IDE_LEFT10,
	IDE_LEFT11,
	IDE_LEFT12,
	IDE_LEFT13,
	IDE_LEFT14,
	IDE_LEFT15,
};
BOOL CALLBACK DialogNoteUsed(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	char str[10] = {NULL};
	long u,l;
//	unsigned short a;
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		for(i = 0; i < MAXTRACK; i++){
			org_data.GetNoteUsed(&u,&l,i);
			itoa(u,str,10);
			SetDlgItemText(hdwnd,usebox[i],str);
			itoa(l,str,10);
			SetDlgItemText(hdwnd,leftbox[i],str);
		}
		EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd,0);
			return 1;
		}
	}
	return 0;
}

BOOL CALLBACK DialogMemo(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	char str[10] = {NULL};
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd,0);
			return 1;
		case ID_ICON_ORG:
			PlaySound( "CAT" , GetModuleHandle(NULL),SND_RESOURCE | SND_ASYNC); 
			return 1;
		}


		return 1;
	
	}
	return 0;
}

bool CheckDefPanVol(HWND hdwnd, int WithSet)
{
	int t,a,b;
	for(t=0;t<MAXTRACK;t++){
		a = GetText(hdwnd , txt_Pan[t]);
		if(a > 12 || a < 0){
			//MessageBox(hdwnd,"0�`12�ɂ��Ă�������","ERROR(�p��)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET012,IDS_ERROR_PAN,MB_OK);	// 2014.10.19 A
			return false;
		}
		b = GetText(hdwnd , txt_Vol[t]);
		if(b > 254 || b < 0){
			//MessageBox(hdwnd,"0�`254�ɂ��Ă�������","ERROR(�{�����[��)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET0254,IDS_ERROR_VOLUME,MB_OK);	// 2014.10.19 A
			return false;
		}
	}
	if(WithSet>0){
		for(t=0;t<MAXTRACK;t++){
			a = GetText(hdwnd , txt_Pan[t]);
			b = GetText(hdwnd , txt_Vol[t]);
			org_data.def_pan[t] = (unsigned char)a;
			org_data.def_volume[t] = (unsigned char)b;
		}
	}
	return true;
}

bool LoadPVIFile(HWND hdwnd, char *FileName)
{
	FILE *fp;
	fp = fopen(FileName, "rt");
	if(fp==NULL) return false;
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		fscanf(fp,"%d",&r);
		SetText(hdwnd,txt_Pan[t],r);
		fscanf(fp,"%d",&r);
		SetText(hdwnd,txt_Vol[t],r);
	}
	fclose(fp);
	return true;
}


bool SavePVIFile(HWND hdwnd, char *FileName)
{
	FILE *fp;
	fp = fopen(FileName, "wt");
	if(fp==NULL) return false;
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		r = GetText(hdwnd , txt_Pan[t]);
		fprintf(fp,"%d\n",r);
		r = GetText(hdwnd , txt_Vol[t]);
		fprintf(fp,"%d\n",r);
	}
	fclose(fp);
	return true;
}

bool GetSetPVInitFile(HWND hdwnd, int IsSave, int iQuiet)
{
	OPENFILENAME ofn;
	FILE *fp;
	char init_file[512];
	int ml;
	strcpy(init_file, music_file);
	ml = strlen(init_file);
	if(ml>4){
		if(init_file[ml-4]=='.'){ //pvi�t��
			//init_file[ml-4]='.';
			init_file[ml-3]='p';
			init_file[ml-2]='v';
			init_file[ml-1]='i';
		}
	}

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = hdwnd;
	ofn.hInstance   = hInst;
	//ofn.lpstrFilter = "�p���^�{�����[�������l�f�[�^[*.pvi]\0*.pvi\0�S�Ă̌`�� [*.*]\0*.*\0\0"; //�p���^�{�����[�������l�f�[�^[*.pvi]\0*.pvi\0�S�Ă̌`�� [*.*]\0*.*\0\0	// 2014.10.19 D
	ofn.lpstrFilter = MessageString[IDS_STRING108]; //�p���^�{�����[�������l�f�[�^[*.pvi]\0*.pvi\0�S�Ă̌`�� [*.*]\0*.*\0\0	// 2014.10.19 A
	ofn.lpstrFile   = init_file;
	ofn.nMaxFile    = MAX_PATH;
	ofn.lpstrDefExt = "pvi";
	
	if(IsSave>0){ //�Z�[�u
		ofn.Flags       = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		ofn.lpstrTitle  = MessageString[IDS_STRING65]; //�p���^�{�����[�������l�f�[�^�̕ۑ�
		if(GetSaveFileName(&ofn));	//InvalidateRect(hwnd,NULL,TRUE);
		else return false;			//�L�����Z����0���Ԃ�
		
	}else{			//���[�h
		ofn.Flags       = OFN_HIDEREADONLY;
		ofn.lpstrTitle  = MessageString[IDS_STRING66]; //�p���^�{�����[�������l�f�[�^�̓ǂݍ���
		if(GetOpenFileName(&ofn));	//InvalidateRect(hwnd,NULL,TRUE);
		else return false;			//�L�����Z����0���Ԃ�
	}
	fp = fopen(init_file,"rb");		//����
	if(fp == NULL){
		if(IsSave==0){
			//if(iQuiet == 0) MessageBox(hdwnd,"�t�@�C���ɃA�N�Z�X�ł��܂���","",MB_OK);	// 2014.10.19 D
			if(iQuiet == 0) msgbox(hdwnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR,MB_OK);	// 2014.10.19 A
			return false;			//�w��t�@�C�������݂��Ȃ�
		}
	}else{
		fclose(fp);
	}
	bool b;
	if(IsSave==0){
		b = LoadPVIFile(hdwnd, init_file);
	}else{
		b = SavePVIFile(hdwnd, init_file);
	}
	return b;
	
}



BOOL CALLBACK DialogDefault(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[32] = {NULL};
	int i,a,b;
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		//���s�[�g�͈͂̏�����//////////////////
		/*
		itoa(org_data.def_pan,str,10);
		SetDlgItemText(hdwnd,IDD_DEFPAN,str);
		itoa(org_data.def_volume,str,10);
		SetDlgItemText(hdwnd,IDD_DEFVOLUME,str);
		*/
		for(i=0;i<MAXTRACK;i++){
			SetText(hdwnd, txt_Pan[i] , (int) org_data.def_pan[i] );
			SetText(hdwnd, txt_Vol[i] , (int) org_data.def_volume[i] );
			
		}
		EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_PANPOT1: case IDC_PANPOT2: case IDC_PANPOT3: case IDC_PANPOT4: case IDC_PANPOT5: case IDC_PANPOT6: case IDC_PANPOT7: case IDC_PANPOT8: case IDC_PANPOT9: case IDC_PANPOT10: case IDC_PANPOT11: case IDC_PANPOT12: case IDC_PANPOT13: case IDC_PANPOT14: case IDC_PANPOT15: case IDC_PANPOT16:
		case IDC_VOLUME1: case IDC_VOLUME2: case IDC_VOLUME3: case IDC_VOLUME4: case IDC_VOLUME5: case IDC_VOLUME6: case IDC_VOLUME7: case IDC_VOLUME8: case IDC_VOLUME9: case IDC_VOLUME10: case IDC_VOLUME11: case IDC_VOLUME12: case IDC_VOLUME13: case IDC_VOLUME14: case IDC_VOLUME15: case IDC_VOLUME16:
			if(HIWORD(wParam) == EN_SETFOCUS)PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1); //�t�H�[�J�X���Ƀe�L�X�g��S�I������	// 2014.10.19 
			return 1;
		case IDC_DEF_NOWLOAD:
			GetSetPVInitFile(hdwnd, 0, 0);
			break;
		case IDC_DEF_NOWSAVE:
			GetSetPVInitFile(hdwnd, 1, 0);
			break;
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			return 1;
		case IDC_BTN_SORT:
			a = GetText(hdwnd,txt_Pan[0]);
			b = GetText(hdwnd,txt_Vol[0]);
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , a );
				SetText(hdwnd, txt_Vol[i] , b );
			}			
			break;
		case IDC_PRESET1:
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , Preset1_Pan[i] );
				SetText(hdwnd, txt_Vol[i] , Preset1_Vol[i] );
			}
			break;
		case IDC_PRESET2:
			for(i=0;i<MAXTRACK;i++){
				b = GetText(hdwnd,txt_Vol[i]);
				SetText(hdwnd, txt_Vol[i] , (b*95/100) );
			}
			break;
		case IDC_PRESET3:
			for(i=0;i<MAXTRACK;i++){
				a = (int)(GetNormDistRand() * 2.0 + 6.0); a = (a < 0) ? 0 : ((a > 12) ? 12 : a);
				SetText(hdwnd, txt_Pan[i] , a );
			}
			break;
		case IDC_PRESET4:
			for(i=0;i<MAXTRACK;i++){
				b = (int)(GetNormDistRand() * 18.0 + 185.0); b = (b < 0) ? 0 : ((b > 253) ? 253 : b);;
				SetText(hdwnd, txt_Vol[i] , b );
			}
			break;
		case IDC_PRESET5:
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , 6 );
				SetText(hdwnd, txt_Vol[i] , 200 );
			}
			break;
		case IDOK:
			/*
			//�p���f�t�H���g�l
			GetDlgItemText(hdwnd,IDD_DEFPAN,str,7);
			i = atol(str);
			if(i > 12 || i < 0){
				MessageBox(hdwnd,"0�`12�ɂ��Ă�������","ERROR(�p��)",MB_OK);
				return FALSE;
			}
			org_data.def_pan = (unsigned char)i;
			//�p���f�t�H���g�l
			GetDlgItemText(hdwnd,IDD_DEFVOLUME,str,7);
			i = atol(str);
			if(i > 254 || i < 0){
				MessageBox(hdwnd,"0�`254�ɂ��Ă�������","ERROR(�{�����[��)",MB_OK);
				return FALSE;
			}
			org_data.def_volume = (unsigned char)i;
			*/
			if( !CheckDefPanVol(hdwnd, 1) )return FALSE; //�����ƃZ�b�e�B���O
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			return 1;
		}
	}
	return 0;
}
BOOL CALLBACK DialogHelp(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	char str[10] = {NULL};
//	char HelpString[10000]; //�Ƃ肠����10kB
	char *HelpString; //A 2010.9.22 �w���v�t�@�C���̗e�ʊg��ɔ����A���I�m�ۂɕύX�B
	HRSRC hrscr;
	HWND haDlg;
	MINMAXINFO *pmmi;
	//DWORD *lpdword;//���\�[�X�̃A�h���X
	char *cc,*ce,*cf;
	//int i;
	int DlgWidth, DlgHeight;
	static int t=0;
	if(GetDlgItem(hdwnd, IDC_HELPTEXT)!=NULL){
		t++;
		if(t<5)SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SETSEL, 0, 0);
		//SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SCROLLCARET, 0, 0);
	}
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		t = 0;
		HelpString = (char *)malloc(1048576 * sizeof(char));//���I�e�ʊm��(�Ƃ肠����1MB) A 2010.09.22
		//EnableDialogWindow(FALSE); //D 2014.05.25
		hrscr = FindResource(NULL, "TEXTHELP" , "TEXT");// ���\�[�X�̌���
		cc = (char*)LockResource(LoadResource(NULL, hrscr));// ���\�[�X�̃A�h���X���擾
		cf = HelpString; ce = cc;
		do{
			if(*ce=='\n'){ //���s�R�[�h�ϊ��i���C���t�B�[�h�������Ȃ��Ƃ����Ȃ��炵���j
				*cf='\r';
				cf++;
			}
			*cf = *ce;
			cf++;
			ce++;
		}while(*ce !=0);
		*cf = *ce;
		SetDlgItemText(hdwnd,IDC_HELPTEXT,HelpString);
		free(HelpString); //�J�� A 2010.09.22

		if(GetDlgItem(hdwnd, IDC_HELPTEXT)!=NULL)
			SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SETSEL, 1, 2);
			SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SCROLLCARET, 0, 0);

		return 1;
	case WM_SIZE:
		DlgWidth  = LOWORD(lParam);	//�N���C�A���g�̈�̃T�C�Y
		DlgHeight = HIWORD(lParam);
		haDlg = GetDlgItem(hdwnd, IDC_HELPTEXT);
		SetWindowPos(haDlg, HWND_TOP, 1, 1, DlgWidth-4, DlgHeight-36, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
		
		return 1;
	case WM_GETMINMAXINFO:
		pmmi = (MINMAXINFO *)lParam;
		if ( pmmi )
		{
			pmmi->ptMinTrackSize.x = 648;  // �ŏ���
			pmmi->ptMinTrackSize.y = 329;  // �ŏ���
		}
		return 1;
	case WM_MOUSEMOVE:
		SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SETSEL, 0,0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			//EnableDialogWindow(TRUE); //D 2014.05.25
			//EndDialog(hdwnd,0);
			ShowWindow(hdwnd, SW_HIDE);
			return 1;
		}
		return 1;
	
	}
	return 0;
}
