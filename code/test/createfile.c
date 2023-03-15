#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main(int argc, char* argv[])
{
    int len;
    char filename[maxlen + 1];
    int id = Open("text.txt",0);
    if (id == -1)
    {
        PrintString("khoi mo that bai");
    }
    else 
    {

   

    Read(filename,maxlen,id);
    PrintString(filename);
    }
    /*Create a file*/
    // if (Create("text.txt") == -1)
    // {
    //     // xuất thông báo lỗi tạo tập tin
    //     PrintString("Create fail\n");

    // }
    // else
    // {
    //     PrintString("Create file successfully\n");
    // }
     
    Halt();
}