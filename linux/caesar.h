// caesar .h
# ifndef CAESAR_H
# define CAESAR_H
# include <stddef.h>
# include <stdint.h>

size_t caesar_encrypt_bytes ( const char * plaintext , uint8_t shift ,uint8_t * out , size_t out_cap ) ;
/* out [0] = shift ; returns total bytes written ( including first shift byte ) */

size_t caesar_decrypt_bytes ( const uint8_t * in , size_t in_len ,char * out , size_t out_cap ) ;
/* reads in [0] as shift ; writes NUL - terminated plaintext if space permits */

# endif // CAESAR_H