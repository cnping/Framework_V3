
#ifndef _POST_PROCESS_COM_H_
#define _POST_PROCESS_COM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MaxSpecGroup        (10)

//���������Ϊ1024����������������ٲ��ţ���Ҫ�������������ĵ�ַ����ͬ��
typedef struct __PostProcessSt
{
    short       fadeinoutflag;  //0:normal 1:fade in 2:fade out
    short       VPS;            //-40 - 100, �����ٶ�Ϊ: (1+vps%),���Ա��ٷ�Χ��ԭʼ�ٶȵ�0.6~2��
    short       spectrumflag;   //0 : disable 1:enable
    short       spectrumOutNum;//���Ƶ����
    short       SpectrumOutval[MaxSpecGroup][32];//��ΧΪ 0-20��melis����10�θ߶���ʾ��32��Ƶ�Ρ�1024����������һ��Ƶ�ף�32Ƶ�Σ�
    short       UserEQ[11];     //0-10
    int         channal;        //1:mono 2:stereo
    int         samplerate;

    short       *InputPcmBuf;
    int         InputPcmLen;//����������bufferlenth��

    short       *OutputPcmBuf;
    int         OutputPcmBufTotLen;//outputpcmbuf ���sample����bufferlenth ��2��channels �� OutputPcmBufTotLen
    int         OutputPcmLen;

}PostProcessSt;

extern int AudioEQdo_auPostProc(PostProcessSt *PostProInfo); //0:fail 1:succed

#ifdef __cplusplus
}
#endif

#endif  //_POST_PROCESS_COM_H_
