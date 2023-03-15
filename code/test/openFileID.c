#include "syscall.h"
#include "copyright.h"

int main()
{
	// OpenFileId fileId;
	// int filesz, i;
	// char c;
	// char name[255];
	// // PrintString("Input file name:");
	// // ReadString(name, 255);
	// if ( (fileId= Open(name)) == -1)
	// {
	// 	// PrintString("Can not open file ");
	// 	// return 0;
	// }
	// filesz = SeekFile(-1, fileId);
	// i = 0;
	// SeekFile(0, fileId);
	
	// for (; i < filesz; ++i)
	// {
	// 	// Read(&c, 1, fileId);
	// 	// PrintChar(c);
	// } 
	// Close(fileId);
	// Halt();
	int len;
    char filename[maxlen + 1];
    /*Create a file*/
    if (Create("text.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
        // print("\nCreate file ~");
        // print(filename);
        // print(" fail.");
    }
    else
    {
        // xuất thông báo tạo tập tin thành công
        // print("\nCreate file ~");
        // print(filename);
        // print(" success.~"); 
    }
    Halt(); 
}