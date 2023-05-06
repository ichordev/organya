/*****							  *****/
/*****   DirectSound�T�|�[�g�֐�   *****/
/*****							  *****/

//#include <dsound.h>

// DirectSound�̊J�n
BOOL InitDirectSound(HWND hwnd);
BOOL LoadSoundObject( char *file_name, int no);
// DirectSound�̏I��
void EndDirectSound(void);
void ReleaseSoundObject(int no);//�o�b�t�@�̊J��
BOOL InitSoundObject(LPCSTR resname, int no);
// �T�E���h�̍Đ�
void PlaySoundObject(int no, int mode);
void ChangeSoundVolume(int no, long volume);//9999��MAX2195?��ɰ��
void ChangeSoundFrequency(int no, DWORD rate);//300��MAX300��ɰ��
void ChangeSoundPan(int no, long pan);

BOOL MakeSoundObject8(char *wavep,char track, char pipi);

void ChangeOrganFrequency(unsigned char key,char track,DWORD a);//9999��MAX��2195?��ɰ��
void ChangeOrganVolume(int no, long volume,char track);//300��MAX��300��ɰ��
void ChangeOrganPan(unsigned char key, unsigned char pan,char track);//0������6��ɰ��11����
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq);
void PlayOrganObject2(unsigned char key, int mode,char track,DWORD freq);
void PlayOrganKey(unsigned char key,char track,DWORD freq, int Nagasa = 80);
BOOL LoadWaveData100(void);
BOOL InitWaveData100(void);
BOOL DeleteWaveData100(void);

BOOL MakeOrganyaWave(char track,char wave_no, char pipi);

//BOOL LoadDramObject(char *file_name, int no);
BOOL InitDramObject(LPCSTR resname, int no);
void ReleaseDramObject(char track);
void ChangeDramFrequency(unsigned char key,char track);
void ChangeDramPan(unsigned char pan,char track);//512��MAX��256��ɰ��
void ChangeDramVolume(long volume,char track);//300��MAX��300��ɰ��
void PlayDramObject(unsigned char key, int mode,char track);


void Rxo_ShowDirectSoundObject(HWND hwnd);
void Rxo_StopAllSoundNow(void);

void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase);
void Rxo_StopKey(unsigned char key,char track, int Phase);