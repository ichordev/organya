#include <string.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include <stdio.h>
#include "Sound.h"
#include "resource.h"
#include "Scroll.h"
#include "rxoFunction.h"

//�Ȃ�PATH�͂����ɒu���Ă���
char music_file[MAX_PATH]; //NewData.org
char pass[7] = "Org-01";
char pass2[7] = "Org-02";//Pipi
char pass3[7] = "Org-03";//����Ɋe�퉹�F�ǉ��B

////�ȉ��̓I���K�[�j���ȃf�[�^�\���́i�t�B���Ɋ܂܂�鍀�ځj
typedef struct{
	long x;//�ʒu
	unsigned char y;//���̍���
	unsigned char length;//���̒���
	unsigned char volume;//���H�����[��
	unsigned char pan;//�p��
}ORGANYANOTE;
typedef struct{
	unsigned short freq;//+�����g��(1000��Default)
	unsigned char wave_no;//�g�`No
	unsigned char pipi;//��
	unsigned short note_num;//�����̐�
}ORGANYATRACK;
typedef struct{
	unsigned short wait;
	unsigned char line;
	unsigned char dot;
	long repeat_x;//���s�[�g
	long end_x;//�Ȃ̏I���(���s�[�g�ɖ߂�)
	ORGANYATRACK tdata[MAXTRACK];
}ORGANYADATA;


//���̃g���b�N�Ɏg���Ă��鉹���̐������o
unsigned short OrgData::GetNoteNumber(char track,NOTECOPY *nc)
{
	NOTELIST *np;
	unsigned short num = 0;
	np = info.tdata[track].note_list;
	if(nc == NULL){
		while(np != NULL){
			num++;
			np = np->to;
		}
		return num;
	}else{
		//�͈͂܂łƂ�
		while(np != NULL && np->x < nc->x1_1){
			np = np->to;
		}
		if(np == NULL)return 0;
		//�͈͂𒴂�����(�ő�X�������������o)
		while(np != NULL && np->x <= nc->x1_2){
			num++;
			np = np->to;
		}
		return num;
	}
}
//�ȃf�[�^��ۑ�
BOOL OrgData::SaveMusicData(void)
{
	ORGANYADATA org_data;
	NOTELIST *np;
	int i,j;
	//�L�^�p�ȏ��̐���
	org_data.wait = info.wait;
	org_data.line = info.line;
	org_data.dot = info.dot;
	org_data.repeat_x = info.repeat_x;
	org_data.end_x = info.end_x;

	for(i = 0; i < MAXTRACK; i++){
		org_data.tdata[i].freq = info.tdata[i].freq;
		org_data.tdata[i].wave_no = info.tdata[i].wave_no;
		org_data.tdata[i].pipi = info.tdata[i].pipi;
		org_data.tdata[i].note_num = GetNoteNumber(i,NULL);
	}
	//����������������Z�[�u
	FILE *fp;
	if((fp=fopen(music_file,"wb"))==NULL){
		//MessageBox(hWnd,"�t�@�C���ɃA�N�Z�X�ł��܂���","Error (Save)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR_SAVE,MB_OK);	// 2014.10.19 A
		return(FALSE);
	}
	//�p�X���[�h
	j=2;
	for(i=8;i<15;i++){
		if(org_data.tdata[i].wave_no>=12)j=3;	//�h�����̐V���������g���Ă����Ver.3
	}
	if(j==2)fwrite(&pass2[0], sizeof(char), 6, fp);
	else fwrite(&pass3[0], sizeof(char), 6, fp);
	//�ȏ��̏�������
	fwrite(&org_data, sizeof(ORGANYADATA), 1, fp);
	//�����̕ۑ�
	for(j = 0; j < MAXTRACK; j++){
		if(info.tdata[j].note_list == NULL)continue;
		np = info.tdata[j].note_list;//�w���W
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->x,      sizeof(long), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//�x���W
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->y,      sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//����
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->length, sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//�{�����[��
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->volume, sizeof(unsigned char), 1, fp);
			np = np->to;
		}
		np = info.tdata[j].note_list;//�p��
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fwrite(&np->pan,    sizeof(unsigned char), 1, fp);
			np = np->to;
		}
	}	
	fclose(fp);
	PutRecentFile(music_file);
