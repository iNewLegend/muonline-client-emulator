#pragma once


template <typename T>
class TSingleton
{
public:
	TSingleton(void){m_pInstace=static_cast<T*>(this);}
	~TSingleton(void){}

	static T& getInstance()
	{
		if (NULL == m_pInstace)
		{
			m_pInstace = new T;
		}
		return  *m_pInstace;
	}

protected:
	static T * m_pInstace;

private:
	TSingleton<T>* operator =(const TSingleton<T>&);

};


template <typename T>
T* TSingleton<T>::m_pInstace = NULL;
