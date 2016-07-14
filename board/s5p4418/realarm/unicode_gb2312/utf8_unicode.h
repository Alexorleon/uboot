#ifndef UTF8_UNICODE_H
#define UTF8_UNICODE_H

int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,  
        int outSize);
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned char *Unic);

#endif
