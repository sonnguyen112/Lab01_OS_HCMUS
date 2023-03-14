/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

bool SysCreateFile(char *fileName)
{
  if (strlen(fileName) == 0)
  {
    DEBUG(dbgSys, "\nFile name can't be empty");
    return false;
  }
  else if (fileName == NULL)
  {
    DEBUG(dbgSys, "\nNot enough memory in system");
    return false;
  }
  else
  {
    DEBUG(dbgSys, "\nFile's name read successfully");
    if (!kernel->fileSystem->Create(fileName))
    {
      DEBUG(dbgSys, "\nError creating file");
      return false;
    }
  }
  return true;
}

int SysOpen(char* fileName, int type) {
    if (type != 0 && type != 1) return -1;

    int id = kernel->fileSystem->Open(fileName, type);
    if (id == -1) return -1;
    DEBUG(dbgSys, "\nOpened file");
    return id;
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

#endif /* ! __USERPROG_KSYSCALL_H__ */
