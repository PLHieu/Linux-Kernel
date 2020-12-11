#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

int main(){
    int ret, fd;
    int value;
    
    fd = open("/dev/mynull", O_RDONLY); //Open file device
    if (fd < 0){
        perror("Failed to open file\n");
        return errno;
    }

    //Read number in mynull
    ret = read(fd, &value, sizeof(value));
    if (ret < 0){
        perror("Failed to read random number\n");
        return errno;
    }

    printf("Random number is %d", value);
    
    return 0;
}
