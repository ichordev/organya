#include <windows.h>
#define MAXBITMAP		64

//void PutMusic(void);
//GDI�̏�����
BOOL StartGDI(HWND hwnd);

BOOL ResizeGDI(HWND hwnd);

//GDI�̊J��
void EndGDI(void);
//�摜�̃��[�h(���\�[�X����)
HBITMAP InitBitmap(char *name,int bmp_no);
//������t���b�v
void RefleshScreen(HDC hdc);
void PutBitmap(long x,long y, RECT *rect, int bmp_no);
void PutBitmapCenter16(long x,long y, RECT *rect, int bmp_no);
////////////////////////////////
///�ȉ��̓��j�[�N�Ȋ֐�//////////
bool MakeMusicParts(unsigned char line,unsigned char dot);
void PutMusicParts(long x,long y);
void PutPanParts(void);
void MakePanParts(unsigned char line,unsigned char dot);

void PutSelectParts(void);

//�ȉ��̓`�g����B������`���Ƃ��݂̂ɗp���邱�ƂƂ���B
void Dw_BeginToDraw(void);
void Dw_FinishToDraw(void);
void Dw_PutBitmap(long x,long y, RECT *rect, int bmp_no);
int  Dw_PutBitmap_Head(long x,long y, RECT *rect, int bmp_no, int iNoteLength);
void Dw_PutBitmap_Center(long x,long y, RECT *rect, int bmp_no); //2014.05.26 A

