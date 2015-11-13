#ifndef  _GSENSOR_H_
#define  _GSENSOR_H_

/*******************************************/
//�������
/*******************************************/
typedef	  struct
{
	    struct
		{
			   float xa;//x����ٶ�
			   float ya;//y����ٶ�
			   float za;//z����ٶ�
		}data;

}GFRAMEIN;

/*******************************************/
//�������
/*******************************************/

typedef struct
{
	    struct
		{
			  int    isStop;//�Ƿ�ֹͣ
			  float  a;     //���ٶ�
			  float  v;     //����		
		}data;

}GSENSOROUT;


/*******************************************/
//��ʼ������
/*******************************************/
typedef struct
{
	    char *version;  //�汾��,����Ҫ���룬���ó�ʼ����������Զ�ȡ
		int  enable;    //�Ƿ���gsensorģ��
}GSENSORIN;

///*******************************************/
typedef void (*GsensorIn)(GFRAMEIN &frameIn,void *dv);
typedef void (*GsensorOut)(GSENSOROUT &gsensorOut,void *dv);
extern GsensorIn  gcallbackIn;
extern GsensorOut gcallbackOut;


//#ifdef __cplusplus
//extern "C" {
//#endif
///*******************************************/
void InitialGsensor(GSENSORIN &gsensorIn, void **dev);
///*******************************************/
void ReleaseGsensor();
/*******************************************/
//#ifdef __cplusplus
//}  /* end of extern "C" */
//#endif
#endif