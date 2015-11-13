#ifndef __HTTP_SRV_H__
#define __HTTP_SRV_H__

#include "http.h"

#ifndef WRITE_ERROR
#define WRITE_ERROR -1
#endif

#ifndef WRITE_SUCCESS
#define WRITE_SUCCESS 0
#endif

#ifndef WRITE_START
#define WRITE_START 1
#endif

#ifndef WRITE_PUASE
#define WRITE_PUASE 2
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

/*
** http_srv_init()���ڳ�ʼ��
** http_srv_open(http_srv_t *srv)���ڴ�http����,����srv��http_srv_init�������ػ��
** http_srv_close(http_srv_t *srv)���ڹر�http����
** http_srv_release(http_srv_t *srv)�����ͷ���Դ
** getURL(http_srv_t *srv, char *path_ptr)���ڻ�ȡURL��path�ǵ������ṩ����Դ·��
**
*/
//int get_url(http_srv_t *srv, char *out, char *path);
http_srv_t *http_srv_init();
int http_srv_open(http_srv_t *srv, char *local_ip);
//int http_srv_open(http_srv_t *srv);
int http_srv_close(http_srv_t *srv);
void http_srv_release(http_srv_t *srv);
int http_srv_getURL(http_srv_t *srv, const char *path_ptr, const char * ip, char * url);
int http_srv_get_abspath(char *out, char *uri);

//����д����ǰ������ֵ���Ժ���Ϊ��λ��2015.1.5�����ӿ�
void setWriteSleepTime(int usec);

/*
** ֪ͨ��ȡ���ݻ��߻�ȡ��������ȡ����ʱ����������ֵ1����ȡ����ʱ����������ֵ0, ��ȡ������ֹʱ����������ֵ2
** typedef void (*GetDataOrEnd)(void * cookies, char *uri, int flag);
*/
//����ӿ�����
void registerGetDataOrEnd(http_srv_t * srv, void * cookies, GetDataOrEnd _getDataOrEnd);






int http_srv_write(int socket, char * httpHeader, unsigned char * IPCbuffer, int buff_size); //�����ӿڣ�����������дBuffer

//typedef void (*StartWrite)(void * cookies, char * uri, char * header, int socket); ֪ͨIPCӦ��д����
void registerStartWrite(http_srv_t * srv, void * cookies, StartWrite _startWrite);

//typedef void (*BufferSize)(void * cookies, char * uri, int *size);  IPC��֪Ҫд���ݵĳ���
void registerBufferSize(http_srv_t * srv, void * cookies, BufferSize _bufferSize);






#ifdef __cplusplus
}
#endif

#endif //__HTTP_SRV_H__
