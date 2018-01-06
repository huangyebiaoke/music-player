#include <stdio.h>   
#include <stdlib.h>   
#include <memory.h>  
#include <string.h>  
  
int ReadAIPCFromMP3(const char* path);  
  
int main(int argc, char **argv)  
{  
    ReadAIPCFromMP3("music/Aash Mehta - Clsr (Aash Mehta Flip).mp3");  
    return 0;   
}  
  
bool isFrameAPIC(const char* cID3V2Fra_head)  
{  
    // 在win7中vs2012下，没发现strncasecmp函数，不然不需要两次判断  
    // 当然了，这种判断也可以逐字节判断  
    if ((strncmp(cID3V2Fra_head, "APIC", 4) == 0) ||  
        (strncmp(cID3V2Fra_head, "apic", 4) == 0))   
        return true;  
    return false;  
}  
  
int calcID3V2Len(const char* cID3V2_head)  
{  
    int len = (cID3V2_head[6] & 0x7f) << 21  
                | (cID3V2_head[7] & 0x7f) << 14  
                | (cID3V2_head[8] & 0x7f) << 7  
                | (cID3V2_head[9] & 0x7f);  
    return len;  
}  
  
int calclID3V2FraLength(const char* cID3V2Fra_head)  
{  
    int len = (int)(cID3V2Fra_head[4] * 0x100000000 +  
            cID3V2Fra_head[5] * 0x10000 +  
            cID3V2Fra_head[6] * 0x100 +  
            cID3V2Fra_head[7]);  
    return len;  
}  
  
bool isJPEG(const char* data)  
{  
    // JPEG格式数据的起始为0xFFD8，当然了，结束也有标志，但是这里不需要了  
    if (((unsigned char)data[0] == 0xFF) &&  
        ((unsigned char)data[1] == 0xD8))  
        return true;  
    return false;  
}  
  
int ReadAIPCFromMP3(const char* path)  
{  
    FILE* fp = fopen(path, "rb");  
    if (!fp)  
    {  
        printf("cannot open this mp3\n");  
        return -1;  
    }  
  
    // 这里的ID3V2以及帧标识的大小由标准规定均为10个字节  
    // 这里其实应该是作为字节存储，用unsigned char更好，这里就简单用char替代吧  
    char  cID3V2_head[10] = {0}, cID3V2Fra_head[10] = {0};  
    long  ID3V2_len = 0, lID3V2Fra_length = 0;  
    char* cID3V2Fra = NULL;  
  
    // 读取帧头，这里就是为了判断是否是ID3V2的标签头  
    fread(cID3V2_head, 10, 1, fp);  
    if ((cID3V2_head[0] == 'I' || cID3V2_head[0] == 'i') &&  
        (cID3V2_head[1] == 'D' || cID3V2_head[1] == 'd') &&  
        cID3V2_head[2] == '3')  
    {  
        // 获取ID3V2标签的长度  
        ID3V2_len = calcID3V2Len(cID3V2_head);  
    }  
  
    bool hasAPIC = false;  
    // 这里来逐个读取帧标识，这里的专辑图片即保存在APIC标识里  
    while ((ftell(fp) + 10) <= ID3V2_len)  
    {  
        // 这里每个帧标识的长度也为10，由于每个帧标识的存储的数据的长度不一  
        // 每次要读取出来，进行运算获取真正数据长度  
        memset(cID3V2Fra_head, 0, 10);  
        fread(cID3V2Fra_head, 10, 1, fp);  
        lID3V2Fra_length = (cID3V2Fra_head[4] * 0x100000000 +  
            cID3V2Fra_head[5] * 0x10000 +  
            cID3V2Fra_head[6] * 0x100 +  
            cID3V2Fra_head[7]);  
          
        // 这里判断是否为专辑图片的帧标识  
        if (isFrameAPIC(cID3V2Fra_head))  
        {  
            cID3V2Fra = (char*)calloc(lID3V2Fra_length, 1);  
            if (cID3V2Fra != NULL)  
            {  
                hasAPIC = true;  
                fread(cID3V2Fra, lID3V2Fra_length, 1, fp);  
            }  
            break;  
        }  
        else  
        {  
            // 移动到下一帧标识  
            fseek(fp, lID3V2Fra_length, SEEK_CUR);  
        }  
    }  
  
    fclose(fp);  
  
    // 到这里如果，专辑图片要么读出，要么就不存在  
    if (hasAPIC)  
    {
        // 这里整个数据的前面一部分数据是用来记录专辑图片的格式  
        // 例如 image/jpeg image/png等，这里大部分的专辑图片都是jpeg的  
        // 因此这里简单的只判断jpeg的格式，除去图片格式，数据前部依然有些是空数据  
        // 因此以jpeg的标识来定位图片数据的起始  
        int start = 0;  
        while (start < lID3V2Fra_length)  
        {  
            if (isJPEG(cID3V2Fra + start))  
                break;  
            ++start;
        }
  
        // 是以JPEG格式存在，则存储为jpeg的文件  
        if (start != lID3V2Fra_length)  
        {  
            // 这里没有错误处理，从简  
            FILE* jpegFP = fopen("test.jpeg", "wb");  
            fwrite(cID3V2Fra + start, lID3V2Fra_length - start, 1, jpegFP);  
            fclose(jpegFP);  
        }
    }  
    return 0;  
}
