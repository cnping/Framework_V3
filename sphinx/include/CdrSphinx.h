#ifndef _SPHINX_H
#define _SPHINX_H

namespace android {

	class CdrSphinx{
	public:	
		CdrSphinx();
    	~CdrSphinx();
		int VoiceDeal();
		int	VoiceDeal(const char * filename);
		int	VoiceDeal(FILE *fh);
		int	VoiceDealBuf(short *buf,int bytes);
		int VoiceRecAndDeal(bool *speechRecSwitch);
	};
}

#endif
