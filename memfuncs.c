#include<stddef.h>
#include<stdint.h>

void *memcpy(void *dest, const void *src, size_t len){
    
    char *d = dest;
    const char *s = src;

    while (len--)
        *d++ = *s++;
    
    return dest;
}

void *memmove(void *dest, const void *src, size_t len){

    char *d = dest;
    const char *s = src;

    //Si chevauchement pas grave car le début de src aura déjà été recopié dans dest
    if(d < s){
        while(len--)
            *d++ = *s++;
    }

    else{
        const char *endsrc = s + (len-1);
        char *enddest = d + (len-1);

        while(len--)
            *enddest++ = *endsrc++;
    }

    return dest;
}

void *memset(void *dest, int val, size_t len){

    unsigned char *ptr = dest;

    while(len-- > 0){
        *ptr++ = val;
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t len){

    const char *s = s1;
    const char *c = s2;

    for(uint8_t i=0; i<len; i++){
        int diff = *(s+i) - *(c+i);

        if(diff != 0)
            return diff;
    }
    
    return 0;
}

