#include <windows.h>
#include <stdio.h>
#include <dplay.h>
#include <dsound.h>
#include "Setting.h"
#include "DefOrg.h"
#define SE_MAX	512

// �V���{����`.
#define	SMPFRQ			48000				//!< �T���v�����O���g��.
#define	BUFSIZE			((SMPFRQ * 4) / 10)	//!< �f�[�^�o�b�t�@�T�C�Y (100ms����).


// DirectSound�\���� 
LPDIRECTSOUND       lpDS = NULL;            // DirectSound�I�u�W�F�N�g
LPDIRECTSOUNDBUFFER lpPRIMARYBUFFER = NULL; // �ꎞ�o�b�t�@
LPDIRECTSOUNDBUFFER lpSECONDARYBUFFER[SE_MAX] = {NULL};
LPDIRECTSOUNDBUFFER lpORGANBUFFER[8][8][2] = {NULL};
LPDIRECTSOUNDBUFFER lpDRAMBUFFER[8] = {NULL};

//�^���p
//HANDLE						CapEvent[2];			//!< ���̓C�x���g�E�I�u�W�F�N�g.
//DWORD						CapBufSize;				//!< �L���v�`���o�b�t�@�E�T�C�Y.
//DWORD						GetPos;					//!< �L���v�`���o�b�t�@�̓ǂݍ��݊J�n�ʒu.
//DWORD						PutPos;					//!< �L���v�`���o�b�t�@�̏������݊J�n�ʒu.
//BYTE*						DataBuff;				//!< �f�[�^�o�b�t�@.
//LPDIRECTSOUNDCAPTURE 		CapDev;					//!< IDirectSoundCapture�C���^�[�t�F�C�X �|�C���^.
//LPDIRECTSOUNDCAPTUREBUFFER	CapBuf;					//!< IDirectSoundBuffer�C���^�[�t�F�C�X �|�C���^.

DWORD						OutBufSize;				//!< �X�g���[���o�b�t�@�E�T�C�Y.


// DirectSound�̊J�n 
BOOL InitDirectSound(HWND hwnd)
{
//    int i;
    DSBUFFERDESC dsbd;

    // DirectDraw�̏�����
    if(DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK){
		lpDS = NULL;
		return(FALSE);
	}
    lpDS->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);

    // �ꎟ�o�b�t�@�̏�����
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER; // | DSBCAPS_CTRLPOSITIONNOTIFY;
    lpDS->CreateSoundBuffer(&dsbd, &lpPRIMARYBUFFER, NULL);

//    for(i = 0; i < SE_MAX; i++) lpSECONDARYBUFFER[i] = NULL;
	
	//�L���v�`���o�b�t�@�̍쐬 ������NULL�Ńf�t�H���g�B����͂ǂ����B
//	if( DirectSoundCaptureCreate( NULL, &CapDev, NULL ) != S_OK ){
//		return FALSE;
//	}
//	dsbd.dwFlags = 0; //�Z�J���_���o�b�t�@
//	CapDev->CreateCaptureBuffer(&dsbd, &CapBuf, NULL);


    return(TRUE);
}

// DirectSound�̏I�� 
void EndDirectSound(void)
{
    int i;

    for(i = 0; i < 8; i++){
        if(lpSECONDARYBUFFER[i] != NULL){
			lpSECONDARYBUFFER[i]->Release();
			lpSECONDARYBUFFER[i] = NULL;
		}
    }
    if(lpPRIMARYBUFFER != NULL){
		lpPRIMARYBUFFER->Release();
		lpPRIMARYBUFFER = NULL;
	}
    if(lpDS != NULL){
		lpDS->Release();
		lpDS = NULL;
	}
//	if( CapBuf ){
//		CapBuf->Stop();
//	}
//    if(CapDev != NULL){
//		CapDev->Release();
//		CapDev = NULL;
//	}
}
void ReleaseSoundObject(int no){
	if(lpSECONDARYBUFFER[no] != NULL){
		lpSECONDARYBUFFER[no]->Release();
		lpSECONDARYBUFFER[no] = NULL;
	}
}


