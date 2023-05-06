module organya.setting;

//#include <windows.h>
//ウインドウの広さ(枠は省く)
enum WINDOWWIDTH = 1180;
enum WDWHEIGHTPLUS = 0;
enum WINDOWHEIGHT = 432+WDWHEIGHTPLUS;
enum WHNM = 432 + 16;

enum KEYWIDTH = 64;

//ビットマップNO
enum BMPMUSIC = 0;
enum BMPNOTE = 1;
enum BMPNUMBER = 2;
enum BMPPAN = 3;


enum MSGCANCEL = 1;
enum MSGEXISFILE = 2;	//既存ファイルあり
enum MSGSAVEOK = 3;	//保存完了
enum MSGLOADOK = 3;	//読み込み完了

// extern char music_file[];
// extern HINSTANCE hInst;//インスタンスハンドル
// extern HWND hWnd;//メインウィンドウハンドル
// extern BOOL actApp;//ウインドウのアクティブ

// extern int  WWidth, WHeight;	//ウィンドウのサイズ
