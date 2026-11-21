
#include <stdlib.h>
#include <string.h>
#include "codec.h"
#include "vorbisfile.h"
#include "dumb.h"
#include <fxdll.h>
#include "mixer.h"

typedef struct WAV_SIGNAL WAV_SIGNAL;
typedef struct WAV_SAMPINFO WAV_SAMPINFO;


#define CONVERT16(src, pos, signconv)				 \
{													 \
	signed int f = src;								 \
	f = MID(-32768, f, 32767);						 \
	((short *)dst)[pos] = (short)(f ^ signconv);	 \
}


struct WAV_SAMPINFO
{
	char *stream;
	unsigned short bitstream;
	unsigned short n_channels;
	long pos;
	long length;
};



struct WAV_SIGNAL
{
	char *filename;
	long length;
	unsigned long  average_bitrate;
	unsigned short channels;
	unsigned long  frequency;

};



static long wav_render_samples(void *sampinfo, float volume, float delta, long size, sample_t **samples);
static void *wav_start_samples(DUH *duh, void *signal, int n_channels, long pos);
static void wav_end_samples(void *sampinfo);
static void wav_unload_signal(void *signal);



static DUH_SIGTYPE_DESC sigtype_wav = {
	NULL,
	NULL,				/* entheh says I don't need you */
	&wav_start_samples,
	NULL,				/* don't need this either */
	&wav_render_samples,
	&wav_end_samples,
	&wav_unload_signal
};


long z=1;


DUH *dumb_load_wav(const char *filename) {


    file *fp;
	WAV_SIGNAL *signal;

	DUH_SIGTYPE_DESC *descptr = &sigtype_wav;

	signal = malloc(sizeof(WAV_SIGNAL));
	signal->filename = malloc((strlen(filename) + 1));
	strcpy(signal->filename,filename);
	
	

	fp = file_open_mixer(filename);
    if (fp)
    {

        char id[4], *sound_buffer; //four bytes to hold 'RIFF'
        long size; //32 bit value to hold file size
		long           chunkSize;
        short          wFormatTag;
		unsigned short wChannels;
		unsigned long  dwSamplesPerSec;
		unsigned long  dwAvgBytesPerSec;
		unsigned short wBlockAlign;
		unsigned short wBitsPerSample;
		unsigned long  data_size;
		
		
		file_getnc_mixer(&id,4,fp);								//RIFF
		 
	

        if ((id[0]=='R') && (id[1]=='I') && (id[2]=='F') && (id[3]=='F')) { 
			
		
			
			file_getnc_mixer(&size,4,fp);						//SIZE	
			file_getnc_mixer(&id,4,fp);							//WAVE
			
			if ((id[0]=='W') && (id[1]=='A') && (id[2]=='V') && (id[3]=='E')) {  
					

					
					//Es un wav, por lo tanto vamos a leerlo.
					file_getnc_mixer(&id,4,fp);					//format

					file_getnc_mixer(&chunkSize,4,fp);  

					file_getnc_mixer(&wFormatTag,2,fp);
					file_getnc_mixer(&wChannels,2,fp);			//channels
					file_getnc_mixer(&dwSamplesPerSec,4,fp);	//frequency
					file_getnc_mixer(&dwAvgBytesPerSec,4,fp);   //bytes per second
					file_getnc_mixer(&wBlockAlign,2,fp);	    //bloq alignment
					file_getnc_mixer(&wBitsPerSample,2,fp);     //bits per sample
					
					
					file_getnc_mixer(&id,4,fp);					//data	

			

					file_getnc_mixer(&data_size,4,fp);			//data written;
				 		
					

					if (wFormatTag!=1) {
						file_close_mixer(fp);
						return NULL;
					}
				

					signal->length= data_size;
					signal->average_bitrate = wBitsPerSample;
					signal->channels = wChannels;
					signal->frequency = dwSamplesPerSec;					

					file_close_mixer (fp);				

					return make_duh(((WAV_SIGNAL*)signal)->length-8, 1, &descptr, &signal);
					
								
			} else {
				file_close_mixer (fp);
				return NULL;		
			}

		} else {
			
			file_close_mixer (fp);
			return NULL;

		}
    }
	
}


