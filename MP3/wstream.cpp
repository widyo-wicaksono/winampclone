#define STRICT
#include <windows.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "wstream.h"

#define ERRORMESSAGE_SIZE 1024


WStream::WStream()
{
	_hFile = 0;
	_dwFileSize = 0;
	_lpErrorMessage = 0;
	_lpErrorMessage = (char*) malloc(ERRORMESSAGE_SIZE);
	_dwStreamSize = 0;
	_dwStreamPointer = 0;
	_lpInternalBuffer = 0;
	_dwStreamType = 0;
	_lpMemStream = 0;
	error(0);

}


WStream::~WStream()
{
	if(_hFile)
		::CloseHandle(_hFile);

	if(_lpInternalBuffer)
		free(_lpInternalBuffer);

	if(_lpErrorMessage)
		free(_lpErrorMessage);

	_dwStreamType = 0;

}



int WStream::OpenFile(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes)
{
	
	if(_dwStreamType) {
		error("WStream::OpenFile\n\nStream already open.Close stream before!");
		return 0;

	}

	if(lpFileName == 0) {
		error("WStream::OpenFile\n\nNeed file name!");
		return 0;

	}


	_dwStreamPointer = 0;
	
	_hFile = ::CreateFile(lpFileName,dwDesiredAccess,dwShareMode,NULL,dwCreationDistribution,dwFlagsAndAttributes,NULL);
    if( _hFile == INVALID_HANDLE_VALUE) {
		error("WStream::OpenFile\n\nCan't open file!");
		return 0;
	}
    	

    _dwFileSize = ::GetFileSize(_hFile, NULL);
    if(_dwFileSize ==  0xFFFFFFFF) {
    	::CloseHandle(_hFile);
        _hFile = 0;
        _dwFileSize = 0;
		error("WStream::OpenFile\n\nCan't open file!");
        return 0;
    }

	DWORD ret = ::SetFilePointer(_hFile, 0, NULL, FILE_BEGIN); 
	if(ret == 0xFFFFFFFF) {
		::CloseHandle(_hFile);
        _hFile = 0;
        _dwFileSize = 0;
		error("WStream::OpenFile\n\nCan't seek to begining!");
        return 0;
	} 


	_dwStreamSize = _dwFileSize;
	_dwStreamType = STREAM_FILE;
	_dwStreamPointer = 0;

    
	return 1;
}


int WStream::Seek(LONG lDistanceToMove, DWORD  dwMoveMethod)
{
	
	if(_dwStreamType == 0) {
		error("WStream::Seek\n\nStream isn't open!");
		return 0;
	}
	
	switch(dwMoveMethod) {
		case STREAM_BEGIN:
			if(lDistanceToMove < 0 || (DWORD) lDistanceToMove > _dwStreamSize) {
				error("WStream::Seek\n\nCan't seek stream !\nlDistanceToMove out of bounds.");
				return 0;

			}
			
			if(_dwStreamType == STREAM_MEMORY) {
				_dwStreamPointer = lDistanceToMove;
			}


		break;

		case STREAM_CURRENT:
			if(lDistanceToMove < 0) {
				if((DWORD)abs(lDistanceToMove) > _dwStreamPointer) {
					error("WStream::Seek\n\nCan't seek stream !\nlDistanceToMove out of bounds.");
					return 0;
				}
			}
			else {
				if((DWORD)lDistanceToMove > (_dwStreamSize - _dwStreamPointer)) {
					error("WStream::Seek\n\nCan't seek stream !\nlDistanceToMove out of bounds.");
					return 0;
				}
			}

			if(_dwStreamType == STREAM_MEMORY) {
				_dwStreamPointer += lDistanceToMove;
			}

		break;

		case STREAM_END:
			if(lDistanceToMove > 0 || (DWORD)abs(lDistanceToMove) > _dwStreamSize) {
				error("WStream::Seek\n\nCan't seek stream !\nlDistanceToMove out of bounds.");
				return 0;
			}

			if(_dwStreamType == STREAM_MEMORY) {
				_dwStreamPointer = _dwStreamSize + lDistanceToMove;
			}
		break;
	}
	
	
	if(_dwStreamType == STREAM_FILE && _hFile) { // file stream
		DWORD ret = ::SetFilePointer(_hFile, lDistanceToMove, NULL, dwMoveMethod); 
		if(ret == 0xFFFFFFFF) {
			error("WStream::Seek\n\nCan't seek file pointer!");
			return 0;
		} 

		_dwStreamPointer = ret;
	}

	return 1;
}


