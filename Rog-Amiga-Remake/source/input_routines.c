/*

*/

#include "includes.prl"
#include <devices/keyboard.h>

extern struct Library *SysBase;

/* Keyboard device */
struct MsgPort  *KeyMP;         /* Pointer for Message Port */
struct IOStdReq *KeyIO;         /* Pointer for I/O request */
UBYTE    *keyMatrix;

#define KEY_MATRIX_SIZE 16

int InitKeyboardDevice(void)
{
    if (KeyMP=CreatePort(NULL,NULL))
      if (KeyIO=(struct IOStdReq *)CreateExtIO(KeyMP,sizeof(struct IOStdReq)))
        if (OpenDevice( "keyboard.device",NULL,(struct IORequest *)KeyIO,NULL))
        {
          printf("keyboard.device did not open\n");
          return(0);
        }
        else
        if (!(keyMatrix = AllocMem(KEY_MATRIX_SIZE,MEMF_PUBLIC|MEMF_CLEAR)))
        {
          printf("Cannot allocate keyboard buffer\n");
          return(0);
        }
}

void  CloseKeyboardDevice(void)
{
	/*  Close the keyboard device */
	if (!(CheckIO((struct IORequest *)KeyIO)))
		AbortIO((struct IORequest *)KeyIO);   //  Ask device to abort request, if pending 
	// WaitIO((struct IORequest *)KeyIO);   /* Wait for abort, then clean up */
	CloseDevice((struct IORequest *)KeyIO);
	FreeMem(keyMatrix,KEY_MATRIX_SIZE);
}

void KeyboardAbortCheck(void (*quit_callback)(void))
{
	KeyIO->io_Command=KBD_READMATRIX;
	KeyIO->io_Data=(APTR)keyMatrix;
	KeyIO->io_Length = SysBase->lib_Version >= 36 ? KEY_MATRIX_SIZE : 13;
	DoIO((struct IORequest *)KeyIO);

	//   printf("%i", (int)(keyMatrix[0x45/8] & (0x20)));
	if (keyMatrix[0x45 >> 3] & (0x20))
	{
		CloseKeyboardDevice();
		(*quit_callback)();
	}

	// ModuleGetNormalizedPosition();
}