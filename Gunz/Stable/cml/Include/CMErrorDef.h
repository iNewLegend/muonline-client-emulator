/*
 *	CMErrorDef.h
 *		Erroró���� ���� �ڵ�
 *		����ȣ ( 98-01-04 3:31:28 ���� )
 *
 *		���� �ڵ�� �տ� ESTR_�� ���̸�, ���� �������� ��µǴ�
 *			���� �޼����̰�,
 *		KSTR_�� ���̸�, �ѱ� �������� ��µǴ� ���� �޼����̴�.
 ********************************************************************/
#ifndef _CMERRORDEF_H
#define _CMERRORDEF_H

/*	OK	*/
#define CM_OK						0
#define ESTR_CM_OK					"OK."
#define KSTR_CM_OK					"�����Դϴ�."

/*	Generic Error	*/
#define CMERR_GENERIC				100
#define ESTR_CMERR_GENERIC			"Generic Error Occured."
#define KSTR_CMERR_GENERIC			"�Ϲ����� �����Դϴ�."

/*	Out of Memory	*/
#define CMERR_OUT_OF_MEMORY			101
#define ESTR_CMERR_OUT_OF_MEMORY	"Out of Memory."
#define KSTR_CMERR_OUT_OF_MEMORY	"�޸𸮰� �����մϴ�."

/*	pErrStr : ������ �߻��� ���� �̸�	*/
#define CMERR_CANT_OPEN_FILE		102
#define ESTR_CMERR_CANT_OPEN_FILE	"Can't Open File."
#define KSTR_CMERR_CANT_OPEN_FILE	"������ �� �� �����ϴ�."

/*	pErrStr : ������ �߻��� ���� �̸�	*/
#define CMERR_CANT_SAVE_FILE		103
#define ESTR_CMERR_CANT_SAVE_FILE	"Can't Save File."
#define KSTR_CMERR_CANT_SAVE_FILE	"������ ������ �� �����ϴ�."

/*	Cannot Create Window Error	*/
/*	Modified by CJP */
#define CMERR_CANT_CREATE_WIN		104
#define ESTR_CMERR_CANT_CREATE_WIN	"Can't not create a window."
#define KSTR_CMERR_CANT_CREATE_WIN	"�����츦 ������ �� �����ϴ�."

/* ������ �����ų� ���ڵ��� ������ ������ �� ���� ��� */
#define CMERR_INVALID_FILE			105
#define ESTR_CMERR_INVALID_FILE		"The file is invalid. Please re-install or find a patch program."
#define KSTR_CMERR_INVALID_FILE		"������ ��ȿ���� �ʽ��ϴ�. �ٽ� ��ġ�ϰų� ��ġ ���α׷��� ã�� ��ġ�Ͻñ� �ٶ��ϴ�."

/*	Error Code by Joongpil, Cho	*/
#define CMERR_JOONGPIL_CHO					1000

#ifdef ACESAGA	// AceSaga Specific

#define	CMERR_CANNOT_OPEN_UITEX				1001
#define ESTR_CMERR_CANNOT_OPEN_UITEX		"Cannot open ui.txz."
#define KSTR_CMERR_CANNOT_OPEN_UITEX		"Data\\ui.txz�� �о�� �� �����ϴ�."

#define	CMERR_FAIL_INIT_RS					1002
#define ESTR_CMERR_FAIL_INIT_RS				"Cannot init RealSpace. Please configure for your system."
#define KSTR_CMERR_FAIL_INIT_RS				"RealSpace�� �ʱ�ȭ�� �� �����ϴ�. Config ��ƿ��Ƽ�� �̿��Ͽ� �ٽ� �������ּ���."

#endif

#ifdef ARDNET	// ArdNet Specific

#define	CMERR_CANNOT_OPEN_UITEX				1001
#define ESTR_CMERR_CANNOT_OPEN_UITEX		"Cannot open ui.txz."
#define KSTR_CMERR_CANNOT_OPEN_UITEX		"Data\\ui.txz�� �о�� �� �����ϴ�."

#define	CMERR_FAIL_INIT_RS					1002
#define ESTR_CMERR_FAIL_INIT_RS				"Cannot init RealSpace. Please configure for your system."
#define KSTR_CMERR_FAIL_INIT_RS				"RealSpace�� �ʱ�ȭ�� �� �����ϴ�. Config ��ƿ��Ƽ�� �̿��Ͽ� �ٽ� �������ּ���."

#define CMERR_FAIL_OPEN_MAPFILE				1003
#define ESTR_CMERR_FAIL_OPEN_MAPFILE		"Cannot open the map file. Please re-install Ard|Net."
#define KSTR_CMERR_FAIL_OPEN_MAPFILE		"������ �������� �о�� �� �����ϴ�. �ٽ� ��ġ���ֽñ� �ٶ��ϴ�."

