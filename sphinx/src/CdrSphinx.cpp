#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>


#include <CdrSphinx.h>

#include "pocketsphinx.h"
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/cont_ad.h>


#include <errno.h>
#include <cutils/log.h>


namespace android {

int ActWordNum = 0;


#define ACK_WAKEUPASK1  "С��"
#define ACK_WAKEUPASK2  "����"


#define ACK_TAKEPIC 		"����"
//action par1
#define ACT_PAR1_OPEN 	"����"
#define ACT_PAR1_CLOSE	"�ر�"
#define ACT_PAR1_LOCK	"����"
//action par2
#define ACT_PAR2_REC 		"¼��"
#define ACT_PAR2_BACKCAR	"����"
#define ACT_PAR2_SS		"����"
#define ACT_PAR2_LOCK		"��Ƶ"

enum actWordType{
	ACT_NONEED_TYPE = 0,
	ACT_WAKEUP_TYPETWOTWO,
	ACT_OPEN_TYPE,
	ACT_CLOSE_TYPE,
	ACT_LOCK_TYPE,
};

enum speechRec {
	SPEECH_START_NEEDNEXTWORD 	= 0,
	SPEECH_START_WAKEUP,		
	SPEECH_START_RECORD,
	SPEECH_STOP_RECORD,
	SPEECH_TAKE_PHOTOS,
	SPEECH_AWMD,
	SPEECH_PARKING,
	SPEECH_IMPACT,
	SPEECH_BACK_CAR,
	SPEECH_CANCEL_BACK_CAR,
	SPEECH_HELLO,
	SPEECH_OPENSCREEN,
	SPEECH_CLOSESCREEN,
};

#define MAXLINE 4096 
#define HALFMAXLINE 2048
#define SERV_PORT 8888 
#define SERVER_IP "127.0.0.1"


//test
CdrSphinx::CdrSphinx()
{
	
}

CdrSphinx::~CdrSphinx()
{
	
}

#if 1
int	CdrSphinx::VoiceDeal()
{
	ps_decoder_t *ps;
	cmd_ln_t *config;
	FILE *fh;
	char const *hyp, *uttid;
	int16 buf[512];
	int rv;
	int score;
	int ret = -1;

	//1����ʼ��������һ�����ö��� cmd_ln_t *
	//cmd_ln_init������һ��������������Ҫ���µ���һ�����ã���Ϊ�����ǳ��δ��������Դ���NULL��
	//�ڶ���������һ��������������飬���ʹ�õ��Ǳ�׼���õĲ������Ļ�����ͨ������ps_args()ȥ��á�
	//��������������һ����־���������˲����Ľ����Ƿ��ϸ����ΪTRUE����ô�����ظ��Ļ���δ֪�Ĳ�
	//�������ᵼ�½���ʧ�ܣ�
	//MODELDIR����ָ꣬����ģ�͵�·����������ѧģ�ͣ�����ģ�ͺ��ֵ������ļ�������gcc�����д��룬
	//����ͨ��pkg-config���ߴ�PocketSphinx��������ȥ������modeldir����
	/*
	config = cmd_ln_init(NULL, ps_args(), TRUE,
				 "-hmm", "/res/others/",
				 "-lm", "/res/others/eyesee.DMP",
				 "-dict", "/res/others/eyesee.dic",
				 NULL);
	*/
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		     "-hmm",  "/res/others/",
		     "-lm",  "/res/others/eyesee.lm",
		     "-dict",  "/res/others/eyesee.dic",
		     "-debug", "0",
		     NULL);
	
	if (config == NULL){
		ALOGE("cmd_ln_init is false!");
		return -1;
	}
	
	//2����ʼ��������������ʶ�����һ��������̣�ͨ�׵Ľ����ǽ���˵�Ļ�����ɶ�Ӧ�����ִ���
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3�������ļ���
	//��Ϊ��Ƶ����ӿڣ���˷磩�ܵ�һЩ�ض�ƽ̨��Ӱ�죬������������ʾ����������ͨ��������Ƶ�ļ���
	//����ʾPocketSphinx API���÷���goforward.raw��һ��������һЩ���硰go forward ten meters��������
	//���ƻ����˵Ķ��ָ�����Ƶ�ļ�������test/data/goforward.raw���������Ƶ���ǰĿ¼
	//fh = fopen("/sys/class/sound/pcmC0D0c", "rb");// /dev/input/event14
	//fh = fopen("/dev/snd/pcmC0D0p", "rb");
	//fh = fopen("/dev/input/mice", "rb");
	fh = fopen("/res/others/detvoice.raw", "rb");
	if (fh == NULL) {
		ALOGE("Failed to open dev");
		return -1;
	}
	fseek(fh, 42, SEEK_SET);
	/**/
	//4��ʹ��ps_decode_raw()���н���
	
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5���õ�����Ľ�������������ִ��� hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("1###################Recognized: *%s*  %d\n", hyp,score);