int WStream::Close()
{
	if(_hFile) {
		if(!::CloseHandle(_hFile)) {
			error("WStream::Close\n\nCan't close file handle!");
			return 0;

		}
		_hFile = 0;
		_dwFileSize = 0;
	}

	if(_lpInternalBuffer)
		free(_lpInternalBuffer);

	_lpInternalBuffer = 0;

	_dwStreamSize = 0;
	_dwStreamPointer = 0;

	_dwStreamType = 0;
	_lpMemStream = 0;

	return 1;

}




void WStream::error(char* err_message)
{
	size_t size = 0;
	if(err_message != 0) {
		size = strlen(err_message);
		size = (size > 	ERRORMESSAGE_SIZE) ? ( ERRORMESSAGE_SIZE - 1) : size;
		strncpy(_lpErrorMessage, err_message, size);
	}
	_lpErrorMessage[size]= 0;
	
}

char* WStream::GetError()
{
	return _lpErrorMessage;
	
}


int WStream::Read(char* lpBuffer, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead)
{
	
	if(_dwStreamType == 0) {
		error("WStream::Seek\n\nStream isn't open!");
		return 0;
	}
	
	if(lpNumberOfBytesRead == 0) {
		error("WStream::Read\n\nlpNumberOfBytesRead can't be 0 !");
		return 0;


	}

	*lpNumberOfBytesRead = 0;

	if(lpBuffer == 0) {
		error("WStream::Read\n\nlpBuffer can't be 0 !");
		return 0;
	}

	if(_dwStreamPointer > _dwStreamSize) {
		error("WStream::Read\n\nStream pointer out of bounds !");
		return 0;
	}


	if(( nNumberOfBytesToRead + _dwStreamPointer)  > _dwStreamSize) {
		nNumberOfBytesToRead = _dwStreamSize - _dwStreamPointer;	
	}
		
	if(_dwStreamType == STREAM_FILE && _hFile) {
		BOOL ret;
		ret = ::ReadFile(_hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, NULL); 
		if(ret == FALSE || (ret == TRUE && *lpNumberOfBytesRead == 0))  {
			error("WStream::Read\n\nCan't read from file");
			::SetFilePointer(_hFile,_dwStreamPointer,0,STREAM_BEGIN);
			return 0;	
		}
		_dwStreamPointer += *lpNumberOfBytesRead;
		
	}
	else if(_dwStreamType == STREAM_MEMORY) { // other stream type
		//memcpy(lpBuffer,_lpMemStream + _dwStreamPointer, nNumberOfBytesToRead);
		CopyMemory(lpBuffer,_lpMemStream + _dwStreamPointer, nNumberOfBytesToRead);
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		_dwStreamPointer += nNumberOfBytesToRead;
		return 1;

	}
	else
		return 0;


	return 1;
}



