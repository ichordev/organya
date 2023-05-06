
#define DIRECTION_UP	0
#define DIRECTION_DOWN	1
#define DIRECTION_LEFT	2
#define DIRECTION_RIGHT	3

typedef struct ScrollData{
	private:
		long hpos;//���X�N���[���l
		long vpos;//�c�X�N���[���l
		SCROLLINFO scr_info;
		long vScrollMax;
	public:
		void SetIniFile();
		BOOL InitScroll(void);//������
		void VertScrollProc(WPARAM wParam);//�X�N���[�����������̏���
		void HorzScrollProc(WPARAM wParam);
		void WheelScrollProc(LPARAM lParam, WPARAM wParam); //�z�C�[����...
		void GetScrollPosition(long *hp,long *vp);
		void SetHorzScroll(long x);
		void AttachScroll(void);	//�E�B���h�E�T�C�Y�ύX�ɉ����ăX�N���[�����C��
		void KeyScroll(int iDirection); //�L�[����ɂ��X�N���[���p
		void PrintHorzPosition(void);
		void ChangeVerticalRange(int WindowHeight = -1); //�E�B���h�E�T�C�Y�ɉ����ăX�N���[���o�[Range��ύX
}SCROLLDATA;
extern SCROLLDATA scr_data;//�X�N���[���f�[�^