// �T�E���h�̐ݒ� 
BOOL InitSoundObject( LPCSTR resname, int no)
{
    HRSRC hrscr;
    DSBUFFERDESC dsbd;
    DWORD *lpdword;//���\�[�X�̃A�h���X
    // ���\�[�X�̌���
    if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // ���\�[�X�̃A�h���X���擾
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	// �񎟃o�b�t�@�̐���
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);//WAVE�f�[�^�̃T�C�Y
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK) return(FALSE);
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // �񎟃o�b�t�@�̃��b�N
    lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36),
                        &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// �����f�[�^�̐ݒ�
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3a, dwbuf1);
    if(dwbuf2 != 0) CopyMemory(lpbuf2, (BYTE*)lpdword+0x3a+dwbuf1, dwbuf2);
	// �񎟃o�b�t�@�̃��b�N����
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

    return(TRUE);
}

//extern LPDIRECTDRAW            lpDD;	// DirectDraw�I�u�W�F�N�g
BOOL LoadSoundObject(char *file_name, int no)
{
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//���l�m�F�p
//		lpDD->FlipToGDISurface(); //���b�Z�[�W�\���̕��Ƀt���b�v
//		sprintf(msg_str,"%s����������܂���",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // �J�[�\������
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//�t�@�C���̃��[�N�X�y�[�X�����
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//�Z�J���_���o�b�t�@�̐���
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|DSBCAPS_STICKYFOCUS|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVE�f�[�^�̃T�C�Y
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK){
		free(wp);
		return(FALSE);	
	}
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpSECONDARYBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK){
		free(wp);
		return (FALSE);
	}
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3a�̓f�[�^�̓�
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);
}

// �T�E���h�̍Đ� 
void PlaySoundObject(int no, int mode)
{
    if(lpSECONDARYBUFFER[no] != NULL){
		switch(mode){
		case 0: // ��~
			lpSECONDARYBUFFER[no]->Stop();
			break;
		case 1: // �Đ�
			lpSECONDARYBUFFER[no]->Stop();
			lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
            lpSECONDARYBUFFER[no]->Play(0, 0, 0);
            break;
		case -1: // ���[�v�Đ�
			lpSECONDARYBUFFER[no]->Play(0, 0, DSBPLAY_LOOPING);
			break;
		}
    }
}

void ChangeSoundFrequency(int no, DWORD rate)//100��MIN9999��MAX��2195?��ɰ��
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetFrequency( rate );
}
void ChangeSoundVolume(int no, long volume)//300��MAX��300��ɰ��
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetVolume((volume-300)*8);
}
void ChangeSoundPan(int no, long pan)//512��MAX��256��ɰ��
{
	if(lpSECONDARYBUFFER[no] != NULL)
		lpSECONDARYBUFFER[no]->SetPan((pan-256)*10);
}

/////////////////////////////////////////////
//���I���K�[�j��������������������������///////
/////////////////////


typedef struct{
	short wave_size;//�g�`�̃T�C�Y
	short oct_par;//�I�N�^�[�u����������|����(/8)
	short oct_size;//�I�N�^�[�u����������|����(/8)
}OCTWAVE;

