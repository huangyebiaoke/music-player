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
    // ��win7��vs2012�£�û����strncasecmp��������Ȼ����Ҫ�����ж�  
    // ��Ȼ�ˣ������ж�Ҳ�������ֽ��ж�  
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
    // JPEG��ʽ���ݵ���ʼΪ0xFFD8����Ȼ�ˣ�����Ҳ�б�־���������ﲻ��Ҫ��  
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
  
    // �����ID3V2�Լ�֡��ʶ�Ĵ�С�ɱ�׼�涨��Ϊ10���ֽ�  
    // ������ʵӦ������Ϊ�ֽڴ洢����unsigned char���ã�����ͼ���char�����  
    char  cID3V2_head[10] = {0}, cID3V2Fra_head[10] = {0};  
    long  ID3V2_len = 0, lID3V2Fra_length = 0;  
    char* cID3V2Fra = NULL;  
  
    // ��ȡ֡ͷ���������Ϊ���ж��Ƿ���ID3V2�ı�ǩͷ  
    fread(cID3V2_head, 10, 1, fp);  
    if ((cID3V2_head[0] == 'I' || cID3V2_head[0] == 'i') &&  
        (cID3V2_head[1] == 'D' || cID3V2_head[1] == 'd') &&  
        cID3V2_head[2] == '3')  
    {  
        // ��ȡID3V2��ǩ�ĳ���  
        ID3V2_len = calcID3V2Len(cID3V2_head);  
    }  
  
    bool hasAPIC = false;  
    // �����������ȡ֡��ʶ�������ר��ͼƬ��������APIC��ʶ��  
    while ((ftell(fp) + 10) <= ID3V2_len)  
    {  
        // ����ÿ��֡��ʶ�ĳ���ҲΪ10������ÿ��֡��ʶ�Ĵ洢�����ݵĳ��Ȳ�һ  
        // ÿ��Ҫ��ȡ���������������ȡ�������ݳ���  
        memset(cID3V2Fra_head, 0, 10);  
        fread(cID3V2Fra_head, 10, 1, fp);  
        lID3V2Fra_length = (cID3V2Fra_head[4] * 0x100000000 +  
            cID3V2Fra_head[5] * 0x10000 +  
            cID3V2Fra_head[6] * 0x100 +  
            cID3V2Fra_head[7]);  
          
        // �����ж��Ƿ�Ϊר��ͼƬ��֡��ʶ  
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
            // �ƶ�����һ֡��ʶ  
            fseek(fp, lID3V2Fra_length, SEEK_CUR);  
        }  
    }  
  
    fclose(fp);  
  
    // �����������ר��ͼƬҪô������Ҫô�Ͳ�����  
    if (hasAPIC)  
    {
        // �����������ݵ�ǰ��һ����������������¼ר��ͼƬ�ĸ�ʽ  
        // ���� image/jpeg image/png�ȣ�����󲿷ֵ�ר��ͼƬ����jpeg��  
        // �������򵥵�ֻ�ж�jpeg�ĸ�ʽ����ȥͼƬ��ʽ������ǰ����Ȼ��Щ�ǿ�����  
        // �����jpeg�ı�ʶ����λͼƬ���ݵ���ʼ  
        int start = 0;  
        while (start < lID3V2Fra_length)  
        {  
            if (isJPEG(cID3V2Fra + start))  
                break;  
            ++start;
        }
  
        // ����JPEG��ʽ���ڣ���洢Ϊjpeg���ļ�  
        if (start != lID3V2Fra_length)  
        {  
            // ����û�д������Ӽ�  
            FILE* jpegFP = fopen("test.jpeg", "wb");  
            fwrite(cID3V2Fra + start, lID3V2Fra_length - start, 1, jpegFP);  
            fclose(jpegFP);  
        }
    }  
    return 0;  
}
