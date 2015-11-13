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


#define ACK_WAKEUPASK1  "小易"
#define ACK_WAKEUPASK2  "在吗"


#define ACK_TAKEPIC 		"拍照"
//action par1
#define ACT_PAR1_OPEN 	"开启"
#define ACT_PAR1_CLOSE	"关闭"
#define ACT_PAR1_LOCK	"上锁"
//action par2
#define ACT_PAR2_REC 		"录像"
#define ACT_PAR2_BACKCAR	"倒车"
#define ACT_PAR2_SS		"屏保"
#define ACT_PAR2_LOCK		"视频"

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

	//1、初始化：创建一个配置对象 cmd_ln_t *
	//cmd_ln_init函数第一个参数是我们需要更新的上一个配置，因为这里是初次创建，所以传入NULL；
	//第二个参数是一个定义参数的数组，如果使用的是标准配置的参数集的话可以通过调用ps_args()去获得。
	//第三个参数是是一个标志，它决定了参数的解释是否严格，如果为TRUE，那么遇到重复的或者未知的参
	//数，将会导致解释失败；
	//MODELDIR这个宏，指定了模型的路径，包括声学模型，语言模型和字典三个文件，是由gcc命令行传入，
	//我们通过pkg-config工具从PocketSphinx的配置中去获得这个modeldir变量
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
	
	//2、初始化解码器（语言识别就是一个解码过程，通俗的将就是将你说的话解码成对应的文字串）
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3、解码文件流
	//因为音频输入接口（麦克风）受到一些特定平台的影响，不利用我们演示，所以我们通过解码音频文件流
	//来演示PocketSphinx API的用法，goforward.raw是一个包含了一些诸如“go forward ten meters”等用来
	//控制机器人的短语（指令）的音频文件，其在test/data/goforward.raw。把它复制到当前目录
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
	//4、使用ps_decode_raw()进行解码
	
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5、得到解码的结果（概率最大的字串） hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("1###################Recognized: *%s*  %d\n", hyp,score);

	hyp = ps_get_hyp_final(ps, &score);
	ALOGE("2###################Recognized: *%s*  %d\n", hyp,score);

	if(strstr(hyp,"开启录像") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"停止录像") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"进入倒车") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"退出倒车") != NULL)
	{
		ret = SPEECH_CANCEL_BACK_CAR;
	}
	if(strstr(hyp,"拍照") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"小易在吗") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"移动侦测") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"停车监控") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"视频上锁") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"开屏") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"关屏") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}

	#if 0
	//从内存中解码音频数据
	//现在我们将再次解码相同的文件，但是使用API从内存块中解码音频数据。在这种情况下，首先我们
	//需要使用ps_start_utt()开始说话：
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("你好!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//我们将每次从文件中读取512大小的样本，使用ps_process_raw()把它们放到解码器中:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//我们需要使用ps_end_utt()去标记说话的结尾处：
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//以相同精确的方式运行来检索假设的字符串：
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6、清理工作：使用ps_free()释放使用ps_init()返回的对象，不用释放配置对象。
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

	//1、初始化：创建一个配置对象 cmd_ln_t *
	//cmd_ln_init函数第一个参数是我们需要更新的上一个配置，因为这里是初次创建，所以传入NULL；
	//第二个参数是一个定义参数的数组，如果使用的是标准配置的参数集的话可以通过调用ps_args()去获得。
	//第三个参数是是一个标志，它决定了参数的解释是否严格，如果为TRUE，那么遇到重复的或者未知的参
	//数，将会导致解释失败；
	//MODELDIR这个宏，指定了模型的路径，包括声学模型，语言模型和字典三个文件，是由gcc命令行传入，
	//我们通过pkg-config工具从PocketSphinx的配置中去获得这个modeldir变量
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
	
	//2、初始化解码器（语言识别就是一个解码过程，通俗的将就是将你说的话解码成对应的文字串）
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3、解码文件流
	//因为音频输入接口（麦克风）受到一些特定平台的影响，不利用我们演示，所以我们通过解码音频文件流
	//来演示PocketSphinx API的用法，goforward.raw是一个包含了一些诸如“go forward ten meters”等用来
	//控制机器人的短语（指令）的音频文件，其在test/data/goforward.raw。把它复制到当前目录
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
	//4、使用ps_decode_raw()进行解码
	
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5、得到解码的结果（概率最大的字串） hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("###################Recognized: *%s*\n", hyp);

	if(strstr(hyp,"录像") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"停止") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"倒车") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"返回") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"拍照") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"小易在吗") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"移动") != NULL || strstr(hyp,"侦测") != NULL  || \
		strstr(hyp,"移动 侦测") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"停车") != NULL || strstr(hyp,"监控") != NULL  || \
		strstr(hyp,"停车 监控") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"碰撞") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"开屏") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"关屏") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}

	#if 0
	//从内存中解码音频数据
	//现在我们将再次解码相同的文件，但是使用API从内存块中解码音频数据。在这种情况下，首先我们
	//需要使用ps_start_utt()开始说话：
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("你好!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//我们将每次从文件中读取512大小的样本，使用ps_process_raw()把它们放到解码器中:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//我们需要使用ps_end_utt()去标记说话的结尾处：
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//以相同精确的方式运行来检索假设的字符串：
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6、清理工作：使用ps_free()释放使用ps_init()返回的对象，不用释放配置对象。
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

	//1、初始化：创建一个配置对象 cmd_ln_t *
	//cmd_ln_init函数第一个参数是我们需要更新的上一个配置，因为这里是初次创建，所以传入NULL；
	//第二个参数是一个定义参数的数组，如果使用的是标准配置的参数集的话可以通过调用ps_args()去获得。
	//第三个参数是是一个标志，它决定了参数的解释是否严格，如果为TRUE，那么遇到重复的或者未知的参
	//数，将会导致解释失败；
	//MODELDIR这个宏，指定了模型的路径，包括声学模型，语言模型和字典三个文件，是由gcc命令行传入，
	//我们通过pkg-config工具从PocketSphinx的配置中去获得这个modeldir变量
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
	
	//2、初始化解码器（语言识别就是一个解码过程，通俗的将就是将你说的话解码成对应的文字串）
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		return -1;
	}

	//3、解码文件流
	//因为音频输入接口（麦克风）受到一些特定平台的影响，不利用我们演示，所以我们通过解码音频文件流
	//来演示PocketSphinx API的用法，goforward.raw是一个包含了一些诸如“go forward ten meters”等用来
	//控制机器人的短语（指令）的音频文件，其在test/data/goforward.raw。把它复制到当前目录
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
	//4、使用ps_decode_raw()进行解码
