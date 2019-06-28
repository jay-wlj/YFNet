#pragma once

#define BUFSIZE     1024*4

class CRC32
{
public:
	CRC32()
	{
		init_crc_table();
	}

	static CRC32 &Instance()
	{
		static CRC32 crc32 ;

		return crc32 ;
	}

public:
	/*
	**初始化crc表,生成32位大小的crc表
	**也可以直接定义出crc表,直接查表,
	**但总共有256个,看着眼花,用生成的比较方便.
	*/
	void init_crc_table(void)
	{
		unsigned int c;
		unsigned int i, j;
		
		for (i = 0; i < 256; i++) 
		{
			c = (unsigned int)i;

			for (j = 0; j < 8; j++) 
			{
				if (c & 1)
					c = 0xedb88320L ^ (c >> 1);
				else
					c = c >> 1;
			}

			crc_table[i] = c;
		}
	}

	/*计算buffer的crc校验码*/
	unsigned int crc32(unsigned int crc,unsigned char *buffer, unsigned int size)
	{
		unsigned int i;
		for (i = 0; i < size; i++) 
		{
			crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
		}
		return ~crc ;
	}

	/*
	**计算大文件的CRC校验码:crc32函数,是对一个buffer进行处理,
	**但如果一个文件相对较大,显然不能直接读取到内存当中
	**所以只能将文件分段读取出来进行crc校验,
	**然后循环将上一次的crc校验码再传递给新的buffer校验函数,
	**到最后，生成的crc校验码就是该文件的crc校验码.(经过测试)
	*/
	int calc_img_crc(const char *in_file, unsigned int *img_crc)
	{
		int nread;
		int ret;
		unsigned char buf[BUFSIZE];
		/*第一次传入的值需要固定,如果发送端使用该值计算crc校验码,
		**那么接收端也同样需要使用该值进行计算*/
		unsigned int crc = 0xffffffff; 

		FILE *fd = fopen(in_file, "r");
		if (fd < 0) 
		{
			return -1;
		}
			
		while ( (nread = fread(buf, 1, BUFSIZE, fd)) > 0) 
		{
			crc = crc32(crc, buf, nread);
		}

		*img_crc = crc;

		fclose(fd);
		
		if (nread < 0) 
		{
			return -1;
		}
		
		return 0;
	}

private:
	unsigned int crc_table[256] ;
};