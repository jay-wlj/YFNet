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
	**��ʼ��crc��,����32λ��С��crc��
	**Ҳ����ֱ�Ӷ����crc��,ֱ�Ӳ��,
	**���ܹ���256��,�����ۻ�,�����ɵıȽϷ���.
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

	/*����buffer��crcУ����*/
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
	**������ļ���CRCУ����:crc32����,�Ƕ�һ��buffer���д���,
	**�����һ���ļ���Խϴ�,��Ȼ����ֱ�Ӷ�ȡ���ڴ浱��
	**����ֻ�ܽ��ļ��ֶζ�ȡ��������crcУ��,
	**Ȼ��ѭ������һ�ε�crcУ�����ٴ��ݸ��µ�bufferУ�麯��,
	**��������ɵ�crcУ������Ǹ��ļ���crcУ����.(��������)
	*/
	int calc_img_crc(const char *in_file, unsigned int *img_crc)
	{
		int nread;
		int ret;
		unsigned char buf[BUFSIZE];
		/*��һ�δ����ֵ��Ҫ�̶�,������Ͷ�ʹ�ø�ֵ����crcУ����,
		**��ô���ն�Ҳͬ����Ҫʹ�ø�ֵ���м���*/
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