OCTWAVE oct_wave[8] = {
	{256,  1, 4},//0 Oct
	{256,  2, 8},//1 Oct
	{128,  4, 12},//2 Oct
	{128,  8, 16},//3 Oct
	{ 64, 16, 20},//4 Oct
	{ 32, 32, 24},
	{ 16, 64, 28},
	{  8,128, 32},
};
BYTE format_tbl2[] = {0x01,0x00,0x01,0x00,0x22,0x56,0x00,//22050Hz��Format
0x00,0x22,0x56,0x00,0x00,0x01,0x00,0x08,0x00,0x00,0x00};
//BYTE format_tbl3[] = {0x01,0x00,0x01,0x00,0x44,0xac,0x00,//441000Hz��Format
//0x00,0x44,0xac,0x00,0x00,0x08,0x00,0x00,0x00,0x66,0x61};
BOOL MakeSoundObject8(char *wavep,char track, char pipi )
{
	DWORD i,j,k;
	unsigned long wav_tp;//WAV�e�[�u���������|�C���^
	DWORD wave_size;//256;
	DWORD data_size;
	BYTE *wp;
	BYTE *wp_sub;
	int work;
	//�Z�J���_���o�b�t�@�̐���
	DSBUFFERDESC dsbd;

	for(j = 0; j < 8; j++){
		for(k = 0; k < 2; k++){
			wave_size = oct_wave[j].wave_size;
			if( pipi )data_size = wave_size * oct_wave[j].oct_size;
			else data_size = wave_size;
			ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
			dsbd.dwSize = sizeof(DSBUFFERDESC);
			dsbd.dwFlags = DSBCAPS_STATIC|
					DSBCAPS_STICKYFOCUS|
					DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;//DSBCAPS_STATIC|DSBCAPS_CTRLDEFAULT;
			dsbd.dwBufferBytes = data_size;//file_size;
			dsbd.lpwfxFormat = (LPWAVEFORMATEX)(&format_tbl2[0]);
				if(lpDS->CreateSoundBuffer(&dsbd, &lpORGANBUFFER[track][j][k],//j = se_no
										NULL) != DS_OK) return(FALSE);
			wp = (BYTE*)malloc(data_size);//�t�@�C���̃��[�N�X�y�[�X�����
			wp_sub = wp;
			wav_tp = 0;
			for(i = 0; i < data_size; i++){
				work = *(wavep+wav_tp);
				work+=0x80;
				*wp_sub=(BYTE)work;
				wav_tp += 256/wave_size;
				if( wav_tp > 255 ) wav_tp -= 256;
				wp_sub++;
			}
			//�f�[�^�̓]��
			LPVOID lpbuf1, lpbuf2;
			DWORD dwbuf1, dwbuf2=0;
			HRESULT hr;
			hr = lpORGANBUFFER[track][j][k]->Lock(0, data_size,//-58,
								&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);		
			if(hr != DS_OK){
				free( wp );
				return (FALSE);
			}
			CopyMemory(lpbuf1, (BYTE*)wp,dwbuf1);//+3a�̓f�[�^�̓�
			if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+dwbuf1, dwbuf2);
			lpORGANBUFFER[track][j][k]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
			lpORGANBUFFER[track][j][k]->SetCurrentPosition(0);
			free( wp );
		}
	}
	return(TRUE);
}
//2.1.0�� �����^���珬���_�^�ɕύX���Ă݂��B20140401
//short freq_tbl[12] = {261,278,294,311,329,349,371,391,414,440,466,494};
double freq_tbl[12] = {261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612};
void ChangeOrganFrequency(unsigned char key,char track, DWORD a)
{
	double tmpDouble;
	for(int j = 0; j < 8; j++)
		for(int i = 0; i < 2; i++){
			tmpDouble = (((double)oct_wave[j].wave_size * freq_tbl[key])*(double)oct_wave[j].oct_par)/8.00f + ((double)a - 1000.0f);
			
			
			lpORGANBUFFER[track][j][i]->SetFrequency(//1000��+���̃f�t�H���g�l�Ƃ���
				(DWORD)tmpDouble
//				((oct_wave[j].wave_size*freq_tbl[key])*oct_wave[j].oct_par)/8 + (a-1000)
			);
		}
}
short pan_tbl[13] = {0,43,86,129,172,215,256,297,340,383,426,469,512}; 
unsigned char old_key[MAXTRACK] = {255,255,255,255,255,255,255,255};//�Đ����̉�
unsigned char key_on[MAXTRACK] = {0};//�L�[�X�C�b�`
unsigned char key_twin[MAXTRACK] = {0};//���g���Ă���L�[(�A�����̃m�C�Y�h�~�ׂ̈ɓ�p��)
void ChangeOrganPan(unsigned char key, unsigned char pan,char track)//512��MAX��256��ɰ��
{
	if(old_key[track] != 255)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetPan((pan_tbl[pan]-256)*10);
}
void ChangeOrganVolume(int no, long volume,char track)//300��MAX��300��ɰ��
{
	if(old_key[track] != 255)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetVolume((volume-255)*8);
}
// �T�E���h�̍Đ� 
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq)
{
	
    if(lpORGANBUFFER[track][key/12][key_twin[track]] != NULL){
		switch(mode){
		case 0: // ��~
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Stop();
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetCurrentPosition(0);
			break;
		case 1: // �Đ�
//			if(key_on == 1 && no == old_key/12)//
//				lpORGANBUFFER[old_key/12]->Stop();
//				ChangeOrganFrequency(key%12);//���g����ݒ肵��
//				lpORGANBUFFER[no]->Play(0, 0, 0);
//			if(key_on == 1 && no == old_key/12){//���Ă�WAV������WAVNO�Ȃ�
//				old_key = key;
//				ChangeOrganFrequency(key%12);//���g����ς��邾��
//			}
			break;
		case 2: // ��������~
			if(old_key[track] != 255){
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				old_key[track] = 255;
			}
            break;
		case -1:
			if(old_key[track] == 255){//�V�K�炷
				ChangeOrganFrequency(key%12,track,freq);//���g����ݒ肵��
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
				old_key[track] = key;
				key_on[track] = 1;
			}else if(key_on[track] == 1 && old_key[track] == key){//������
				//���Ȃ��Ă���̂��������~
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
			}else{//�Ⴄ����炷�Ȃ�
				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);//���Ȃ��Ă���̂��������~
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				ChangeOrganFrequency(key%12,track,freq);//���g����ݒ肵��
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, DSBPLAY_LOOPING);
				old_key[track] = key;
			}
			break;
		}
    }
}
//�҂�
void PlayOrganObject2(unsigned char key, int mode,char track,DWORD freq)
{
	
    if(lpORGANBUFFER[track][key/12][key_twin[track]] != NULL){
		switch(mode){
		case 0: // ��~
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Stop();
			lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetCurrentPosition(0);
			break;
		case 1: // �Đ�
//			if(key_on == 1 && no == old_key/12)//
//				lpORGANBUFFER[old_key/12]->Stop();
//				ChangeOrganFrequency(key%12);//���g����ݒ肵��
//				lpORGANBUFFER[no]->Play(0, 0, 0);
//			if(key_on == 1 && no == old_key/12){//���Ă�WAV������WAVNO�Ȃ�
//				old_key = key;
//				ChangeOrganFrequency(key%12);//���g����ς��邾��
//			}
			break;
		case 2: // ��������~
			if(old_key[track] != 255){
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				old_key[track] = 255;
			}
            break;
		case -1:
			if(old_key[track] == 255){//�V�K�炷
				ChangeOrganFrequency(key%12,track,freq);//���g����ݒ肵��
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
				old_key[track] = key;
				key_on[track] = 1;
			}else if(key_on[track] == 1 && old_key[track] == key){//������
				//���Ȃ��Ă���̂��������~
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
			}else{//�Ⴄ����炷�Ȃ�
//				lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(0, 0, 0);//���Ȃ��Ă���̂��������~
				key_twin[track]++;
				if(key_twin[track] == 2)key_twin[track] = 0; 
				ChangeOrganFrequency(key%12,track,freq);//���g����ݒ肵��
				lpORGANBUFFER[track][key/12][key_twin[track]]->Play(0, 0, 0);//DSBPLAY_LOOPING);
				old_key[track] = key;
			}
			break;
		}
    }
}
//�I���K�[�j���I�u�W�F�N�g���J��
void ReleaseOrganyaObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpORGANBUFFER[track][i][0] != NULL){
			lpORGANBUFFER[track][i][0]->Release();
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if(lpORGANBUFFER[track][i][1] != NULL){
			lpORGANBUFFER[track][i][1]->Release();
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}
//�g�`�f�[�^�����[�h
//char wave_data[100*256];
char *wave_data = NULL;
BOOL InitWaveData100(void)
{
	if(wave_data == NULL)wave_data = (char *)malloc(sizeof(char) * 256 * 256);
    HRSRC hrscr;
    DWORD *lpdword;//���\�[�X�̃A�h���X
    // ���\�[�X�̌���
    if((hrscr = FindResource(NULL, "WAVE100", "WAVE")) == NULL)
                                                    return(FALSE);
    // ���\�[�X�̃A�h���X���擾
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	memcpy(wave_data,lpdword,100*256);
	return TRUE;
}
BOOL LoadWaveData100(void)
{
	if(wave_data == NULL)wave_data = (char *)malloc(sizeof(char) * 256 * 256);
	FILE *fp;
	if((fp=fopen("Wave.dat","rb"))==NULL){
		return FALSE;
	}
//	wave_data = new char[100*256];
	fread(wave_data, sizeof(char), 256*100, fp);
	fclose(fp);
	return TRUE;
}
BOOL DeleteWaveData100(void)
{
//	delete wave_data;
	free(wave_data);
	return TRUE;
}
//�g�`���P�O�O�̒�����I�����č쐬
BOOL MakeOrganyaWave(char track,char wave_no, char pipi)
{
	if(wave_no > 99)return FALSE;
	ReleaseOrganyaObject(track);
	MakeSoundObject8(&wave_data[0] + wave_no*256,track, pipi);	
	return TRUE;
}
/////////////////////////////////////////////
//���I���K�[�j���h�����X����������������///////
/////////////////////
//�I���K�[�j���I�u�W�F�N�g���J��
void ReleaseDramObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpDRAMBUFFER[track] != NULL){
			lpDRAMBUFFER[track]->Release();
			lpDRAMBUFFER[track] = NULL;
		}
	}
}
// �T�E���h�̐ݒ� 
BOOL InitDramObject( LPCSTR resname, int no)
{
    HRSRC hrscr;
    DSBUFFERDESC dsbd;
    DWORD *lpdword;//���\�[�X�̃A�h���X
    // ���\�[�X�̌���
	ReleaseDramObject(no); //�����ɂ����Ă݂��B

    if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // ���\�[�X�̃A�h���X���擾
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	// �񎟃o�b�t�@�̐���
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);//WAVE�f�[�^�̃T�C�Y
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],NULL) != DS_OK) return(FALSE);
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // �񎟃o�b�t�@�̃��b�N
    lpDRAMBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36),
                        &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// �����f�[�^�̐ݒ�
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3a, dwbuf1);
    if(dwbuf2 != 0){
		CopyMemory(lpbuf2, (BYTE*)lpdword+0x3a+dwbuf1, dwbuf2);
		
		
	}

	// �񎟃o�b�t�@�̃��b�N����
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	lpDRAMBUFFER[no]->SetCurrentPosition(0);

    return(TRUE);
}

