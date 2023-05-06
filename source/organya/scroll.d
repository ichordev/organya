import organya.setting; 
import organya.deforg;
import organya.orgdata;
import core.stdc.stdio;
import organya.resource;

import bindbc.sdl;

enum DIRECTION_UP = 0;
enum DIRECTION_DOWN = 1;
enum DIRECTION_LEFT = 2;
enum DIRECTION_RIGHT = 3;

struct ScrollData{
	private:
	long hpos; //横スクロール値
	long vpos; //縦スクロール値
	SCROLLINFO scr_info;
	long vScrollMax;
	
	public @nogc nothrow @safe:
	///初期化
	bool InitScroll(){
		scr_info.cbSize = SCROLLINFO.sizeof;
		scr_info.fMask = SIF_RANGE | SIF_PAGE;
		scr_info.nMin = 0;scr_info.nMax = MAXHORZRANGE;
		scr_info.nPage = 4;
		SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1); //横
		scr_info.cbSize = SCROLLINFO.sizeof;
		scr_info.fMask = SIF_RANGE | SIF_PAGE;
		scr_info.nMax = MAXVERTRANGE;
		scr_info.nPage = 4;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, 1); //縦
		hpos = 0; //水平スクロール値
		//vpos = MAXVERTRANGE-27; //垂直初期値
		//vpos = GetPrivateProfileInt(MAIN_WINDOW, "VPOS", MAXVERTRANGE-27, app_path);
		vpos = GetPrivateProfileInt(MAIN_WINDOW, "VPOS", MAXVERTRANGE-27, app_path);
		
		//以下はvposを有効にする処理
		scr_info.fMask = SIF_POS;
		scr_info.nPos = vpos;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, 1);
		
		return true;
	}
	
	/**
	ウィンドウサイズに応じてスクロールバーRangeを変更
	ウィンドウサイズを元にスクロール可能域を設定
	*/
	void ChangeVerticalRange(int WindowHeight){
		if(WindowHeight>0){
			int ap;
			ap = (WindowHeight - 158)/12;
			
			scr_info.nMax = 100 - ap;
			vScrollMax = scr_info.nMax;
		}else{
			scr_info.nMax = MAXVERTRANGE;
		}
		scr_info.cbSize = SCROLLINFO.sizeof;
		scr_info.fMask = SIF_RANGE;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, true); //縦
		return;
	}
	
	///ウィンドウサイズ変更に応じてスクロールを修正
	void AttachScroll(){	
	}
	
	void SetIniFile(){
		wsprintf(num_buf, "%d", vpos);
		WritePrivateProfileString(MAIN_WINDOW, "VPOS", num_buf, app_path);

	}

	void SetHorzScroll(long x){
		RECT rect = {0, 0, WWidth, WHeight};//更新する領域
		hpos = x;
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		scr_info.fMask = SIF_POS;//nPosを有効に
		scr_info.nPos = hpos;
		SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1);
		org_data.PutMusic();
		RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE|RDW_ERASENOW);
	}
	
	void PrintHorzPosition(){
		char[10] str;
		itoa(hpos, str, 10);
		SetDlgItemText(hDlgPlayer, IDE_VIEWMEAS, str);
		SetDlgItemText(hDlgPlayer, IDE_VIEWXPOS, "0");
	}
	
	void HorzScrollProc(WPARAM wParam){ //スクロールさせた時の処理
		RECT rect = {0, 0, WWidth, WHeight}; //更新する領域
		MUSICINFO mi;
		switch(LOWORD(wParam)){
		case SB_LINERIGHT: //右へ
			hpos++;
			if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
			break;
		case SB_LINELEFT: //左へ
			hpos--;
			if(hpos < 0)hpos = 0;
			break;
		case SB_THUMBPOSITION:
			hpos = HIWORD(wParam); //現在位置を取得
			break;
		case SB_THUMBTRACK:
			hpos = HIWORD(wParam); //現在位置を取得
			break;
		case SB_PAGERIGHT: //右へ
			hpos += 1;
			if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
			break;
		case SB_PAGELEFT: //左へ
			hpos -= 1;
			if(hpos < 0)hpos = 0;
			break;
		}
		//プレイヤーに反映
		PrintHorzPosition();
		if(timer_sw == 0){
			org_data.GetMusicInfo(&mi);
			org_data.SetPlayPointer(hpos*mi.dot*mi.line);
		}
		scr_info.fMask = SIF_POS; //nPosを有効に
		scr_info.nPos = hpos;
		SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1);
		org_data.PutMusic();
		RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE|RDW_ERASENOW);
		//以下はテスト用
	//	char str[80];
	//	HDC hdc;
	//	hdc = GetDC(hWnd);
	//	sprintf(str, "Horizontal:%4d", hpos);
	//	TextOut(hdc, 200, 1, str, strlen(str));
	//	ReleaseDC(hWnd, hdc);
	}
	
	void VertScrollProc(WPARAM wParam){
		auto rect = RECT(0, 0, WWidth, WHeight); //更新する領域
		switch(LOWORD(wParam)){
			case SB_LINEDOWN: //下へ
				vpos++;
				if(vpos > vScrollMax)vpos = vScrollMax;
				break;
			case SB_LINEUP: //上へ
				vpos--;
				if(vpos < 0)vpos = 0;
				break;
			case SB_THUMBPOSITION:
				vpos = HIWORD(wParam); //現在位置を取得
				break;
			case SB_THUMBTRACK:
				vpos = HIWORD(wParam); //現在位置を取得
				break;
			case SB_PAGEDOWN: //下へ
				vpos += 6;
				if(vpos > vScrollMax)vpos = vScrollMax;
				break;
			case SB_PAGEUP: //上へ
				vpos -= 6;
				if(vpos < 0)vpos = 0;
				break;
		}
		PrintHorzPosition();
		scr_info.fMask = SIF_POS;	//nPosを有効に
		scr_info.nPos = vpos;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, 1);
		org_data.PutMusic();
		RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE|RDW_ERASENOW);
		//以下はテスト用
	//	char str[80];
	//	HDC hdc;
	//	hdc = GetDC(hWnd);
	//	sprintf(str, "Vertical:%4d", vpos);
	//	TextOut(hdc, 100, 1, str, strlen(str));
	//	ReleaseDC(hWnd, hdc);
	}
	
	///スクロールポジションの取得
	void GetScrollPosition(long* hp, long* vp){
		*hp = hpos;
		*vp = vpos;
	}
	
	///ホイールを...
	void WheelScrollProc(LPARAM lParam, WPARAM wParam){
		RECT rect = {0, 0, WWidth, WHeight};//更新する領域
		
		int fwKeys, zDelta, xPos, yPos;
		
		fwKeys = LOWORD(wParam);            // key flags  MK_CONTROL
		zDelta = cast(short)HIWORD(wParam); // wheel rotation
		xPos   = cast(short)LOWORD(lParam); // horizontal position of pointer
		yPos   = cast(short)HIWORD(lParam); // vertical position of pointer
		
		/*
		switch(LOWORD(wParam)){
		case SB_LINEDOWN: //下へ
			vpos++;
			if(vpos > MAXVERTRANGE)vpos = MAXVERTRANGE;
			break;
		case SB_LINEUP: //上へ
			vpos--;
			if(vpos < 0)vpos = 0;
			break;
		case SB_THUMBPOSITION:
			vpos = HIWORD(wParam); //現在位置を取得
			break;
		case SB_THUMBTRACK:
			vpos = HIWORD(wParam); //現在位置を取得
			break;
		case SB_PAGEDOWN: //下へ
			vpos += 6;
			if(vpos > MAXVERTRANGE)vpos = MAXVERTRANGE;
			break;
		case SB_PAGEUP: //上へ
			vpos -= 6;
			if(vpos < 0)vpos = 0;
			break;
		}
		*/
		
		if(zDelta<0){
			if(fwKeys && MK_CONTROL){
				hpos -= 1;
				if(hpos < 0)hpos = 0;
			}else{
				vpos+=4;
				if(vpos > vScrollMax)vpos = vScrollMax;
			}
		}else{
			if(fwKeys && MK_CONTROL){
				hpos += 1;
				if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
				
			}else{
				vpos-=4;
				if(vpos < 0)vpos = 0;
			}
		}
		
		PrintHorzPosition();
		scr_info.fMask = SIF_POS; //nPosを有効に
		scr_info.nPos = vpos;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, 1);
		scr_info.fMask = SIF_POS; //nPosを有効に
		scr_info.nPos = hpos;
		SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1);
		
		org_data.PutMusic();
		RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE|RDW_ERASENOW);
	}
	
	/////キー操作によるスクロール用
	void KeyScroll(int iDirection){
		RECT rect = {0, 0, WWidth, WHeight}; //更新する領域
		switch(	iDirection ){
		case DIRECTION_UP:
			vpos-=4;
			break;
		case DIRECTION_DOWN:
			vpos+=4;
			break;
		case DIRECTION_LEFT:
			hpos -= 1;
			break;
		case DIRECTION_RIGHT:
			hpos += 1;
			break;
		}
		if(hpos < 0)hpos = 0;
		if(vpos > vScrollMax)vpos = vScrollMax;
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		if(vpos < 0)vpos = 0;
		
		PrintHorzPosition();
		scr_info.fMask = SIF_POS;//nPosを有効に
		scr_info.nPos = vpos;
		SetScrollInfo(hWnd, SB_VERT, &scr_info, 1);
		scr_info.fMask = SIF_POS;//nPosを有効に
		scr_info.nPos = hpos;
		SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1);
		
		org_data.PutMusic();
		RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE|RDW_ERASENOW);

	}
}
ScrollData scr_data; //スクロールデータ


//スクロールバーの移動値
enum MAXHORZRANGE = 256;
enum MAXVERTRANGE = 72-12; //8オクターブ

enum BUF_SIZE = 256;
enum MAIN_WINDOW = "WINDOW";


SDL_Window hDlgPlayer;
byte timer_sw;

char[BUF_SIZE] app_path;
char[BUF_SIZE] num_buf;
