/*

*/

#include <exec/types.h>

/* Keyboard device */
extern struct MsgPort  *KeyMP;         /* Pointer for Message Port */
extern struct IOStdReq *KeyIO;         /* Pointer for I/O request */
extern UBYTE    *keyMatrix;

int InitKeyboardDevice(void);
void KeyboardAbortCheck(void *quit_callback);
void  CloseKeyboardDevice(void);