int WStream::Read(char** lpBuffer,BOOL bFree, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead)
{
	
	if(_dwStreamType == 0) {
		error("WStream::Seek\n\nStream isn't open!");
		return 0;
	}
	
	
	if(bFree) {
		free(_lpInternalBuffer);
		_lpInternalBuffer = 0;
	}
	
	if(lpNumberOfBytesRead == 0) {
		error("WStream::Read\n\nlpNumberOfBytesRead can't be 0 !");
		return 0;


	}
	*lpNumberOfBytesRead = 0;

	if(lpBuffer == 0) {
		error("WStream::Read\n\nlpBuffer can't be 0 !");
		return 0;
	}

	if(_dwStreamPointer > _dwStreamSize) {
		error("WStream::Read\n\nStream pointer out of bounds !");
		return 0;
	}


	if(( nNumberOfBytesToRead + _dwStreamPointer)  > _dwStreamSize) {
		nNumberOfBytesToRead = _dwStreamSize - _dwStreamPointer;	
	}


	if(_hFile) {
		_lpInternalBuffer = (char*) malloc(nNumberOfBytesToRead);
		if(_lpInternalBuffer == NULL) {
			error("WStream::Read\n\nCan't allocate memory for lpBuffer !");
			return 0;
		}

		BOOL ret;
		ret = ::ReadFile(_hFile, _lpInternalBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, NULL); 
		if(ret == FALSE || (ret == TRUE && *lpNumberOfBytesRead == 0))  {
			error("WStream::Read\n\nCan't read from file");
			::SetFilePointer(_hFile,_dwStreamPointer,0,STREAM_BEGIN);
			return 0;	
		}
		_dwStreamPointer += *lpNumberOfBytesRead;
		*lpBuffer = _lpInternalBuffer;
	}
	else if(_dwStreamType == STREAM_MEMORY) { // other stream type
		//memcpy(_lpInternalBuffer,_lpMemStream + _dwStreamPointer, nNumberOfBytesToRead);
		CopyMemory(_lpInternalBuffer,_lpMemStream + _dwStreamPointer, nNumberOfBytesToRead);
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		_dwStreamPointer += nNumberOfBytesToRead;
		return 1;

	}
	else
		return 0;

	return 1;
}


void WStream::Free(char* lpBuffer)
{
	if(lpBuffer == 0 && _hFile && _lpInternalBuffer != 0) {
		free(_lpInternalBuffer);
		_lpInternalBuffer = 0;
	}

	if(_hFile)
		free(lpBuffer);
}

DWORD WStream::GetStreamSize()
{
	return _dwStreamSize;
}

DWORD WStream::GetStreamPointer()
{
	return _dwStreamPointer;
}




int WStream::OpenResource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType)
{

	if(_dwStreamType) {
		error("WStream::OpenResource\n\nStream already open.Close stream before!");
		return 0;

	}
	
	
	_dwStreamSize = 0;
	_dwStreamType = 0;
	_dwStreamPointer = 0;
	
	HRSRC hrSrc = FindResource(hModule,lpName,lpType);
	if(!hrSrc) {
		error("WStream::OpenResource\n\nCan't find resource!");
		return FALSE;

	}

	HGLOBAL hRes = LoadResource(hModule,hrSrc);

	if(!hRes) {
		error("WStream::OpenResource\n\nCan't load resource!");
		return FALSE;

	}

	char* res = (char*) LockResource(hRes);

	if(!res) {
		error("WStream::OpenResource\n\nCan't lock resource");
		return FALSE;

	}

	DWORD size = SizeofResource(hModule, hrSrc);
	if(size == 0) {
		error("WStream::OpenResource\n\nCan't get resource size!");
		return FALSE;

	}

	_dwStreamSize = size;
	_dwStreamType = STREAM_MEMORY;
	_dwStreamPointer = 0;
	_lpMemStream = res;

	return TRUE;


}




int WStream::OpenMemory(char* lpMemory, DWORD dwSize)
{
	if(_dwStreamType) {
		error("WStream::OpenMemory\n\nStream already open.Close stream before!");
		return 0;
	}

	if(lpMemory == 0 || dwSize == 0) {
		error("WStream::OpenMemory\n\nCan't open NULL stream");
		return 0;
	}

	_dwStreamSize = dwSize;
	_dwStreamType = STREAM_MEMORY;
	_dwStreamPointer = 0;
	_lpMemStream = lpMemory;

	return 1;
}



int WStream::SetStreamSize(DWORD dwSize)
{
	_dwStreamSize = dwSize;
	return 1;
}