#define CMERR_FAIL_OPEN_ATTFILE				1004
#define ESTR_CMERR_FAIL_OPEN_ATTFILE		"Cannot open the map attribute file. Please re-install Ard|Net."
#define KSTR_CMERR_FAIL_OPEN_ATTFILE		"������ �� Ư�� ������ �о�� �� �����ϴ�. �ٽ� ��ġ���ֽñ� �ٶ��ϴ�."

#define CMERR_FAIL_OPEN_MESH				1005
#define ESTR_CMERR_FAIL_OPEN_MESH			"OBSOLUTE FILE FORMAT - Please constct with us."
#define KSTR_CMERR_FAIL_OPEN_MESH			"���̻� ��ȿ���� �ʴ� �����Դϴ�. ���� �� ������ �߻��Ǿ��ٸ� ���񿡰� �������ּ���."

#define CMERR_FAIL_INIT_CONSOLE				1006
#define ESTR_CMERR_FAIL_INIT_CONSOLE		"Cannot init the console. Please re-install Ard|Net."
#define KSTR_CMERR_FAIL_INIT_CONSOLE		"�ܼ� �ʱ�ȭ�� �����Ͽ����ϴ�."


#endif


#define CMERR_EXCEED_SIZE					1010
#define ESTR_CMERR_EXCEED_SIZE				"Logical Error: Size is exceed."
#define KSTR_CMERR_EXCEED_SIZE				"ũ�Ⱑ �ʹ� Ů�ϴ�."


/*	Error Code by Jangho, Lee	*/
#define CMERR_JANGHO_LEE					2000

#define CMERR_CANT_MAP_FILE_TO_MEMORY		2001
#define ESTR_CMERR_CANT_MAP_FILE_TO_MEMORY	"Cannot Map File to Memory."
#define KSTR_CMERR_CANT_MAP_FILE_TO_MEMORY	"������ �޸𸮷� ������ �� �����ϴ�."

#define CMERR_CANT_CREATE_BITMAP			2002
#define ESTR_CMERR_CANT_CREATE_BITMAP		"Cannot create(convert) bitmap handle."
#define KSTR_CMERR_CANT_CREATE_BITMAP		"��Ʈ�� �ڵ��� ����(��ȯ)�� �� �����ϴ�."

#define CMERR_CANT_TXZ_BACKUP_FILE			2003
#define ESTR_CMERR_CANT_TXZ_BACKUP_FILE		"Cannot save backup file ( backup.txz )"
#define KSTR_CMERR_CANT_TXZ_BACKUP_FILE		"�Ʒ� Second Error Message�� ������ backup.txz�� ����� �� �����ϴ�."

#define CMERR_CANT_OPEN_FOR_SAVE			2004
#define ESTR_CMERR_CANT_OPEN_FOR_SAVE		"Cannot open file to save. File might be used another program."
#define KSTR_CMERR_CANT_OPEN_FOR_SAVE		"������ �����Ϸ��� �Ͽ�����, ������ �� �� �����ϴ�. �̹� ������� ���ɼ��� �����ϴ�."

#define CMERR_CANT_WRITE_FILE				2005
#define ESTR_CMERR_CANT_WRITE_FILE			"Cannot write to file."
#define KSTR_CMERR_CANT_WRITE_FILE			"���Ͽ� ����Ÿ�� �� �� �����ϴ�."

#define CMERR_CANT_READ_FILE				2005
#define ESTR_CMERR_CANT_READ_FILE			"Cannot read from file."
#define KSTR_CMERR_CANT_READ_FILE			"������ ����Ÿ�� ���� �� �����ϴ�."

#define CMERR_INVALID_FILE_FORMAT			2006
#define ESTR_CMERR_INVALID_FILE_FORMAT		"Cannot open file. Invalid file format."
#define KSTR_CMERR_INVALID_FILE_FORMAT		"������ �� �� �����ϴ�. ���� ������ �ٸ��ϴ�."

#define CMERR_PREVIOUS_VERSION				2007
#define ESTR_CMERR_PREVIOUS_VERSION			"Previous vesion."
#define KSTR_CMERR_PREVIOUS_VERSION			"���� �����Դϴ�."

#define CMERR_NEXT_VERSION					2008
#define ESTR_CMERR_NEXT_VERSION				"Next vesion."
#define KSTR_CMERR_NEXT_VERSION				"���� �����Դϴ�."

	/* Map|Blast Errors */
#define CMERR_INCORRECT_MAP_ID				2100
#define ESTR_CMERR_INCORRECT_MAP_ID			"Not Map|Blast file. ( *.map )"
#define KSTR_CMERR_INCORRECT_MAP_ID			"Map|Blast ( *.map ) ������ �ƴմϴ�. ���� ���� ID�� Ʋ���ϴ�."

/*	Error Code by Jayoung, Na	*/
#define CMERR_JAYOUNG_NA					3000

/*	Error Code by Youngho, Kim	*/
#define CMERR_YOUNGHO_KIM					4000

#define CMERR_WINSOCK_NOT_AVAIL				4001
#define ESTR_CMERR_WINSOCK_NOT_AVAIL	"Winsock not available."
#define KSTR_CMERR_WINSOCK_NOT_AVAIL	"Winsock�� ���Ҵ��Դϴ�."

