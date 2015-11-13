#ifndef __HTTP_CLT_H__
#define __HTTP_CLT_H__

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <unistd.h> 
#include <netinet/in.h>
#include <limits.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <ctype.h>  

#ifdef __cplusplus
extern "C" 
{
#endif

/* path���ļ�����·���������ļ���  URL�������ļ�������URL*/
int downloadFile(char *path, char *URL);

/*uri: ���ַ������������ļ�������URL�� flag����DOWNLOAD_STARTֵΪ1����DOWNLOAD_ENDֵΪ0*/
typedef void (*DownloadCommand)(char *uri, int flag);
void registerDownloadCommand(DownloadCommand _downloadCommand);

#ifdef __cplusplus
}
#endif


#endif//__HTTP_CLT_H__