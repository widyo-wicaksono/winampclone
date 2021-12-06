#ifndef _WWAVE_Z_
#define _WWAVE_Z_


#include <mmsystem.h>




#define SAMPLES 0
#define MILLISECONDS 1
#define SECONDS 2
#define BYTES 3


#define MAX_ERROR_STR 1024

class WWaveOut {
	public:
    	WWaveOut(); // constructor
    	~WWaveOut();	// destructor

    // initialize data for output ( structure and  alocate buffers
    BOOL Initialize(WORD FormatTag, WORD Channels, DWORD SamplesPerSec,
    					WORD BitsPerSample, UINT NumOfBuffers, UINT BufferSize);
    // free allocated buffers
    BOOL Uninitialize();
    // open wave output
    BOOL OpenAudioOutput(UINT uDeviceID, DWORD dwCallback , DWORD dwCallbackInstance,
   						DWORD fdwOpen);
    // close wave output
    BOOL Close();
    // get info about all output devices
    BOOL GetInfo();
    // play buffer
    BOOL PlayBuffer(int buffer);
    // unprepare header
    BOOL UnprepareHeader(int buffer);
    // stop playing
    BOOL Stop();
    // pause playing
    BOOL Pause();
    // resume playing
    BOOL Resume();
    // get position
    DWORD GetPosition(UINT wType = MILLISECONDS);

    DWORD SetVolume(WORD lvolume, WORD rvolume);
	DWORD GetVolume(WORD* lvolume, WORD* rvolume);

   // DWORD GetNumberOfBuffers() { return _number_of_buffers; };

    //void SetHeaderData(unsigned char* data
    LPSTR GetBufferData(DWORD index);
    void SetBufferSize(DWORD index, DWORD size);
    BOOL IsBufferDone(DWORD index);
    DWORD GetNumOfBuffers();
    void SetBufferDone(DWORD index);

	DWORD GetBufferSize(DWORD index);
	DWORD GetBufferAllocSize() { return _alloc_buffer_size;}


    // play stream
    BOOL PlayStream(char *stream, int size, int buffer);
    BOOL OpenFile(char *filename);

    // callback function
    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
    				DWORD dwParam1, DWORD dwParam2);

    private:
    	WAVEHDR* _waveHdr;
        WAVEFORMATEX _wfx;
    	DWORD _number_of_buffers; // number of allocated output buffers
        DWORD _buffer_size;	// size of single output buffer
        DWORD _samples_per_sec; // samples per second
		DWORD channel;
        HWAVEOUT _whandle; // handle of open device
        WAVEOUTCAPS* _woc;
        UINT _num_devs; // number of waveform-audio output devices present in the system
		DWORD _alloc_buffer_size;

	



		void Error(char* errormessage);
		char error_str[MAX_ERROR_STR];

	

};



#endif
