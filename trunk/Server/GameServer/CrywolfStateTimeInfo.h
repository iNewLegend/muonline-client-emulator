// CrywolfStateTimeInfo.h: interface for the CCrywolfStateTimeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRYWOLFSTATETIMEINFO_H__0CB24640_4832_4A12_85E0_88DF55431959__INCLUDED_)
#define AFX_CRYWOLFSTATETIMEINFO_H__0CB24640_4832_4A12_85E0_88DF55431959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCrywolfStateTimeInfo
{

public:

	CCrywolfStateTimeInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		this->m_bUsed = FALSE;
		this->m_iStateNumber = -1;	
		this->m_iYear = -1;	
		this->m_iMonth = -1;	
		this->m_iDay = -1;	
		this->m_iDayOfWeek = -1;	
		this->m_iHour = -1;	
		this->m_iMinute = -1;	
		this->m_iSecond = -1;	
		this->m_iContinuanceTime = 0;	
		this->m_dwAppliedTickCount = 0;	
	}

	void SetAppliedTime()
	{
		this->m_dwAppliedTickCount = GetTickCount();
	}

	void ResetAppliedTime()
	{
		this->m_dwAppliedTickCount = 0;
	}

	BOOL CheckContinuanceTime()
	{
		if ( (GetTickCount() - this->m_dwAppliedTickCount ) > this->m_iContinuanceTime )
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL CheckScheduleTime()
	{
		CTime tCurrentTime = CTime::GetTickCount();

		int iYear = tCurrentTime.GetYear();
		int iMonth = tCurrentTime.GetMonth();
		int iDay = tCurrentTime.GetDay();
		int iDayOfWeek = tCurrentTime.GetDayOfWeek();
		int iHour = tCurrentTime.GetHour();
		int iMinute = tCurrentTime.GetMinute();
		int iCrywolfState = -1;

		CTimeSpan tTimeRange(0, 0, this->m_iContinuanceTime, 0);

		//La forma de configuracion en crywolf.dat queda de la siguiente manera
		//configuro por mes y dia o por dia de la semana
		//Para configurar por mes dia hora y minutos
		//0		1		11		5		-1		23		55		-1
		//En este ejemplo el evento se realizara el mes de noviembre, dia 5 a las 23 55hs
		
		//Para configurar por dia de la semana 
		//0		1		-1		-1		1		23		55		-1
		//En el ejemplo el evento se realizara el Domingo(Primer dia de la semana)  a las 23 55hs

		//Podemos seguir agregandole lineas y asi configurar muchos dias al mes a la hora que deseemos asi tambien como 
		//con los dias de la semana
		//Desarrollado por CALYSTO®

//Fixed by CALYSTO

	if ( this->m_iDayOfWeek == -1 )
		{	
			if ( this->m_iMonth != -1 )
				{
					if ( this->m_iMonth == iMonth )
						{
							if ( this->m_iDay != -1 )
								{
									if ( this->m_iDay == iDay )
										{
											if ( this->m_iHour != -1 )
												{
													if ( this->m_iHour == iHour )
														{
															if ( this->m_iMinute != -1 )
																{
																	if ( this->m_iMinute == iMinute )
																		{	
																			return TRUE;
																		}
																}
														}
												}
										}
								}
						}
				}
		}
		else	
		{
			if ( this->m_iDayOfWeek == iDayOfWeek )
			{
				if ( this->m_iHour != -1 )
				{
					if ( this->m_iHour == iHour )
					{
						if ( this->m_iMinute != -1 )
							{
								if ( this->m_iMinute == iMinute )
									{	
									return TRUE;
									}
							}					
					}
				}
			}
		}
return FALSE;	

//End Fixed by CALYSTO®
	

	}

	int GetLeftTime()
	{
		if ( this->CheckContinuanceTime()==TRUE )//CALYSTO®
		{
			int iLeftTime = this->m_iContinuanceTime - (GetTickCount() - this->m_dwAppliedTickCount);

			if ( iLeftTime < 0 )
				iLeftTime = 0;

			return iLeftTime;
		}

		return 0;
	}

public:

	BOOL m_bUsed;	// 0
	int m_iStateNumber;	// 4
	int m_iYear;	// 8
	int m_iMonth;	// C
	int m_iDay;	// 10
	int m_iDayOfWeek;	// 14
	int m_iHour;	// 18
	int m_iMinute;	// 1C
	int m_iSecond;	// 20
	int m_iContinuanceTime;	// 24
	DWORD m_dwAppliedTickCount;	// 28

};

#endif // !defined(AFX_CRYWOLFSTATETIMEINFO_H__0CB24640_4832_4A12_85E0_88DF55431959__INCLUDED_)