static long wav_render_samples(void *sampinfo, float volume, float delta, long size, sample_t **samples) {

	long total_samples_read=0;
	WAV_SAMPINFO *wavsampinfo;
	sample_t *dst = samples[0];
 

	wavsampinfo=sampinfo;
	
		

		{
			int i, j;
			float mul = 1.0f * volume;
			for (j = 0; j < size; j++) {
				
			

				if (wavsampinfo->bitstream==16) {

						dst[j]= (wavsampinfo->stream[(long)wavsampinfo->pos+1]<<8);
						dst[j]=dst[j]|(unsigned char)(wavsampinfo->stream[(long)wavsampinfo->pos]);
						dst[j]=dst[j]*mul;											
							wavsampinfo->pos=wavsampinfo->pos+2;											
						if (get_mixer_channels()==2) {
							if (wavsampinfo->n_channels==2) {
									dst[j+size]= wavsampinfo->stream[(long)wavsampinfo->pos+1]<<8;
									dst[j+size]=dst[j+size]| (unsigned char) (wavsampinfo->stream[(long)wavsampinfo->pos]);
									dst[j+size]=dst[j+size]*mul;									
									wavsampinfo->pos=wavsampinfo->pos+2;
							} else {
									dst[j+size]=dst[j];
							}
						}
						
				} else {
						
						dst[j]=(unsigned char) (wavsampinfo->stream[(long)wavsampinfo->pos]);
						dst[j]=dst[j]*mul*100;
						wavsampinfo->pos=wavsampinfo->pos+1;
						
						if (get_mixer_channels()==2) {

							if (wavsampinfo->n_channels==2) {							
									dst[j+size]=(unsigned char) (wavsampinfo->stream[(long)wavsampinfo->pos]);	
									dst[j+size]=dst[j+size]*mul*100;
									wavsampinfo->pos=wavsampinfo->pos+1;
							} else {
									dst[j+size]=dst[j];
							}
						}


				}
				
				
				total_samples_read++;
				if (wavsampinfo->pos==wavsampinfo->length)
					 return total_samples_read;

			}
		}


		return total_samples_read;
}


static void *wav_start_samples(DUH *duh, void *signal, int n_channels, long pos)
{
	
	FILE *file;
	WAV_SAMPINFO *sampinfo;
	WAV_SIGNAL *signalx;
	unsigned short wChannels;

	(void)duh;
	signalx=signal;


	file = file_open_mixer  (signalx->filename);

	if (!file)
		return NULL;

	
	
	sampinfo = malloc(sizeof(WAV_SAMPINFO));
	if(!sampinfo) {
		fclose(file);
		return NULL;
	}

	
	file_skip_mixer (file, 22);

	file_getnc_mixer(&wChannels,2,file);

	file_skip_mixer (file, 20);

	
	
	sampinfo->stream=(char *) malloc (signalx->length); 	
	
	file_getnc_mixer(sampinfo->stream,signalx->length,file);


	
	((WAV_SAMPINFO*)sampinfo)->n_channels = wChannels;
	((WAV_SAMPINFO*)sampinfo)->pos = pos;
	((WAV_SAMPINFO*)sampinfo)->length=signalx->length;
	((WAV_SAMPINFO*)sampinfo)->bitstream=signalx->average_bitrate;

	file_close_mixer (file);

	return sampinfo;
}


static void wav_end_samples(void *sampinfo) {

	

	if(sampinfo)
	{
		free ( ((WAV_SAMPINFO*)sampinfo)->stream);
		free(sampinfo);
	}

	return;

}


static void wav_unload_signal(void *signal) {

	if(signal)
	{
		free(signal);
	}
}
