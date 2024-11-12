#include <stdio.h>
#include <stdlib.h>
#include <string.h>	 //需用到strcpy()
#define MAX_SIZE 100 //文件名长度
#define ERROR -1
#define OK 1

typedef struct
{
	unsigned int weight; //字符权重
	unsigned int parent, lchild, rchild;
} HTNode, *HuffmanTree;		//动态分配数组存储霍夫曼树
typedef char **HuffmanCode; //动态分配数组存储霍夫曼编码表

void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC, unsigned int *w, int n);
void Compress(char in_file[], char out_file[]);
void Decompress(char in_file[]);

void Compress(char in_file[], char out_file[])
{
	FILE *in, *out; //in为待压缩文件指针，out为压缩后文件指针
	in = fopen(in_file, "rb");
	if (in == NULL)
	{
		printf("文件不存在！");
	}
	else
	{
		fseek(in, 0L, SEEK_END);													//将文件指针移到末尾
		unsigned long long totalsize = ftell(in);									//记录文件总字节数
		fseek(in, 0L, SEEK_SET);													//将指针移到开头
		unsigned int *Fweight = (unsigned int *)malloc(256 * sizeof(unsigned int)); //记录256个字符是否存在及权值
		for (int i = 0; i < 256; i++)
			Fweight[i] = 0;								   //初始化
		for (unsigned long long i = 0; i < totalsize; i++) //将文件的全部字符一个个读取出来
			Fweight[(unsigned char)fgetc(in)]++;		   //char转换为unsigned
		fclose(in);
		int kind = 0; //字符种数
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
				kind++;
		unsigned char *CharSet = (unsigned char *)malloc(kind * sizeof(unsigned char)); //记录文件含有的字符
		unsigned int *WeightSet = (unsigned int *)malloc(kind * sizeof(unsigned int));	//记录字符的权值
		if (CharSet == NULL || WeightSet == NULL)
			exit(ERROR);
		int j = 0; //最后的结果j=kind-1
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
			{
				CharSet[j] = i; 
				WeightSet[j] = Fweight[i];
				j++;
			}
		HuffmanTree HT;
		HuffmanCode HC;
		HuffmanCoding(HT, HC, WeightSet, kind); //编写霍夫曼树和霍夫曼编码

		int name = 0;
		int totalsizen = strlen(in_file);
		for (int i = 0; i < 256; i++)
			Fweight[i] = 0;									//初始化
		for (unsigned long long i = 0; i < totalsizen; i++) //将文件名的全部字符一个个读取出来
			Fweight[(unsigned char)in_file[name++]]++;		//转换为unsigned char 
		int kindn = 0;										//字符种数
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
				kindn++;
		unsigned char *CharSetn = (unsigned char *)malloc(kindn * sizeof(unsigned char)); //记录文件含有的字符
		unsigned int *WeightSetn = (unsigned int *)malloc(kindn * sizeof(unsigned int));  //记录字符的权值
		if (CharSetn == NULL || WeightSetn == NULL)
			exit(ERROR);
		int jn = 0; //最后的结果j=kind-1
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
			{
				CharSetn[jn] = i; 
				WeightSetn[jn] = Fweight[i];
				jn++;
			}
		free(Fweight);
		HuffmanTree HTn;
		HuffmanCode HCn;
		HuffmanCoding(HTn, HCn, WeightSetn, kindn); //编写霍夫曼树和霍夫曼编码

		out = fopen(out_file, "wb"); //以byte的形式写入
		if (out == NULL)
			exit(ERROR);
		fwrite(&kindn, sizeof(int), 1, out); //写入字符个数
											 //fwrite(&totalsize, sizeof(unsigned long long), 1, out);//写入原文件字节总数
											 //2n个哈夫曼树元素写入
		fwrite(HTn, sizeof(HTNode), 2 * kindn, out);
		fwrite(CharSetn, sizeof(unsigned char), kindn, out);  //写入字符表
		fwrite(WeightSetn, sizeof(unsigned int), kindn, out); //写入权值表
															  //以下开始编码
		int offset = 8;										  //记录一个字节内剩余位数
		int a = 0;											  //字符在CharSet里的位置
		unsigned char ReadByte;								  //读取的一个字节
		unsigned char TempByte = 0;							  //暂时存储要写入的编码
		unsigned long long BitSizen = 0;					  //记录位数，方便进行解压

		for (unsigned long long i = 0; i < totalsizen; i++)
		{
			ReadByte = in_file[i];
			a = 0;
			while (1)
			{
				if (CharSetn[a] == ReadByte)
					break;
				a++;
			}
			for (int b = 0; HCn[a + 1][b]; b++)
			{
				TempByte = (TempByte << 1) | (HCn[a + 1][b] - '0'); //利用位或TempByte左移一位并写入一位
				BitSizen++;
				offset--;
				if (offset == 0)
				{
				                                                	//字节8位已填满
					offset = 8;                                     //重置为8位
					fwrite(&TempByte, sizeof(unsigned char), 1, out);
					TempByte = 0;
				}
			}
		}
		unsigned long long BitSizenn;
		BitSizenn = BitSizen;
		if (offset != 8)
		{
		                                                 	//若最后一个字节用不完，也要强行用到8位
			TempByte <<= offset;
			BitSizenn = BitSizen + offset;
			fwrite(&TempByte, sizeof(unsigned char), 1, out);
		}

		fwrite(&kind, sizeof(int), 1, out);                 //写入字符个数
										                  	//fwrite(&totalsize, sizeof(unsigned long long), 1, out);//写入原文件字节总数
										                	//2n个哈夫曼树元素写入
		fwrite(HT, sizeof(HTNode), 2 * kind, out);
		fwrite(CharSet, sizeof(unsigned char), kind, out);	//写入字符表
		fwrite(WeightSet, sizeof(unsigned int), kind, out); //写入权值表
															//以下开始编码
		offset = 8;											//记录一个字节内剩余位数
		a = 0;												//字符在CharSet里的位置
		TempByte = 0;										//暂时存储要写入的编码
		unsigned long long BitSize = 0;						//记录位数，方便进行解压
		in = fopen(in_file, "rb");							//以二进制的形式写入
		if (in == NULL)
			exit(ERROR);
		for (unsigned long long i = 0; i < totalsize; i++)
		{
			fread(&ReadByte, sizeof(unsigned char), 1, in);
			a = 0;
			while (1)
			{
				if (CharSet[a] == ReadByte)
					break;
				a++;
			}
			for (int b = 0; HC[a + 1][b]; b++)
			{
				//若读到'\0'，则跳出
				//第一个没用，为HC[a+1][b]
				TempByte = (TempByte << 1) | (HC[a + 1][b] - '0'); //利用位或TempByte左移一位并写入一位
				BitSize++;
				offset--;
				if (offset == 0)
				{
					//字节8位已填满
					offset = 8; //重置为8位
					fwrite(&TempByte, sizeof(unsigned char), 1, out);
					TempByte = 0;
				}
			}
		}
		if (offset != 8)
		{
			//若最后一个字节用不完，也要强行用到8位
			TempByte <<= offset;
			fwrite(&TempByte, sizeof(unsigned char), 1, out);
		}
		fwrite(&BitSize, sizeof(unsigned long long), 1, out);	//将文件位数写在文件的最后
		fwrite(&BitSizen, sizeof(unsigned long long), 1, out);	//将名字位数写在文件最后
		fwrite(&BitSizenn, sizeof(unsigned long long), 1, out); //将名字占用位数写在文件最后
		fclose(in);
		fclose(out);
		free(HT);
		free(HC);
		free(CharSet);
		free(WeightSet);
		free(HTn);
		free(HCn);
		free(CharSetn);
		free(WeightSetn);
		printf("已成功压缩！");
	}
}