	hyp = ps_get_hyp_final(ps, &score);
	ALOGE("2###################Recognized: *%s*  %d\n", hyp,score);

	if(strstr(hyp,"����¼��") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"ֹͣ¼��") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"���뵹��") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"�˳�����") != NULL)
	{
		ret = SPEECH_CANCEL_BACK_CAR;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"С������") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"�ƶ����") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"ͣ�����") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"��Ƶ����") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}

	#if 0
	//���ڴ��н�����Ƶ����
	//�������ǽ��ٴν�����ͬ���ļ�������ʹ��API���ڴ���н�����Ƶ���ݡ�����������£���������
	//��Ҫʹ��ps_start_utt()��ʼ˵����
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("���!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//���ǽ�ÿ�δ��ļ��ж�ȡ512��С��������ʹ��ps_process_raw()�����Ƿŵ���������:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//������Ҫʹ��ps_end_utt()ȥ���˵���Ľ�β����
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//����ͬ��ȷ�ķ�ʽ����������������ַ�����
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6����������ʹ��ps_free()�ͷ�ʹ��ps_init()���صĶ��󣬲����ͷ����ö���
	fclose(fh);
	ps_free(ps);

	return ret;
	
}

#endif


int	CdrSphinx::VoiceDeal(const char * filename)
{
	ps_decoder_t *ps;
	cmd_ln_t *config;
	FILE *fh;
	char const *hyp, *uttid;
	int16 buf[512];
	int rv;
	int score;
	int ret = -1;

	//1����ʼ��������һ�����ö��� cmd_ln_t *
	//cmd_ln_init������һ��������������Ҫ���µ���һ�����ã���Ϊ�����ǳ��δ��������Դ���NULL��
	//�ڶ���������һ��������������飬���ʹ�õ��Ǳ�׼���õĲ������Ļ�����ͨ������ps_args()ȥ��á�
	//��������������һ����־���������˲����Ľ����Ƿ��ϸ����ΪTRUE����ô�����ظ��Ļ���δ֪�Ĳ�
	//�������ᵼ�½���ʧ�ܣ�
	//MODELDIR����ָ꣬����ģ�͵�·����������ѧģ�ͣ�����ģ�ͺ��ֵ������ļ�������gcc�����д��룬
	//����ͨ��pkg-config���ߴ�PocketSphinx��������ȥ������modeldir����
	/*
	config = cmd_ln_init(NULL, ps_args(), TRUE,
				 "-hmm", "/res/others/",
				 "-lm", "/res/others/eyesee.DMP",
				 "-dict", "/res/others/eyesee.dic",
				 NULL);
	*/
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		     "-hmm",  "/system/res/others/",
		     "-lm",  "/system/res/others/eyesee.lm",
		     "-dict",  "/system/res/others/eyesee.dic",
		     "-debug", "0",
		     NULL);
	
	if (config == NULL){
		ALOGE("cmd_ln_init is false!");
		return -1;
	}
	
	//2����ʼ��������������ʶ�����һ��������̣�ͨ�׵Ľ����ǽ���˵�Ļ�����ɶ�Ӧ�����ִ���
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3�������ļ���
	//��Ϊ��Ƶ����ӿڣ���˷磩�ܵ�һЩ�ض�ƽ̨��Ӱ�죬������������ʾ����������ͨ��������Ƶ�ļ���
	//����ʾPocketSphinx API���÷���goforward.raw��һ��������һЩ���硰go forward ten meters��������
	//���ƻ����˵Ķ��ָ�����Ƶ�ļ�������test/data/goforward.raw���������Ƶ���ǰĿ¼
	//fh = fopen("/sys/class/sound/pcmC0D0c", "rb");// /dev/input/event14
	//fh = fopen("/dev/snd/pcmC0D0p", "rb");
	//fh = fopen("/dev/input/mice", "rb");
	fh = fopen(filename, "rb");
	if (fh == NULL) {
		ALOGE("Failed to open dev");
		return -1;
	}
	fseek(fh, 42, SEEK_SET);
	/**/
	//4��ʹ��ps_decode_raw()���н���
	
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5���õ�����Ľ�������������ִ��� hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("###################Recognized: *%s*\n", hyp);

	if(strstr(hyp,"¼��") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"ֹͣ") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"С������") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"�ƶ�") != NULL || strstr(hyp,"���") != NULL  || \
		strstr(hyp,"�ƶ� ���") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"ͣ��") != NULL || strstr(hyp,"���") != NULL  || \
		strstr(hyp,"ͣ�� ���") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"��ײ") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}

	#if 0
	//���ڴ��н�����Ƶ����
	//�������ǽ��ٴν�����ͬ���ļ�������ʹ��API���ڴ���н�����Ƶ���ݡ�����������£���������
	//��Ҫʹ��ps_start_utt()��ʼ˵����
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("���!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//���ǽ�ÿ�δ��ļ��ж�ȡ512��С��������ʹ��ps_process_raw()�����Ƿŵ���������:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//������Ҫʹ��ps_end_utt()ȥ���˵���Ľ�β����
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//����ͬ��ȷ�ķ�ʽ����������������ַ�����
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6����������ʹ��ps_free()�ͷ�ʹ��ps_init()���صĶ��󣬲����ͷ����ö���
	fclose(fh);
	ps_free(ps);

	return ret;
	
}