//	MessageBox(hWnd,"�ۑ����܂���","Message (Save)",MB_OK);
	//��2014.05.06 A
	if(SaveWithInitVolFile != 0){
		AutoSavePVIFile();
	}

	return TRUE;
}
//�ȃf�[�^��ǂݍ���
extern char *dram_name[];
extern HWND hDlgPlayer;

//�t�@�C�������[�h�\�ł����0�ُ�ł����1��Ԃ��B�ް����[�h�͂���Ȃ��B 2014.05.22
int OrgData::FileCheckBeforeLoad(char *checkfile)
{
	FILE *fp;
	char pass_check[6];
	char ver = 0;
	if((fp=fopen(checkfile,"rb"))==NULL){
		//MessageBox(hWnd,"�t�@�C���ɃA�N�Z�X�ł��܂���","Error (Load)",MB_OK);
		return 1;
	}

	fread(&pass_check[0], sizeof(char), 6, fp);
	if( !memcmp( pass_check, pass, 6 ) )ver = 1;
	if( !memcmp( pass_check, pass2, 6 ) )ver = 2;
	if( !memcmp( pass_check, pass3, 6 ) )ver = 2;
	if( !ver ){
		fclose(fp);
		//MessageBox(hWnd,"���̃t�@�C���͎g���܂���","Error (Load)",MB_OK);
		return 1;
	}
	fclose(fp);

	return 0;
}
BOOL OrgData::LoadMusicData(void)
{
	ORGANYADATA org_data;
	NOTELIST *np;
	int i,j;
	char pass_check[6];
	char ver = 0;

	//������������������[�h
	FILE *fp;
	if((fp=fopen(music_file,"rb"))==NULL){
		//MessageBox(hWnd,"�t�@�C���ɃA�N�Z�X�ł��܂���","Error (Load)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR_LOAD,MB_OK);	// 2014.10.19 A
		return(FALSE);
	}
	//�p�X���[�h�`�F�b�N
	fread(&pass_check[0], sizeof(char), 6, fp);
	if( !memcmp( pass_check, pass, 6 ) )ver = 1;
	if( !memcmp( pass_check, pass2, 6 ) )ver = 2;
	if( !memcmp( pass_check, pass3, 6 ) )ver = 2;
	if( !ver ){
		fclose(fp);
		//MessageBox(hWnd,"���̃t�@�C���͎g���܂���","Error (Load)",MB_OK);	// 2014.10.19 D
		msgbox(hWnd,IDS_ERROR_FILE,IDS_ERROR_LOAD,MB_OK);	// 2014.10.19 A
		return FALSE;
	}
//	for(i = 0; i < 4; i++){
//		if(pass[i] != pass_check[i]){
//			MessageBox(hWnd,"���̃t�@�C���͎g���܂���","Error (Load)",MB_OK);
//			fclose(fp);
//			return FALSE;
//		}
//	}
	//�ȏ��̓ǂݍ���
	fread(&org_data, sizeof(ORGANYADATA), 1, fp);

	//�Ȃ̏���ݒ�
	info.wait = org_data.wait;
	info.line = org_data.line;
	info.dot = org_data.dot;
	info.repeat_x = org_data.repeat_x;
	info.end_x = org_data.end_x;
	for(i = 0; i < MAXTRACK; i++){
		info.tdata[i].freq = org_data.tdata[i].freq;
		if( ver == 1 )info.tdata[i].pipi = 0;
		else info.tdata[i].pipi = org_data.tdata[i].pipi;
		info.tdata[i].wave_no = org_data.tdata[i].wave_no;
	}

	//�����̃��[�h
	for(j = 0; j < MAXTRACK; j++){
		//�ŏ��̉�����from��NULL�ƂȂ�
		if(org_data.tdata[j].note_num == 0){
			info.tdata[j].note_list = NULL;
			continue;
		}
		//���X�g�����
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;
		for(i = 1; i < org_data.tdata[j].note_num; i++){
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}
		//�Ō�̉�����to��NULL
		np--;
		np->to = NULL;

		//���e����
		np = info.tdata[j].note_p;//�w���W
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->x,      sizeof(long), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//�x���W
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->y,      sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//����
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->length, sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//�{�����[��
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->volume, sizeof(unsigned char), 1, fp);
			np++;
		}
		np = info.tdata[j].note_p;//�p��
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			fread(&np->pan,    sizeof(unsigned char), 1, fp);
			np++;
		}
	}
	fclose(fp);
	//�f�[�^��L����
	for(j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j,info.tdata[j].wave_no, info.tdata[j].pipi);
	for(j = MAXMELODY; j < MAXTRACK; j++){
		i = info.tdata[j].wave_no;
		InitDramObject(dram_name[i],j-MAXMELODY);
	}
	//�v���C���[�ɕ\��
	char str[32];
	SetPlayPointer(0);//���o��
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	MakeMusicParts(info.line,info.dot);//�p�[�c�𐶐�
	MakePanParts(info.line,info.dot);
	PutRecentFile(music_file);
	//��2014.05.06 A
	if(SaveWithInitVolFile != 0){
		AutoLoadPVIFile();
	}
	return TRUE;
}

