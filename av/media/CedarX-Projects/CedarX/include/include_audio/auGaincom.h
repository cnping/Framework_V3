
#ifndef	___AUGAINCOM_H__
#define	___AUGAINCOM_H__

typedef	struct ___AudioGain__{
//input para
	int		preamp;//-20 -- 20 db	
	int   InputChan;
	short *InputPtr;
	int		InputLen;//total byte
	
	int   OutputChan;//0 ����� 1: ���������only 2�������������  3: double left 4:double right
	short *OutputPtr;
		

}AudioGain;
int		do_AudioGain(AudioGain	*AGX);
#endif//___AUGAINCOM_H__