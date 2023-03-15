#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main(int argc, char* argv[])
{
<<<<<<< HEAD
    int len;
    char filename[maxlen + 1];
    /*Create a file*/
    if (Create("text.txt") == -1)
    {
        // xuất thông báo lỗi tạo tập tin
    }
    else
    {
        PrintString("Create file successfully");
    }
    Halt();
=======
    // // int len;
    // // char filename[maxlen + 1];
    // /*Create a file*/
    // if ( == -1)
    // {
    //     // xuất thông báo lỗi tạo tập tin
    //     // print("\nCreate file ~");
    //     // print(filename);
    //     // print(" fail.");
    // }
    // else
    // {
    //     // xuất thông báo tạo tập tin thành công
    //     // print("\nCreate file ~");
    //     // print(filename);
    //     // print(" success.~"); 
    // }
    Halt(); 
>>>>>>> cd1f085e292de6aa7cbbac8547cc270c2441b4df
}