#define CMERR_CANT_CREATE_SOCKET			4002
#define ESTR_CMERR_CANT_CREATE_SOCKET	"Can't create socket."
#define KSTR_CMERR_CANT_CREATE_SOCKET	"������ �� �� �����ϴ�."

#define CMERR_CANT_SO_REUSEADDR				4003
#define ESTR_CMERR_CANT_SO_REUSEADDR	"Can't setsockopt SO_REUSEADDR"
#define KSTR_CMERR_CANT_SO_REUSEADDR	"SO_REUSEADDR������ �����߽��ϴ�."

#define CMERR_CANT_SO_LINGER				4004
#define ESTR_CMERR_CANT_SO_LINGER		"Can't setsockopt SO_LINGER"
#define KSTR_CMERR_CANT_SO_LINGER		"SO_LINGER������ �����߽��ϴ�."

#define CMERR_CANT_BIND_SOCKET				4005
#define ESTR_CMERR_CANT_BIND_SOCKET		"Can't bind address."
#define KSTR_CMERR_CANT_BIND_SOCKET		"bind()�� �����߽��ϴ�."

#define CMERR_INVALID_ARGUMENT				4006
#define ESTR_CMERR_INVALID_ARGUMENT		"Invalid Argument."
#define KSTR_CMERR_INVALID_ARGUMENT		"���ڰ� �߸��Ǿ����ϴ�."

#define CMERR_SENDTO_FAILED					4007
#define ESTR_CMERR_SENDTO_FAILED		"sendto() failed."
#define KSTR_CMERR_SENDTO_FAILED		"sendto() ���� �����߽��ϴ�."

#define CMERR_CANT_SET_WSA_READ				4008
#define ESTR_CMERR_CANT_SET_WSA_READ	"Can't set WSA_READ Message."
#define KSTR_CMERR_CANT_SET_WSA_READ	"WSA_READ Message ������ �����߽��ϴ�."

#define CMERR_FILE_READ_ERROR				4009
#define ESTR_CMERR_FILE_READ_ERROR		"Can't read FILE"
#define KSTR_CMERR_FILE_READ_ERROR		"������ ���� �� �����ϴ�."

#define CMERR_FILE_WRITE_ERROR				4010
#define ESTR_CMERR_FILE_WRITE_ERROR		"Can't write FILE"
#define KSTR_CMERR_FILE_WRITE_ERROR		"���Ͽ� ������ �� �����ϴ�."

#define CMERR_UNKNOWN_USER					4011
#define ESTR_CMERR_UNKNOWN_USER			"Unknown user name."
#define KSTR_CMERR_UNKNOWN_USER			"����� �ȵ� �����Դϴ�."

#define CMERR_INVALID_PASSWORD				4012
#define ESTR_CMERR_INVALID_PASSWORD		"Invalid password."
#define KSTR_CMERR_INVALID_PASSWORD		"��ȣ�� Ʋ���ϴ�."


/*******************************
	Blast|3D Error
*******************************/
#define CMERR_CANT_INIT_B3D					5001
#define ESTR_CMERR_CANT_INIT_B3D			"Can't init Blast|3D."
#define KSTR_CMERR_CANT_INIT_B3D			"Blast|3D�� �ʱ�ȭ�ϴµ� �����Ͽ����ϴ�."

#define CMERR_B3D_INVALID_DRIVER			5002
#define ESTR_CMERR_B3D_INVALID_DRIVER		"Driver is invalid."
#define KSTR_CMERR_B3D_INVALID_DRIVER		"��ȿ�� ����̹��� �������� �ʽ��ϴ�."

#define CMERR_OBJ_NOTFOUND_MATERIAL			5002
#define ESTR_CMERR_OBJ_NOTFOUND_MATERIAL	"Material(s) or Texture(s) Not Found."
#define KSTR_CMERR_OBJ_NOTFOUND_MATERIAL	"�ؽ��� Ȥ�� ������ �������� �ʽ��ϴ�."

#define CMERR_OBJ_NOTFOUND_BMP			5003
#define ESTR_CMERR_OBJ_NOTFOUND_BMP		"*.BMP Not Found."
#define KSTR_CMERR_OBJ_NOTFOUND_BMP		"��Ʈ�������� �������� �ʽ��ϴ�."

#define CMERR_INCORRECT_FILEFORMAT			5004
#define ESTR_CMERR_INCORRECT_FILEFORMAT		"It's Invalid File or Old Version of RSM"
#define KSTR_CMERR_INCORRECT_FILEFORMAT		"�߸��� �����̰ų� �����ȹ����Դϴ�."

#define CMERROR_RML_MODIFIED			5005
#define ESTR_CMERROR_RML_MODIFIED		"RML Modified."
#define KSTR_CMERROR_RML_MODIFIED		"RML�� �ܺο��� �����Ǿ����ϴ�."



#endif	/*	_CMERRORDEF_H	*/

