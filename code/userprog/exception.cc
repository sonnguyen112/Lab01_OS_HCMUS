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

char *User2System(int virtAddr, int limit)
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
			int virtAddr;
			virtAddr = kernel->machine->ReadRegister(4);
			int type = kernel->machine->ReadRegister(5);
			char *buf = new char[MaxFileLength+1];
			if (kernel->fileSystem->Open())
			{
				machine->WriteRegister(2, -1);
				delete[] buf;
				break;
			}
			buf = User2System(virtAddr, MaxFileLength+1);
			if (strcmp(buf,"stdin") == 0)
			{
				printf("stdin mode\n");
				machine->WriteRegister(2, 0);
				break;
			}
			if (strcmp(buf,"stdout") == 0)
			{
				printf("stdout mode\n");
				machine->WriteRegister(2, 1);
				break;
			}
			if ((fileSystem->openf[fileSystem->index] = fileSystem->Open(buf, type)) != NULL)
			{
				DEBUG('f',"open file successfully");
				machine->WriteRegister(2, fileSystem->index-1);
			} else 
			{
				DEBUG('f',"can not open file");
				machine->WriteRegister(2, -1);
			};
		case SC_Close:
			int m_index = machine->ReadRegister(4);
						if (fileSystem->openf[m_index] == NULL) break;
						delete fileSystem->openf[m_index];
						fileSystem->openf[m_index] = NULL;
						break;
			delete [] buf;
			break;
		case SC_Seek:
				{
					int pos = machine->ReadRegister(4);
					int m_index = machine->ReadRegister(5);
					if (m_index < 0 || m_index > 10)
					{
						machine->WriteRegister(2, -1);
						break;
					}
					// check openf[m_index]
					if (fileSystem->openf[m_index] == NULL)
					{
						printf("seek fail \n");
						machine->WriteRegister(2, -1);
						break;
					}
						pos = (pos == -1) ? fileSystem->openf[m_index]->Length() : pos;
					if (pos > fileSystem->openf[m_index]->Length() || pos < 0) {
						machine->WriteRegister(2, -1);
					} else 
					{
						fileSystem->openf[m_index]->Seek(pos);
						machine->WriteRegister(2, pos);
					}
					break;
				}
				
		}
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