#if 0
	rv = ps_decode_raw(ps, fh, NULL, -1);
	if (rv < 0){
		ALOGE("ps_decode_raw is false!");
		return -1;
	}
	
	//5、得到解码的结果（概率最大的字串） hypothesis
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return -1;
	}
	ALOGE("###################Recognized: *%s*\n", hyp);

	if(strstr(hyp,"录像") != NULL)
	{
		ret =  SPEECH_START_RECORD;
	}
	if(strstr(hyp,"停止") != NULL)
	{
		ret =  SPEECH_STOP_RECORD;
	}
	if(strstr(hyp,"倒车") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"返回") != NULL)
	{
		ret = SPEECH_BACK_CAR;
	}
	if(strstr(hyp,"拍照") != NULL)
	{
		ret = SPEECH_TAKE_PHOTOS;
	}
	if(strstr(hyp,"小易在吗") != NULL)
	{
		ret = SPEECH_START_WAKEUP;
	}

	if(strstr(hyp,"移动") != NULL || strstr(hyp,"侦测") != NULL  || \
		strstr(hyp,"移动 侦测") != NULL)
	{
		ret = SPEECH_AWMD;
	}
	if(strstr(hyp,"停车") != NULL || strstr(hyp,"监控") != NULL  || \
		strstr(hyp,"停车 监控") != NULL)
	{
		ret = SPEECH_PARKING;
	}
	if(strstr(hyp,"碰撞") != NULL)
	{
		ret = SPEECH_IMPACT;
	}
	if(strstr(hyp,"开屏") != NULL)
	{
		ret = SPEECH_OPENSCREEN;
	}
	if(strstr(hyp,"关屏") != NULL)
	{
		ret = SPEECH_CLOSESCREEN;
	}
#endif
	#if 1
	//从内存中解码音频数据
	//现在我们将再次解码相同的文件，但是使用API从内存块中解码音频数据。在这种情况下，首先我们
	//需要使用ps_start_utt()开始说话：
	fseek(fh, 0, SEEK_SET);
	
	rv = ps_start_utt(ps, NULL);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}

	ALOGE("你好!");

	while (!feof(fh)) {
		ALOGE("ready:\n");
			size_t nsamp;
			nsamp = fread(buf, 2, 512, fh);
		ALOGE("read:\n");
			//我们将每次从文件中读取512大小的样本，使用ps_process_raw()把它们放到解码器中:
			rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
		ALOGE("process:\n");
		}

	//我们需要使用ps_end_utt()去标记说话的结尾处：
	rv = ps_end_utt(ps);
	if (rv < 0){
		ALOGE("ps_start_utt is false!");
		return 1;
	}
		
	//以相同精确的方式运行来检索假设的字符串：
	hyp = ps_get_hyp(ps, &score, &uttid);
	if (hyp == NULL){
		ALOGE("ps_get_hyp is false!");
		return 1;
	}
	ALOGE("Recognized: %s\n", hyp);
	#endif


	//6、清理工作：使用ps_free()释放使用ps_init()返回的对象，不用释放配置对象。
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

	ALOGE("你好!");
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
	
	//以相同精确的方式运行来检索假设的字符串：
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
	
	//2、初始化解码器（语言识别就是一个解码过程，通俗的将就是将你说的话解码成对应的文字串）
	ps = ps_init(config);
	if (ps == NULL){
		ALOGE("ps_init is false!");
		goto out1;
	}
	ALOGE("++++++++++++++++++++++socket init");
#if 1
	/*建立socket*/
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
		ALOGE ("socket");
		goto out1;
	}
	/*填写sockaddr_in 结构*/
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
	//6、清理工作：使用ps_free()释放使用ps_init()返回的对象，不用释放配置对象。
out3:
	ps_free(ps);
out2:
	close(sockfd);
out1:
	return ret;
}

}