#if 1
int	CdrSphinx::VoiceDeal(FILE *fh)
{
	ps_decoder_t *ps;
	cmd_ln_t *config;
	char const *hyp, *uttid;
	int16 buf[512];
	int rv;
	int score;
	int ret = -1;

	//1����ʼ��������һ�����ö��� cmd_ln_t *
	//cmd_ln_init������һ��������������Ҫ���µ���һ�����ã���Ϊ�����ǳ��δ��������Դ���NULL��
	//�ڶ���������һ��������������飬���ʹ�õ��Ǳ�׼���õĲ������Ļ�����ͨ������ps_args()ȥ��á�
	//��������������һ����־���������˲����Ľ����Ƿ��ϸ����ΪTRUE����ô�����ظ��Ļ���δ֪�Ĳ�
	//�������ᵼ�½���ʧ�ܣ�
	//MODELDIR����ָ꣬����ģ�͵�·����������ѧģ�ͣ�����ģ�ͺ��ֵ������ļ�������gcc�����д��룬
	//����ͨ��pkg-config���ߴ�PocketSphinx��������ȥ������modeldir����
	/*
	config = cmd_ln_init(NULL, ps_args(), TRUE,
				 "-hmm", "/res/others/",
				 "-lm", "/res/others/eyesee.DMP",
				 "-dict", "/res/others/eyesee.dic",
				 NULL);
	*/
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		     "-hmm",  "/system/res/others/",
		     "-lm",  "/system/res/others/eyesee.lm",
		     "-dict",  "/system/res/others/eyesee.dic",
		     "-debug", "0",
		     NULL);
	
	if (config == NULL){
		ALOGE("cmd_ln_init is false!");
		return -1;
	}
	
	//2����ʼ��������������ʶ�����һ��������̣�ͨ�׵Ľ����ǽ���˵�Ļ�����ɶ�Ӧ�����ִ���
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3�������ļ���
	//��Ϊ��Ƶ����ӿڣ���˷磩�ܵ�һЩ�ض�ƽ̨��Ӱ�죬������������ʾ����������ͨ��������Ƶ�ļ���
	//����ʾPocketSphinx API���÷���goforward.raw��һ��������һЩ���硰go forward ten meters��������
	//���ƻ����˵Ķ��ָ�����Ƶ�ļ�������test/data/goforward.raw���������Ƶ���ǰĿ¼
	//fh = fopen("/sys/class/sound/pcmC0D0c", "rb");// /dev/input/event14
	//fh = fopen("/dev/snd/pcmC0D0p", "rb");
	//fh = fopen("/dev/input/mice", "rb");
	/*
	fh = fopen(filename, "rb");
	if (fh == NULL) {
		ALOGE("Failed to open dev");
		return -1;
	}
	fseek(fh, 42, SEEK_SET);
	*/
	//4��ʹ��ps_decode_raw()���н���
