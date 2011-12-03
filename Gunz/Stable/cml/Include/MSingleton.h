#include <cassert>

template <typename T> class MSingleton
{
    static T* m_pSingleton;

public:
    MSingleton( void )
    {
        assert( !m_pSingleton );

		/* 
		���߻���� �޾����� �����Ͱ� �ٸ��� ���� ������ �ذ��� ��Ŭ�� ���ø�

		gpg �� ���� ���
        int offset = (int)((T*)1) - (int)(MSingleton <T>*)((T*)1);
        m_pSingleton = (T*)((int)this + offset);
		*/
        
		// �̰��� ǥ�ؿ� ����� ����ε�.
        m_pSingleton = static_cast<T *>(this); 

	}
   ~MSingleton( void )
        {  assert( m_pSingleton );  m_pSingleton = 0;  }
    static T& GetInstance( void )
        {  assert( m_pSingleton );  return ( *m_pSingleton );  }
    static T* GetInstancePtr( void )
        {  return ( m_pSingleton );  }
};

template <typename T> T* MSingleton <T>::m_pSingleton = 0;


/*
// ����

class TextureMgr : public MSingleton <TextureMgr>
{
public:
	   char type;
	   char type2;
    char* GetTexture( const char* name );
	   
    // ...
};

char* TextureMgr::GetTexture( const char* name )
{
	   return NULL;
}

#define g_TextureMgr TextureMgr::GetInstance()

void main()
{


	TextureMgr aa;
	char * stone1 = TextureMgr::GetInstance().GetTexture( "stone1" );

	//   Texture* wood6 = g_TextureMgr.GetTexture( "wood6" );
    // ...
}

*/