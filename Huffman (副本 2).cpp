#include <stdio.h>
#include <stdlib.h>
#include <string.h>	 //���õ�strcpy()
#define MAX_SIZE 100 //�ļ�������
#define ERROR -1
#define OK 1

typedef struct
{
	unsigned int weight; //�ַ�Ȩ��
	unsigned int parent, lchild, rchild;
} HTNode, *HuffmanTree;		//��̬��������洢��������
typedef char **HuffmanCode; //��̬��������洢�����������

void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC, unsigned int *w, int n);
void Compress(char in_file[], char out_file[]);
void Decompress(char in_file[]);

void Compress(char in_file[], char out_file[])
{
	FILE *in, *out; //inΪ��ѹ���ļ�ָ�룬outΪѹ�����ļ�ָ��
	in = fopen(in_file, "rb");
	if (in == NULL)
	{
		printf("�ļ������ڣ�");
	}
	else
	{
		fseek(in, 0L, SEEK_END);													//���ļ�ָ���Ƶ�ĩβ
		unsigned long long totalsize = ftell(in);									//��¼�ļ����ֽ���
		fseek(in, 0L, SEEK_SET);													//��ָ���Ƶ���ͷ
		unsigned int *Fweight = (unsigned int *)malloc(256 * sizeof(unsigned int)); //��¼256���ַ��Ƿ���ڼ�Ȩֵ
		for (int i = 0; i < 256; i++)
			Fweight[i] = 0;								   //��ʼ��
		for (unsigned long long i = 0; i < totalsize; i++) //���ļ���ȫ���ַ�һ������ȡ����
			Fweight[(unsigned char)fgetc(in)]++;		   //charת��Ϊunsigned
		fclose(in);
		int kind = 0; //�ַ�����
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
				kind++;
		unsigned char *CharSet = (unsigned char *)malloc(kind * sizeof(unsigned char)); //��¼�ļ����е��ַ�
		unsigned int *WeightSet = (unsigned int *)malloc(kind * sizeof(unsigned int));	//��¼�ַ���Ȩֵ
		if (CharSet == NULL || WeightSet == NULL)
			exit(ERROR);
		int j = 0; //���Ľ��j=kind-1
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
			{
				CharSet[j] = i; 
				WeightSet[j] = Fweight[i];
				j++;
			}
		HuffmanTree HT;
		HuffmanCode HC;
		HuffmanCoding(HT, HC, WeightSet, kind); //��д���������ͻ���������

		int name = 0;
		int totalsizen = strlen(in_file);
		for (int i = 0; i < 256; i++)
			Fweight[i] = 0;									//��ʼ��
		for (unsigned long long i = 0; i < totalsizen; i++) //���ļ�����ȫ���ַ�һ������ȡ����
			Fweight[(unsigned char)in_file[name++]]++;		//ת��Ϊunsigned char 
		int kindn = 0;										//�ַ�����
		for (int i = 0; i < 256; i++)
			if (Fweight[i] > 0)
				kindn++;
		unsigned char *CharSetn = (unsigned char *)malloc(kindn * sizeof(unsigned char)); //��¼�ļ����е��ַ�
		unsigned int *WeightSetn = (unsigned int *)malloc(kindn * sizeof(unsigned int));  //��¼�ַ���Ȩֵ
		if (CharSetn == NULL || WeightSetn == NULL)
			exit(ERROR);
		int jn = 0; //���Ľ��j=kind-1
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
		HuffmanCoding(HTn, HCn, WeightSetn, kindn); //��д���������ͻ���������

		out = fopen(out_file, "wb"); //��byte����ʽд��
		if (out == NULL)
			exit(ERROR);
		fwrite(&kindn, sizeof(int), 1, out); //д���ַ�����
											 //fwrite(&totalsize, sizeof(unsigned long long), 1, out);//д��ԭ�ļ��ֽ�����
											 //2n����������Ԫ��д��
		fwrite(HTn, sizeof(HTNode), 2 * kindn, out);
		fwrite(CharSetn, sizeof(unsigned char), kindn, out);  //д���ַ���
		fwrite(WeightSetn, sizeof(unsigned int), kindn, out); //д��Ȩֵ��
															  //���¿�ʼ����
		int offset = 8;										  //��¼һ���ֽ���ʣ��λ��
		int a = 0;											  //�ַ���CharSet���λ��
		unsigned char ReadByte;								  //��ȡ��һ���ֽ�
		unsigned char TempByte = 0;							  //��ʱ�洢Ҫд��ı���
		unsigned long long BitSizen = 0;					  //��¼λ����������н�ѹ

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
				TempByte = (TempByte << 1) | (HCn[a + 1][b] - '0'); //����λ��TempByte����һλ��д��һλ
				BitSizen++;
				offset--;
				if (offset == 0)
				{
				                                                	//�ֽ�8λ������
					offset = 8;                                     //����Ϊ8λ
					fwrite(&TempByte, sizeof(unsigned char), 1, out);
					TempByte = 0;
				}
			}
		}
		unsigned long long BitSizenn;
		BitSizenn = BitSizen;
		if (offset != 8)
		{
		                                                 	//�����һ���ֽ��ò��꣬ҲҪǿ���õ�8λ
			TempByte <<= offset;
			BitSizenn = BitSizen + offset;
			fwrite(&TempByte, sizeof(unsigned char), 1, out);
		}

		fwrite(&kind, sizeof(int), 1, out);                 //д���ַ�����
										                  	//fwrite(&totalsize, sizeof(unsigned long long), 1, out);//д��ԭ�ļ��ֽ�����
										                	//2n����������Ԫ��д��
		fwrite(HT, sizeof(HTNode), 2 * kind, out);
		fwrite(CharSet, sizeof(unsigned char), kind, out);	//д���ַ���
		fwrite(WeightSet, sizeof(unsigned int), kind, out); //д��Ȩֵ��
															//���¿�ʼ����
		offset = 8;											//��¼һ���ֽ���ʣ��λ��
		a = 0;												//�ַ���CharSet���λ��
		TempByte = 0;										//��ʱ�洢Ҫд��ı���
		unsigned long long BitSize = 0;						//��¼λ����������н�ѹ
		in = fopen(in_file, "rb");							//�Զ����Ƶ���ʽд��
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
				//������'\0'��������
				//��һ��û�ã�ΪHC[a+1][b]
				TempByte = (TempByte << 1) | (HC[a + 1][b] - '0'); //����λ��TempByte����һλ��д��һλ
				BitSize++;
				offset--;
				if (offset == 0)
				{
					//�ֽ�8λ������
					offset = 8; //����Ϊ8λ
					fwrite(&TempByte, sizeof(unsigned char), 1, out);
					TempByte = 0;
				}
			}
		}
		if (offset != 8)
		{
			//�����һ���ֽ��ò��꣬ҲҪǿ���õ�8λ
			TempByte <<= offset;
			fwrite(&TempByte, sizeof(unsigned char), 1, out);
		}
		fwrite(&BitSize, sizeof(unsigned long long), 1, out);	//���ļ�λ��д���ļ������
		fwrite(&BitSizen, sizeof(unsigned long long), 1, out);	//������λ��д���ļ����
		fwrite(&BitSizenn, sizeof(unsigned long long), 1, out); //������ռ��λ��д���ļ����
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
		printf("�ѳɹ�ѹ����");
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
		printf("�ļ������ڣ�");
	}
	else if (in_file[len - 8] != '.' || in_file[len - 1] != 'n' || in_file[len - 2] != 'a' || in_file[len - 3] != 'm' || in_file[len - 4] != 'f' || in_file[len - 5] != 'f' || in_file[len - 6] != 'u' || in_file[len - 7] != 'h')
	{
		printf("���ǿ��Խ�ѹ���ļ���ʽ��");
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
		printf("��ѹ�ɹ���");
	}
}

