// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
#define MaxFileLength 32

char *User2System(int virtAddr, int limit=-1)
{
	int i;
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

void System2User(int virtAddr,char* buffer, int len = -1)
{
	int length = (len == -1 ? strlen(buffer) : len);
    for (int i = 0; i < length; i++) {
        kernel->machine->WriteMem(virtAddr + i, 1,
                                  buffer[i]);  // copy characters to user space
    }
    kernel->machine->WriteMem(virtAddr + length, 1, '\0');
} 

void moveProgramCounter()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			moveProgramCounter();
			return;

		case SC_Create:
			int virtAddr;
			virtAddr = kernel->machine->ReadRegister(4);
			char *filename;
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (SysCreateFile(filename)){
				kernel->machine->WriteRegister(2, 0);
			}
			else{
				kernel->machine->WriteRegister(2, -1);
			}
			delete filename;
			moveProgramCounter();
			return;
		
		case SC_Open:
			int virtAddr1;
			virtAddr1 = kernel->machine->ReadRegister(4);
			char *filename1;
			filename1 = User2System(virtAddr1, MaxFileLength + 1);
			int type;
			type = kernel->machine->ReadRegister(5);

			kernel->machine->WriteRegister(2, SysOpen(filename1, type));

			delete filename1;
			moveProgramCounter();
			return;

		case SC_Close:
			int id;
			id = kernel->machine->ReadRegister(4);
    		kernel->machine->WriteRegister(2, SysClose(id));
			moveProgramCounter();
			return;

		case SC_Remove:
			int virtAddrRemove;
			virtAddrRemove = kernel->machine->ReadRegister(4);
			char *fileNameRemove;
			fileNameRemove = User2System(virtAddrRemove, MaxFileLength + 1);
			kernel->machine->WriteRegister(2, SysRemove(fileNameRemove));
			delete fileNameRemove;
			moveProgramCounter();
			return;
		
		case SC_PrintString:
			int memPtr; 
			memPtr = kernel->machine->ReadRegister(4);  // read address of C-string
			char* buffer;
			buffer = User2System(memPtr);

			SysPrintString(buffer, strlen(buffer));
			delete[] buffer;
			return moveProgramCounter();

		case SC_Read:
			int virtAddrRead	;
			virtAddrRead = kernel->machine->ReadRegister(4);
			int charCountRead;
			charCountRead = kernel->machine->ReadRegister(5);
			char* bufferRead;
			bufferRead = User2System(virtAddrRead, charCountRead);
			int fileIdRead;
			fileIdRead = kernel->machine->ReadRegister(6);
			kernel->machine->WriteRegister(2, SysRead(bufferRead, charCountRead, fileIdRead));
			System2User(virtAddrRead, bufferRead, charCountRead);
			delete[] buffer;
			return moveProgramCounter();
		


		case SC_Write:
			int virtAddrWrite;
			virtAddrWrite = kernel->machine->ReadRegister(4);
			int charCountWrite ;
			charCountWrite = kernel->machine->ReadRegister(5);
			char* bufferWrite;
			bufferWrite = User2System(virtAddrWrite, charCountWrite);
			int fileIdWrite;
			fileIdWrite = kernel->machine->ReadRegister(6);
			kernel->machine->WriteRegister(2, SysWrite(bufferWrite, charCountWrite, fileIdWrite));
			System2User(virtAddrWrite,bufferWrite, charCountWrite );
			delete[] buffer;
			return moveProgramCounter();
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}