void Decompress(char in_file[])
{
	int kind, kindn;
	FILE *in, *out;
	HuffmanTree HT, HTn;
	unsigned long long BitSize, BitSizen, BitSizenn;
	unsigned char *CharSet, *CharSetn;
	unsigned int *WeightSet, *WeightSetn;
	in = fopen(in_file, "rb");
	int len;
	len = strlen(in_file);
	if (in == NULL)
	{
		printf("文件不存在！");
	}
	else if (in_file[len - 8] != '.' || in_file[len - 1] != 'n' || in_file[len - 2] != 'a' || in_file[len - 3] != 'm' || in_file[len - 4] != 'f' || in_file[len - 5] != 'f' || in_file[len - 6] != 'u' || in_file[len - 7] != 'h')
	{
		printf("不是可以解压的文件格式！");
	}
	else
	{
		fread(&kindn, sizeof(int), 1, in);
		HTn = (HuffmanTree)malloc(2 * kindn * sizeof(HTNode));
		CharSetn = (unsigned char *)malloc(kindn * sizeof(unsigned char));
		WeightSetn = (unsigned int *)malloc(kindn * sizeof(unsigned int));
		if ((HTn == NULL) || (CharSetn == NULL) || (WeightSetn == NULL))
		{
			exit(ERROR);
		}
		fread(HTn, sizeof(HTNode), 2 * kindn, in);
		fread(CharSetn, sizeof(unsigned char), kindn, in);
		fread(WeightSetn, sizeof(unsigned int), kindn, in);
		fseek(in, -3L * sizeof(unsigned long long), SEEK_END);
		fread(&BitSize, sizeof(unsigned long long), 1, in);
		fread(&BitSizen, sizeof(unsigned long long), 1, in);
		fread(&BitSizenn, sizeof(unsigned long long), 1, in);
		fseek(in, sizeof(int) + 2 * kindn * sizeof(HTNode) + kindn * sizeof(unsigned char) + kindn * sizeof(unsigned int), SEEK_SET);
		unsigned char TempByte = 0;
		unsigned char ReadByte;
		int offset = 8;
		int lndex = 2 * kindn - 1;

		char out_file[100];

		int x = 0;
		fread(&ReadByte, sizeof(unsigned char), 1, in);
		for (unsigned long long a = 0; a < BitSizen; a++)
		{
			TempByte = 1 & (ReadByte >> 7);
			if (TempByte)
			{
				lndex = HTn[lndex].rchild;
			}
			else
			{
				lndex = HTn[lndex].lchild;
			}
			if ((!HTn[lndex].lchild) && (!HTn[lndex].rchild))
			{
				out_file[x] = CharSetn[lndex - 1];
				lndex = 2 * kindn - 1;
				x++;
			}
			offset--;
			ReadByte = ReadByte << 1;
			if (offset == 0)
			{
				fread(&ReadByte, sizeof(unsigned char), 1, in);
				offset = 8;
			}
		}

		out = fopen(out_file, "wb");
		fseek(in, sizeof(int) + 2 * kindn * sizeof(HTNode) + kindn * sizeof(unsigned char) + kindn * sizeof(unsigned int) + (BitSizenn / 8) * sizeof(unsigned char), SEEK_SET);
		fread(&kind, sizeof(int), 1, in);
		HT = (HuffmanTree)malloc(2 * kind * sizeof(HTNode));
		CharSet = (unsigned char *)malloc(kind * sizeof(unsigned char));
		WeightSet = (unsigned int *)malloc(kind * sizeof(unsigned int));
		if ((HT == NULL) || (CharSet == NULL) || (WeightSet == NULL))
		{
			exit(ERROR);
		}
		fread(HT, sizeof(HTNode), 2 * kind, in);
		fread(CharSet, sizeof(unsigned char), kind, in);
		fread(WeightSet, sizeof(unsigned int), kind, in);
		TempByte = 0;
		offset = 8;
		lndex = 2 * kind - 1;

		if (out == NULL)
		{
			exit(ERROR);
		}
		fread(&ReadByte, sizeof(unsigned char), 1, in);
		for (unsigned long long a = 0; a < BitSize; a++)
		{
			TempByte = 1 & (ReadByte >> 7);
			if (TempByte)
			{
				lndex = HT[lndex].rchild;
			}
			else
			{
				lndex = HT[lndex].lchild;
			}
			if ((!HT[lndex].lchild) && (!HT[lndex].rchild))
			{
				fwrite(&CharSet[lndex - 1], sizeof(unsigned char), 1, out);
				lndex = 2 * kind - 1;
			}
			offset--;
			ReadByte = ReadByte << 1;
			if (offset == 0)
			{
				fread(&ReadByte, sizeof(unsigned char), 1, in);
				offset = 8;
			}
		}
		free(CharSet);
		free(WeightSet);
		free(HT);
		free(CharSetn);
		free(WeightSetn);
		free(HTn);
		fclose(in);
		fclose(out);
		printf("解压成功！");
	}
}