void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC, unsigned int *w, int n)
{
	if (n <= 1)
		return;
	int m = 2 * n - 1;									//�ܽ����
	HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode)); //Ԥ��һ��δ�õ�0�ŵ�Ԫ
	if (HT == NULL)
		exit(ERROR);
	HTNode *p = HT + 1; //����0�ŵ�Ԫ
	int i = 1;
	while (i <= n)
	{
		*p = {w[i - 1], 0, 0, 0}; //ǰn������ʼ��
		i++;
		p++;
	}
	while (i <= m)
	{
		*p = {0, 0, 0, 0}; //����n�Ľ���ʼ��
		i++;
		p++;
	}
	unsigned int m1;
	unsigned int m2;
	for (i = n + 1; i <= m; i++) //������������
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
			if ((HT[j].weight < m2) && (j != s1) && (HT[j].parent == 0)) //ѡȡ��s1��Ȩֵ��С��Ԫ��s2
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
	puts("-------------------------------Huffmanѹ��--------------------------------");
	puts("���롰z+�س���������ѹ��");
	puts("���롰u+�س��������н�ѹ");
	puts("���롰l+�س��������˳�");
	puts("--------------------------------------------------------------------------");
	char mark[100];
	int marka, len;
	char in_file_name[MAX_SIZE];  //��ѹ��&&����ѹ�ļ���
	char out_file_name[MAX_SIZE]; //��ѹ��&&ѹ�����ļ���
	char Hu[10] = ".huffman";
	for (;;)
	{
		printf("����ѡ�");
		scanf("%s", mark);
		len = strlen(mark);
		getchar(); //ȡ���س��ַ�
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
			printf("�������ѹ�����ļ����ƣ�");
			scanf("%s", in_file_name, MAX_SIZE);
			getchar();
			printf("������ѹ������ļ����ƣ�");
			scanf("%s", out_file_name, MAX_SIZE);
			getchar();
			strcat(out_file_name, Hu);
			Compress(in_file_name, out_file_name);
			break;
		case 2:
			printf("���������ѹ���ļ����ƣ�");
			scanf("%s", in_file_name, MAX_SIZE);
			getchar();
			Decompress(in_file_name);
			break;
		case 3:
			printf("���˳���");
			return 0;
		default:
			printf("��������Чָ�");
		}
		putchar('\n');
	}
	return 0;
}

