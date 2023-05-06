module organya.deforg;

enum MAXNOTELENGTH = 255; //音符の最大長
enum MAXMELODY = 8;
enum MAXDRAM = 8;
enum MAXTRACK = 16;

enum KEYDUMMY = 255; //キーの変更は加えない音符
enum PANDUMMY = 255; //パンの変更は加えない音符
enum VOLDUMMY = 255; //ボリュームの変更は加えない音符
//曲情報をセットする時のフラグ
enum SETALL = 0xffffffff; //全てをセット
enum SETWAIT = 0x00000001;
enum SETGRID = 0x00000002;
enum SETALLOC = 0x00000004;
enum SETREPEAT = 0x00000008;
enum SETFREQ = 0x00000010;
enum SETWAVE = 0x00000020;
enum SETPIPI = 0x00000040;


enum ALLOCNOTE = 4096;
enum MODEPARADD = 0;
enum MODEPARSUB = 1;
enum MODEMULTIPLY = 10; //2014.04.30 A
enum MODEDECAY = 64; //2014.05.01 A


//enum NUMDRAMITEM = 39;	//１２＋５＋９＋１(猫)＋１１	// 2011.10.17 D
enum NUMDRAMITEM = 42;	//１２＋５＋９＋１(猫)＋１１＋３	// 2011.10.17 A

