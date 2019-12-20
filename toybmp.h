#ifndef TOYBMP_H_
#define TOYBMP_H_

#ifndef TOYBMP_LINKAGE
#define TOYBMP_LINKAGE //Empty by default, change for static or inline
#endif

#include <stdio.h>
#ifndef TOYBMP_OUTPUT
#define TOYBMP_OUTPUT FILE* fp
#endif

#ifndef TOYBMP_PUT
#define TOYBMP_PUT(u) fputc(u,fp)
#endif

TOYBMP_LINKAGE void toybmp(TOYBMP_OUTPUT, unsigned w, unsigned h, const unsigned char* img, int alpha) {
	unsigned b = ((w * (alpha ? 32 : 24) + 7) >> 3);
	unsigned ROWsize = (b + 3) & (~3);
	unsigned ImageDataSize = ROWsize * h + 54;
	unsigned p = ROWsize - b, i, x, y;	/* pad */
#define TOYBMP_FILE_HEAD(u,l) for(i = 0; i < l; ++i)TOYBMP_PUT((u)[i])
#define TOYBMP_PAD_ZERO(u) for(i=0;i<u;++i)TOYBMP_PUT(0)
#define TOYBMP_U32B(u) do{TOYBMP_PUT((u)&255);TOYBMP_PUT(((u)>>8)&255);TOYBMP_PUT(((u)>>16)&255);TOYBMP_PUT((u)>>24);}while(0)
#define TOYBMP_BGR(u) do{TOYBMP_PUT(*(img + x + 2));TOYBMP_PUT(*(img + 1 + x));TOYBMP_PUT(*(img + x));}while(0)
#define TOYBMP_C24B(u,ua) do{for(y=0;y<h;++y){img -= u;for(x = 0; x < u; x+=3){TOYBMP_BGR(x);}TOYBMP_PAD_ZERO(ua);}}while(0)
#define TOYBMP_C32B(u,ua) do{for(y=0;y<h;++y){img -= u;for(x = 0; x < u; x+=4){TOYBMP_BGR(x);TOYBMP_PUT(*(img + x + 3));}TOYBMP_PAD_ZERO(ua);}}while(0)
	TOYBMP_FILE_HEAD("BM", 2);											/*BitMap File Header*/
	TOYBMP_U32B(ImageDataSize);
	TOYBMP_PAD_ZERO(4);
	TOYBMP_U32B(0x36);													/*bfOffBit*/
																		/*BitMap INFO Header*/
	TOYBMP_U32B(0x28);													/*Size of Info Header*/
	TOYBMP_U32B(w); TOYBMP_U32B(h);										/*Width and Height*/
	TOYBMP_PUT(1); TOYBMP_PUT(0);										/*biPlanes*/
	TOYBMP_PUT(alpha?0x20:0x18); TOYBMP_PUT(0);							/*BitCount*/
	TOYBMP_PAD_ZERO(4);
	TOYBMP_U32B(ROWsize*h);
	TOYBMP_PAD_ZERO(16);
	if (alpha) TOYBMP_C32B(b,p);									/*BitMap Data*/
	else TOYBMP_C24B(b,p);
}
#endif TOYBMP_H_ /* TOYBMP_H_ */