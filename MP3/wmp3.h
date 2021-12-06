#ifndef _WMP3_Z_
#define _WMP3_Z_


#include "wwave.h"
#include "wstream.h"
#include "wmp3decoder.h"


#define MAX_ERROR_STR 1024

#define ECHO_BUFFER_NUMBER 40
#define ECHO_MAX_DELAY_NUMBER 20

// MPEG version
#define MPEG1 		0
#define MPEG2		1
#define MPEG25		2

// ID3 TAG
#define ID3_VERSION2	2
#define ID3_VERSION1	1

// status
#define STATUS_PLAY 1
#define STATUS_STOP 2
#define STATUS_PAUSE 4



#define VBR_TOC_SIZE        100

#define VBR_FRAMES_FLAG     0x0001
#define VBR_BYTES_FLAG      0x0002
#define VBR_TOC_FLAG        0x0004
#define VBR_SCALE_FLAG      0x0008

static unsigned long get32bits(unsigned char *buf);
typedef real SFX_MODE[ECHO_MAX_DELAY_NUMBER];

typedef struct {

	DWORD dwFirstFrame;
	DWORD dwStreamSize;
// stream info
	LONG channel;			// number of chennels ( 1 = MONO, 2 = STEREO )
    LONG sampling_rate;     // sampling rate frequency  ( Hz )
    LONG layer;				// layer  1, 2, 3
    LONG mpeg_version;		// MPEG1, MPEG2, MPEG25
	LONG sample_per_frame;	// number of wave samples in one frame
	char mpeg_version_str[64];
    char channel_str[40];
    char emphasis_str[40];
    char layer_str[40];
	LONG bitrate;			// bitrate - average bitrate for VBR streams
    BOOL vbr;				// variable bitrate
    LONG length;			// length of song ( secconds)
    BOOL xing;				// xing frame found
    LONG frame_number;		// number of mp3 frames in stream
	int framesize;				// average frame size for VBR, first frame size for constant bitrate
	int input_buffer_length; // number of mp3 frames need to fill wave buffer
	int stereo;		// channel number 1= mono, 2=stereo
	int lsf;		//
    int mpeg25;
    int lay;
    int sampling_frequency; 

} MP3STREAM;


typedef struct {
    char Header[10];
	DWORD MajorVersion;
    DWORD RevisionNumber;
    DWORD _flags;
    struct {
    	BOOL Unsync;
        BOOL Extended;
        BOOL Experimental;
        BOOL Footer;
    } Flags;
    DWORD Size;

} ID3v2STRUCT;


typedef struct {
	DWORD Size;
    DWORD _flags;
    struct {
    	BOOL Tag_alter_preservation;
        BOOL File_alter_preservation;
        BOOL Read_only;
        BOOL Grouping_identity;
        BOOL Compression;
        BOOL Encryption;
        BOOL Unsynchronisation;
        BOOL Data_length_indicator;
    } Flags;
} ID3v2FRAME;


typedef struct
{
	char *artist;
	char *album;
	char *title;
	char *comment;
	char *genre;
	char *year;
    char *tracknum;
	char *ID3TAG;
	char *NEWID3;

	char header[10];
	int currsize;
	int currpos;
	int size;
	int ready;
	int dynbuffer;
	int	footer, extended, experimental;
	DWORD flags;

    ID3v2STRUCT Id3v2;
} ID3STRUCT;



typedef struct {
	int stereo;		// channel number 1= mono, 2=stereo
	int lsf;		//
    int mpeg25;
    int lay;
    int error_protection;
    int sampling_frequency;
    int bitrate_index;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize;  /* computed framesize */

} MP3FRAMEHEADER;



// structure to receive extracted header
// toc may be NULL
typedef struct {
    DWORD flags;      // from Xing header data
    DWORD frames;     // total bit stream frames from Xing header data
    DWORD bytes;      // total bit stream bytes from Xing header data
    int vbr_scale;  // encoded vbr scale from Xing header data
    unsigned char toc[VBR_TOC_SIZE];  // pointer to unsigned char toc_buffer[100]
                         // may be NULL if toc not desired
}   XHEADDATA;


class WMp3 {
	public:
        WMp3(); // constructor
        ~WMp3();

		// get last error string
		char* GetError() { return _lpErrorMessage;};

		// open mp3 file
		// INPUT:
		//		const char *filename - mp3 file name
		//		int WaveBufferLength - wave buffer in milliseconds, minimal 200 ms
		// RETURN: TRUE - all OK, FALSE - error, call GetError() to get error string
    	BOOL OpenMp3File(const char *filename ,int WaveBufferLengthMs);

		BOOL OpenMp3File(const char *filename, int WaveBufferLengthMs,DWORD seek, DWORD size);
		
