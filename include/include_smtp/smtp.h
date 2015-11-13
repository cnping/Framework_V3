#ifndef WL_SMTP_H
#define WL_SMTP_H 1

#ifdef __cplusplus
extern "C" {
#endif


#define MAIL_PORT 25
#define NAME_FROM_SIZE 128
#define NAME_to_SIZE 128
#define ADD_FROM_SIZE 128
#define ADD_TO_SIZE 128
#define HOST_SIZE 128
#define SUBJECT_SIZE 128
#define FILEPATH_SIZE 128
#define CONTENT_SIZE 65536
#define ATTACH_NUM 3

typedef struct
{
	char name_from[NAME_FROM_SIZE];	//��������
	char name_to[NAME_to_SIZE];		//��������
	char add_from[ADD_FROM_SIZE];	//���Ե�����
	char add_to[ADD_TO_SIZE];		//����������
	char subject[SUBJECT_SIZE];		//����
	char *filebuf[ATTACH_NUM];		//����buf							
	char *filename[ATTACH_NUM];		//������������׺����File.jpeg
	unsigned int filesz[ATTACH_NUM];//����buf����
	char content[CONTENT_SIZE];		//�ż���������
	unsigned short port;			//�ʼ��������˿ں�															


}struct_send_mail;

int send_mail(struct_send_mail *msg);



#ifdef __cplusplus
}
#endif

#endif /* wl_smtp.h */