#if 0
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5���õ�����Ľ�������������ִ��� hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("###################Recognized: *%s*\n", hyp);

	if(strstr(hyp,"¼��") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"ֹͣ") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"С������") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"�ƶ�") != NULL || strstr(hyp,"���") != NULL  || \
		strstr(hyp,"�ƶ� ���") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"ͣ��") != NULL || strstr(hyp,"���") != NULL  || \
		strstr(hyp,"ͣ�� ���") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"��ײ") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"����") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}
#endif
	#if 1
	//���ڴ��н�����Ƶ����
	//�������ǽ��ٴν�����ͬ���ļ�������ʹ��API���ڴ���н�����Ƶ���ݡ�����������£���������
	//��Ҫʹ��ps_start_utt()��ʼ˵����
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("���!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//���ǽ�ÿ�δ��ļ��ж�ȡ512��С��������ʹ��ps_process_raw()�����Ƿŵ���������:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//������Ҫʹ��ps_end_utt()ȥ���˵���Ľ�β����
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//����ͬ��ȷ�ķ�ʽ����������������ַ�����
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6����������ʹ��ps_free()�ͷ�ʹ��ps_init()���صĶ��󣬲����ͷ����ö���
	fclose(fh);
	ps_free(ps);

	return ret;
	
}
#endif


int	CdrSphinx::VoiceDealBuf(short *buf,int bytes)
{
	ps_decoder_t *ps;
	cmd_ln_t *config;

	char const *hyp, *uttid;

	int rv;
	int score;
	int ret = 0;

	config = cmd_ln_init(NULL, ps_args(), TRUE,
		     "-hmm",  "/res/others/",
		     "-lm",  "/res/others/eyesee.lm",
		     "-dict",  "/res/others/eyesee.dic",
		     "-debug", "0",
		     NULL);
	
	if (config == NULL){
		ALOGE("cmd_ln_init is false!");
		return 1;
	}
	
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return 1;
	}

	ALOGE("���!");
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	rv = ps_process_raw(ps, buf, bytes, FALSE, FALSE);
	if (rv < 0){
		ALOGE("ps_process_raw is false!");
		return 1;
	}

	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
	
	//����ͬ��ȷ�ķ�ʽ����������������ַ�����
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}

	ALOGE("1###################Recognized: *%s*\n", hyp);
	if(strstr(hyp,"") == NULL){
		ALOGE("2###################Recognized: *%s*\n", hyp);
		ret = 1;
	}
	if(strstr(hyp," ") == NULL){
		ALOGE("3###################Recognized: *%s*\n", hyp);
		ret = 2;
	}
	
	ps_free(ps);

	return ret;
	
}
#if 1
int DealWithChar(char const *hyp)
{
	int ret = -1;
	ALOGE("ActWordNum-------------= %d",ActWordNum);
	
	if(strstr(hyp,ACK_WAKEUPASK1) != NULL && strstr(hyp,ACK_WAKEUPASK2) != NULL){
		ActWordNum = ACT_NONEED_TYPE;
		return SPEECH_START_WAKEUP;
	}else if(strstr(hyp,ACK_WAKEUPASK1) != NULL){
		ActWordNum = ACT_WAKEUP_TYPETWOTWO;
		return SPEECH_START_NEEDNEXTWORD;
	}
	else if(strstr(hyp,ACK_TAKEPIC)){
		ActWordNum = ACT_NONEED_TYPE;
		return  SPEECH_TAKE_PHOTOS;
	}

	/**/
	if(ACT_WAKEUP_TYPETWOTWO == ActWordNum){
		if(strstr(hyp,ACK_WAKEUPASK2) != NULL){
			ActWordNum = ACT_NONEED_TYPE;
			return SPEECH_START_WAKEUP;
		}
	}
	
	if(strstr(hyp,ACT_PAR1_OPEN) != NULL){
		ActWordNum = ACT_OPEN_TYPE;
		ret = SPEECH_START_NEEDNEXTWORD;
	}else if(strstr(hyp,ACT_PAR1_CLOSE) != NULL){
		ActWordNum = ACT_CLOSE_TYPE;
		ret = SPEECH_START_NEEDNEXTWORD;
	}else if(strstr(hyp,ACT_PAR1_LOCK) != NULL){
		ActWordNum = ACT_LOCK_TYPE;
		ret = SPEECH_START_NEEDNEXTWORD;
	}
	
	if(ACT_OPEN_TYPE == ActWordNum){
		if(strstr(hyp,ACT_PAR2_REC)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_START_RECORD;
		}else if(strstr(hyp,ACT_PAR2_BACKCAR)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_BACK_CAR;
		}else if(strstr(hyp,ACT_PAR2_SS)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_CLOSESCREEN;
		}
	}else if(ACT_CLOSE_TYPE == ActWordNum){
		if(strstr(hyp,ACT_PAR2_REC)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_STOP_RECORD;
		}else if(strstr(hyp,ACT_PAR2_BACKCAR)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_CANCEL_BACK_CAR;
		}else if(strstr(hyp,ACT_PAR2_SS)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_OPENSCREEN;
		}
	}else if(ACT_LOCK_TYPE == ActWordNum){
		if(strstr(hyp,ACT_PAR2_LOCK)){
			ActWordNum = ACT_NONEED_TYPE;
			ret = SPEECH_IMPACT;
		}
	}
	return ret;
}
#endif

