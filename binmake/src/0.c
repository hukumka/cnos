#ifdef CNOS
	#include <macros.h>
	#include <apilib.h>
#else
	#include <stdlib.h>
#endif

int pow(int,int,int);

int abs;

int main(){
	int a = pow(123, 314, 257);
	printf("%d\n",a);
	#ifdef CNOS
		_exit(a);
	#endif
}

int pow(int a,int n, int mod){
	a %= mod;
	if( n == 0 )
		return a % mod;
	int row = pow( (a*a)%mod, n/2, mod);
	if( n%2 )
		return (a*row)%mod;
	return row%mod;
};