void OrgData::SortNotes()
{
	NOTELIST *pntl,*pNtls,*np;
	ORGANYADATA org_data;
	int i,j;

	org_data.wait = info.wait;
	org_data.line = info.line;
	org_data.dot = info.dot;
	org_data.repeat_x = info.repeat_x;
	org_data.end_x = info.end_x;

	for(i = 0; i < MAXTRACK; i++){
		org_data.tdata[i].freq = info.tdata[i].freq;
		org_data.tdata[i].wave_no = info.tdata[i].wave_no;
		org_data.tdata[i].pipi = info.tdata[i].pipi;
		org_data.tdata[i].note_num = GetNoteNumber(i,NULL);
	}

	pNtls = new NOTELIST[4096]; //�ޔ�p
	
	for(j = 0; j < MAXTRACK; j++){
		if(info.tdata[j].note_list == NULL)continue;
		pntl = pNtls;
		np = info.tdata[j].note_list;//�����̐擪
		for(i = 0; i < org_data.tdata[j].note_num; i++){
			pNtls[i].x = np->x;
			pNtls[i].y = np->y;
			pNtls[i].pan = np->pan;
			pNtls[i].volume = np->volume;
			pNtls[i].length = np->length;
			np = np->to;
		}
		//���X�g�����
		if(true){
			np = info.tdata[j].note_p; //�̈�̐擪
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np + 1);
			np++;
			for(i = 1; i < org_data.tdata[j].note_num; i++){
				np->from = (np - 1);
				np->to = (np + 1);
				np++;
			}
			//�Ō�̉�����to��NULL
			np--;
			np->to = NULL;
			np = info.tdata[j].note_p;//�w���W
			for(i = 0; i < org_data.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}
		}else{ //�����I�ɃT�J�V�ɂ���
			np = info.tdata[j].note_p+4095; //�̈�̐擪
			info.tdata[j].note_list = info.tdata[j].note_p;
			np->from = NULL;
			np->to = (np - 1);
			np--;
			for(i = 1; i < org_data.tdata[j].note_num; i++){
				np->from = (np + 1);
				np->to = (np - 1);
				np--;
			}
			//�Ō�̉�����to��NULL
			np++;
			np->to = NULL;
			np = info.tdata[j].note_p;//�w���W
			for(i = 0; i < org_data.tdata[j].note_num; i++){
				np->x = pNtls[i].x;
				np->y = pNtls[i].y;
				np->length = pNtls[i].length;
				np->pan = pNtls[i].pan;
				np->volume = pNtls[i].volume;
				np++;
			}

		}
	}	
	delete [] pNtls;	// 2014.10.18 �����ǉ������B
	//�v���C���[�ɕ\��
	char str[32];
	SetPlayPointer(0);//���o��
	scr_data.SetHorzScroll(0);
	itoa(info.wait,str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWWAIT,str);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,"0");
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,"0");

	MakeMusicParts(info.line,info.dot);//�p�[�c�𐶐�
	MakePanParts(info.line,info.dot);

}
