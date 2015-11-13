#include <asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h> 

#include <errno.h>
#include <cutils/log.h>

#include <CdrTinyCap.h>
#include <CdrSphinx.h>

#include "pocketsphinx.h"
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/cont_ad.h>

#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>




namespace android {
	
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

#define MAXLINE 4096 
#define SERV_PORT 8888
#define SERVER_IP "127.0.0.1"


static int cdrcapturing = 1;

CdrTinyCap::CdrTinyCap()
	:SphinxStartFlag(false)
	,TinyCapSwitch(false)
{
	
}

CdrTinyCap::~CdrTinyCap()
{
	
}

#if 1
int CdrTinyCap::cdrcapture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            unsigned int bits, unsigned int period_size,
                            unsigned int period_count)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    unsigned int size;
    unsigned int bytes_read = 0;

    config.channels = channels;
    config.in_init_channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    if (bits == 32)
        config.format = PCM_FORMAT_S32_LE;
    else if (bits == 16)
        config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open_req(card, device, PCM_IN, &config, rate);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device (%s)\n",
                pcm_get_error(pcm));
        return 0;
    }

    size = pcm_get_buffer_size(pcm);
	ALOGE("-------------pcm_get_buffer_size------size =%d",size);
    buffer = (char *)malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return 0;
    }

    printf("Capturing sample: %u ch, %u hz, %u bit\n", channels, rate, bits);

    while (cdrcapturing && !pcm_read(pcm, buffer, size)) {
		ALOGE("-------------pcm_read------size =%d",size);
        if (fwrite(buffer, sizeof(char), size, file) != size) {
            fprintf(stderr,"Error capturing sample\n");
            break;
        }
        bytes_read += size;
		if((bytes_read / ((bits / 8) * channels)) >  20000){
		//if((bytes_read / ((bits / 8) * channels)) >  5000){
			free(buffer);
    		pcm_close(pcm);
    		return bytes_read / ((bits / 8) * channels);
		}
    }

    free(buffer);
    pcm_close(pcm);
    return bytes_read / ((bits / 8) * channels);
}


int CdrTinyCap::MyTinyCap(FILE *file)
{
    struct wav_header header;
	#if 1
    unsigned int card = 0;
    unsigned int device = 0;
	unsigned int channels = 1;
    unsigned int rate = 16000;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
	#else
    unsigned int card = 0;
    unsigned int device = 0;
    unsigned int channels = 2;
    unsigned int rate = 44100;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
	#endif
    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = channels;
    header.sample_rate = rate;
    header.bits_per_sample = bits;
    header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
    header.block_align = channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;


    fseek(file, sizeof(struct wav_header), SEEK_SET);

    frames = cdrcapture_sample(file, card, device, header.num_channels,
                                 header.sample_rate, header.bits_per_sample,
                                 period_size, period_count);
	
    printf("Captured %d frames\n", frames);

    /* write header now all information is known */
    header.data_sz = frames * header.block_align;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct wav_header), 1, file);

    fclose(file);

    return 0;
}
#endif

int CdrTinyCap::DoPcmTinyCap(unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            unsigned int bits, unsigned int period_size,
                            unsigned int period_count,bool *speechRecSwitch)
{
	struct pcm_config config;
	struct pcm *pcm;
	char *buffer;
	unsigned int size;

	config.channels = channels;
	config.in_init_channels = channels;
	config.rate = rate;
	config.period_size = period_size;
	config.period_count = period_count;
	if (bits == 32)
		config.format = PCM_FORMAT_S32_LE;
	else if (bits == 16)
		config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	pcm = pcm_open_req(card, device, PCM_IN, &config, rate);
	if (!pcm || !pcm_is_ready(pcm)) {
		ALOGE("----------------------pcm_open_req false");
		return 0;
	}

	size = pcm_get_buffer_size(pcm);
	buffer = (char *)malloc(size);
	if (!buffer) {
		fprintf(stderr, "Unable to allocate %d bytes\n", size);
		free(buffer);
		pcm_close(pcm);
		return 0;
	}
#if 1
	int s;
	struct sockaddr_in addr;
	int addr_len =sizeof(struct sockaddr_in);
	if((s = socket(AF_INET,SOCK_DGRAM,0))<0){
		ALOGE("socket");
	}
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERV_PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
#endif 

#if 1
	while (!pcm_read(pcm, buffer, size) && *speechRecSwitch) {
		//sendto(s,buffer,strlen(buffer)+1,0,(struct sockaddr *)&addr,addr_len);
		//strcpy(buffer,"aaaaaaaaaass");
		
#if 0//no us
		int i;
		for(i = 0;i< MAXLINE;i++){
			ALOGE("1------------%x",buffer[i]);
		}	
#endif
		int senbyte = sendto(s,buffer,size,0,(struct sockaddr *)&addr,addr_len);
		//ALOGE("--------senbyte =%d---size =%d speechRecSwitch=%d",senbyte,size,*speechRecSwitch);
		memset(buffer,0,sizeof(buffer));
	}
#endif

	free(buffer);
	pcm_close(pcm);
	close(s);
	return 0;
}

int CdrTinyCap::VoiceTinyCap(bool * speechRecSwitch)
{
	struct wav_header header;
#if 0
	unsigned int card = 0;
	unsigned int device = 0;
	unsigned int channels = 1;
	unsigned int rate = 8000;
	unsigned int bits = 16;
	unsigned int frames;
	unsigned int period_size = 1024;//512;
	unsigned int period_count = 4;//2;
#else
	unsigned int card = 0;
	unsigned int device = 0;
	unsigned int channels = 1;
	unsigned int rate = 16000;
	unsigned int bits = 16;
	unsigned int frames;
	unsigned int period_size = 1024;//512;
	unsigned int period_count = 4;//2;
#endif
	header.riff_id = ID_RIFF;
	header.riff_sz = 0;
	header.riff_fmt = ID_WAVE;
	header.fmt_id = ID_FMT;
	header.fmt_sz = 16;
	header.audio_format = FORMAT_PCM;
	header.num_channels = channels;
	header.sample_rate = rate;
	header.bits_per_sample = bits;
	header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
	header.block_align = channels * (header.bits_per_sample / 8);
	header.data_id = ID_DATA;


	frames = DoPcmTinyCap(card, device, header.num_channels,
								 header.sample_rate, header.bits_per_sample,
								 period_size, period_count,speechRecSwitch);
	
	header.data_sz = frames * header.block_align;

	return 0;
}



}