		// close mp3 file, free memory ...
		BOOL Close();
	
	
		// open memory mp3 stream, 
		// INPUT:
		//		char* mem_stream - pointer to memory
		//		DWORD size - size of mem_stream,
		//      int WaveBufferLength - wave buffer in milliseconds, minimal 200 ms
		BOOL OpenMp3Stream(char* mem_stream, DWORD size,  int WaveBufferLengthMs);
		
		
		// open resource mp3 stream, to play mp3 from exe resource
		// INPUT:
		//		 hModule - instance of exe file,
		//       LPCTSTR lpName - name of resource ( string or MAKEINTRESOURCE )
		//      int WaveBufferLength - wave buffer in milliseconds, minimal 200 ms
		BOOL OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType,int WaveBufferLengthMs);
		
		BOOL OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType,  int WaveBufferLengthMs, DWORD seek);
		

        // Set and get output volume
        BOOL SetVolume(int lvolume, int rvolume);
		BOOL GetVolume(WORD* lvolume, WORD* rvolume);
        
        // mp3 infos, return string description 
        char* GetMpegVersion(); // mpeg version
        char* GetMpegLayer();	// layer version
        char* GetChannelMode();	// stereo, mono ot dual stereo
        char* GetEmphasis();

        LONG GetSamplingRate();	// return sampling rate
        LONG GetHeaderStart(); // get position of first valid mp3 frame
        LONG GetSongLength(); // song length in seconds
        LONG GetBitrate();	// bitrate: average for VBR, firstframe bitrate for CBR
        LONG GetCurrentBitrate(); // bitrate of current decoding frame
        LONG GetStatus();	// get current status ( STATUS_PLAY, STATUS_STOP, STATUS_PAUSE )
		BOOL IsVBR() { return w_mp3stream.vbr;};

		// return VU data for left and right channel
		// call this function and draw VU meter with this values
		int GetVUData(int* left, int* right);

		int VocalCut(BOOL fEnable);

		int InternalVolume(BOOL fEnable);

		// enable equalizer
		int EnableEQ(BOOL fEnable);

		// set equalizer data
		// int iPreAmpValue ( -5 - 0 - +5 ) 
		// int iBand ( 1 - 10) select band to change equalizer
		// int iValue ( - 10 - 0 - +10 ) value for selected eq band
		int SetEQ(int iPreAmpValue, int iBand, int iValue);


        // ID3 tag
		// get available ID3: return ID3_VERSION2,int id3Version or ID3_VERSION2|ID3_VERSION2
        int GetID3Version();

		// load ID3 data
		// int id3Version: ID3_VERSION2 or ID3_VERSION1
        BOOL LoadID3(int id3Version);
        char *GetArtist();
		char *GetTrackTitle();
		char *GetTrackNum();
		char *GetAlbum();
		char *GetYear();
		char *GetComment();
		char *GetGenre();

 
		// playing
        BOOL Play();
        BOOL Play(LONG sec);
        BOOL Stop();
        BOOL Pause();
        BOOL Resume();
        int GetPosition();  // return possition in seconds
        BOOL Seek(LONG seconds);	// seek to


		// sound processing echo and reverb
		// enable echo and set parematers
		// int iMode: echo mode ( 0 - 9) , change SFX_MODE sfx_mode[NUM_SFX_MODE * 2] in Wmp3.cpp to create new echo mode
		// int uDelayMs:echo delay in millisecond, max 1000 ms
		// int InputGain: volume of original sound ( -50 - 0 - +50) -50 no sound, 0 no change, +50 max sound
		// int EchoGain: volume of added  echo  ( -50 - 0 - +50)
		// int OutputGain: volume of output sound ( -50 - 0 - +50)
		int SetSfxParam(BOOL fEnable, int iMode,  int uDelayMs, int InputGain, int EchoGain, int OutputGain);
		
		int GetSfxMode() { return w_echo_mode; };
	

		
    private:
		BOOL w_bVocalCut; // enable/disable vocal cut processing
		WStream* w_stream; // data stream ( file stream, memory stream or resource stream )
		MP3STREAM w_mp3stream; // structure for mp3 infos ( bitrate, position, ...)
		MP3FRAMEHEADER w_mp3header; // structure for current mp3 header infos ( current bitrate, frame size ...)
		XHEADDATA w_xingheader; // structure with XING header data ( seek infos, number of frames ...)
		unsigned char* w_search_buffer; // memory buffer for mp3 chunks, needed to read data chunks from disk, no need to read whole file
		MPSTR w_mp;	// MPSTR structure, data interface to mp3decoder routines
		WWaveOut* w_wave; // wave out class, initialize wave out, play wave data to soundcard
		HANDLE w_hThread; // playing thread, decode mp3, run sound processing, send wave data to soundcard
		DWORD w_hThreadID; // playing thread ID
		HANDLE w_hEvent;	// event, trigger on wave header DONE, decode new mp3 data and send buffer to soundcard
		BOOL w_run_thread;	// run thread or exit thread
		unsigned char* w_in_data; // read necessary mp3 frames into this buffer and send this buffer to decoding function
		CRITICAL_SECTION  w_wcs; // protect stream pointer movement among main thread and playing thread 
		CRITICAL_SECTION w_wcsThreadDone; // assurance for Stop() function, when Stop() function exits, playing thread must be dead
		CRITICAL_SECTION w_wcsSfx; //  protect sound processing functions ( allocation and deallocation of buffers ) from SetSfxParam
		BOOL w_internalVolume; // adjust internal volume ( process wave samples )
        short w_lvolume; // left channel volume for internal volume control
        short w_rvolume; // right channel volume for internal volume control
        DWORD w_position; // starting position ( set on Stop(), Seek(..) ), this position + wave position = current position
        BOOL w_playing; // playing flag, control playing thread, decoding ..., set this to FALSE and thread will terminate
        BOOL w_pause;	// pause flag, control Pause() and Reset() 
        ID3STRUCT w_myID3; // ID3 structure, LoadID3(...) fill this structure with ID3 data
        ID3v2FRAME w_ID3frame; // ID3v2 support
		DWORD w_dwSeek;	// seek from beginning of stream
		DWORD w_dwSize; // stream size
		// VU meter
		DWORD w_dwPos; // position in current playing wave buffer, need to get corect VU data
		DWORD w_dwIndex; // index of current playing wave buffer
		BOOL w_vudata_ready; // get VU data only when playing
		// sfx
		char* sfx_buffer[ECHO_BUFFER_NUMBER]; // circular echo wave buffers, each buffer contains small wave chunk ( about 50 ms )
		char* sfx_buffer1[ECHO_BUFFER_NUMBER]; // there are 40 buffers ( 40 * 50 ms = 2 sec )
		BOOL w_echo; // enable echo processing 
		real w_input_gain;	// change input audio data volume
		real w_output_gain; // change output audio data volume ( input + echo )
		real w_echo_gain;	// change echo audio data volume
		unsigned int w_delay; // echo delay
		int w_old_echo_buffer_size; // use to check if we need to allocate new echo buffers
		BOOL w_shift; // shift echo buffers
		int w_echo_alloc_buffer_size; // size of allocated echo buffer
		SFX_MODE w_sfx_mode_l; // SFX_MODE data, use this data to add echo
		SFX_MODE w_sfx_mode_r; // SFX_MODE data, use this data to add echo
		int w_right; // process right and left channel
		int w_echo_mode; // current echo_mode
		int w_sfx_offset; // offset ( number of echo buffers to skip into past )

		int w_preamp; // preamp value for equalizer
		int w_eq[32]; // equalizer data
		
		
		int InitSfx(); // initalize data for echo processing
		static int DoEcho(WMp3* mp3, char* buf, int len); // process adion data and add echo
        static BOOL _HeadCheck(unsigned long head); // check if  valid mp3 header
       
		static BOOL _LoadWaveBuffer(WMp3* mp3, int frame_num, int bufferID); // decode mp3 frames into wave buffers
		static int _GetID3Version(WMp3* mp3); // return available ID3 versions
		static BOOL _LoadID3(WMp3* mp3,int id3Version); // load ID3 data
		static char* _RetrField(WMp3* mp3, char *identifier, DWORD* Size);
        int _SeekPoint(int seconds);

		static void _ThreadFunc(void* lpdwParam);
		static BOOL FindNextValidFrame (WMp3* mp3);
		static BOOL _DecodeMp3FrameHeader(unsigned char *frame,MP3FRAMEHEADER* header);
		static void CALLBACK _WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
				DWORD dwParam1, DWORD dwParam2);

		static unsigned long _Get32bits(unsigned char *buf);
        BOOL _FrameIsNull(char *frame, int chars);
        static DWORD _RemoveLeadingNULL( char *str , DWORD size);
        static BOOL _DecodeFrameHeader(WMp3* mp3, char *header, char *identifier);
        static DWORD _DecodeUnsync(unsigned char* header, DWORD size);
		static BOOL _DecodeXingHeader(WMp3* mp3);
		static BOOL _Open(WMp3* mp3, WStream* stream, int WaveBufferLength, DWORD seek, DWORD size);
		char* _lpErrorMessage;
		void error(char* err_message);

};



#endif

