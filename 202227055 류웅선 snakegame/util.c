#include "util.h"

void CHECKERROR(int e)
{
	if (e == 1)
	{
		printf("[line:%d, %s] ������ �߻��Ͽ����ϴ�.\n", __LINE__, __func__);
		exit(1);
	}
}