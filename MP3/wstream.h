// WControl class
//
// Author: Zoran Cindori
// Email: zcindori@inet.hr
// Date: 06/06/2004


#ifndef _WSTRREAM_
#define _WSTRREAM_


#define STREAM_BEGIN FILE_BEGIN
#define STREAM_CURRENT FILE_CURRENT
#define STREAM_END FILE_END

#define STREAM_FILE 1
#define STREAM_MEMORY 2


#define ACCESS_READ 			GENERIC_READ
#define ACCESS_WRITE 			GENERIC_WRITE

#define NO_SHARE		0
#define SHARE_READ 		FILE_SHARE_READ
#define SHARE_WRITE		FILE_SHARE_WRITE



class WStream {
public:
	WStream();
	~WStream();

// open stream from disk file
// INPUT:	const char* lpFileName			- file name
//			DWORD dwDesiredAccess			- 0, GENERIC_READ, GENERIC_WRITE	
//			DWORD dwShareMode				- 0, FILE_SHARE_READ, FILE_SHARE_WRITE
//			DWORD dwCreationDistribution	- CREATE_NEW, CREATE_ALWAYS, OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING
//			DWORD dwFlagsAndAttributes		- 0, FILE_FLAG_RANDOM_ACCESS
//	
// RETURN:	1 - all OK, file open
//			0 - error, file not open
	
int OpenFile(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes);




// open resource stream
// INPUT:	HMODULE hModule					- resource-module handle 
//			LPCTSTR lpName					- address of resource name  	
//			LPCTSTR lpType					- address of resource type,RT_RCDATA
//
// RETURN:	1 - all OK, file open
//			0 - error, file not open
	
int OpenResource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType);




// open memory stream
// INPUT:	HMODULE hModule					- resource-module handle 
//			LPCTSTR lpName					- address of resource name  	
//			LPCTSTR lpType					- address of resource type,RT_RCDATA
//
// RETURN:	1 - all OK, file open
//			0 - error, file not open
	
int OpenMemory(char* lpMemory, DWORD dwSize);






// seek stream pointer
// INPUT:	LONG lDistanceToMove	- number of bytes to move stream pointer 
// 			DWORD  dwMoveMethod		- STREAM_BEGIN, STREAM_CURRENT, STREAM_END
//
// RETURN:	1 - all OK, seek successfull, stream pointer moved
//			0 - error, stream pointer not moved

int Seek(LONG lDistanceToMove, DWORD  dwMoveMethod); 

// read data from stream
// INPUT:	char* lpBuffer				- address of buffer that receives data  
//			DWORD nNumberOfBytesToRead	- number of bytes to read 
// 			DWORD* lpNumberOfBytesRead	- address of number of bytes read 
//
// RETURN:	1 - all OK, read successfull, stream pointer moved
//			0 - read error, stream pointer not moved

int Read(char* lpBuffer, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead);
 
 
 
// read data from stream, intrenal buffer allocation
// INPUT:	char** lpBuffer				- address of variable that receives address to allocated buffer with data
//			BOOL bFree					- free previous buffer
//			DWORD nNumberOfBytesToRead	- number of bytes to read 
// 			DWORD* lpNumberOfBytesRead	- address of number of bytes read 
//
// RETURN:	1 - all OK, read successfull, stream pointer moved
//			0 - read error, stream pointer not moved

int Read(char** lpBuffer, BOOL bFree, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead);
 
// free memory allocated with Read function
void Free(char* lpBuffer); 



int SetStreamSize(DWORD dwSize);
 
  
// close stream, free all memory, close file
int WStream::Close();

DWORD GetStreamSize(); 
DWORD GetStreamPointer();

char* GetError();

private:
	HANDLE _hFile;		// open file handle for file stream
	DWORD _dwFileSize;	// opened file size
	DWORD _dwStreamSize;// length of stream
	DWORD _dwStreamPointer;	// current stream pointer
	char* _lpInternalBuffer;
	char* _lpErrorMessage;
	DWORD _dwStreamType;	// type of stream, STREAM_FILE, STREAM_MEMORY
	char* _lpMemStream;		// memory stream

	void error(char* err_message);

protected:







};










#endif
