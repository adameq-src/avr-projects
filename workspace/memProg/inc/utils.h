#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
	extern "C" {
#endif

void UsartInit(uint16_t brr);
void Tx(uint8_t ch);
uint8_t Rx(void);
void NewLine(void);
void TxString(uint8_t *string);
void println(uint8_t *string2);

#ifdef __cplusplus
}
#endif
#endif // __UTILS_H
