module organya.orgdata;

import organya.setting;
import organya.gdi;
import organya.deforg;
import organya.orgdata;
import organya.resource;
import core.stdc.memory;
import core.stdc.math;

import organya.sound;
import organya.rxofunction;

//編集用構造体///////////////

///●コピー構造体
struct NOTECOPY{
	byte track1;//このトラックの
	long x1_1;//ここから
	long x1_2;//ここまでを
	byte track2;//このトラックの
	long x2;//ここに
	long num;//何回
}

///●トランスポートやパンポット
struct PARCHANGE{
	byte track;//このトラックの
	long x1;//ここから
	long x2;//ここまでを
	ubyte a;//これだけ 
	ubyte mode;//たす（ひく）
}

///音符リスト構造体
struct NOTELIST{
	NOTELIST* from; //前のアドレス
	NOTELIST* to; //次のアドレス
	long x; //位置
	ubyte length; //音の長さ
	ubyte y; //音の高さ
	ubyte volume; //ヴォリューム
	ubyte pan; //パン
}

///トラックデータ＊８
struct TRACKDATA{
	ushort freq; //+α周波数(1000がDefault)
//	ushort note_num; //音符の数
	ubyte wave_no; //波形No
	bool pipi;
	NOTELIST* note_p; //NoteData領域の先頭アドレス
	NOTELIST* note_list; //リストの先頭アドレス1
}

///曲単位で持つ固有の情報
struct MUSICINFO{
	ushort wait;
	ubyte line; //１小節のライン数
	ubyte dot; //一泊のドット数
	ushort alloc_note; //確保した音符の数
	long repeat_x; //リピート
	long end_x; //曲の終わり(リピートに戻る)
	TRACKDATA[16] tdata;
}

///メインクラス。このアプリケーションの中心。（クラスってやつを初めて使う）
struct OrgData{
	private:
	MUSICINFO info;
	
	public:
	//ubyte* noteon;
	MUSICINFO[32] ud_tdata; //とりあえず、アンドゥは32回
	NOTELIST[32][16][4096] ud_note;
	bool UndoEnable;
	bool RedoEnable;
	int CurrentUndoCursor; //この番号に書き込む
	int MinimumUndoCursor; //この番号までならさかのぼれる
	int MaximumUndoCursor; //この番号までならやり直せる（リドゥ可能）

	byte track;
	byte[MAXTRACK] mute;
	ubyte[MAXTRACK] def_pan;
	ubyte[MAXTRACK] def_volume;
	
	///指定の数だけNoteDataの領域を確保(初期化)
	bool NoteAlloc(ushort alloc){
		int i, j;
		for(j = 0; j < MAXTRACK; j++){
			info.tdata[j].wave_no = 0;
			info.tdata[j].note_list = null; //コンストラクタにやらせたい
			info.tdata[j].note_p = cast(NOTELIST*)malloc(NOTELIST.sizeof * alloc)[0..alloc];
			if(info.tdata[j].note_p is null) return false;
			//	info.alloc_note = alloc;
			for(i = 0; i < alloc; i++){
				(info.tdata[j].note_p + i).from = null;
				(info.tdata[j].note_p + i).to = null;
				(info.tdata[j].note_p + i).length = 0;
				(info.tdata[j].note_p + i).pan = PANDUMMY;
				(info.tdata[j].note_p + i).volume = VOLDUMMY;
				(info.tdata[j].note_p + i).y = KEYDUMMY;
			}
		}
		for(j = 0; j < MAXMELODY; j++)
			MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);
		for(j = 0; j < MAXDRAM; j++)
			InitDramObject("Bass01", j);

