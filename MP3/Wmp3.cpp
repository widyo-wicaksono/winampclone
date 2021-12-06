#define STRICT
#include <windows.h>
#pragma hdrstop

#define LAYER1
#define LAYER2



#include <stdio.h>
#include <math.h>


#include "wmp3decoder.h"
#include "wmp3.h"







#define ERRORMESSAGE_SIZE 1024 // max error message string size

#define AVG_SINE_16 23169.00 // average sine size in 16 bit audio data, need to calculate VU data
#define SINGLE_AUDIO_BUFFER_SIZE 100  // single sdio buffer size in ms ( 100 ms each buffer )

#define SEARCH_DEEP_VBR 20 // check next 20 mp3 frames to determine constant or variable bitrate if no XING header found 


// channel modes
#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3





// vbr header
#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008


#define MIN_FRAME_SIZE 24 // minimal mp3 frame size
#define MAX_FRAME_SIZE 5761 // max frame size


#define SEARCH_BUFFER_SIZE 8192		// buffer size for stream search functions

const char empty_string[2] = "\0";


void RemoveEndingSpaces(char* buff);	
unsigned long Get32bits(unsigned char *buf);

extern long freqs[9];
extern int tabsel_123[2][3][16];


// gain table for equalizer
real gaintable[21]={0.112, 0.177, 0.214, 0.251, 0.303, 0.355, 0.527, 0.5, 0.603, 0.707,  1.0, 1.707, 1.414, 2.0, 2.121,  2.828, 3.404,  3.98, 4.08, 4.38, 5.08};
real gtable2[11]={0.355, 0.5, 0.603, 0.707, 0.853, 1.0, 1.207, 1.414, 1.707, 2.0, 2.12};


// echo processing

#define NUM_SFX_MODE 10 // number of ECHO modes
#define MAX_DELAY 1000 // max echo delay in ms ( 1000 ms = 1 sec )

// SFX_MODE
// 10 echo modes, separate left and right channel.
// there are 20 echo buffers, each buffer has about 50 ms audio data.
// 20 bufers * 50 ms = 1 second past audio data.
// first value represents current playing buffer, second is 50 ms old, third is 100 ms old ....
// buffer contains half volume audio data.
// value in sfx_mode multiply each audio sample volume
// example
// for 50 ms delay set { 0, 0.7, 0, 0, 0, 0, ......
// for e.g. 150 ms delay set {0, 0, 0, 0.6, 0, 0, ....
// for multiple echo set e.g. {0.5, 0.2, 0, 0, 0, 0.3, 0.2, .....
//
// delay value in SetSfxParam determines deley for first value in sfx_mode
//
// changing values in sfx_mode you can create varios reverb and multiple echo effects 

