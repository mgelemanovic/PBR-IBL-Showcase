#include "FileSystem.h"

#include <iostream>

extern bool _fsReadFile(char **astrLines, const char *strFilename)
{
	// try opening file for read
	FILE *pfFile = fopen(strFilename, "rb");
	if (pfFile == NULL) {
		return false;
	}

	// find length of file
	fseek(pfFile, 0, SEEK_END);
	int iLength = ftell(pfFile);
	// position to start
	fseek(pfFile, 0, SEEK_SET);

	// allocate a buffer for read data
	char *strBuffer = (char*)malloc(iLength + 1);
	if (strBuffer == NULL) {
		fclose(pfFile);
		return false;
	}

	// read data and add ending 0
	bool success = iLength > 0 ? fread(strBuffer, iLength, 1, pfFile) == 1 : true;
	strBuffer[iLength] = '\0';

	// done with reading file
	fclose(pfFile);

	// if error occured during reading, destroy buffer data
	if (!success) {
		free(strBuffer);
		return false;
	}

	// output read data
	*astrLines = strBuffer;

	return true;
}