		track = 0; //////////今はここに書いておく
		return true;
	}
	
	///NoteDataを開放
	void ReleaseNote(){
		for(int i = 0; i < MAXTRACK; i++){
			if(info.tdata[i].note_p !is null) free(info.tdata[i].note_p);
		}
	}

	///曲情報を取得
	void GetMusicInfo(MUSICINFO* mi){
		mi.dot = info.dot;
		mi.line = info.line;
		mi.alloc_note = info.alloc_note;
		mi.wait = info.wait;
		mi.repeat_x = info.repeat_x;
		mi.end_x = info.end_x;
		for(int i = 0; i < MAXTRACK; i++){
			mi.tdata[i].freq = info.tdata[i].freq;
			mi.tdata[i].wave_no = info.tdata[i].wave_no;
			mi.tdata[i].pipi = info.tdata[i].pipi;
		}
	}
	
	bool PutBackGround(void){
		if(!MakeMusicParts(info.line, info.dot)) return false; //パーツを生成
		MakePanParts(info.line, info.dot);
		//MessageBox(hWnd, "グリッド", "", MB_OK);
		return true;
	}
	
	///曲情報を設定。flagは設定アイテムを指定
	bool SetMusicInfo(MUSICINFO* mi, ulong flag){
		char[32] str;
		int i;
		if(flag & SETGRID){//グリッドを有効に
			info.dot = mi.dot;
			info.line = mi.line;
			MakeMusicParts(info.line, info.dot);//パーツを生成
			MakePanParts(info.line, info.dot);
	//		MessageBox(hWnd, "グリッド", "", MB_OK);

		}
		if(flag & SETALLOC){//領域確保
			info.alloc_note = mi.alloc_note;
			ReleaseNote();
			NoteAlloc(info.alloc_note);
	//		MessageBox(hWnd, "唖ロック", "", MB_OK);
		}
		if(flag & SETWAIT){
			info.wait = mi.wait;
			itoa(mi.wait, str, 10);
			SetDlgItemText(hDlgTrack, IDE_VIEWWAIT, str);
	//		MessageBox(hWnd, "うえいと", "", MB_OK);
		}
		if(flag & SETREPEAT){
			info.repeat_x = mi.repeat_x;
			info.end_x = mi.end_x;
		}
		if(flag & SETFREQ){
			for(int i = 0; i < MAXMELODY; i++){
				info.tdata[i].freq = mi.tdata[i].freq;
				info.tdata[i].pipi = info.tdata[i].pipi;
			}
		}
		if(flag & SETWAVE){
			for(i = 0; i < MAXTRACK; i++)
				info.tdata[i].wave_no = mi.tdata[i].wave_no;
		}
		if(flag & SETPIPI){
			for(i = 0; i < MAXTRACK; i++)
				info.tdata[i].pipi = mi.tdata[i].pipi;
		}

		return true;
	}
	
	///未使用音符を検索
	NOTELIST* SearchNote(NOTELIST* np){
		int i;
		for(i = 0; i < info.alloc_note; i++, np++){
			if(np.length == 0){
				return np;
			}
		}
		return null;
	}
	
	/**
	■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	以下は音符（キー）の配置、削除
	■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	音符を配置(左クリックの処理)
	*/
	void TouchKeyboard(ubyte y){
		PlayOrganKey(y, track, info.tdata[track].freq, 320);//■
	}
	
	bool SetNote(long x, ubyte y, int DragMode){
		NOTELIST* note;//生成NOTE
		NOTELIST* p;//リストを指すポインター
		NOTELIST* cut_p;//それを置く事によってカットすべき音符
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null)return false;
		//初音符ならリストに登録
		if(info.tdata[track].note_list is null){
			PlayOrganKey(y, track, info.tdata[track].freq, 100);//■
			info.tdata[track].note_list = note;
			note.from = null;
			note.to = null;
			note.length = 1;
			note.pan = def_pan[track];
			note.volume = def_volume[track];
			note.x = x;
			note.y = y;
			return true;
		}
		//頭から検索
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		//挿入
		if(p.x > x){
			PlayOrganKey(y, track, info.tdata[track].freq, 100);//■
			note.to = p;
			note.from = p.from;
			if(p.from is null){
				info.tdata[track].note_list = note;//先頭
			}
			else p.from.to = note;//じゃなければ前のやつの次に
			p.from = note;
			note.length = 1;
			note.pan = def_pan[track];
			note.volume = def_volume[track];
			note.x = x;
			note.y = y;
			//カットすべき音符を検索
			cut_p = note.from;
			while(cut_p !is null && cut_p.y == KEYDUMMY)cut_p = cut_p.from;
			if(cut_p is null)return true;
			if(note.x < cut_p.x + cut_p.length)//カット
				cut_p.length = cast(ubyte)(note.x - cut_p.x);
		}
		//パラメータ変更
		else if(p.x == x){
			if(p.y == y){
				if(p.length == MAXNOTELENGTH)return false;
				//if(DragMode==0)
				if(iPushStratch)p.length++;//長く
				else p.length = iLastEditNoteLength; //頭を...オプションだったら伸ばすのではなく最後の長さを使う
				//カットすべき音符を検索
				cut_p = p.to;
				while(cut_p !is null && cut_p.y == KEYDUMMY){
					cut_p = cut_p.to;
				}
				if(cut_p !is null && p.length + p.x > cut_p.x){//伸ばしすぎると次のを飲み込む
					if(cut_p.to is null){//そいつが最後尾なら
						cut_p.from.to = null;//一つ前の人に最後尾を任せる。
					}
					else{
						cut_p.from.to = cut_p.to;
						cut_p.to.from = cut_p.from;
					}
					cut_p.length = 0;
					cut_p.from = null;
				}
			}else{
				PlayOrganKey(y, track, info.tdata[track].freq, 100);//■
				p.y = y;//Ｙ変更
				//カットすべき音符を検索
				cut_p = p.from;
				while(cut_p !is null && cut_p.y == KEYDUMMY)cut_p = cut_p.from;
				if(cut_p is null)return true;
				if(p.x < cut_p.x + cut_p.length)//カット
					cut_p.length = cast(ubyte)(p.x - cut_p.x);
			}
		}
		//最後尾追加
		else if(p.to is null){
			PlayOrganKey(y, track, info.tdata[track].freq, 100);//■
			note.from = p;
			p.to = note;
			note.to = null;
			note.length = 1;
			note.pan = def_pan[track];
			note.volume = def_volume[track];
			note.x = x;
			note.y = y;
			//カットすべき音符を検索
			cut_p = note.from;
			while(cut_p !is null && cut_p.y == KEYDUMMY)cut_p = cut_p.from;
			if(cut_p is null)return true;
			if(note.x < cut_p.x + cut_p.length)//カット
				cut_p.length = cast(ubyte)(note.x - cut_p.x);
		}
		return true;
	}
	//音符のカット(右クリックの処理)
	bool CutNote(long x, ubyte y)
	{
		NOTELIST* p;//リストを指すポインター
		//頭から検索
		if(info.tdata[track].note_list is null) return false;
		p = info.tdata[track].note_list;
		while(p !is null && p.x < x) p = p.to;
		if(p is null) return false;
		//パラメータ変更
		if(p.x == x && p.y == y){
			if(iPushStratch) p.length--;//短く
			else p.length = 0; //頭を...オプションだったら短くするのではなく消す。
			if(p.length == 0){
				if(p.from is null) info.tdata[track].note_list = p.to;
				else p.from.to = p.to;
				if(p.to !is null) p.to.from = p.from;
			}
			return true;
		}
		return false;
	}
	//どのトラックにある音符か？の検索(返り血はトラック)
	int SearchNote(long x, ubyte y, int YuusenTrack){
		NOTELIST* p; //リストを指すポインター
		//頭から検索
		int i;
		int is_, ie;

		p = info.tdata[YuusenTrack].note_list;
		while(p !is null && !(p.x <= x && x < (p.x+p.length)))p = p.to;
		if(p !is null){
			//hit!!
			if((p.x <= x && x < (p.x+p.length)) &&  p.y == y){
				return YuusenTrack;
			}
		}

		if(track < MAXDRAM){
			is_ -= 0;
			ie = MAXDRAM;
		}else{
			is_ = MAXDRAM; ie = MAXTRACK;
		}
		for(i=ie-1; i>=is_; i--){
			if(info.tdata[i].note_list !is null){
				p = info.tdata[i].note_list;
				//while(p !is null && p.x < x)p = p.to;
				while(p !is null && !(p.x <= x && x < (p.x+p.length)))p = p.to;
				if(p !is null){
					//hit!!
					if((p.x <= x && x < (p.x+p.length)) &&  p.y == y){
						return i;
					}
				}
			}
		}
		return -1;
	}
	//どのトラックにある音符か？の検索(返り血はトラック)
	//上と同じだが、該当する音符の最初と最後をポインタで返す。↓  と       ↓
	//YuusenTrack>=0のとき、そのトラックを優先して検索する。
	int SearchNoteB(long x, ubyte y, long* lStartx, long* lLastx, int YuusenTrack){
		NOTELIST* p;//リストを指すポインター
		//頭から検索
		int i;
		int is_, ie;
		if(YuusenTrack>=0){
			p = info.tdata[YuusenTrack].note_list;
			while(p !is null && !(p.x <= x && x < (p.x+p.length)))p = p.to;
			if(p !is null){
				//hit!!
				if((p.x <= x && x < (p.x+p.length)) &&  p.y != KEYDUMMY){
					*lStartx = p.x;
					*lLastx  = p.x + p.length - 1;
					return YuusenTrack;
				}
			}
		}
		if(track<MAXDRAM){
			is_ = 0; ie = MAXDRAM;
		}else{
			is_ = MAXDRAM; ie = MAXTRACK;
		}
		for(i=ie-1; i>=is_; i--){
			if(info.tdata[i].note_list !is null){
				p = info.tdata[i].note_list;
				//while(p !is null && p.x < x)p = p.to;
				while(p !is null && !(p.x <= x && x < (p.x+p.length)))p = p.to;
				if(p !is null){
					//hit!!
					if((p.x <= x && x < (p.x+p.length)) &&  p.y == y){
						*lStartx = p.x;
						*lLastx  = p.x + p.length - 1;
						return i;
					}
				}
			}
		}
		return -1;
	}

	//縮小表示時の検索。
	int SearchNoteC(long x, ubyte y, long xWidth, long xMod){
		NOTELIST* p;//リストを指すポインター
		//頭から検索
		int i;
		
		for(p = info.tdata[track].note_list; p !is null ; p = p.to){
			//if(p !is null && p.y == y){	// 2014.05.28 D
			if(p !is null){	// 2014.05.28 A
				for(i = 0; i <= xWidth; i++){
					if( p.x == x - i && x < (p.x + p.length) ){
						return i;
					}
				}
				if(p.x > x + xWidth)break;
			}
		}
		
		return -1;
	}

	//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	//以下は音符（パン）の配置、削除
	//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	//音符を配置(左クリックの処理)
	bool SetPan(long x, ubyte y){
		NOTELIST* note;//生成NOTE
		NOTELIST* p;//リストを指すポインター
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null)return false;
		//初音符ならリストに登録
		if(info.tdata[track].note_list is null){
			info.tdata[track].note_list = note;
			note.from = null;
			note.to = null;
			note.length = 1;
			note.pan = y;
			note.volume = VOLDUMMY;
			note.x = x;
			note.y = KEYDUMMY;
			return true;
		}
		//頭から検索
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		//挿入
		if(p.x > x){
			note.to = p;
			note.from = p.from;
			if(p.from is null){
				info.tdata[track].note_list = note;//先頭
			}
			else p.from.to = note;//じゃなければ前のやつの次に
			p.from = note;
			note.length = 1;
			note.pan = y;
			note.volume = 255;
			note.x = x;
			note.y = KEYDUMMY;
		}
		//パラメータ変更
		else if(p.x == x){
			p.pan = y;//Ｙ変更
		}
		//最後尾追加
		else if(p.to is null){
			note.from = p;
			p.to = note;
			note.to = null;
			note.length = 1;
			note.pan = y;
			note.volume = VOLDUMMY;
			note.x = x;
			note.y = KEYDUMMY;
		}
		return true;
	}
	
	bool SetPan2(long x, ubyte y){
		NOTELIST* note;//生成NOTE
		NOTELIST* p;//リストを指すポインター
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null)return false;
		//初音符ならリストに登録
		if(info.tdata[track].note_list is null){
			return false;
		}
		//頭から検索
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		//挿入
		if(p.x == x){
			if(p.pan != y){ // 2010.08.14 A
				p.pan = y;//Ｙ変更
			}else{
				return false;	// 2010.08.14 A
			}
		}
		return true;
	}
	
	///音符のカット(右クリックの処理)
	bool CutPan(long x, ubyte y){
		NOTELIST* p;//リストを指すポインター
		//頭から検索
		if(info.tdata[track].note_list is null) return false;
		p = info.tdata[track].note_list;
		while(p !is null && p.x < x)p = p.to;
		if(p is null)return false;
		//パラメータ変更
		if(p.x == x){
			p.length = 0;
			if(p.from is null)info.tdata[track].note_list = p.to;
			else p.from.to = p.to;
			if(p.to !is null) p.to.from = p.from;
		}
		return true;
	}
	
	/**
	■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	以下は音符（ボリューム）の配置、削除
	■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
	音符を配置(左クリックの処理)
	*/
	bool SetVolume(long x, ubyte y){
		NOTELIST* note;//生成NOTE
		NOTELIST* p;//リストを指すポインター
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null) return false;
		//初音符ならリストに登録
		if(info.tdata[track].note_list is null){
			info.tdata[track].note_list = note;
			note.from = null;
			note.to = null;
			note.length = 1;
			note.pan = PANDUMMY;
			note.volume = y;
			note.x = x;
			note.y = KEYDUMMY;
			return true;
		}
		//頭から検索
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		//挿入
		if(p.x > x){
			note.to = p;
			note.from = p.from;
			if(p.from is null){
				info.tdata[track].note_list = note;//先頭
			}
			else p.from.to = note;//じゃなければ前のやつの次に
			p.from = note;
			note.length = 1;
			note.pan = PANDUMMY;
			note.volume = y;
			note.x = x;
			note.y = KEYDUMMY;
	//		if(note.from !is null && note.x < note.from.x + note.from.length)
	//			note.length = (ubyte)(note.from.length - (note.x - note.from.x));
		}else if(p.x == x){ //パラメータ変更
			if(p.volume != y){	// 2010.08.14 A
				p.volume = y;//Ｙ変更
			}else{
				return false;
			}
		}else if(p.to is null){ //最後尾追加
			note.from = p;
			p.to = note;
			note.to = null;
			note.length = 1;
			note.pan = PANDUMMY;
			note.volume = y;
			note.x = x;
			note.y = KEYDUMMY;
	//		if(note.from !is null && note.x < note.from.x + note.from.length)
	//			note.length = (ubyte)(note.from.length - (note.x - note.from.x));
		}
		return true;
	}
	
	bool SetVolume2(long x, ubyte y, long fade){
		NOTELIST* note;//生成NOTE
		NOTELIST* p;//リストを指すポインター
		ubyte lastlength = 1;
		ubyte vv;
		double dv;
		long lastx = 0;
		int i;
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null) return false;
		//初音符ならリストに登録
		if(info.tdata[track].note_list is null){
			info.tdata[track].note_list = note;
			note.from = null;
			note.to = null;
			note.length = 1;
			note.pan = PANDUMMY;
			note.volume = y;
			note.x = x;
			note.y = KEYDUMMY;
			return true;
		}
		//頭から検索
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			if(p.y != KEYDUMMY){
				lastx = p.x;
				lastlength = p.length;
			}
			p = p.to;
		}
		//挿入
		if(p.x > x){
			note.to = p;
			note.from = p.from;
			if(p.from is null){
				info.tdata[track].note_list = note;//先頭
			}
			else p.from.to = note;//じゃなければ前のやつの次に
			p.from = note;
			note.length = 1;
			note.pan = PANDUMMY;
			switch(fade){
			case 0: //直線的減衰
				dv = (p.x - lastx) / cast(double)lastlength;
				dv = 200 - dv * 200;
				if(dv < 0) dv = 0;
				else if(dv > 255) dv = 255;
				vv = cast(ubyte)dv;
				break;
			case 1: //すぐに減衰
				dv = 200f;
				for(i=0;i<(p.x - lastx);i++){
					dv = dv / 2f;
				}
				if(dv < 0) dv = 0;
				else if(dv > 255) dv = 255;
				vv = cast(ubyte)dv;
				break;
			case 2: //ゆっくりと減衰
			case 3: //ゆっくりと減衰(ビブラート）
				dv = sqrt(sqrt(sqrt((lastlength-cast(double)p.x) / cast(double)lastlength))) * 200;
				i = (p.x - lastx) % 2;
				if(i==1 && fade==3)dv = dv - 10.0;
				if(dv < 0)dv = 0; else if(dv > 255)dv = 255;
				vv = cast(ubyte)dv;
				break;
			}
			note.volume = vv;
			note.x = x;
			note.y = KEYDUMMY;
	//		if(note.from !is null && note.x < note.from.x + note.from.length)
	//			note.length = (ubyte)(note.from.length - (note.x - note.from.x));
		}
		//パラメータ変更
		else if(p.x == x){
			if(p.volume != y){	// 2010.08.14 A
				p.volume = y;//Ｙ変更
			}else{
				return false;
			}
		}
		//最後尾追加
		else if(p.to is null){
			note.from = p;
			p.to = note;
			note.to = null;
			note.length = 1;
			note.pan = PANDUMMY;
			note.volume = y;
			note.x = x;
			note.y = KEYDUMMY;
	//		if(note.from !is null && note.x < note.from.x + note.from.length)
	//			note.length = (ubyte)(note.from.length - (note.x - note.from.x));
		}
		return true;
	}
	
	///音符のカット(右クリックの処理)
	bool CutVolume(long x, ubyte y){
		NOTELIST* p; //リストを指すポインター
		//頭から検索
		if(info.tdata[track].note_list is null)return false;
		p = info.tdata[track].note_list;
		while(p !is null && p.x < x)p = p.to;
		if(p is null)return false;
		//パラメータ変更
		if(p.x == x){
			p.length = 0;
			if(p.from is null)info.tdata[track].note_list = p.to;
			else p.from.to = p.to;
			if(p.to !is null) p.to.from = p.from;
		}else{
			return false; //2010.08.14 A
		}
		return true;
	}
	
	///■■■■■■■■■■■■■■■■■■■■■■■■
	void InitOrgData(){
	//	MUSICINFO mi;
		track = 0;
		info.alloc_note = ALLOCNOTE; //とりあえず10000個確保
		info.dot = 4;
		info.line = 4;
		info.wait = 128;
		info.repeat_x = info.dot*  info.line*  0;
		info.end_x = info.dot*  info.line*  255;
		int i;
		for(i = 0; i < MAXTRACK; i++){
			info.tdata[i].freq = 1000;
			info.tdata[i].wave_no = 0;
			info.tdata[i].pipi = 0;
			def_pan[i] = DEFPAN;
			def_volume[i] = DEFVOLUME;
		}
		SetMusicInfo(&info, SETALL);
		
		// 以下 2014.05.07 追加
		for(i=0; i<MAXMELODY; i++){
			info.tdata[i].wave_no = i*11;
			MakeOrganyaWave(i, info.tdata[i].wave_no, info.tdata[i].pipi);
		}
		info.tdata[ 8].wave_no = 39;
		info.tdata[ 9].wave_no = 32;
		info.tdata[10].wave_no = 35;
		info.tdata[11].wave_no = 34;
		info.tdata[12].wave_no = 20;
		info.tdata[13].wave_no = 33;
		info.tdata[14].wave_no = 22;
		info.tdata[15].wave_no = 40;
		for(i = MAXMELODY; i < MAXTRACK; i++){
			InitDramObject(dram_name[info.tdata[i].wave_no], i-MAXMELODY);
		}
	}
	
	void GetNoteUsed(long* use, long* left, byte track){
		NOTELIST* np;
		long u, l;
		int i;
		u = 0;
		l = 0;
		np = info.tdata[track].note_p;
		for(i = 0; i < ALLOCNOTE; i++){
			if(np.length)u++;
			else l++;
			np++;
		}
		*use = u;
		*left = l;
	}
	
	///コンストラクタ
	this(){
		int i;
		for(i = 0; i < MAXTRACK; i++){
			info.tdata[i].note_list = null;
			info.tdata[i].note_p = null;
		}
		CurrentUndoCursor = 0;
		MinimumUndoCursor = 0;
		MaximumUndoCursor = 0;
		RedoEnable = false;
		UndoEnable = false;
		//noteon = new ubyte[65536];
	}
	
	///デストラクタ
	~this(){
		//delete [] noteon;
	}
	
	int SetUndoData(){
		int j, cc;
		cc = (CurrentUndoCursor % 32);
		memcpy(&ud_tdata[cc], &info, MUSICINFO.sizeof);
		for(j=0;j<16;j++){
			memcpy(&ud_note[cc][j], info.tdata[j].note_p, NOTELIST.sizeof*4096);
		}
		CurrentUndoCursor++;
		MaximumUndoCursor = CurrentUndoCursor;
		if(CurrentUndoCursor - 32+1 > MinimumUndoCursor) MinimumUndoCursor = CurrentUndoCursor - 32+1;
		
		if(!UndoEnable){
			UndoEnable = true;
			return 1;
		}
		if(RedoEnable){
			RedoEnable = false;
			return 2;
		}
		return 0;
	}
	
	void ClearUndoData(){
		CurrentUndoCursor = 0;
		MinimumUndoCursor = 0;
		MaximumUndoCursor = 0;
		RedoEnable = false;
		UndoEnable = false;
	}
	
	///アンドゥから復帰
	int ReplaceFromUndoData(){
		if(!UndoEnable)return 1;
		if(MaximumUndoCursor == CurrentUndoCursor){
			SetUndoData();
			CurrentUndoCursor--;
		}
		int j, cc, r;
		CurrentUndoCursor--;
		r=0;
		RedoEnable = true;
		if(CurrentUndoCursor <= MinimumUndoCursor){
			CurrentUndoCursor = MinimumUndoCursor;
			r=2; //もうこれ以上元に戻せないぞ。
			UndoEnable = false;
		}
		cc = (CurrentUndoCursor % 32);
		memcpy(&info, &ud_tdata[cc],  MUSICINFO.sizeof);
		for(j=0;j<16;j++){
			memcpy(info.tdata[j].note_p, &ud_note[cc][j], NOTELIST.sizeof*4096);
		}
		return r;
	}
	
	///リドゥ
	int ReplaceFromRedoData(){
		if(!RedoEnable)return 1;
		int j, cc, r;
		CurrentUndoCursor++;
		r=0;
		UndoEnable = true;
		if(CurrentUndoCursor >= MaximumUndoCursor-1){
			CurrentUndoCursor = MaximumUndoCursor-1;
			r=2; //もうこれ以上元に戻せないぞ。
			RedoEnable = false;
		}
		cc = (CurrentUndoCursor % 32);
		memcpy(&info, &ud_tdata[cc],  sizeof(MUSICINFO));
		for(j=0;j<16;j++){
			memcpy(info.tdata[j].note_p, &ud_note[cc][j],  sizeof(NOTELIST)*4096);
		}
		return r;

	}
	
	///最後のアンドゥはなかったことに！（空クリックだったとか）
	int ResetLastUndo(){
		int r = 0;
		CurrentUndoCursor--;
		MaximumUndoCursor = CurrentUndoCursor;
		if(CurrentUndoCursor <= MinimumUndoCursor){
			CurrentUndoCursor = MinimumUndoCursor;
			r=2; //もうこれ以上元に戻せないぞ。
			UndoEnable = false;
		}
		return r;
	}
	
	///最後の判定をしない
	bool SetNote_onlyLength(long x, long Length){
		NOTELIST* note; //生成NOTE
		NOTELIST* p; //リストを指すポインター
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null) return false;
		if(info.tdata[track].note_list is null) return false;
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		if(p.x == x){
			if(Length<MAXNOTELENGTH){
				if(p.length != cast(byte)Length){ //A 2010.08.14 
					p.length = cast(byte)Length;
				}else{
					return false; //A 2010.08.14 
				}
			}
		}
		return true;
	}
	
	bool SetNote_afterSetLength(long x){
		NOTELIST* note; //生成NOTE
		NOTELIST* p; //リストを指すポインター
		NOTELIST* cut_p; //それを置く事によってカットすべき音符
		//未使用NOTEを検索
		if((note = SearchNote(info.tdata[track].note_p)) is null)return false;
		if(info.tdata[track].note_list is null)return false;
		p = info.tdata[track].note_list;
		while(p.x < x && p.to !is null){
			p = p.to;
		}
		if(p.x == x){
			cut_p = p.to;
			while(cut_p !is null && cut_p.y == KEYDUMMY){
				cut_p = cut_p.to;
			}
			if(cut_p !is null && p.length + p.x > cut_p.x){ //伸ばしすぎると次のを飲み込む
				if(cut_p.to is null){ //そいつが最後尾なら
					cut_p.from.to = null; //一つ前の人に最後尾を任せる。
				}else{
					cut_p.from.to = cut_p.to;
					cut_p.to.from = cut_p.from;
				}
				cut_p.length = 0;
				cut_p.from = null;
			}

		}
		return true;

	}

}
OrgData org_data;

enum DEFVOLUME = 200; //255はVOLDUMMY。MAXは254
enum DEFPAN = 6;
SDL_Window hDlgTrack;
ubyte[][] dram_name;
