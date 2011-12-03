// Ư�� �޸� ���� �迭�� �� ������ �ϳ��� �޸� ���� �ֱ� ���� �Լ���
#ifndef MBLOBARRAY_H
#define MBLOBARRAY_H

/// nOneBlobSize��ŭ nBlobCount������ŭ �迭�� �� �����
void* MMakeBlobArray(int nOneBlobSize, int nBlobCount);
/// �� �����
void MEraseBlobArray(void* pBlob);
/// ������ �� ���� ����
void* MGetBlobArrayElement(void* pBlob, int i);
/// ���� ���� ����
int MGetBlobArrayCount(void* pBlob);
/// ��� ��ü ������ ����
int MGetBlobArraySize(void* pBlob);
/// Array Block���� ���� �� ó�� ������ ����
void* MGetBlobArrayPointer(void* pBlob);

#endif