int CdrSphinx::VoiceRecAndDeal(bool *speechRecSwitch)
{
	ALOGE("++++++++++++++++++++++VoiceRecAndDeal");
	ps_decoder_t *ps;
	cmd_ln_t *config;
	char const *hyp, *uttid;
	int rv;
	int score;
	int ret = -1;
	static int speechRecCnt = 0;

	int sockfd,len;
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	int16 intbuffer[HALFMAXLINE];
	
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		     "-hmm",  "/system/res/others/",
		     "-lm",  "/system/res/others/eyesee.lm",
		     "-dict",  "/system/res/others/eyesee.dic",
		     "-debug", "0",
		     NULL);
	
	if (config == NULL){
		ALOGE("cmd_ln_init is false!");
		//return ret;
		goto out1;
	}
	
	//2����ʼ��������������ʶ�����һ��������̣�ͨ�׵Ľ����ǽ���˵�Ļ�����ɶ�Ӧ�����ִ���
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		goto out1;
	}
	ALOGE("++++++++++++++++++++++socket init");
#if 1
	/*����socket*/
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
		ALOGE ("socket");
		goto out1;
	}
	/*��дsockaddr_in �ṹ*/
	bzero ( &addr, sizeof(addr) );
	addr.sin_family=AF_INET;
	addr.sin_port=htons(SERV_PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY) ;
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0){
		ALOGE("connect");
		goto out2;
	}
#endif

#if 1
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		goto out2;
	}

	ALOGE("++++++++++++++++++++++socket init ok");
#if 1
	while (*speechRecSwitch) 
	{
		memset(intbuffer,0,sizeof(intbuffer));
		len = recvfrom(sockfd,intbuffer,sizeof(intbuffer), 0 , (struct sockaddr *)&addr ,&addr_len);
		//ALOGE("++++++recvbyte = %d speechRecSwitch=%d\n",len,*speechRecSwitch);
		rv = ps_process_raw(ps, intbuffer, HALFMAXLINE, FALSE, FALSE);
		speechRecCnt++;
		if(speechRecCnt > 10){
			speechRecCnt = 0;
			rv = ps_end_utt(ps);
			if (rv < 0){
				ALOGE("ps_start_utt is false!");
				goto out3;
			}
			
			hyp = ps_get_hyp(ps, &score, &uttid);
			if (hyp == NULL){
				ALOGE("ps_get_hyp is false!");
				goto out3;
			}

			ALOGE("1###################Recognized: *%s*  %d\n", hyp,score);

			hyp = ps_get_hyp_final(ps, &score);//0929
			ALOGE("2###################Recognized: *%s*  %d\n", hyp,score);//0929
	

			ret = DealWithChar(hyp);
			ALOGE("###################ret: *%d*\n", ret);

			if(ret == 0){
				//ALOGE("need next word ,%d park1word  is %s ",ActWordNum,hyp);
			}else if(ret < 0){
				ActWordNum = ACT_NONEED_TYPE;
				//ALOGE("no need next word ,%d park1word",ActWordNum);
			}
			if(ret > 0){
				//ALOGE("###################ret: *%d*\n", ret);
				goto out3;
			}
			rv = ps_start_utt(ps, NULL);
			if (rv < 0){
				goto out3;
			}
		}
		
	}
#endif
#endif
	//6����������ʹ��ps_free()�ͷ�ʹ��ps_init()���صĶ��󣬲����ͷ����ö���
out3:
	ps_free(ps);
out2:
	close(sockfd);
out1:
	return ret;
}

}
