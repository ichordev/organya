//Module   : WinTimer.cpp
//include  : mmsystem.h
//Import   : WinMM.lib
//
//Contents : �u�}���`���f�B�A�^�C�}�[�̎g�p���@�v
//

#include <windows.h>  //Win32n API's
#include <windowsx.h> //mmsystem.h
#include "DefOrg.h"
#include "OrgData.h"

//�G���[�`�F�b�N�}�N��
//#define MMInspect(ret)  if((ret) != TIMERR_NOERROR) return FALSE;

/*��������������������������������������������������������������������������������������*/
//�v���g�^�C�v�錾
/*��������������������������������������������������������������������������������������*/

BOOL InitMMTimer();
BOOL StartTimer(DWORD dwTimer);
VOID CALLBACK TimerProc(UINT uTID,UINT uMsg,DWORD dwUser,DWORD dwParam1,DWORD dwParam2);
BOOL QuitMMTimer();

/*��������������������������������������������������������������������������������������*/
//�O���[�o���ϐ�
/*��������������������������������������������������������������������������������������*/
static UINT ExactTime   = 13;//�ŏ����x
static UINT TimerID     = NULL;

/*��������������������������������������������������������������������������������������*/
//�^�C�}�[���x��ݒ肷��B
//���̊֐��̓A�v���P�[�V�������������Ɉ�x�Ăяo���B
/*��������������������������������������������������������������������������������������*/
BOOL InitMMTimer()
{
	TIMECAPS tc;
	MMRESULT ret;

	//�^�C�}�[�̐��x�����擾����
	ret = timeGetDevCaps(&tc,sizeof(TIMECAPS));
	if(ret != TIMERR_NOERROR) return FALSE;
	if(ExactTime < tc.wPeriodMin)ExactTime = tc.wPeriodMin;
	//���̐��x�ŏ���������
	ret = timeBeginPeriod(ExactTime);
	if(ret != TIMERR_NOERROR) return FALSE;
	return TRUE;
}

/*��������������������������������������������������������������������������������������*/
//�^�C�}�[���N������B
//dwTimer   �ݒ肷��^�C�}�[�Ԋu
/*��������������������������������������������������������������������������������������*/
BOOL StartTimer(DWORD dwTimer)
{
	MMRESULT ret = NULL;
	ExactTime = dwTimer;
	//�^�C�}�[�𐶐�����
	TimerID = timeSetEvent
	(
		dwTimer,       //�^�C�}�[����
		10,             //���e�ł���^�C�}�[���x
		(LPTIMECALLBACK)TimerProc, //�R�[���o�b�N�v���V�[�W��
		NULL,          //���[�U�[���R�[���o�b�N�֐���dwUser�ɑ�����l
		TIME_PERIODIC //�^�C�}�[���Ԗ��ɃC�x���g�𔭐�������
	);
	if(ret != TIMERR_NOERROR) return FALSE;
	return TRUE;
}
/*��������������������������������������������������������������������������������������*/
//�^�C�}�[�̃R�[���o�b�N�֐�
/*��������������������������������������������������������������������������������������*/
VOID CALLBACK TimerProc(UINT uTID,UINT uMsg,DWORD dwUser,DWORD dwParam1,DWORD dwParam2)
{
	DWORD dwNowTime;
	dwNowTime = timeGetTime();
	//===================================================================================
	//�����Ƀ��[�U�[��`�̃\�[�X�������B
	//��{�I�Ɋ֐����Ăяo�������ŏ����͑��̊֐��ł���ׂ����낤�B
	//===================================================================================
	org_data.PlayData();
}

/*��������������������������������������������������������������������������������������*/
//�^�C�}�[���\�[�X���J������B
//�A�v���P�[�V�����I�����Ɉ�x�Ăяo���B
/*��������������������������������������������������������������������������������������*/
BOOL QuitMMTimer()
{
	MMRESULT ret;

	if(TimerID != TIMERR_NOERROR)
	{
		//�^�C�}�[���g�p���Ȃ�I��������
		ret = timeKillEvent(TimerID);
		if((ret) != TIMERR_NOERROR) return FALSE;
	}
	//�^�C�}�[���\�[�X���J������
	ret = timeEndPeriod(ExactTime);
	if((ret) != TIMERR_NOERROR) return FALSE;
	return TRUE;
}