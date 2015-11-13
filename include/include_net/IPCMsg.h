#ifndef __IPCMSG__H__
#define __IPCMSG__H__

#ifndef bool
#define bool char
#endif 

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif 
typedef struct
{
	int     check;			//У��ֵ������Ϊ0xabcd123
	char    findData[8];	//ʶ���ַ�����ΪIPCamera
}DevFind;

typedef struct
{
	int 	check;			//0xabcd123
	char 	devName[16];	//�豸����
	short	port;			//����˿ں�
}DevFindAck;


typedef struct
{
	int		check;			//У��ֵ��Ϊ0xabcd123
	int		iLength;		//������Ϣ������
	char    Content[];		//��Ϣ����
}DPMsg;







#endif //#ifdef __IPCMSG__H__
