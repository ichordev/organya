#include <windows.h>
//�E�C���h�E�̍L��(�g�͏Ȃ�)
#define WINDOWWIDTH		1180
#define WDWHEIGHTPLUS	0
#define WINDOWHEIGHT	(432+WDWHEIGHTPLUS)
#define WHNM			(432 + 16)

#define KEYWIDTH		64

//�r�b�g�}�b�vNO
#define BMPMUSIC		0
#define BMPNOTE			1
#define BMPNUMBER		2
#define BMPPAN			3


#define MSGCANCEL		1
#define MSGEXISFILE		2	//�����t�@�C������
#define MSGSAVEOK		3	//�ۑ�����
#define MSGLOADOK		3	//�ǂݍ��݊���

extern char music_file[];
extern HINSTANCE hInst;//�C���X�^���X�n���h��
extern HWND hWnd;//���C���E�B���h�E�n���h��
extern BOOL actApp;//�E�C���h�E�̃A�N�e�B�u

extern int  WWidth, WHeight;	//�E�B���h�E�̃T�C�Y