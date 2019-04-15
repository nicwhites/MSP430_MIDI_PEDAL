

#include "typedef.h"

void wait_ms(uint m_sec)
{
	while(m_sec--)
		__delay_cycles(16000);	// 1000 for 1MHz
}

void wait_us(uint u_sec)
{
	while(u_sec--)
		__delay_cycles(16);		// 1 for 1MHz
}
