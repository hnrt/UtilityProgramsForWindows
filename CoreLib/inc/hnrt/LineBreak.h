#pragma once

#ifdef CRLF
#undef CRLF
#endif
#ifdef LF
#undef LF
#endif
#ifdef DONOTCARE
#undef DONOTCARE
#endif

namespace hnrt
{
	enum LineBreak
	{
		CRLF = 0x0d0a,
		LF = 0x0a,
		DONOTCARE = 0
	};
}
