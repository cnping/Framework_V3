#ifndef _ADAS_H_
#define _ADAS_H_



typedef struct 
{
	   union
	   {
		       int x;
		       int col;
	   };
	   union
	   {
		       int y;
		       int row;
	   };
}PointC;
typedef struct 
{
	   union
	   {
		       int x;
 		       int col;
	   };
	   union
	   {
		       int y;
		       int row;
	   };
	   union
	   {
		       int width;	
		       int cols;
	   };
	   union
	   {
		       int height;
		       int rows;	
	   };
}RectC;


typedef struct
{
	    union
	    {
		       int width;	
		       int cols;
	    };
	    union
	    {
		       int height;
		       int rows;	
	    };	  	  
}IMGSIZE;

typedef struct
{
	    unsigned char *ptr;
		IMGSIZE imgSize;
}MtxUchar;
typedef	  struct
{
	    unsigned short *ptr;
	    IMGSIZE imgSize;
}MtxUshort;
/*******************************************/
//�������
/*******************************************/
typedef	  struct
{
      int mode;             //ģʽѡ��,mode ==1ʱ��ͣ��⣬��v3֮���������������Ϊ 0
	  MtxUchar imgSmall;    //������ͼ 960*540
	  MtxUchar imgBig;      //������ͼ 1920*1080
	  struct
	  {
		      int     enable;//����GPS�ź�
		      float   speed; //����
	  }gps;
	  struct
	  {   
		      unsigned char isStop; //�����Ƿ�ֹͣ
			  float         a;      //�����ļ��ٶ�
			  float         v;      //�����ĳ���
	  }gsensor;//��v3֮������������������
	  struct
	  {

	  }obd;//OBD������Ԥ��
	  struct
	  {
		      int fcwSensity;//ǰ����ײ�����ȣ�0���ߣ�1���У�2����    Ĭ��ֵΪ1
		      int ldwSensity;//����ƫ�������ȣ�0���ߣ�1���У�2����    Ĭ��ֵΪ1

	  }sensity;

}FRAMEIN;



typedef	  struct
{
      unsigned char angH;      //��ͷ��ˮƽ�ӽ�
	  unsigned char angV;      //��ͷ�Ĵ�ֱ�ӽ�

	  unsigned short widthOri; //ԭʼ��ͼ����
	  unsigned short heightOri;//ԭʼ��ͼ��߶�
}ViewAng;


/*******************************************/
//�������
/*******************************************/
typedef struct
{
	  RectC idx;                   //�ó�����ͼ���ϵ�λ��(���Ͻ�����Ϳ��)
}PLATE;

typedef struct
{
	   int Num;                    //�ж��ٳ���
	   PLATE *plateP;              //���г��Ƶ���Ϣ

	   struct
	   {
		        int   refresh;    //refreshΪ��ʱ�����³��Ƹ���
		        int   isDisp;     //isDisp Ϊ��ʱ����ʾ���Ƹ���
	   }watermark;	//����ˮӡ
}PLATES;

typedef struct
{
	   unsigned char isWarn;     //�ó��Ƿ񱨾�
	   unsigned char color;      //�ó���ҪͿ�ϵ���ɫ��0-��Ϳ��1-��ɫ��2-��ɫ
	   RectC idx;                //�ó���ͼ���ϵ�λ��(���Ͻ�����Ϳ��)
	   float dist;	             //�ó��������ľ���
	   float time;	             //��ó�����ײʱ��
}CAR;

typedef struct
{
	   unsigned char isDisp;     //�Ƿ���ʾ������
	   unsigned char ltWarn;     //�󳵵��߱������ ��128��ѹ��(С������)��255��ѹ�߳����������趨��ʱ��(��������)
	   unsigned char rtWarn;     //�ҳ����߱������ ��128��ѹ��(С������)��255��ѹ�߳����������趨��ʱ��(��������)
	   PointC ltIdxs[2];         //�󳵵�����ͼ���ϵ�λ��(��������ȷ��)
	   PointC rtIdxs[2];         //�ҳ�������ͼ���ϵ�λ��(��������ȷ��)



	   int    colorPointsNum;    //��������״��ָ��ĸ���
	   unsigned char dnColor;    //�·�һ�����ɫ�� 1-����2-��
	   unsigned short *rows;     //��������״��ָ���������
	   unsigned short *ltCols;   //��������״����߷ָ���������
	   unsigned short *mdCols;   //��������״���м�ָ���������
	   unsigned short *rtCols;   //��������״���ұ߷ָ���������
}LANE;
typedef struct
{
	   int Num;                  //�ж�������
	   CAR *carP;                //���г�������Ϣ
}CARS;
typedef struct
{
	   unsigned char  isSave;    //�ñ��λΪ��ʱ vanishLine��table���浽���衣
	   unsigned short vanishLine;
	   unsigned char  vanishLineIsOk;
	   MtxUshort table;
}SAVEPARA;

typedef struct
{
	   LANE lane;                //��������Ϣ
	   CARS cars;	             //������Ϣ
	   PLATES   plates;          //������Ϣ
	   SAVEPARA savePara;
	   int  score;               //��ʻϰ�ߴ��
	   
}ADASOUT;
/*******************************************/
//��ʼ���������
/*******************************************/

typedef struct
{
	  char     *version;                //�㷨�汾��
	  struct
	  {
		      unsigned char isGps;       //����gps
		      unsigned char isGsensor;   //����Gsensor����v3֮��������������Ϊ0
		      unsigned char isObd;       //���޶�ȡOBD
			  unsigned char isCalibrate; //�Ƿ���а�װ�궨
	  }config;
	  struct
	  {

      }calibrate;//��װ������Ԥ��

	  int fps;                          //֡��
	  IMGSIZE imgSizeSmall;             //������ͼ 960*540
	  IMGSIZE imgSizeBig;               //������ͼ 1920*1080
	 
	  ViewAng           viewAng;	    //��ʼ����ʱ�����
	  unsigned short    vanishLine;     //��ʼ����ʱ�����
	  unsigned char     vanishLineIsOk;
	  MtxUshort         table;          //��ʼ����ʱ�����
	  void   *setPara;	                //������ָ�����㷨����Ҫ��������һЩ������ȡ����·��Ľ������ʱ���岻��
      struct
	  {
		      PointC ltLane[2]; //���У׼�ߵ������˵�
			  PointC rtLane[2]; //�ұ�У׼�ߵ������˵�		    
	  }adjustLane;//У׼��

}ADASIN;




/*******************************************/
typedef void (*AdasIn)(FRAMEIN &frameIn,void *dv); //�ص���������adas�������
typedef void (*AdasOut)(ADASOUT &adasOut,void *dv);//�ص���������ȡadas�ļ�����
extern  AdasIn  callbackIn;
extern  AdasOut callbackOut;
/*******************************************/
//�ӿں���
/*******************************************/
void InitialAdas(ADASIN &adasIn,void **dev);
void ReleaseAdas();


#endif