void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC, unsigned int *w, int n)
{
	if (n <= 1)
		return;
	int m = 2 * n - 1;									//总结点数
	HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode)); //预留一个未用的0号单元
	if (HT == NULL)
		exit(ERROR);
	HTNode *p = HT + 1; //跳过0号单元
	int i = 1;
	while (i <= n)
	{
		*p = {w[i - 1], 0, 0, 0}; //前n个结点初始化
		i++;
		p++;
	}
	while (i <= m)
	{
		*p = {0, 0, 0, 0}; //大于n的结点初始化
		i++;
		p++;
	}
	unsigned int m1;
	unsigned int m2;
	for (i = n + 1; i <= m; i++) //建立霍夫曼树
	{
		int s1, s2;
		if (i - 1 <= 1)
			exit(ERROR);
		m1 = 4294967295;
		m2 = 4294967295;
		for (int j = 1; j < i; j++)
		{
			if ((HT[j].weight < m1) && (HT[j].parent == 0))
			{
				m1 = HT[j].weight;
				s1 = j;
			}
		}
		for (int j = 1; j < i; j++)
		{
			if ((HT[j].weight < m2) && (j != s1) && (HT[j].parent == 0)) //选取除s1外权值最小的元素s2
			{
				m2 = HT[j].weight;
				s2 = j;
			}
		}
		HT[s1].parent = i;
		HT[s2].parent = i;
		HT[i].lchild = s1;
		HT[i].rchild = s2;
		HT[i].weight = HT[s1].weight + HT[s2].weight;
	}

	HC = (HuffmanCode)malloc((n + 1) * sizeof(char *));
	char *area = (char *)malloc(n * sizeof(char));
	if ((area == NULL) || (HC == NULL))
	{
		exit(ERROR);
	}
	area[n - 1] = '\0';
	int seat;
	int g;
	for (int s = 1; s <= n; s++)
	{
		seat = n - 1;
		g = s;
		int b = HT[s].parent;
		while (b != 0)
		{
			if (HT[b].lchild == g)
				area[--seat] = '0';
			else
				area[--seat] = '1';
			g = b;
			b = HT[b].parent;
		}
		HC[s] = (char *)malloc((n - seat) * sizeof(char));
		if (!HC[s])
			exit(ERROR);
		strcpy(HC[s], &area[seat]);
	}
	free(area);
}

