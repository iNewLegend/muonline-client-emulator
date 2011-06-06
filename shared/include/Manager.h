// manager.h : 管理系统的父类
#pragma once
#include "Common.h"

// STL
#include <string>
#include <map>

template <typename IDTYPE, typename ITEMTYPE>
class CMapContainer
{
public:
	CMapContainer() : BaseIndex(0){}
	~CMapContainer()
	{
		clear();
	}

	int add(ITEMTYPE* item)
	{
		BaseIndex++;
		m_Items[ BaseIndex ] = item;
		return BaseIndex;
	}
public:
	bool del(IDTYPE index)
	{
		std::map<IDTYPE, ITEMTYPE*>::iterator it = m_Items.find(index);
		if (it == m_Items.end())
		{
			return false; // 找不到
		}
		ITEMTYPE* pItem = it->second;
		m_Items.erase(it);
		S_DEL(pItem);
		return true;
	}

	void clear()
	{
		for (std::map<IDTYPE, ITEMTYPE*>::iterator it=m_Items.begin(); it!=m_Items.end(); ++it)
		{
			S_DEL(it->second);
		}
		m_Items.clear();
	}

	ITEMTYPE* getItem(IDTYPE index)
	{
		if (m_Items.find(index) != m_Items.end())
		{
			return m_Items[ index ];
		}
		return NULL;
	}
public:
	std::map<IDTYPE, ITEMTYPE*> m_Items;
protected:
	IDTYPE BaseIndex;
};

template <class T>
class CManager
{
public:
	class CManagedItem
	{
		int nRefCount;
		
	public:
		T* pItem;
		std::string strName;
		CManagedItem():pItem(NULL),nRefCount(0){}
		~CManagedItem()
		{
			S_DEL(pItem);
		}
		void addRef()
		{
			++nRefCount;
		}
		bool delRef()
		{
			return --nRefCount==0;
		}
	};
	unsigned long m_uBaseID;
public:
	std::map<std::string, unsigned long> m_Names;
	std::map<unsigned long, CManagedItem> m_Items;

	CManager():m_uBaseID(0)
	{
	}

	//virtual unsigned long Add(std::string name) = 0;

	virtual void del(unsigned long id)
	{
		if (m_Items.find(id) == m_Items.end())
		{
			doDelete(id);
			return; // 如果找不到这一item id, 删除它
		}

		if (m_Items[id].delRef()) // 是不是为真正的删除
		{
			#ifdef _DEBUG
				//LOG(_T("Removing : %s"), i->name.c_str());
			#endif

			doDelete(id);
			for (std::map<std::string, unsigned long>::iterator it=m_Names.begin(); it!=m_Names.end(); ++it)
			{
				if (it->second == id)
				{
					m_Names.erase(it);
					break;
				}
			}
			
			m_Items.erase(m_Items.find(id));
		}
	}

	void del(const std::string& strName)
	{
		unsigned long id = find(strName)
		if (id>0) 
		{
			del(id);
		}
	}

	virtual void doDelete(unsigned long id) {}

	unsigned long find(const std::string& strName)
	{
		if (m_Names.find(strName) != m_Names.end())
		{
			return m_Names[strName];
		}
		return 0;
	}

	T* getItem(unsigned long id)
	{
		std::map<unsigned long, CManagedItem>::iterator it = m_Items.find(id);
		if (m_Items.end() != it)
		{
			return it->second.pItem;
		}
		return NULL;
	}

	const std::string& getItemName(unsigned long id)
	{
		std::map<unsigned long, CManagedItem>::iterator it = m_Items.find(id);
		if (m_Items.end() != it)
		{
			return it->second.strName;
		}
		const static std::string s_strName="NULL";
		return s_strName;
	}

	void clear()
	{
		for (std::map<unsigned long, CManagedItem>::iterator it=m_Items.begin(); it!=m_Items.end(); ++it)
		{
			DoDelete(it->first);
		}
		m_Names.clear();
		m_Items.clear();
	}

protected:
	unsigned long addRef(const std::string& strName)
	{
		unsigned long ID = find(strName);
		if (ID!=0)
		{
			m_Items[ID].addRef();
		}
		return ID;
	}

	unsigned long add(const std::string& strName, T* pItem)
	{
		m_Names[strName] = ++m_uBaseID;
		m_Items[m_uBaseID].addRef();
		m_Items[m_uBaseID].pItem = pItem;
		m_Items[m_uBaseID].strName = strName;
		return m_uBaseID;
	}
};
