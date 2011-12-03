#include "stdafx.h"
#include "MBlobArray.h"
#include <memory.h>

void* MMakeBlobArray(int nOneBlobSize, int nBlobCount)
{
	// nBlobCount����ŭ Blob�� �ΰ� ����(ī��Ʈ)�� ���� ���� ���, �� ó���� �ΰ� ����(ī��Ʈ)�� nOneBlobSize�� nBlobCount���� �ִ´�.
	unsigned char* pBlob = new unsigned char[sizeof(nOneBlobSize)+sizeof(nBlobCount)+nOneBlobSize*nBlobCount];
	memcpy(pBlob, &nOneBlobSize, sizeof(nOneBlobSize));
	memcpy(pBlob+sizeof(nBlobCount), &nBlobCount, sizeof(nOneBlobSize));
	return pBlob;
}

void MEraseBlobArray(void* pBlob)
{
	delete[] pBlob;
}

void* MGetBlobArrayElement(void* pBlob, int i)
{
	int nBlobCount = 0;
	int nOneBlobSize = 0;
	memcpy(&nOneBlobSize, (unsigned char*)pBlob, sizeof(nOneBlobSize));
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(nOneBlobSize), sizeof(nBlobCount));
	if(i<0 || i>=nBlobCount) return 0;

	return ((unsigned char*)pBlob+sizeof(int)*2+nOneBlobSize*i);
}

int MGetBlobArrayCount(void* pBlob)
{
	int nBlobCount;
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(int), sizeof(nBlobCount));
	return nBlobCount;
}

int MGetBlobArraySize(void* pBlob)
{
	int nBlobCount, nOneBlobSize;
	memcpy(&nOneBlobSize, (unsigned char*)pBlob, sizeof(nOneBlobSize));
	memcpy(&nBlobCount, (unsigned char*)pBlob+sizeof(int), sizeof(nBlobCount));

	return (nOneBlobSize*nBlobCount+sizeof(int)*2);
}

void* MGetBlobArrayPointer(void* pBlob)
{
	return ((unsigned char*)pBlob+sizeof(int)*2);
}