SFX_MODE sfx_mode[NUM_SFX_MODE * 2] = {
// mode 0
{0.5,0.2 ,0.1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0, 0, 0}, // left channel
{0.5, 0.2, 0.1, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0}, // right channel
// mode 1
{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
// mode 2
{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
// mode 3
{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
{0, 0, 0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.02},
// mode 4
{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
{0.5,0.1,0,0,0,0,0,0,0,0.5,0.2,0.1,0,0,0,0,0,0.3,0.2,0.04},
// mode 5
{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
// mode 6
{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
// mode 7
{0.5,0.4,0.2,0.1,0.025,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1,0.05,0.01},
{0,0,0,0.5,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0.3,0.2,0.05,0.01},
// mode 8
{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
// mode 9
{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01},
{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01}

};




const char *sGenres [] =
	{
		"Blues",
		"Classic Rock",
		"Country",
		"Dance",
		"Disco",
		"Funk",
		"Grunge",
		"Hip-Hop",
		"Jazz",
		"Metal",
		"New Age",
		"Oldies",
		"Other",
		"Pop",
		"R&B",
		"Rap",
		"Reggae",
		"Rock",
		"Techno",
		"Industrial",
		"Alternative",
		"Ska",
		"Death Metal",
		"Pranks",
		"Soundtrack",
		"Euro-Techno",
		"Ambient",
		"Trip Hop",
		"Vocal",
		"Jazz+Funk",
		"Fusion",
		"Trance",
		"Classical",
		"Instrumental",
		"Acid",
		"House",
		"Game",
		"Sound Clip",
		"Gospel",
		"Noise",
		"Alternative Rock",
		"Bass",
		"Soul",
		"Punk",
		"Space",
		"Meditative",
		"Instrumental Pop",
		"Instrumental Rock",
		"Ethnic",
		"Gothic",
		"Darkwave",
		"Techno-Industrial",
		"Electronic",
		"Pop-Folk",
		"Eurodance",
		"Dream",
		"Southern Rock",
		"Comedy",
		"Cult",
		"Gangsta Rap",
		"Top 40",
		"Christian Rap",
		"Pop/Punk",
		"Jungle",
		"Native American",
		"Cabaret",
		"New Wave",
		"Phychedelic",
		"Rave",
		"Showtunes",
		"Trailer",
		"Lo-Fi",
		"Tribal",
		"Acid Punk",
		"Acid Jazz",
		"Polka",
		"Retro",
		"Musical",
		"Rock & Roll",
		"Hard Rock",
		"Folk",
		"Folk/Rock",
		"National Folk",
		"Swing",
		"Fast-Fusion",
		"Bebob",
		"Latin",
		"Revival",
		"Celtic",
		"Blue Grass",
		"Avantegarde",
		"Gothic Rock",
		"Progressive Rock",
		"Psychedelic Rock",
		"Symphonic Rock",
		"Slow Rock",
		"Big Band",
		"Chorus",
		"Easy Listening",
		"Acoustic",
		"Humour",
		"Speech",
		"Chanson",
		"Opera",
		"Chamber Music",
		"Sonata",
		"Symphony",
		"Booty Bass",
		"Primus",
		"Porn Groove",
		"Satire",
		"Slow Jam",
		"Club",
		"Tango",
		"Samba",
		"Folklore",
		"Ballad",
		"power Ballad",
		"Rhythmic Soul",
		"Freestyle",
		"Duet",
		"Punk Rock",
		"Drum Solo",
		"A Capella",
		"Euro-House",
		"Dance Hall",
		"Goa",
		"Drum & Bass",
		"Club-House",
		"Hardcore",
		"Terror",
		"indie",
		"Brit Pop",
		"Negerpunk",
		"Polsk Punk",
		"Beat",
		"Christian Gangsta Rap",
		"Heavy Metal",
		"Black Metal",
		"Crossover",
		"Comteporary Christian",
		"Christian Rock",
		"Merengue",
		"Salsa",
		"Trash Metal",
		"Anime",
		"JPop",
		"Synth Pop"
	};




typedef char mystr[40];

const mystr sMPEGaudioVersion[2][2]=
	{
    	{"MPEG Version 1","MPEG Version 2"},
    	{ "reserver","MPEG Version 2.5"}

    };

const mystr sMPEGlayer[3]=
	{	"Layer I",
    	"Layer II",
        "Layer III"
    };

const mystr sChannelMode[4]=
	{	"Stereo",
    	"Joint stereo (Stereo)",
        "Dual channel (Stereo)",
        "Single channel (Mono)"
    };

const mystr sEmphasis[4]=
	{   "none",
    	"50/15 ms",
    	"reserved",
        "CCIT J.17"
    };


#define GetFourByteSyncSafe(value1, value2, value3, value4) (((value1 & 255) << 21) | ((value2 & 255) << 14) | ((value3 & 255) << 7) | (value4 & 255))
#define CreateFourByteField(value1, value2, value3, value4) (((value1 & 255) << 24) | ((value2 & 255) << 16) | ((value3 & 255) << 8) | (value4 & 255))
#define ExtractBits(value, start, length) (( value >> start ) & (( 1 << length ) -1 ))





WMp3::WMp3()
{
// class constructor, initalize all variables


// initialize equalizer values
    for(int j=0; j<32; j++) {
        w_mp.equalizer[0][j]=w_mp.equalizer[1][j]=1.0;
		w_eq[j] = 10;
    }

	w_preamp = 5; // preamp value
	w_mp.have_eq_settings = 0; // start vith disabled equalizer

	w_bVocalCut = FALSE; // start with disabled echo cut


// sfx
	w_echo = 0; // start with disabled echo processing
	w_echo_mode = 0; // starting echo mode
	w_input_gain = 1; // input gain 1.0 ( no volume change )
	w_output_gain = 1; // output gain 1.0 ( no volume change )
	w_echo_gain = 1; // echo gain 1.0 ( no volume change )
	w_delay = 0;	// delay 0 ms, but some echo modes produces delay with this value set to 0
	w_echo_alloc_buffer_size = 0; // size of single circular echo buffer
	w_old_echo_buffer_size = 0; // need this to check if we need to reallocate memory for echo buffers

	// set all pointers to echo buffers to 0, there is no alocated buffer
	for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
		sfx_buffer[i] = 0;
					
	w_shift = 0; // for first entry into DoEcho function don't shift buffers

// sfx end

	w_search_buffer = 0;
	_lpErrorMessage = (char*) malloc(ERRORMESSAGE_SIZE);
	*_lpErrorMessage = 0;	
	w_stream = 0;
	w_in_data = 0;

// VU data variables
	w_dwPos = 0;
	w_dwIndex = 0;
	w_vudata_ready = FALSE;


	ZeroMemory(&w_mp3stream, sizeof(MP3STREAM));
	ZeroMemory(&w_mp3header, sizeof(MP3FRAMEHEADER));

  // internal volume control
    w_lvolume = 100;
    w_rvolume = 100;

	w_dwSeek = 0;

// initialize critial sections
	InitializeCriticalSection(&w_wcs);
	InitializeCriticalSection(&w_wcsThreadDone);
	InitializeCriticalSection(&w_wcsSfx);

	w_playing = FALSE; // this value controls playing thread, when FALSE thread ends
    w_pause = FALSE; // need for Pause() and Resume() functionality

    w_position = 0; // starting song position

    w_wave = new WWaveOut(); // class for playing wave buffers to soundcard


    w_hThread = 0; // playing thread handle
	w_hEvent = 0; // event handle 
	w_run_thread = FALSE; // this variable controls playing thread, when FALSE thread ends

 

    // initialize ID3 fielda
    w_myID3.album = (char*) malloc(2);
	*w_myID3.album = 0;
    w_myID3.artist = (char*) malloc(2);
	*w_myID3.artist = 0;
    w_myID3.comment = (char*) malloc(2);
	*w_myID3.comment = 0;
    w_myID3.genre = (char*) malloc(2);
	*w_myID3.genre = 0;
    w_myID3.title = (char*) malloc(2);
	*w_myID3.title = 0;
    w_myID3.year = (char*) malloc(2);
	*w_myID3.year = 0;
    w_myID3.tracknum = (char*) malloc(2);
	*w_myID3.tracknum = 0;

}


WMp3::~WMp3()
{
// class destructor

 	Stop(); // stop playing

// free possibly allocated memory


	if(w_myID3.album)
    	free(w_myID3.album);

    if(w_myID3.artist)
    	free(w_myID3.artist);
 
    if(w_myID3.comment)
    	free(w_myID3.comment);

    if(w_myID3.title)
    	free(w_myID3.title);

    if(w_myID3.year)
    	free(w_myID3.year);

    if(w_myID3.tracknum)
    	free(w_myID3.tracknum);

	if(w_myID3.genre)
    	free(w_myID3.genre);

	if(w_hEvent)
		CloseHandle(w_hEvent);

	if(w_in_data)
		free(w_in_data);


	if(_lpErrorMessage)
		free(_lpErrorMessage);

	if(w_stream) // delete wstream class
		delete w_stream;

	if(w_wave) // delete wwaveoutclass
		delete w_wave;


	for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
		if(sfx_buffer[i])
			free(sfx_buffer[i]);

	if(w_search_buffer)
		free(w_search_buffer);


// free critical sections		
	DeleteCriticalSection(&w_wcs);
	DeleteCriticalSection(&w_wcsThreadDone);
	DeleteCriticalSection(&w_wcsSfx);


}



BOOL WMp3::OpenMp3File(const char *filename, int WaveBufferLengthMs)
{
// open mp3 file from disk
// INPUT:	const char *filename	- filename
//			int WaveBufferLength		
	return OpenMp3File(filename,WaveBufferLengthMs,0,0);
}


BOOL WMp3::OpenMp3File(const char *filename, int WaveBufferLengthMs, DWORD seek, DWORD size)
{
// open mp3 file
// INPUT:	const char *filename		- filename


	if(w_stream) {
		error("You need to close mp3 file before open new one");
		return FALSE;
	
	}

	WStream* stream = new WStream();
	if(!stream->OpenFile(filename, ACCESS_READ, SHARE_READ, OPEN_EXISTING,0)) {
		error("WMp3::OpenMp3File\n\nCan't open file");
		delete stream;
        return FALSE;

	}

	BOOL ret = _Open(this, stream, WaveBufferLengthMs,seek,size);
	if(!ret) {
		delete stream;
		return FALSE;
	}

	return TRUE;
}




BOOL WMp3::_Open(WMp3* mp3, WStream* stream, int WaveBufferLength, DWORD seek, DWORD size)
{
// open mp3 file
// INPUT:	WMp3* mp3	- valid mp3 class pointer
//			WStream* stream	- valid stream class pointer
//			WaveBufferLength	- wave buffer length in milliseconds ( this value will be divided into small 100 ms buffers
//			DWORD seek			- if you need to skip some data in stream, e.g. set real stream beginning to new position
//			DWORD size			- size of stream, if you seek stream, set correct size, if size = 0 stream size is calculated


	// allocate memory for search buffer, need this buffer to read small chunks from disk file
	mp3->w_search_buffer = (unsigned char*) malloc(SEARCH_BUFFER_SIZE);
	if(!mp3->w_search_buffer) {
		mp3->error("WMp3::_Open\n\nCan't allocate memory for search buffer!");
		mp3->w_stream = 0;
        return FALSE;
	}

// initialize some mp3 stream values into crazy values
	mp3->w_mp3stream.lay = -1; // can't be
	mp3->w_mp3stream.sampling_frequency = -1; //can't be
	mp3->w_mp3stream.stereo = -1; //can't be


	// set stream value
	mp3->w_stream = stream;
	// set seek value
	mp3->w_dwSeek = seek;

// seek stream, mostly this function seek to beginning of stream
// because we don't need to
	if(!mp3->w_stream->Seek(mp3->w_dwSeek,STREAM_BEGIN)) {
		mp3->error("WMp3::_Open\n\nCan't seek !");
		mp3->w_stream = 0;
        return FALSE;
	}


// search stream for first valid mp3 frame, skip possible ID3 data ...
	if(!FindNextValidFrame (mp3)) {
    	mp3->error("WMp3::_Open   *** Can't find first valid mp3 frame");
    	mp3->w_stream = 0;
		return FALSE;
	}

// first valid mp3 frame found, we have valid mp3 stream
// mp3->w_mp3header structure is filled with data about this mp3 stream ( bitrate, layer, channel ...)


// set or calculate stream size
	if(size)
		mp3->w_dwSize = size;
	else
		mp3->w_dwSize = mp3->w_stream->GetStreamSize() - seek;	

// set pointer to first frame, we need this so seek stream to beginning on Stop() function
// and to perform seeking operations on mp3 song
	mp3->w_mp3stream.dwFirstFrame = mp3->w_stream->GetStreamPointer();
	
// set mp3 stream size, consider position of first valid mp3 frame
	if(size == 0)
		mp3->w_mp3stream.dwStreamSize = mp3->w_stream->GetStreamSize() - mp3->w_mp3stream.dwFirstFrame;
	else
		mp3->w_mp3stream.dwStreamSize = size - ( mp3->w_mp3stream.dwFirstFrame - seek);
	
// set some others mp3 stream values ( frequency, channel, layer ...)
	mp3->w_mp3stream.sampling_frequency = mp3->w_mp3header.sampling_frequency;
	mp3->w_mp3stream.stereo = mp3->w_mp3header.stereo;
	mp3->w_mp3stream.lsf = mp3->w_mp3header.lsf;
	mp3->w_mp3stream.mpeg25 = mp3->w_mp3header.mpeg25;


	// number of channels
	mp3->w_mp3stream.channel = mp3->w_mp3header.stereo;
	// sampling rate frequency
	mp3->w_mp3stream.sampling_rate = freqs[mp3->w_mp3header.sampling_frequency];
	// layer version
	mp3->w_mp3stream.layer = mp3->w_mp3header.lay;

	if(mp3->w_mp3header.mpeg25 == 1)
    	mp3->w_mp3stream.mpeg_version = MPEG25;
	else if (mp3->w_mp3header.lsf == 0)
   		mp3->w_mp3stream.mpeg_version = MPEG1;
	else mp3->w_mp3stream.mpeg_version = MPEG2;

	// size of frame ( number of samples contained in one mp3 frame )
	if (mp3->w_mp3stream.mpeg_version == MPEG2 || mp3->w_mp3stream.mpeg_version == MPEG25 ) // MPEG 2 & MPEG 2.5
    	mp3->w_mp3stream.sample_per_frame = 576;
	else	// MPEG 1
    	mp3->w_mp3stream.sample_per_frame = 1152;

	// MPEG version
	strcpy(mp3->w_mp3stream.mpeg_version_str, sMPEGaudioVersion[mp3->w_mp3header.mpeg25][mp3->w_mp3header.lsf]);
	// MPEG layer
	strcpy(mp3->w_mp3stream.layer_str, sMPEGlayer[mp3->w_mp3header.lay-1]);
	// channel mode
	strcpy(mp3->w_mp3stream.channel_str, sChannelMode[mp3->w_mp3header.mode]);
	// emphasis
	strcpy(mp3->w_mp3stream.emphasis_str, sEmphasis[mp3->w_mp3header.emphasis]);

	mp3->w_mp3stream.bitrate = tabsel_123[mp3->w_mp3header.lsf][mp3->w_mp3header.lay-1][mp3->w_mp3header.bitrate_index];

	int framesize = mp3->w_mp3header.framesize;

	mp3->w_mp3stream.lay = mp3->w_mp3header.lay;


	// check for ID3 tag
	int id3ver = _GetID3Version(mp3);


	// calculate stream size, consider ID3 tags
	if(  id3ver & ID3_VERSION1 ) {
		mp3->w_mp3stream.dwStreamSize -=  128;	
	}


// now, chech if constant or variable bitrate

	mp3->w_mp3stream.vbr = FALSE;
	// search XING header
	mp3->w_mp3stream.xing = _DecodeXingHeader(mp3);
	// seek again to first frame
	mp3->w_stream->Seek(mp3->w_mp3stream.dwFirstFrame,STREAM_BEGIN);

	if(mp3->w_mp3stream.xing) { // xing header found, variable bitrate
		mp3->w_mp3stream.vbr = TRUE;
	}
	else { // no xing header, check if variable bitrate, check next SEARCH_DEEP_VBR frames
		// there is no XING header, so we don't know if this is CBR or VBR
		int bitrate = mp3->w_mp3header.bitrate_index;
		mp3->w_mp3stream.vbr = FALSE;
		int num = 1;
		int sum_bitrate = tabsel_123[mp3->w_mp3header.lsf][mp3->w_mp3header.lay-1][mp3->w_mp3header.bitrate_index];
		// read mp3 frames and check bitrate of this frames
		for(int i = 0; i < SEARCH_DEEP_VBR; i++) { 
			if(!FindNextValidFrame (mp3)) {
				break;

			}
			if(! mp3->w_mp3stream.vbr && bitrate != mp3->w_mp3header.bitrate_index) {
				mp3->w_mp3stream.vbr = TRUE;
				// there are differences in bitrates, so this mp3 is VBR
			}
			
			// need this values to calculate average bitrate, this value is needed to seek song position
			sum_bitrate += tabsel_123[mp3->w_mp3header.lsf][mp3->w_mp3header.lay-1][mp3->w_mp3header.bitrate_index];
			num++;
			framesize += mp3->w_mp3header.framesize;
			mp3->w_stream->Seek(framesize,STREAM_CURRENT);
					
		}
		// VBR file without XING header
		mp3->w_mp3stream.bitrate = sum_bitrate / num; // average bitrate
		framesize = framesize / num; // average frame size
	}



    if(mp3->w_mp3stream.vbr) { // variable bitrate
    	if(mp3->w_mp3stream.xing) { // found xing header
        	mp3->w_mp3stream.frame_number = mp3->w_xingheader.frames;
            // average frame size
            mp3->w_mp3stream.framesize = mp3->w_mp3stream.dwStreamSize / mp3->w_mp3stream.frame_number;
            // calculate length of song ( seconds)
            mp3->w_mp3stream.length = ( mp3->w_mp3stream.frame_number * mp3->w_mp3stream.sample_per_frame ) / mp3->w_mp3stream.sampling_rate;
            // average bitrate
            mp3->w_mp3stream.bitrate = mp3->w_mp3stream.dwStreamSize  / mp3->w_mp3stream.length * 8 / 1000;

        }
        else {
        	// calculate frame number or count frames in file
            // for now just do use precalculated average framesize( possible huge error  )
            
			mp3->w_mp3stream.framesize = framesize;
			mp3->w_mp3stream.frame_number = mp3->w_mp3stream.dwStreamSize / framesize;
        	
        	// calculate length of song ( secconds)
        	mp3->w_mp3stream.length = ( mp3->w_mp3stream.frame_number * mp3->w_mp3stream.sample_per_frame ) / mp3->w_mp3stream.sampling_rate;
            // average bitrate
            mp3->w_mp3stream.bitrate = mp3->w_mp3stream.dwStreamSize  / mp3->w_mp3stream.length * 8 / 1000;

        }
    }
    else { // constant bitrate
		if(mp3->w_mp3header.framesize==0)
			mp3->w_mp3header.framesize=1;
    	mp3->w_mp3stream.frame_number = mp3->w_mp3stream.dwStreamSize / mp3->w_mp3header.framesize;
        mp3->w_mp3stream.framesize = mp3->w_mp3header.framesize;
        // calculate length of song ( seconds)
        mp3->w_mp3stream.length = ( mp3->w_mp3stream.frame_number * mp3->w_mp3stream.sample_per_frame ) / mp3->w_mp3stream.sampling_rate;
    }

// seek stream to first frame
	if(!mp3->w_stream->Seek(mp3->w_mp3stream.dwFirstFrame,STREAM_BEGIN)) {
		mp3->error("WMp3::_Open\n\nCan't seek!!!!!");
		mp3->w_stream = 0;
		return FALSE;
	}




// initalize wave out

// check buffer length, we need at least 200 ms,
    if(WaveBufferLength < 200 ) {
    	mp3->error("WMp3::_Open\n\nBuffer can't be smaller than 200 ms");
    	mp3->w_stream = 0;
		return FALSE;
    }

// create event, this event controls playing thread
    if( (mp3->w_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
    	mp3->error("WMp3::_Open\n\nCan't create event");
     	mp3->w_stream = 0;
		return FALSE;
    }


// calculate number of mp3 frames to fill one internal wave buffer ( ~ 100 ms )
	mp3->w_mp3stream.input_buffer_length = MulDiv(SINGLE_AUDIO_BUFFER_SIZE,mp3->w_mp3stream.sampling_rate,1000 * mp3->w_mp3stream.sample_per_frame ); 
// calculate size of single wave buffer
	int single_audio_buffer_len = MulDiv((mp3->w_mp3stream.input_buffer_length + 1) * 1000, mp3->w_mp3stream.sample_per_frame ,mp3->w_mp3stream.sampling_rate);
// now, divide wave buffer length into number of small wave buffers
	int WaveBufferNum = MulDiv(WaveBufferLength,1 , single_audio_buffer_len);


// initialize wave out, sampling rate, channel, bits per sample, ...
     if ( ! mp3->w_wave->Initialize(WAVE_FORMAT_PCM, (WORD)mp3->w_mp3stream.channel, mp3->w_mp3stream.sampling_rate,16,
    		WaveBufferNum, single_audio_buffer_len)) {
            mp3->error("WMp3::_Open\n\nCan't initialize wave out");
			CloseHandle(mp3->w_hEvent);
			mp3->w_stream = 0;
			return FALSE;
     }

// calculate size for one echo buffer ( ~ 50 ms ) 
   mp3->w_echo_alloc_buffer_size = mp3->w_wave->GetBufferAllocSize() / 2 + 1;

// allocate memory for input buffer, this buffer we will send to decoding function
	mp3->w_in_data = (unsigned char*) malloc(MAX_FRAME_SIZE * mp3->w_mp3stream.input_buffer_length);
	if(!mp3->w_in_data) {
		mp3->error("WMp3::Init\n\nCan't allocate input buffer.");
		CloseHandle(mp3->w_hEvent);
		mp3->w_hEvent = 0;
		mp3->w_stream = 0;
		return 0;
	}

// all OK, return TRUE
    return TRUE;
}





BOOL WMp3::_LoadWaveBuffer(WMp3* mp3, int frame_num, int bufferID)
{

//	read frame_num frames from disk and decode in wave buffer
//	INPUT:		WMp3* mp3			- valid mp3 class		
//				int frame_num		- number of frames to read
//              int bufferID		- output buffer ID
//
//	RETURN:		TRUE - all OK,  FALSE - end of stream, no more frames

	
	if(!mp3->w_playing) return FALSE;
// save current stream pos
	int oldpos = mp3->w_stream->GetStreamPointer();
// initalize some variables
	mp3->w_mp3header.framesize = 0;
	DWORD dwRead = 0;
	DWORD dwSize = 0;
	unsigned char* in = mp3->w_in_data;
// each stream seeking and reading operation need to be protected in multithread enviroment
	EnterCriticalSection(&mp3->w_wcs);

	DWORD pointer = mp3->w_stream->GetStreamSize();
	DWORD end = mp3->w_mp3stream.dwFirstFrame + mp3->w_mp3stream.dwStreamSize - MIN_FRAME_SIZE - 1;
// now, read necessary frames into input buffer
	for(int i = 0; i < frame_num; i++) {
		if(!mp3->w_playing) {
			LeaveCriticalSection(&mp3->w_wcs);
			return FALSE;
		}
		pointer = mp3->w_stream->GetStreamPointer();
		if(pointer > end || !mp3->w_stream->Read((char*) in,4,&dwRead) || dwRead < 4) {
			// end of stream
			if(i == 0) {
				LeaveCriticalSection(&mp3->w_wcs);
				return 0;
			}
			else {
				mp3->w_stream->Seek(0,STREAM_END);
				break;
			}
		}

// preforme checking for each frame, we need  valid mp3 frames
		if(!_DecodeMp3FrameHeader(in, &mp3->w_mp3header) || mp3->w_mp3stream.lay != mp3->w_mp3header.lay || 
			mp3->w_mp3stream.stereo != mp3->w_mp3header.stereo || 
			mp3->w_mp3stream.sampling_frequency != mp3->w_mp3header.sampling_frequency || mp3->w_mp3header.framesize < MIN_FRAME_SIZE) { // invalid frame
				
				if(!mp3->w_stream->Seek(-3,STREAM_CURRENT)) {
					LeaveCriticalSection(&mp3->w_wcs);
					return 0;
				}
				if(!FindNextValidFrame (mp3)) { // search next valid frame
				// end of stream, no more valid mp3 frames
					LeaveCriticalSection(&mp3->w_wcs);
					return 0;
				}
				else { // new frame found
					i--;
					continue;

				}
		}

		if(!mp3->w_playing) {
			LeaveCriticalSection(&mp3->w_wcs);
			return FALSE;
		}

		// read rest of frame
		if(pointer > end || !mp3->w_stream->Read((char*) ( in + 4) , mp3->w_mp3header.framesize - 4, &dwRead) || dwRead < (DWORD) (mp3->w_mp3header.framesize - 4)) {
			if( i == 0) { // can't read rest of first frame
				// end of stream, no more valid mp3 frames
				LeaveCriticalSection(&mp3->w_wcs);
				return 0;
			}
			else { // some frames read, OK, but end of stream
				mp3->w_stream->Seek(0,STREAM_END);
			
				break;
			}
		}

		dwSize += mp3->w_mp3header.framesize;
		in +=  mp3->w_mp3header.framesize;

		if(!mp3->w_playing) {
			LeaveCriticalSection(&mp3->w_wcs);
			return FALSE;
		}

		
	}

	LeaveCriticalSection(&mp3->w_wcs);

// now, we have input buffer. Send this buffer to decoding engine

	int ret;
	int size = 0;
	char *out_buff = mp3->w_wave->GetBufferData(bufferID);
	int buff_size = 0;
// mp3 decoding engine
	ret = decodeMP3(&mp3->w_mp, (char*) mp3->w_in_data ,dwSize,out_buff,8192,&size);
	
    if (ret == MP3_ERR) {
			InitMP3(&mp3->w_mp);
			mp3->w_wave->SetBufferSize(bufferID, 0);
			return 1;
	}

 
	while(ret == MP3_OK)
    {
		if(!mp3->w_playing) {
			return FALSE;
		}

        out_buff += size;
        buff_size += size;
		ret = decodeMP3(&mp3->w_mp, NULL, 0, out_buff, 8192, &size);
        if (ret == MP3_ERR) {
        	InitMP3(&mp3->w_mp);
			mp3->w_wave->SetBufferSize(bufferID, 0);
			return 1;
		}	
    }
// now, we have ready new wave buffer, return to playing thread
     mp3->w_wave->SetBufferSize(bufferID, buff_size);
     return TRUE;
}




BOOL WMp3::Stop()
{
// stop playing

// disable VU data, don't waste processor (CPU) time
	w_vudata_ready = FALSE;
	
// prevent program crash if we call this function on empty ( not opened ) mp3
	if(!w_stream) {
		error("WMp3::Play\n\nStream isn't open!");
		return 0;
	}
	
	if(!w_playing)
		return FALSE;

	
// set playing flag to FALSE, this will stop decoding and playing thread exits	
	w_playing = FALSE;
    w_pause = FALSE;
	w_wave->Stop(); // stop wave playing
	SetEvent(w_hEvent);

// be sure that playing thread is ended
	EnterCriticalSection(&w_wcsThreadDone);
    LeaveCriticalSection(&w_wcsThreadDone);

// seek stream to begining of mp3 data ( to first frame )

	EnterCriticalSection(&w_wcs);
	w_stream->Seek(w_mp3stream.dwFirstFrame, STREAM_BEGIN);
	w_position = 0; // reset song position
	w_dwPos = 0; // VU data
	w_dwIndex = 0; // VU data
	LeaveCriticalSection(&w_wcs);

    return TRUE;

}



BOOL WMp3::Play()
{
// play mp3

// disable VU data, don't waste processor (CPU) time	
	w_vudata_ready = FALSE;

// prevent program crash if we call this function on empty ( not opened ) mp3	
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	if(w_pause)
	{
		return Resume();
	}
    
	
	if(w_playing)
    	return FALSE;

// set playing flag
    w_playing = TRUE;
	w_pause = FALSE;

	w_dwPos = 0;
	w_dwIndex = 0;

// previous playing thread must be terminated	
	EnterCriticalSection(&w_wcsThreadDone);
// create playing thread
	if(!w_hThread)
		w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFunc, (void*) this,0,&w_hThreadID);
	
	LeaveCriticalSection(&w_wcsThreadDone);

    if( w_hThread == NULL) {
    	error("WMp3::Initialize   *** Can't create thread");
        return FALSE;
	}	
	
// trigger thread, start decoding mp3 frames and send wave data to soundcard
	SetEvent(w_hEvent);

    return TRUE;
}





void WMp3::_ThreadFunc(void* lpdwParam)
{
// main playing thread ( decoding mp3, equalizer, echo processing ...)
	WMp3* mp3 = (WMp3*) lpdwParam;

// protect Stop() function, when Stop() function returns, this thread is terminated
	EnterCriticalSection(&mp3->w_wcsThreadDone);
// initialize echo processing
	if(mp3->w_echo)
		mp3->InitSfx();
// initialize mp3 decoder
	InitMP3(&mp3->w_mp);
	UINT i;
// unprepare wave buffers
	for( i = 0; i < mp3->w_wave->GetNumOfBuffers(); i++) {
   		mp3->w_wave->SetBufferDone(i);
		mp3->w_wave->UnprepareHeader(i);
	}

// open wave output
	  if ( !mp3->w_wave->OpenAudioOutput(WAVE_MAPPER, (DWORD)  mp3->_WaveOutProc, (DWORD)  mp3, CALLBACK_FUNCTION)) {
		LeaveCriticalSection(&mp3->w_wcsThreadDone);
		ExitThread(0);
    	return;
    }

// playing loop
	while(mp3->w_playing) {
	// wait for event ( WAVE BUFFER DONE )
		WaitForSingleObject(mp3->w_hEvent,INFINITE);
		if(!mp3->w_playing)
			break;
	// loop through all wave buffers and decode mp3 into each buffer
	// then send this buffers to soundcard
		for(UINT i = 0; i < mp3->w_wave->GetNumOfBuffers(); i++ ) {
			if(!mp3->w_playing)
				break;
						
			if(mp3->w_wave->IsBufferDone(i)) {
			// if buffer is returned from soundcard, unprepare buffer
				mp3->w_wave->UnprepareHeader(i);
				if(mp3->w_playing) {
					// decode mp3 into wave buffer
					if (mp3->_LoadWaveBuffer(mp3,mp3->w_mp3stream.input_buffer_length, i)) {
						
						if(!mp3->w_playing)
							break;
					

						char* buf = mp3->w_wave->GetBufferData(i);
						int size = mp3->w_wave->GetBufferSize(i);
					// vocal cut processing
                        if(mp3->w_bVocalCut) {
                        	for(DWORD j = 0; j < mp3->w_wave->GetBufferSize(i); j += 4) {
                            	short* left;
								short* right;
								left = (short*) ( buf + j);
                            	right = (short*) ( buf + j + 2);
								short l1;
								short r1;
								l1 = *left - *right;
								r1 = *right - *left;
								*left = l1;
								*right = r1;	
							}
							
                        }

					// internal volume processing
						if(mp3->w_internalVolume) {
                        	for(DWORD j = 0; j < mp3->w_wave->GetBufferSize(i); j += 4) {
                            	short* left;
								short* right;
								left = (short*) ( buf + j);
                            	right = (short*) ( buf + j + 2);
                            	*left = *left * mp3->w_lvolume / 100;
                            	*right = *right *mp3->w_rvolume / 100;
							}	
                        }
						
						if(!mp3->w_playing)
							break;
				// do echo procesing
				//
				// protect SetSfxParam(...) function from deallocating buffers
				// 
						EnterCriticalSection(&mp3->w_wcsSfx);
				 		if(mp3->w_echo) {
							int buf_size = size / 2;
						// split each ( 100 ms ) wave buffer int 2 ( 50 ms ) echo buffers
							DoEcho(mp3, buf, buf_size);
							DoEcho(mp3, buf + buf_size, buf_size);	
							
						}
						LeaveCriticalSection(&mp3->w_wcsSfx);
	
						if(!mp3->w_playing)
							break;

						// send wave buffer to soundcard
                    	mp3->w_wave->PlayBuffer(i);	
							
					}
					else {
						// FAIL to decode mp3 frames into wave
						// this must be end of steam, or fatal error,  stop playing
						BOOL done = TRUE;
                        for(UINT j = 0; j < mp3->w_wave->GetNumOfBuffers(); j++ )
                        	if( !mp3->w_wave->IsBufferDone(j)) done = FALSE;
                        if(done) {
                        	mp3->Stop();
							break;
                        }
					}					
				}
					
			}
		}

	}

	// unprepare all wave buffers
	for(UINT j = 0; j < mp3->w_wave->GetNumOfBuffers(); j++ )
   		mp3->w_wave->UnprepareHeader(j);

   // close wave output
   mp3->w_wave->Close();
// free buffers allocated by mp3 decoding engine
   ExitMP3(&mp3->w_mp);	// deinitialize decoder
	
	mp3->w_hThread = 0;
	// thread is done, Stop() function  can now easy return
	LeaveCriticalSection(&mp3->w_wcsThreadDone);

	return;	
}



int WMp3::GetPosition()
{
// return current playing position i seconds
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}
// seek position + wave position
	return ( w_position + w_wave->GetPosition(SECONDS));

}



int WMp3::_GetID3Version(WMp3* mp3)
{
// return ID3 version

	if( !mp3 || mp3->w_stream == NULL)
    	return 0;
// protect stream pointer between threads
	EnterCriticalSection(&mp3->w_wcs);
// get current pointer
	DWORD pointer = mp3->w_stream->GetStreamPointer();

	char buffer[3];
    BOOL found1, found2;
    DWORD dwRead;
    int ret;
    found1 = FALSE;
    found2 = FALSE;
// seek to beginning of stream
    if(!mp3->w_stream->Seek( mp3->w_dwSeek, STREAM_BEGIN)) {
		mp3->error("WMp3::_GetID3Version\n\nCan't seek to stream begin!");
		return 0;
	}
// read 3 bytes of data
    if(!mp3->w_stream->Read(buffer, 3, &dwRead)) {
		mp3->error("WMp3::_GetID3Version\n\nCan't read ID3!");
		return 0;
	}
// check if ID3v2 tag exist
    if(memcmp(buffer, "ID3",3) == 0)
		found2 = TRUE;

// seek to ID3v1 tag
    if(!mp3->w_stream->Seek(mp3->w_dwSeek + mp3->w_dwSize -128, STREAM_BEGIN)) {
		mp3->error("WMp3::_GetID3Version\n\nCan't seek to stream end!");
		LeaveCriticalSection(&mp3->w_wcs);
		return 0;
	}
// read 3 bytes
    if(!mp3->w_stream->Read(buffer, 3, &dwRead)) {
		mp3->error("WMp3::_GetID3Version\n\nCan't read TAG!");
		LeaveCriticalSection(&mp3->w_wcs);
		return 0;
	}
// check if exits ID3v1 tag
    if(memcmp(buffer, "TAG",3) == 0)
		found1 = TRUE;

    ret = 0;
    if(found1)
    	ret |= ID3_VERSION1;
    if(found2)
    	ret |= ID3_VERSION2;

	mp3->w_stream->Seek(pointer,STREAM_BEGIN);
	LeaveCriticalSection(&mp3->w_wcs);
// if ID3v2 exists return ID3_VERSION2
// if ID3v1 exists return ID3_VERSION1
// if both ID3 version exists return ID3_VERSION2 + ID3_VERSION1 
    return ret;
}



int WMp3::GetID3Version()
{
	
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	return _GetID3Version(this);
}




BOOL WMp3::LoadID3(int id3Version)
{
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	return _LoadID3(this, id3Version);
}


BOOL WMp3::_LoadID3(WMp3* mp3,int id3Version)
{
// load ID3 data into id3 structure

	DWORD dwRead;
	EnterCriticalSection(&mp3->w_wcs);
	DWORD pointer = mp3->w_stream->GetStreamPointer();
	switch(id3Version) {

    	case ID3_VERSION1:
        {
            if(mp3->w_myID3.album) {
            	free(mp3->w_myID3.album);
                mp3->w_myID3.album = 0;
            }
        	mp3->w_myID3.album = (char*) malloc(31);
			*mp3->w_myID3.album = 0;

             if(mp3->w_myID3.artist) {
             	free(mp3->w_myID3.artist);
                mp3->w_myID3.artist = 0;
             }

			mp3->w_myID3.artist = (char*) malloc(31);
			*mp3->w_myID3.artist = 0;

            if(mp3->w_myID3.comment) {
            	free(mp3->w_myID3.comment);
                mp3->w_myID3.comment = 0;
            }

			mp3->w_myID3.comment = (char*) malloc(31);
			*mp3->w_myID3.comment = 0;

             if(mp3->w_myID3.title) {
             	free(mp3->w_myID3.title);
                mp3->w_myID3.title = 0;
             }

			mp3->w_myID3.title = (char*) malloc(31);
			*mp3->w_myID3.title = 0;

            if(mp3->w_myID3.year) {
            	free(mp3->w_myID3.year);
                mp3->w_myID3.year = 0;
            }

			mp3->w_myID3.year = (char*) malloc(5);
			*mp3->w_myID3.year = 0;

            if(mp3->w_myID3.tracknum) {
            	free(mp3->w_myID3.tracknum);
                mp3->w_myID3.tracknum = 0;
            }

            mp3->w_myID3.tracknum = (char*) malloc(4);
			*mp3->w_myID3.tracknum = 0;

			if(mp3->w_myID3.genre) {
            	free(mp3->w_myID3.genre);
                mp3->w_myID3.genre = 0;
            }

            mp3->w_myID3.genre = (char*) malloc(1);
			*mp3->w_myID3.genre = 0;

			mp3->w_stream->Seek( mp3->w_dwSeek + mp3->w_dwSize -128, STREAM_BEGIN);
	
			mp3->w_stream->Read( mp3->w_myID3.header, 3, &dwRead);



            if( memcmp(mp3->w_myID3.header, "TAG", 3) != 0) {
	           	
                mp3->error("WMp3::LoadID3   *** There is no ID3v1 tag"); 
				mp3->w_stream->Seek(pointer, STREAM_BEGIN);
				LeaveCriticalSection(&mp3->w_wcs);
				return FALSE;
            }
 
			
			mp3->w_stream->Read(mp3->w_myID3.title,30, &dwRead);
			mp3->w_myID3.title[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.title);

			mp3->w_stream->Read(mp3->w_myID3.artist,30, &dwRead);
			mp3->w_myID3.artist[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.artist);

			mp3->w_stream->Read(mp3->w_myID3.album,30, &dwRead);
			mp3->w_myID3.album[30] = 0;
			RemoveEndingSpaces(mp3->w_myID3.album);

			mp3->w_stream->Read(mp3->w_myID3.year,4, &dwRead);
			mp3->w_myID3.year[4] = 0;
			RemoveEndingSpaces(mp3->w_myID3.year);

			mp3->w_stream->Read(mp3->w_myID3.comment,30, &dwRead);
			mp3->w_myID3.comment[30] = 0;

			mp3->w_stream->Read(mp3->w_myID3.genre, 1, &dwRead);

            if( (unsigned char) mp3->w_myID3.genre[0] > 146) {
            	*mp3->w_myID3.genre = 0;
			}
            else {
				unsigned char tmp = mp3->w_myID3.genre[0]; 
				free(mp3->w_myID3.genre);
				if(tmp > 147) tmp = 0;
				mp3->w_myID3.genre = (char*) malloc(strlen(sGenres[(BYTE) tmp]) + 1);
				strcpy(mp3->w_myID3.genre,sGenres[(BYTE) tmp]); 
			}

 

            BYTE tracknum = 0;
            if( mp3->w_myID3.comment[28] == 0 )
                tracknum = mp3->w_myID3.comment[29];
          	sprintf( mp3->w_myID3.tracknum, "%u", tracknum);
	
			mp3->w_stream->Seek(pointer, STREAM_BEGIN);
			LeaveCriticalSection(&mp3->w_wcs);
			
        }
        return TRUE;


        case ID3_VERSION2:
        {
        	char	tmpHeader[10];
    		tmpHeader[0] = 0;
			DWORD dwRead;

			mp3->w_stream->Seek( mp3->w_dwSeek, STREAM_BEGIN);
			mp3->w_stream->Read( mp3->w_myID3.Id3v2.Header, 10, &dwRead);


			// check if correct header
			if(memcmp( mp3->w_myID3.Id3v2.Header, "ID3", 3) != 0) {
            	mp3->error("WMp3::LoadID3   *** There is no ID3v2 tag");
				mp3->w_stream->Seek(pointer, STREAM_BEGIN);
				LeaveCriticalSection(&mp3->w_wcs);
				return FALSE;
            }


            // read flag field
            mp3->w_myID3.Id3v2._flags			= ( mp3->w_myID3.Id3v2.Header[5] & 255);
            mp3->w_myID3.Id3v2.Flags.Unsync = ExtractBits ( mp3->w_myID3.Id3v2._flags , 7, 1);
            mp3->w_myID3.Id3v2.Flags.Extended = ExtractBits ( mp3->w_myID3.Id3v2._flags , 6, 1);
            mp3->w_myID3.Id3v2.Flags.Experimental = ExtractBits ( mp3->w_myID3.Id3v2._flags , 5, 1);
            mp3->w_myID3.Id3v2.Flags.Footer = ExtractBits ( mp3->w_myID3.Id3v2._flags , 4, 1);

            // read size of tag
			mp3->w_myID3.Id3v2.Size			= GetFourByteSyncSafe( mp3->w_myID3.Id3v2.Header[6], mp3->w_myID3.Id3v2.Header[7], mp3->w_myID3.Id3v2.Header[8], mp3->w_myID3.Id3v2.Header[9]);


            if(mp3->w_myID3.album) free(mp3->w_myID3.album);
            mp3->w_myID3.album		= _RetrField(mp3, "TALB", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.album, dwRead);

            if(mp3->w_myID3.artist) free( mp3->w_myID3.artist);
			mp3->w_myID3.artist		= _RetrField(mp3, "TPE1", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.artist, dwRead);


            if(mp3->w_myID3.comment) free(mp3->w_myID3.comment);
		   	mp3->w_myID3.comment	= _RetrField(mp3, "COMM", &dwRead);
            DWORD newsize = _RemoveLeadingNULL( mp3->w_myID3.comment, dwRead);

			if(newsize < 2) {
				free(mp3->w_myID3.comment);
				mp3->w_myID3.comment = (char*) malloc(2);
				strcpy(mp3->w_myID3.comment,"");
			}
			else {
				char* tmp = ( char* ) memchr(mp3->w_myID3.comment , '\0', newsize - 1);
				if(tmp) {
            		strcpy(mp3->w_myID3.comment, tmp +1);
				}
			}


            if(mp3->w_myID3.genre) free(mp3->w_myID3.genre);
		   	mp3->w_myID3.genre		= _RetrField(mp3, "TCON", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.genre, dwRead);


            if(mp3->w_myID3.title) free(mp3->w_myID3.title);
		   	mp3->w_myID3.title		= _RetrField(mp3, "TIT2", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.title, dwRead);

            if(mp3->w_myID3.tracknum) free( mp3->w_myID3.tracknum);
		   	mp3->w_myID3.tracknum	= _RetrField(mp3, "TRCK", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.tracknum, dwRead);

            if(mp3->w_myID3.year) free(mp3->w_myID3.year);
		   	mp3->w_myID3.year		= _RetrField(mp3, "TYER", &dwRead);
            _RemoveLeadingNULL( mp3->w_myID3.year, dwRead);

			mp3->w_stream->Seek(pointer, STREAM_BEGIN);
			LeaveCriticalSection(&mp3->w_wcs);
        }
        return TRUE;


    }

	mp3->w_stream->Seek(pointer, STREAM_BEGIN);
	LeaveCriticalSection(&mp3->w_wcs);
    return FALSE;
}




char* WMp3::_RetrField(WMp3* mp3, char *identifier, DWORD* Size)
{
//	return ID3v2 field
// INPUT:   char *identifier		- field identifier
//
// OUTPUT:  DWORD& Size				- size of returned field
// RETURN char*						- pointer to allocated memory with content of fiels
//
// REMARK:	if fiels isn't found function returns 0, and DWORD* Size is 0
// user needs to free memory returned from this function

    int pointer = mp3->w_stream->GetStreamPointer();
	
	char tmpHeader[10];
 
    DWORD framesize;
    unsigned char *field;


	DWORD dwRead;

	DWORD dwSize = mp3->w_myID3.Id3v2.Size;
   

	mp3->w_stream->Seek(mp3->w_dwSeek + 10, STREAM_BEGIN);

    // skip Extended header
	if( mp3->w_myID3.Id3v2.Flags.Extended ) {
		mp3->w_stream->Read( tmpHeader, 4, &dwRead);
        DWORD extHdrSize = GetFourByteSyncSafe( tmpHeader[0], tmpHeader[1], tmpHeader[2], tmpHeader[3]);
		mp3->w_stream->Seek(extHdrSize, STREAM_CURRENT);
	    dwSize -= extHdrSize;

    }


   	*Size = 0;
    BOOL ret;

    while(1) {
    	// read frame header
	
		mp3->w_stream->Read(tmpHeader, 10, &dwRead);
	
        if(tmpHeader[0] == 0 || dwSize <= 0) {
			mp3->w_stream->Seek(pointer,STREAM_BEGIN);
        	return 0;
        }
        // check frame ID
        ret = _DecodeFrameHeader(mp3, tmpHeader, identifier);
        if(  ret ) {

        	field = (unsigned char*) malloc(mp3->w_ID3frame.Size + 1);

            field[mp3->w_ID3frame.Size] = 0;

		

			mp3->w_stream->Read((char*) field, mp3->w_ID3frame.Size, &dwRead);

            if( mp3->w_ID3frame.Flags.Unsynchronisation )
            	*Size = _DecodeUnsync(field, mp3->w_ID3frame.Size);
            else
            	*Size = mp3->w_ID3frame.Size;

            mp3->w_stream->Seek(pointer,STREAM_BEGIN);
			return  (char *) field;
        }
        else {
        	// get frame size
			if( mp3->w_ID3frame.Flags.Unsynchronisation )
				framesize = GetFourByteSyncSafe(tmpHeader[4], tmpHeader[5], tmpHeader[6], tmpHeader[7]);
			
            else
            	framesize = CreateFourByteField(tmpHeader[4], tmpHeader[5], tmpHeader[6], tmpHeader[7]);
			
        //	framesize = GetFourByteSyncSafe(tmpHeader[4], tmpHeader[5], tmpHeader[6], tmpHeader[7]);
			mp3->w_stream->Seek(framesize, STREAM_CURRENT );
            dwSize -= ( framesize + 10 );
        }
    }

	//mp3->w_stream->Seek(pointer,STREAM_BEGIN);

    
}


DWORD WMp3::_RemoveLeadingNULL( char *str , DWORD size)
{
// remove leading NULL from array - shift not NULL substr to left
// INPUT: 	char *str		- input stream
//          DWORD size		- size of input stream
// RETURN: new size of input stream;
	if(!str)
    	return 0;
	for ( DWORD i = 0; i < size; i++ ) {
    	if( str[i] > 0 ) {
        	for( DWORD j = 0; j < size - i; j++ ) {
            	str[j] = str[i+j];
            }
            for( DWORD k = size - i; k < size; k++)
            	str[k] = 0;

            return( size - i );
        }
    }

    return 0;
}



BOOL WMp3::_FrameIsNull(char *frame, int chars)
{
	BOOL	isnull;

	for(int x = 0; x < chars; x++)
		if(frame[x] != 0)
			isnull = FALSE;
	return isnull;
}


BOOL WMp3::_DecodeFrameHeader(WMp3* mp3, char *header, char *identifier)
{
// decode frame header ( check FRAME ID, decode FLAG bits, get frame size)
// frame header:
// FRAME ID:	 $xx xx xx xx   ( 4 byte )
// FRAME SIZE:   4 * %0xxxxxxx ( 4 byte )
// FRAME FLAGS:	 $xx xx			( 2 bytes )
//
	mp3->w_ID3frame.Flags.Tag_alter_preservation = ExtractBits(header[8], 6,1);
    mp3->w_ID3frame.Flags.File_alter_preservation = ExtractBits(header[8], 5,1);
    mp3->w_ID3frame.Flags.Read_only = ExtractBits(header[8], 4,1);
    mp3->w_ID3frame.Flags.Grouping_identity = ExtractBits(header[9], 6,1);
    mp3->w_ID3frame.Flags.Compression = ExtractBits(header[9], 3,1);
    mp3->w_ID3frame.Flags.Encryption = ExtractBits(header[9], 2,1);
    mp3->w_ID3frame.Flags.Unsynchronisation = ExtractBits(header[9], 1,1);
    mp3->w_ID3frame.Flags.Data_length_indicator = ExtractBits(header[9], 0,1);

	DWORD c = 0x1;
	DWORD d = c <<28; 
	char* a = header + 4;
    mp3->w_ID3frame.Size = GetFourByteSyncSafe(header[4], header[5], header[6], header[7]);

	DWORD b = CreateFourByteField(header[4], header[5], header[6], header[7]);  
    if( strncmp(header, identifier, 4) != 0 )
    	return FALSE;
    if(ExtractBits(header[9], 7,1) || ExtractBits(header[9], 5,1) || ExtractBits(header[9], 4,1) )
    		return FALSE;

    return TRUE;


}

DWORD WMp3::_DecodeUnsync(unsigned char* header, DWORD size)
{
    DWORD count = 0;
	for( UINT i = 0; i < size - 2; i++ ) {
    	if(header[i] ==  0xFF && header[i+1] == 0 ) {
        	header[i+1] = header[i+2];
            count++;
        }
    }
    return count;
}






BOOL WMp3::_HeadCheck(unsigned long head)
{
// check if correct mp3 frame header
// return FALSE if this can't be valid mp3 frame header
// if return TRUE, this can be valid mp3 frame header, but more
// checking is necessary

	if( (head & 0xffe00000) != 0xffe00000) // sync
    	return FALSE;
    if( (( head >> 19 ) & 0x3 ) == 0x1) // reserved MPEG version ID
    	return FALSE;
    if(!((head>>17)&3))	// reserved Layer ID
    	return FALSE;
	if( ((head>>12)&0xf) == 0xf)	// invalid bitrateindex
    	return FALSE;
    if( ((head>>10)&0x3) == 0x3 )	// reserved sampling frequency
    	return FALSE;

	 if((head & 0x3) == 0x2 )	// reserved emphasis
    	return FALSE;


    return TRUE;
}


char* WMp3::GetMpegVersion()
{
	return w_mp3stream.mpeg_version_str;
}

char* WMp3::GetMpegLayer()
{
	return w_mp3stream.layer_str;
}

char* WMp3::GetChannelMode()
{
	return w_mp3stream.channel_str;
}

char* WMp3::GetEmphasis()
{
	return w_mp3stream.emphasis_str;
}

LONG WMp3::GetSamplingRate()
{
	return w_mp3stream.sampling_rate;

}

LONG WMp3::GetHeaderStart()
{
	return w_mp3stream.dwFirstFrame;

}





unsigned long Get32bits(unsigned char *buf) {

	unsigned long ret;
    ret = (((unsigned long) buf[0]) << 24) |
	(((unsigned long) buf[1]) << 16) |
	(((unsigned long) buf[2]) << 8) |
	((unsigned long) buf[3]) ;

    return ret;
}



BOOL WMp3::Pause()
{
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	w_wave->Pause();
    w_pause = TRUE;
    return TRUE;
}

BOOL WMp3::Resume()
{
	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	w_wave->Resume();
    w_pause = FALSE;
    return TRUE;
}


BOOL WMp3::Seek(LONG seconds)
{
// INPUT:	LONG seconds		- number of seconds from begining

	if(!w_stream) {
		error("WMp3::Play\n\nStrean isn't open!");
		return 0;
	}

	if(w_playing) {
		Stop();
	}

     if(seconds  > w_mp3stream.length)
     	seconds = w_mp3stream.length;

     if(seconds < 0)
     	seconds = 0;

	int pos = 0;

	if(w_mp3stream.vbr) {	// variable bitrate
		if(w_mp3stream.xing && (w_xingheader.flags & TOC_FLAG)) { // xing header found
			// calculate seek position from XING header seeking table
			pos = w_mp3stream.dwFirstFrame +  _SeekPoint(seconds);			
		
		}
		else {
			// no toc or no xing header
			int frame_num;
			// calculate seek position from precalculated average frame size
			// 
     		frame_num =   seconds *  w_mp3stream.sampling_rate / w_mp3stream.sample_per_frame;
        	pos = w_mp3stream.dwFirstFrame + frame_num * (int) w_mp3stream.framesize;				

		}
	}
	else { // constant bitrate
     	int frame_num;
		// calculate seek position
     	frame_num =   seconds *  w_mp3stream.sampling_rate / w_mp3stream.sample_per_frame;
        pos = w_mp3stream.dwFirstFrame +  ( frame_num *  w_mp3stream.framesize);
	} 

		 w_position = seconds;	
// seek stream to correct position
	EnterCriticalSection(&w_wcs);
	w_stream->Seek(pos,STREAM_BEGIN);
	LeaveCriticalSection(&w_wcs);
	
	return TRUE;
}





int WMp3::_SeekPoint(int seconds)
{
// get seek point from XING header

	int a, seekpoint;
   // interpolate in TOC to get file seek point in bytes

	float fa, fb, fx, percent;

    percent = (float) seconds * 100 / w_mp3stream.length;


	if( percent < 0.0f )
    	percent = 0.0f;
	if( percent > 100.0f )
    	percent = 100.0f;

	a = (int)	percent;
	if( a > 99 )
    	a = 99;
	fa = w_xingheader.toc[a];
	if( a < 99 )
    	fb = w_xingheader.toc[a+1];
	else
    	fb = 256.0f;

	fx = fa + ( fb - fa ) * ( percent - a );

	seekpoint = (int)(( 1.0f / 256.0f ) * fx * w_mp3stream.dwStreamSize ); 

    return seekpoint;
}



LONG WMp3::GetCurrentBitrate()
{
	if(!w_stream)
		return 0;

	return tabsel_123[w_mp3header.lsf][w_mp3header.lay-1][w_mp3header.bitrate_index];
}

LONG WMp3::GetStatus()
{
	if(w_pause)
    	return STATUS_PAUSE;
    if(w_playing)
    	return STATUS_PLAY;

    return STATUS_STOP;

}

BOOL WMp3::Play(LONG sec)
{
	if(! Seek(sec))
    	return FALSE;

    return Play();

}




LONG WMp3::GetSongLength()
{ 
	return w_mp3stream.length; 
}


LONG WMp3::GetBitrate()
{ 
	return w_mp3stream.bitrate; 
	
}

char* WMp3::GetArtist()		{ return w_myID3.artist;}
char* WMp3::GetTrackTitle()	{ return w_myID3.title;}
char* WMp3::GetTrackNum()		{ return w_myID3.tracknum;}
char* WMp3::GetAlbum()		{ return w_myID3.album;}
char* WMp3::GetYear()			{ return w_myID3.year;}
char* WMp3::GetComment()		{ return w_myID3.comment;}



char* WMp3::GetGenre()
{ 
	if(w_myID3.genre)
		return w_myID3.genre;
	else
		return (char*) empty_string;
};








BOOL WMp3::FindNextValidFrame(WMp3* mp3)
{
// search for first valid mp3 frame, seek stream to first valid frame
// search is starting from current stream position
// INPUT:	WMp3* mp3		- mp3 class
//							- _m3stream.sampling_frequency,_m3stream.lay, _m3stream.stereo
//							  need to be -1 or current valid data of previous header
//	
// OUTPUT:	WMp3* mp3		- w_stream position set to first valid frame
//			WMp3* mp3		- w_mp3header filled vith data of first valid frame
//
// RETURN:	TRUE - if found valid mp3 frame
//			FALSE - if fail

	if(!mp3)
    	return FALSE;

	if(mp3->w_stream->GetStreamPointer() + MIN_FRAME_SIZE > mp3->w_stream->GetStreamSize()) {
		// end of file, no more valid frames
		return FALSE;
	} 


	if(!mp3->w_search_buffer) {
		// internal error
		return FALSE;
	}
	
	DWORD dwRead = 0;
	DWORD dwRead1 = 0;
	DWORD count = 0;

	unsigned char* data = (unsigned char*) mp3->w_search_buffer;

	DWORD  pointer = mp3->w_stream->GetStreamPointer();

	unsigned char *head;

	BOOL ret = FALSE;
	int lay = -1;
	int stereo = -1;
	int	 sampling_frequency = -1;
	int newheader = 0;

	unsigned char buff[4];
				

	// read data into search buffer
	while(mp3->w_stream->Read((char*) data, SEARCH_BUFFER_SIZE, &dwRead) && dwRead >= MIN_FRAME_SIZE) {
		// read successful, enough data for one minimal frame
		// search for sync ( 11111111111 )
		unsigned int i;
		for(i = 0; i < dwRead - 3; i++) {
				if(data[i] != 0xFF || (data[i+1] & 0xE0) == 0)
					continue;
			// found sync, now decode header and check if valid header
				head = data + i;
				ret = _DecodeMp3FrameHeader(head,&mp3->w_mp3header);
				if(!ret)
					continue;
				// first check
				if(mp3->w_mp3header.framesize < MIN_FRAME_SIZE)
					continue;
				if(mp3->w_mp3stream.lay != -1 && mp3->w_mp3stream.lay != mp3->w_mp3header.lay)
					continue;
				if(mp3->w_mp3stream.sampling_frequency != -1 && mp3->w_mp3stream.sampling_frequency != mp3->w_mp3header.sampling_frequency)
					continue;
				if(mp3->w_mp3stream.stereo != -1 && mp3->w_mp3stream.stereo != mp3->w_mp3header.stereo)
					continue;
				// still valid header
				// check more, now we have frame size, next frame must be also valid frame
				int lay = mp3->w_mp3header.lay;
				int stereo = mp3->w_mp3header.stereo;
				int	 sampling_frequency = mp3->w_mp3header.sampling_frequency;
				// check if ve have enough data in our buffer
				if(mp3->w_mp3header.framesize + i < dwRead - 3) {
					head += mp3->w_mp3header.framesize;

				}
				else {
					// need to read new buffer
					if(!mp3->w_stream->Seek(i + mp3->w_mp3header.framesize, STREAM_CURRENT))
						return FALSE;
					if(!mp3->w_stream->Read((char*) buff, 4, &dwRead1) || dwRead1 < 4)
						return FALSE;

					head = buff;
					
				}

				ret = _DecodeMp3FrameHeader(head,&mp3->w_mp3header);
				if(!ret)
					continue;

				if(lay != mp3->w_mp3header.lay)
					continue;
				if(sampling_frequency != mp3->w_mp3header.sampling_frequency)
					continue;
				if(stereo != mp3->w_mp3header.stereo)
					continue;

				pointer += i;
				if(!mp3->w_stream->Seek(pointer, STREAM_BEGIN))
					return FALSE;
					
				return TRUE;	
		
		}
		pointer = pointer + i + 1;
		if(!mp3->w_stream->Seek(pointer, STREAM_BEGIN))
			return FALSE;

	}
	return FALSE;
}





BOOL WMp3::_DecodeMp3FrameHeader(unsigned char *frame,MP3FRAMEHEADER* header)
{
// decode valid mp3 frame header to MP3FRAMEHEADER* header
// INPUT:	unsigned char* frame	- pointer to possible mp3 header
//			MP3FRAMEHEADER* header  - pointer to structore for result
// OUTPUT:	MP3FRAMEHEADER* header  - filled with info about mp3 header
//
// RETURN:	TRUE	- frame is valid mp3 frame and MP3FRAMEHEADER* header is filled
//			FALSE	- frame isn't valid mp3 frame



	unsigned long newhead;
    newhead= (unsigned long) ( ( ( frame[0] & 0xFF ) << 24 ) |
                								( ( frame[1] & 0xFF ) << 16 ) |
                                                ( ( frame[2] & 0xFF ) << 8 ) |
                                                ( frame[3] & 0xFF )
                							  );
// check if valid header
	if( ! _HeadCheck(newhead))
    	return FALSE;



    // MPEG audio version
    if( newhead & (1<<20) ) {
		header->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
		header->mpeg25 = 0;
    }
    else {
    	if ( newhead & (1<<19))
        	return FALSE;
		header->lsf = 1;
		header->mpeg25 = 1;
    }

    // MPEG Layer version
    header->lay = 4 - ((newhead>>17)&3);

     // crc error protection
    header->error_protection = ((newhead>>16)&0x1)^0x1;
     // bitrate
    header->bitrate_index = ((newhead>>12)&0xF);
    // sampling frequency rate
    if(header->mpeg25) {
      header->sampling_frequency = 6 + ((newhead>>10)&0x3);
    }
    else
      header->sampling_frequency = ((newhead>>10)&0x3) + (header->lsf * 3);
		
   // padding
    header->padding   = ((newhead>>9)&0x1);
    // private bit
    header->extension = ((newhead>>8)&0x1);
    // channel mode
    header->mode      = ((newhead>>6)&0x3);
    //Mode extension (Only if Joint stereo)
    header->mode_ext  = ((newhead>>4)&0x3);
    // Copyright
    header->copyright = ((newhead>>3)&0x1);
    // Original
    header->original  = ((newhead>>2)&0x1);
    // Emphasis
    header->emphasis  = newhead & 0x3;
    // stereo
    header->stereo    = (header->mode == MPG_MD_MONO) ? 1 : 2;

    // calculate frame size
    switch(header->lay)
    {
	
      case 1:
		#ifdef LAYER1

        header->framesize  = (long) tabsel_123[header->lsf][0][header->bitrate_index] * 12000;
        header->framesize /= freqs[header->sampling_frequency];
        header->framesize  = ((header->framesize + header->padding)<<2);
		#else
			
			return FALSE;
		#endif
	
        break;
	
      case 2:
		#ifdef LAYER1
        header->framesize = (long) tabsel_123[header->lsf][1][header->bitrate_index] * 144000;
        header->framesize /= freqs[header->sampling_frequency];
        header->framesize += header->padding;
		#else
	
			return FALSE;
		#endif
      break;

      case 3:
	  {
          header->framesize  = (long) tabsel_123[header->lsf][2][header->bitrate_index] * 144000;
          header->framesize /= freqs[header->sampling_frequency]<<(header->lsf);
          header->framesize = header->framesize + header->padding ;
		
	  }
      break;

      default:
		// unknown layer
        return FALSE;
    }
   
    return TRUE;
}



BOOL WMp3::_DecodeXingHeader(WMp3* mp3)
{


	DWORD pointer = mp3->w_stream->GetStreamPointer();
	MP3FRAMEHEADER header;

   	unsigned char data[4];

	DWORD dwRead;
	if(!mp3->w_stream->Read((char*) data, 4, &dwRead) && dwRead != 4) {
		mp3->w_stream->Seek(pointer, STREAM_BEGIN);
   		return FALSE;
	}

	// check if valid mp3 frame
   	if( !_DecodeMp3FrameHeader((unsigned char*) data, &header)) {
		mp3->w_stream->Seek(pointer, STREAM_BEGIN);
    	return FALSE;
	}

    if(header.lay != 3) {
		mp3->w_stream->Seek(pointer, STREAM_BEGIN);
		return FALSE;
	}



	mp3->w_stream->Seek(pointer, STREAM_BEGIN);


    unsigned char *ptxingframe = (unsigned char*) malloc(header.framesize);
    unsigned char *xingframe = ptxingframe;

    if(! mp3->w_stream->Read((char*) xingframe, header.framesize, &dwRead) || ( dwRead < (DWORD) header.framesize)) {
    	free(ptxingframe);
		mp3->w_stream->Seek(pointer, STREAM_BEGIN);
   		return FALSE;
    }

	mp3->w_stream->Seek(pointer, STREAM_BEGIN);

    int ssize;
    if(header.lsf)
		ssize = (header.stereo == 1) ? 9 : 17;
    else
		ssize = (header.stereo == 1) ? 17 : 32;

    ssize += 4;

    

    xingframe += ssize;


    if(( xingframe[0] != 'X' ) || ( xingframe[1] != 'i' ) ||
       ( xingframe[2] != 'n' ) || ( xingframe[3] != 'g' ) ) {

		free(ptxingframe);
   		return FALSE;
    }



    xingframe += 4;
   	DWORD head_flags;
    

    head_flags = mp3->w_xingheader.flags = _Get32bits(xingframe);
    xingframe += 4;

    if( head_flags & FRAMES_FLAG ) {
    	mp3->w_xingheader.frames   = _Get32bits(xingframe);
        xingframe += 4;
    }
	if( head_flags & BYTES_FLAG )  {
    	mp3->w_xingheader.bytes = _Get32bits(xingframe);
        xingframe += 4;
    }

    if( head_flags & TOC_FLAG ) {
        for( int i = 0; i < 100; i++)
           mp3->w_xingheader.toc[i] = xingframe[i];

    	xingframe += 100;
    }

	mp3->w_xingheader.vbr_scale = -1;
	if( head_flags & VBR_SCALE_FLAG )
    	mp3->w_xingheader.vbr_scale = _Get32bits(xingframe);


    free(ptxingframe);
    return TRUE;

}










BOOL WMp3::OpenMp3Stream(char* memw_stream, DWORD size, int WaveBufferLength)
{
	if(w_stream) {
		error("WMp3::OpenMp3Stream\n\nYou need to close mp3 stream before open new one");
		return FALSE;
	}

	if(!memw_stream) {
		error("WMp3::OpenMp3Stream\n\nCan't open NULL stream!");
		return FALSE;
	}

	WStream* stream = new WStream();
	if(!stream->OpenMemory(memw_stream, size)) {
		error("WMp3::OpenMp3Stream\n\nCan't open memory stream");
		delete stream;
		stream = 0;
        return FALSE;

	}

	BOOL ret = _Open(this, stream, WaveBufferLength,0,0);
	if(!ret) {
		delete stream;
		stream = 0;
		return FALSE;
	}

	return TRUE;

   
}


BOOL WMp3::Close()
{
	Stop();

	// stop thread
	w_run_thread = FALSE;
	SetEvent(w_hEvent);

	EnterCriticalSection(&w_wcs);

	w_wave->Uninitialize();

    CloseHandle(w_hEvent);
    w_hEvent = 0;

	if(w_in_data)
		free(w_in_data);

	w_in_data = 0;

	if(w_stream)
		delete w_stream;

	w_stream = 0;

	LeaveCriticalSection(&w_wcs);

	ZeroMemory(&w_mp3stream, sizeof(MP3STREAM));
	ZeroMemory(&w_mp3header, sizeof(MP3FRAMEHEADER));

	w_mp3stream.lay = -1;

	if(w_search_buffer) {
		free(w_search_buffer);
		w_search_buffer = 0;
	}

	return TRUE;

}



BOOL WMp3::OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType, int WaveBufferLength)
{

	return OpenMp3Resource(hModule,lpName,lpType,WaveBufferLength, 0);
}


BOOL WMp3::OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType, int WaveBufferLength, DWORD seek)
{

	if(w_stream) {
		error("WMp3::OpenMp3Resource\n\nYou need to close mp3 stream before open new one");
		return FALSE;
	}

	WStream* stream = new WStream();
	if(!stream->OpenResource(hModule, lpName, lpType)) {
		error("WMp3::OpenMp3Resource\n\nCan't open resource");
		delete stream;
		stream = 0;
        return FALSE;

	}

	BOOL ret = _Open(this, stream, WaveBufferLength,seek,0);
	if(!ret) {
		delete stream;
		stream = 0;
		return FALSE;
	}
	return TRUE;
}




BOOL WMp3::SetVolume(int lvolume, int rvolume)
{

    if(lvolume < 0 || lvolume > 100)
    	return FALSE;

    if(rvolume < 0 || rvolume > 100)
    	return FALSE;

	if(w_internalVolume)
    {
		w_lvolume = lvolume;
		w_rvolume = rvolume;

    }
    else
    {
    	w_wave->SetVolume((int) (lvolume * 65535 / 100), (int) (rvolume * 65535 / 100));

    }

	return TRUE;
}



BOOL WMp3::GetVolume(WORD* lvolume, WORD* rvolume)
{

	if(w_internalVolume)
    {
     *lvolume = w_lvolume;
     *rvolume = w_rvolume;

    }
    else
    {
    	w_wave->GetVolume((WORD*) lvolume, (WORD*) rvolume);
		*lvolume = *lvolume * 100 / 65535;
		*rvolume = *rvolume * 100 / 65535;
    }

	return TRUE;
}





void CALLBACK WMp3::_WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
				DWORD dwParam1, DWORD dwParam2)
{

    WMp3* mp3 = (WMp3*) dwInstance;
	WAVEHDR* header = (WAVEHDR *) dwParam1;
 	switch(uMsg) {
    	case WOM_OPEN:
        break;

        case WOM_CLOSE:
        break;

        case WOM_DONE:
		{
			mp3->w_dwPos += header->dwBufferLength;

			if(header->dwUser == mp3->w_wave->GetNumOfBuffers() - 1)
				mp3->w_dwIndex = 0;
			else
				mp3->w_dwIndex = header->dwUser + 1;

			::SetEvent(mp3->w_hEvent);
			mp3->w_vudata_ready = TRUE;

		

		}
        break;



    }
}




void WMp3::error(char* err_message)
{
	size_t size = 0;
	if(err_message != 0) {
		size = strlen(err_message );
		size = (size > 	ERRORMESSAGE_SIZE) ? ( ERRORMESSAGE_SIZE - 1) : size;
		strncpy(_lpErrorMessage, err_message, size);
		_lpErrorMessage[size] = 0;
	}
	_lpErrorMessage[size]= 0;
	
}






unsigned long WMp3::_Get32bits(unsigned char *buf) {

	unsigned long ret;
    ret = (((unsigned long) buf[0]) << 24) |
	(((unsigned long) buf[1]) << 16) |
	(((unsigned long) buf[2]) << 8) |
	((unsigned long) buf[3]) ;

    return ret;
}



int WMp3::GetVUData(int* left, int* right)
{
// return data for VU meter	
	
	if(!w_stream || !w_playing || !w_vudata_ready) {
		*left = 0;
		*right = 0;
			
		return 0;
	}

	 


	short* left1;
	short* right1;
	double sum_left = 0;
	double avg_left;
	double sumww_right = 0;
	double avgww_right;
	double num = 0;
	double la;
	double ra;

	DWORD size = w_mp3stream.sampling_rate * 2 * w_mp3stream.channel / 50 ;
	

	DWORD pos = w_wave->GetPosition(BYTES);
	DWORD buff_size = w_wave->GetBufferSize(w_dwIndex);
	char* data = w_wave->GetBufferData(w_dwIndex);
	DWORD num_chunk = buff_size / size;
	


	DWORD offset;
	

	if(w_mp3stream.channel == 2) {
		if(num_chunk >= 2) {
		
			offset = pos - w_dwPos;
			if(offset > buff_size) {
				if(w_dwIndex == w_wave->GetNumOfBuffers() - 1)
					w_dwIndex = 0;
				else
					w_dwIndex++;

				offset = 0;
				data = w_wave->GetBufferData(w_dwIndex);
			}

			if(offset + size + 4 > buff_size) {
				offset = buff_size - size - 4;
				
				
			}
			
		}
		else {
			offset = 0;
		}

		
		for(DWORD i = 0; i < size; i += 4) {
			left1 = (short*) ( data + i + offset);
			sum_left += (double) abs(*left1);
			right1 = (short*) ( data + i + offset + 2);
			sumww_right += (double) abs(*right1);
			num++;

		}

		avg_left = sum_left / num;
		avgww_right = sumww_right / num;

		

		if(avg_left < 54.12)
			*left = 0;
		else {
			la = avg_left / AVG_SINE_16 ;
			*left = (int) (100 + (38 * log10(la)));
		}

	
		if(avgww_right < 54.12)
			*right = 0;
		else {
			ra = avgww_right / AVG_SINE_16 ;
			*right = (int) (100 +(38 * log10(ra)));
		}

		

	
		
		
	}
	else if(w_mp3stream.channel == 1) {
		if(num_chunk >= 2) {
			offset = pos - w_dwPos;
			if(offset > buff_size) {
				if(w_dwIndex == w_wave->GetNumOfBuffers() - 1)
					w_dwIndex = 0;
				else
					w_dwIndex++;

				offset = 0;
				data = w_wave->GetBufferData(w_dwIndex);
			}

			if(offset + size + 2 > buff_size) {
				offset = buff_size - size - 2;
				
				
				
			}
			
		}
		else {
			offset = 0;
		}


		for(DWORD i = 0; i < size; i += 2) {
			left1 = (short*) ( data + i + offset);
			sum_left += (float) abs(*left1);
			num++;
		}

		avg_left = sum_left / num;


		if(avg_left < 54.12) {
			*left = 0;
			*right = 0;
		}
		else {
			la = avg_left / AVG_SINE_16 ;
			*left = (int) (100 + (38 * log10(la)));
			*right = *left;
		}



	
		
	
	}


	return 1;
}





void RemoveEndingSpaces(char* buff)
{
	int size = strlen(buff);
	for(int i = size - 1; i >= 0; i--) {
		if(buff[i] == ' ')
			buff[i] = 0;
		else
			return;

	}

}



int WMp3::DoEcho(WMp3* mp3, char* buf, int len)
{


	int i;
	short* sample;



	if(mp3->w_shift == TRUE) { // shift buffers
	
		for(i = ECHO_BUFFER_NUMBER - 1; i > 0; i--) {
			mp3->sfx_buffer1[i] = mp3->sfx_buffer1[i - 1];
		
		} 
		mp3->sfx_buffer1[0] = mp3->sfx_buffer1[ECHO_BUFFER_NUMBER - 1];
		mp3->w_shift = FALSE;
	
	}


	if(mp3->w_shift == FALSE) {	 // lower volume to half value and copy input buffer into first buffer 

		for(i = 0; i < len; i += 2) {
			sample = (short*) (buf + i);
			*sample /= 2;

		}
	
		//memcpy(mp3->sfx_buffer1[0],buf,len);
		CopyMemory(mp3->sfx_buffer1[0],buf,len);
		
	}

	

// do echo efect

	real e;
	real input_sample;
	
	 
   for(i = 0; i < len; i += 2) {
		e = 0;
		sample = (short*) (buf + i);
	
		if(mp3->w_right == 0) {
			for(int j = 0; j < ECHO_MAX_DELAY_NUMBER ; j++) {
				 if(mp3->w_sfx_mode_l[j])
					e += ((real) *((short*)(mp3->sfx_buffer1[j + mp3->w_sfx_offset] + i))) * mp3->w_sfx_mode_l[j];
			}
			mp3->w_right = 1;
		}
		else {
			for(int j = 0; j < ECHO_MAX_DELAY_NUMBER ; j++) {
				 if(mp3->w_sfx_mode_r[j])
					e += ((real) *((short*)(mp3->sfx_buffer1[j + mp3->w_sfx_offset] + i))) * mp3->w_sfx_mode_r[j];
			}

			mp3->w_right = 0;
		}


		// adjust inut sample gain
		input_sample = (real) *sample;
		input_sample = input_sample * mp3->w_input_gain;
		// adjust echo gain
		e = e * mp3->w_echo_gain;
		// adjust output gain and write data to output buffer

		*sample = (short) (input_sample + e) * mp3->w_output_gain;

   
   }

 

	
	mp3->w_shift = TRUE;

	return len;
}



int WMp3::InitSfx()
{

	if(w_echo) {
		// clear all buffers to 0
		if(sfx_buffer[0]) {
			for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
			//	memset(sfx_buffer[i],0, w_echo_alloc_buffer_size);
				ZeroMemory(sfx_buffer[i],w_echo_alloc_buffer_size);
		
		}
		w_shift = FALSE;
		w_right = 0;	
	}

	return 1;
}	



int WMp3::SetSfxParam(BOOL fEnable, int iMode, int uDelayMs, int InputGain, int EchoGain, int OutputGain)
{
	EnterCriticalSection(&w_wcsSfx);


	if(uDelayMs < 0)
		uDelayMs = 0;
		
	if(uDelayMs > MAX_DELAY)
		uDelayMs = MAX_DELAY;


		
	if(iMode > NUM_SFX_MODE - 1)
		iMode = NUM_SFX_MODE - 1;	

		int mode = iMode * 2;
	
	int i;
	w_echo = FALSE;
	w_shift = FALSE;


	w_echo_mode = iMode;

	if(fEnable) { // need echo
		// if buffer size changed allocate new buffer
		if(w_old_echo_buffer_size != w_echo_alloc_buffer_size) {
		// if old buffers allocated
			if(sfx_buffer[0]) {
				// free old buffers
				for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
					if(sfx_buffer[i])
						free(sfx_buffer[i]);

					sfx_buffer[i] = 0;
			}
				// all buffers free

			// allocate new buffers
			for(i = 0; i < ECHO_BUFFER_NUMBER; i++) {
				sfx_buffer[i] = (char*) malloc(w_echo_alloc_buffer_size);
				sfx_buffer1[i] = sfx_buffer[i];
				// check allocation
				if(sfx_buffer[i] == NULL) {
				// allocation error, free allocated buffers
					for(int j = 0; j <= i; j++) {
						free(sfx_buffer[j]);
						sfx_buffer[j] = 0;
						sfx_buffer1[j] = 0;
					}
					sfx_buffer1[0] = 0;
					LeaveCriticalSection(&w_wcsSfx);
					return 0;		
				}
			}

			w_old_echo_buffer_size = w_echo_alloc_buffer_size;

			// all buffers allocated
		} // no need for new buffers, old buffers are OK
		

		if(InputGain < -50 ) InputGain = -50;
		if(InputGain > 50 ) InputGain = 50;
		if(EchoGain < -50 ) EchoGain = -50;
		if(EchoGain > 50 ) EchoGain = 50;
		if(OutputGain < -50 ) OutputGain = -50;
		if(OutputGain> 50 ) OutputGain = 50;

		w_echo = fEnable;
		w_input_gain = (real) (InputGain + 50) / 50;
		w_echo_gain = (real) (EchoGain + 50) / 50;
		w_output_gain = (real) (OutputGain + 50) / 50;
		w_delay = uDelayMs;
		
		
		w_sfx_offset = ECHO_MAX_DELAY_NUMBER * w_delay / MAX_DELAY;
	
		for(i = 0; i < ECHO_MAX_DELAY_NUMBER ; i++) {
			w_sfx_mode_l[i] = sfx_mode[mode][i];
			if(w_mp3stream.channel > 1)
				w_sfx_mode_r[i] = sfx_mode[mode + 1][i];
			else
				w_sfx_mode_r[i] = sfx_mode[mode][i];	
		}
		
		InitSfx();		
	
	}
	else { // disable echo
		w_echo = fEnable;
		w_old_echo_buffer_size = 0;
		// free buffers

		if(sfx_buffer[0])
			for(int i = 0; i < ECHO_BUFFER_NUMBER; i++) {
				free(sfx_buffer[i]);
				sfx_buffer[i] = 0;
				sfx_buffer1[i] = 0;
	}		}

	w_shift = FALSE;

	LeaveCriticalSection(&w_wcsSfx);

	return 1;

}



int WMp3::EnableEQ(BOOL fEnable)
{
	w_mp.have_eq_settings = fEnable;

	return 1;
}



int WMp3::SetEQ(int iPreAmpValue, int iBand, int iValue)
{
// iPreAmpValue: -5 0 +5  
// iValue:		-10 0 +10 
	
	real val;
	w_preamp = iPreAmpValue;
	int i;

	if(iBand < 1 || iBand > 10) { // just adjust preamp
		if(iPreAmpValue < -5 || iPreAmpValue > 5) return 0;
		iPreAmpValue += 5;
		for(i = 0; i < 32; i++) {
			val = gtable2[iPreAmpValue] * gaintable[w_eq[i]];
			w_mp.equalizer[0][i] = val;
			w_mp.equalizer[1][i] = val;
		}
		return 1;
	}

	if(iPreAmpValue < -5 || iPreAmpValue > 5) return 0;
	if(iValue < -10 || iValue > 10) return 0;

	iValue += 10;
	iPreAmpValue += 5;
	

	switch(iBand) {
		case 1:
			w_eq[0] = iValue;	
		break;

		case 2:
			w_eq[1] = iValue;	
		break;

		case 3:
			w_eq[2] = iValue;	
		break;

		case 4:
			w_eq[3] = iValue;
		break;

		case 5:
			w_eq[4] = iValue;	
		break;

		case 6:
			w_eq[5] = iValue;
			w_eq[6] = iValue;
			w_eq[7] = iValue;	
		break;

		case 7:
			w_eq[8] = iValue;
			w_eq[9] = iValue;
			w_eq[10] = iValue;
			w_eq[11] = iValue;
			w_eq[12] = iValue;
			w_eq[13] = iValue;	
		break;

		case 8:
			w_eq[14] = iValue;
			w_eq[15] = iValue;
			w_eq[16] = iValue;
			w_eq[17] = iValue;
			w_eq[18] = iValue;
		
		break;

		case 9:
			w_eq[19] = iValue;
			w_eq[20] = iValue;
			w_eq[21] = iValue;
			w_eq[22] = iValue;
			w_eq[23] = iValue;	
		break;

		case 10:
			w_eq[24] = iValue;
			w_eq[25] = iValue;
			w_eq[26] = iValue;
			w_eq[27] = iValue;
			w_eq[28] = iValue;
			w_eq[29] = iValue;
			w_eq[30] = iValue;
			w_eq[31] = iValue;
		break;
	}
	
	for(i = 0; i < 32; i++) {
		val = gtable2[iPreAmpValue] * gaintable[w_eq[i]];
		w_mp.equalizer[0][i] = val;
		w_mp.equalizer[1][i] = val;

	}

	
	return 1;
}



int WMp3::VocalCut(BOOL fEnable)
{
	w_bVocalCut = fEnable;
	return 1;
}



int WMp3::InternalVolume(BOOL fEnable)
{
	w_internalVolume = fEnable;
	return 1;
}