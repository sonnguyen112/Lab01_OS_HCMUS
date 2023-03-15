#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main(int argc, char* argv[])
{
    // int stdin;
    // char fileName[maxlen + 1];
    // stdin = Open("stdin", 2);
    // if (stdin != -1)
	// {
	// 	// int len = Read(fileName, MAX_LENGTH, stdin); // Goi ham Read doc ten file vua nhap
    //     int len = 5;
		
	// 	if(len <= 1)
	// 		PrintString(" -> Ten file khong hop le!!!\n\n");
	// 	else
	// 	{
	// 		if (CreateFile(fileName) == 0) // Goi ham CreateFile
	// 		{
	// 			PrintString(" -> Create file thanh cong.\n\n");
	// 		}
	// 	}
	// 	Close(stdin); // Goi ham Close de dong stdin
	// }
    /*Create a file*/
    if (Create("text.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
        PrintString("Create fail\n");

    }
    else
    {
        PrintString("Create file successfully\n");
    }
    Halt();
}