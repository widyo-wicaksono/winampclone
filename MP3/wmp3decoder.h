#ifndef _W_MP3DECODER_
#define _W_MP3DECODER_



#define MAXFRAMESIZE 1792

#define         SBLIMIT                 32
#define         SSLIMIT                 18

# define M_PI       3.14159265358979323846
# define M_SQRT2	1.41421356237309504880
# define REAL_IS_FLOAT
# define NEW_DCT9

# define random rand
# define srandom srand

/*

#ifdef REAL_IS_FLOAT
#  define real float
#elif defined(REAL_IS_LONG_DOUBLE)
#  define real long double
#else
#  define real double
#endif
*/


#define real double


struct frame {
    int stereo;
    int jsbound;
    int single;
    int lsf;
    int mpeg25;
    int header_change;
    int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */

    /* layer2 stuff */
    int II_sblimit;
    void *alloc;
};

typedef struct {
	struct buf *head,*tail;
	int bsize;
	int framesize;
    int fsizeold;
	struct frame fr;
    unsigned char bsspace[2][MAXFRAMESIZE+512]; /* MAXFRAMESIZE */
	real hybrid_block[2][2][SBLIMIT*SSLIMIT];
	int hybrid_blc[2];
	unsigned long header;
	int bsnum;
	real synth_buffs[2][2][0x110];
    int  synth_bo;
	real equalizer[2][32];
	int have_eq_settings;
} MPSTR;







#ifndef BOOL
#define BOOL int
#endif

#define MP3_ERR -1
#define MP3_OK  0
#define MP3_NEED_MORE 1


BOOL  InitMP3(MPSTR *mp);
int  decodeMP3(MPSTR *mp,char *inmemory,int inmemsize,
     char *outmemory,int outmemsize,int *done);
void ExitMP3(MPSTR *mp);



#endif
