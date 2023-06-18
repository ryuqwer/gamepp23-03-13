#include "util.h"

void CHECKERROR(int e)
{
	if (e == 1)
	{
		printf("[line:%d, %s] 에러가 발생하였습니다.\n", __LINE__, __func__);
		exit(1);
	}
}