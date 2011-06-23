#ifndef MSGdef
#define MSGdef

class Messages
{
public:
	Messages();
	~Messages();
public:
	void DataSendAll(unsigned char* Packet, int Len);
	void DataSendMap(int Map, unsigned char* Packet, int Len);
	void DataSendNear(int aIndex, unsigned char* Packet, int Len);
	void Notice(int aIndex, const char* Format, ...);
	void NoticeMap(int Map, const char* Format, ...);
	void NoticeAll(const char* Format, ...);
	void PM(int aIndex, const char* Format, ...);
	void PMAll(const char* Format, ...);
	void PMMap(int Map, const char* Format, ...);
	void Msg(int aIndex, const char* Format, ...);
	void MsgAll(const char* Format, ...);
	void MsgMap(int Map, const char* Format, ...);
	void Messages::GreenNoticeAll(const char *Format, ...);
};

extern Messages g_Msg;
#endif