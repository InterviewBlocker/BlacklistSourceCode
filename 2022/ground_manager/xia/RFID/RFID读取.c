#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
//添加串口头文件
#include <termios.h>
#include <unistd.h>

int init_tty(int fd)
{
	struct termios new_flags;
	bzero(&new_flags,sizeof(new_flags));
	
	//2. 设置原始模式
	cfmakeraw(&new_flags);
	
	//3. 激活本地连接CLOCAL与接收使能CREAD的选项
	new_flags.c_cflag |= CLOCAL | CREAD; 
	
	//4. 设置波特率
	cfsetispeed(&new_flags, B9600); 
	cfsetospeed(&new_flags, B9600);
	
	//5. 设置数据位为8位
	new_flags.c_cflag &= ~CSIZE; //清空原有的数据位
	new_flags.c_cflag |= CS8;
	
	//6. 设置奇偶检验位
	new_flags.c_cflag &= ~PARENB;
	
	//7. 设置一位停止位
	new_flags.c_cflag &= ~CSTOPB;
	
	//8. 设置等待时间，最少接收字符个数
	new_flags.c_cc[VTIME] = 0;
	new_flags.c_cc[VMIN] = 1;
	
	//9. 清空串口缓冲区
	tcflush(fd, TCIFLUSH);
	
	//10. 设置串口的属性到文件中
	tcsetattr(fd, TCSANOW, &new_flags);
	
	return 0;
}

int main()
{
    //1.打开串口设备 
    int fd=open("/dev/ttySAC2",O_RDWR|O_NOCTTY);
    if(fd < 0)
    {
        perror("打开串口失败\n");
    }else 
    {
        printf("打开串口成功\n");
    }

     int fd1=open("./pipe",O_RDWR);
    if(fd1 < 0)
        {
            perror("打开管道文件失败\n");
            return -1;
        }else 
        {
            printf("打开管道文件成功");
        }

    //2.初始化串口 
    init_tty(fd);

    //1.发送请求命令  
    char  Request[8]; 
    Request[0]=0x07; //设置帧长
    Request[1]=0x02; //设置类型
    Request[2]=0x41; //命令
    Request[3]=0x01; //信息长度
    Request[4]=0x52; //信息
    //计算校验和 
    int BCC = 0;
    int i=0; 
    for(i=0; i<(Request[0]-2); i++) { 
    BCC ^= Request[i]; 
    } 
    Request[5]=~BCC; //BCC  校验和 
    Request[6]=0X03;

    write(fd,Request,0x07);

    //等待从机应答 
    char ret[9]; 
    read(fd,ret,9); 
    if(ret[2]==0) //判断状态是否为 0 
    {     
        printf("请求成功\n");
    }else 
    {
        printf("请求失败\n");
        return -1;
    }
   
    Request[0]=0x08; //设置帧长
    Request[1]=0x02; //设置类型
    Request[2]=0x42; //命令
    Request[3]=0x02; //信息长度
    Request[4]=0x93; //信息
    Request[5]=0x00; //信息
    //计算校验和 
    BCC = 0; 
    for(int i=0; i<(Request[0]-2); i++) { 
    BCC ^= Request[i]; 
    } 
    Request[6]=~BCC; //BCC  校验和 
    Request[7]=0X03;

    write(fd,Request,0x08);

    char res[10];
    char res1[20]={0};  
    read(fd,res,10); 
    if(res[2]==0) //判断状态是否为 0 
    {     
         
        //printf("防碰撞成功\n");
        for(int i=7,j=0; i>3; i--)
        {
            sprintf(res1,"%s%x",res1,res[i]);
            
        }

        printf("%s\n",res1);
        
        
        
        if(strcmp(res1,"adff68c3")==0)
        {   
            
            write(fd1,"ok1",strlen("ok1"));
            sleep(1);
            printf("///////////////////////////////");
            write(fd1,"鲁N·E23A1",strlen("鲁N·E23A1"));
            printf("///////////////////////////////2");
        }
        else  if(strcmp(res1,"3e8944e9")==0)
        {   
            write(fd1,"ok1",strlen("ok1"));
            sleep(1);
            printf("///////////////////////////////");
            write(fd1,"冀F·Q019T",strlen("冀F·Q019T"));
            printf("///////////////////////////////1");
        }
       

    }else 
    {
        printf("防碰撞失败\n");
        return -1;
    }
}