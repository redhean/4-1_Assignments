#include <stdio.h>

int main()
{
	int a, b;
	scanf("%d %d", &a, &b);

	printf("%d %d\n", plus(a,b), minus(a,b));

	return 0;
}