/*
BOOL LoadDramObject(char *file_name, int no)
{
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	ReleaseDramObject(no);
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//���l�m�F�p
//		lpDD->FlipToGDISurface(); //���b�Z�[�W�\���̕��Ƀt���b�v
//		sprintf(msg_str,"%s����������܂���",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // �J�[�\������
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//�t�@�C���̃��[�N�X�y�[�X�����
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//�Z�J���_���o�b�t�@�̐���
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|
		DSBCAPS_STICKYFOCUS
		|DSBCAPS_CTRLDEFAULT;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVE�f�[�^�̃T�C�Y
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],
								NULL) != DS_OK) return(FALSE);	
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpDRAMBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK)return (FALSE);
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3a�̓f�[�^�̓�
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);
}*/
void ChangeDramFrequency(unsigned char key,char track)
{
	lpDRAMBUFFER[track]->SetFrequency(key*800+100);
}
void ChangeDramPan(unsigned char pan,char track)//512��MAX��256��ɰ��
{
	lpDRAMBUFFER[track]->SetPan((pan_tbl[pan]-256)*10);
}
void ChangeDramVolume(long volume,char track)//
{
	lpDRAMBUFFER[track]->SetVolume((volume-255)*8);
}
// �T�E���h�̍Đ� 
void PlayDramObject(unsigned char key, int mode,char track)
{
	
    if(lpDRAMBUFFER[track] != NULL){
		switch(mode){
		case 0: // ��~
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			break;
		case 1: // �Đ�
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			ChangeDramFrequency(key,track);//���g����ݒ肵��
			lpDRAMBUFFER[track]->Play(0, 0, 0);
			break;
		case 2: // ��������~
            break;
		case -1:
			break;
		}
    }
}
void PlayOrganKey(unsigned char key,char track,DWORD freq,int Nagasa)
{
	if(key>96)return;
	if(track < MAXMELODY){
		DWORD wait = timeGetTime();
		ChangeOrganFrequency(key%12,track,freq);//���g����ݒ肵��
		lpORGANBUFFER[track][key/12][0]->SetVolume((160-255)*8);
		lpORGANBUFFER[track][key/12][0]->Play(0, 0, DSBPLAY_LOOPING);
		do{
		}while(timeGetTime() < wait + (DWORD)Nagasa);
//		lpORGANBUFFER[track][key/12][0]->Play(0, 0, 0); //C 2010.09.23 ������~����B
		lpORGANBUFFER[track][key/12][0]->Stop();
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
		ChangeDramFrequency(key,track - MAXMELODY);//���g����ݒ肵��
		lpDRAMBUFFER[track - MAXMELODY]->SetVolume((160-255)*8);
		lpDRAMBUFFER[track - MAXMELODY]->Play(0, 0, 0);
	}
}


