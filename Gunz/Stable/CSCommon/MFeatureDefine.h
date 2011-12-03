#pragma once

// 기능이 국가별로 출시된 상황이 다른 경우 define으로 국가 선별을 해줍니다
// cscommon에 이 define을 만든 이유는 서버와 클라에 동시에 적용되도록 하기 위함입니다


#if defined(_DEBUG) || defined(_RELEASE) || defined(LOCALE_KOREA) || defined(LOCALE_NHNUSA)// || defined(LOCALE_JAPAN) || defined(LOCALE_BRAZIL)
	#define _AVATAR_ENABLE	// 아바타 지원 여부
#endif

//#if defined(_DEBUG) || defined(_RELEASE) || defined(LOCALE_KOREA)
#if 0
#	define _SELL_CASHITEM	// 캐쉬 아이템 되팔기 기능
#endif