int main()
{
	puts("-------------------------------Huffman压缩--------------------------------");
	puts("输入“z+回车键”进行压缩");
	puts("输入“u+回车键”进行解压");
	puts("输入“l+回车键”则退出");
	puts("--------------------------------------------------------------------------");
	char mark[100];
	int marka, len;
	char in_file_name[MAX_SIZE];  //待压缩&&待解压文件名
	char out_file_name[MAX_SIZE]; //解压后&&压缩后文件名
	char Hu[10] = ".huffman";
	for (;;)
	{
		printf("输入选项：");
		scanf("%s", mark);
		len = strlen(mark);
		getchar(); //取掉回车字符
		if ((len == 1) && (mark[0] == 'z'))
		{
			marka = 1;
		}
		else if ((len == 1) && mark[0] == 'u')
		{
			marka = 2;
		}
		else if ((len == 1) && mark[0] == 'l')
		{
			marka = 3;
		}
		else
		{
			marka = 4;
		}
		switch (marka)
		{
		case 1:
			printf("请输入待压缩的文件名称：");
			scanf("%s", in_file_name, MAX_SIZE);
			getchar();
			printf("请输入压缩后的文件名称：");
			scanf("%s", out_file_name, MAX_SIZE);
			getchar();
			strcat(out_file_name, Hu);
			Compress(in_file_name, out_file_name);
			break;
		case 2:
			printf("请输入待解压的文件名称：");
			scanf("%s", in_file_name, MAX_SIZE);
			getchar();
			Decompress(in_file_name);
			break;
		case 3:
			printf("已退出！");
			return 0;
		default:
			printf("请输入有效指令！");
		}
		putchar('\n');
	}
	return 0;
}