//2010.08.14 A
void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase)
{
	if(key>96)return;
	if(track < MAXMELODY){
		ChangeOrganFrequency(key%12,track,freq);
		lpORGANBUFFER[track][key/12][Phase]->SetVolume((160-255)*8);
		lpORGANBUFFER[track][key/12][Phase]->Play(0, 0, DSBPLAY_LOOPING);
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
		ChangeDramFrequency(key,track - MAXMELODY);//���g����ݒ肵��
		lpDRAMBUFFER[track - MAXMELODY]->SetVolume((160-255)*8);
		lpDRAMBUFFER[track - MAXMELODY]->Play(0, 0, 0);
	}
}
//2010.08.14 A
void Rxo_StopKey(unsigned char key,char track, int Phase)
{
	if(track < MAXMELODY){
		//lpORGANBUFFER[track][key/12][Phase]->Play(0, 0, 0);	// 2010.08.14 D
		lpORGANBUFFER[track][key/12][Phase]->Stop();	// 2010.08.14 A
	}else{
		lpDRAMBUFFER[track - MAXMELODY]->Stop();
		lpDRAMBUFFER[track - MAXMELODY]->SetCurrentPosition(0);
	}	
}

//�f�o�b�O�p�B�����ȏ�Ԃ��o�́B
void Rxo_ShowDirectSoundObject(HWND hwnd)
{
	
}

//���������ɑS���~�߂�
void Rxo_StopAllSoundNow(void)
{
	int i,j,k;
	for(i=0;i<SE_MAX;i++) if(lpSECONDARYBUFFER[i]!=NULL)lpSECONDARYBUFFER[i]->Stop();
	
	for(i=0;i<8;i++){
		for(j=0;j<8;j++)for(k=0;k<2;k++)lpORGANBUFFER[i][j][k]->Stop();
		lpDRAMBUFFER[i]->Stop();
	}
	for(i=0;i<MAXTRACK;i++)old_key[i]=255; //2014.05.02 A ����œ����ςȉ��ɂȂ�Ȃ�����B
}
