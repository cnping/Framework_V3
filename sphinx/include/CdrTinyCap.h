#ifndef _TINYCAP_H_
#define _TINYCAP_H_


#include <CdrSphinx.h>

namespace android {

	class CdrTinyCap{
	public:	
		CdrTinyCap();
		~CdrTinyCap();

		CdrSphinx * mSphinx;
		int cdrcapture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            unsigned int bits, unsigned int period_size,
                            unsigned int period_count);
		int MyTinyCap(FILE *file);

		int DoPcmTinyCap(unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            unsigned int bits, unsigned int period_size,
                            unsigned int period_count,bool * speechRecSwitch);

		int VoiceTinyCap(bool * speechRecSwitch);
		
		volatile bool SphinxStartFlag;
		bool TinyCapSwitch;
		};
}

#endif
