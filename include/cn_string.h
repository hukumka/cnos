#pragma once

#ifndef CN_STRING
#define CN_STRING
static inline int strcmp(const char *a, const char *b){
	int i;
	for(i=0; a[i]==b[i] && a[i]!=0; ++i)
		;
	return a[i]-b[i];
}

static inline int strcmpl(const char *a, const char *b, int len){
	int i;
	for(i=0; a[i]==b[i] && a[i]!=0 && i<len; ++i)
		;
	if(i==len)
		return 0;
	return a[i]-b[i];
}
#endif
