#ifndef __NATCLIENT_INTERFACE__
#define __NATCLIENT_INTERFACE__

#ifdef __cplusplus
extern "C"{
#endif	

typedef struct 
{
	char devName[24];
	void * session;
}NATMainThreadArgV;

//��ȡ�û����ĺ���ָ�룻
//����ֵ��0���ɹ��� ��0 ʧ�ܣ�
//������usrname,��ȡ�ɹ��û�����д����ַ����飻
//      usertype, ��ȡ���˻���������Ϣ��  0������Ա��admin��   1, ��ͨ�û���user��     
//      cookie�� Ӧ�ó�������
typedef int (* GetUserNameFunc)(char * username, int usertype, void * cookie);

//��ȡָ���û������˻�����ĺ���ָ�룻
//����ֵ��0���ɹ��� ��0 ʧ�ܣ�
//������username,  Ҫ��ȡ������˻����û�����
//      password, ָ����username��������Ϣ�� 
//      cookie�� Ӧ�ó�������
typedef int (* GetPassWordFunc)(int usertype, char* password, void *);

//����ָ���û������˻�����ĺ���ָ�룻
//����ֵ��0���ɹ��� ��0 ʧ�ܣ�
//������username,  Ҫ����������˻����û�����
//      password, ����username��������Ϣ�� 
//      cookie�� Ӧ�ó�������
typedef int (* SetPassWordFunc)(int usertype, char* password, void *);


void natMain(void * in);

void RegisterGetUserNameFunc(GetUserNameFunc, void *);
void RegisterGetPassWordFunc(GetPassWordFunc, void *);
void RegisterSetPassWordFunc(SetPassWordFunc, void *);

#ifdef __cplusplus
}
#endif	

#endif	//__NATCLIENT_INTERFACE__