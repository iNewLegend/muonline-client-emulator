#ifndef _MINETUTIL_H
#define _MINETUTIL_H



#include <string>

using std::string;



/// . �ִ� IP ���ڿ��� . ���� IP ���ڿ�(12����Ʈ)�� ��ȯ
void MConvertCompactIP(char* szOut, const char* szInputDottedIP);

void GetLocalIP( char* szOutIP, int nSize );

const bool MGetIPbyHostName( const string& strName, string& outIP );


#endif