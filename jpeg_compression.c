#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define pi 3.14159265
#define PI 3.14159265

int i=0,j=0,k=0,l=0,m=0;
int u=0,v=0;
int r=0,c=0,s;

float sum_Y,sum_Cb,sum_Cr;

//亮度量化表
int Q_Y[64]={
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68,109,103, 77,
    24, 35, 55, 64, 81,104,113, 92,
    49, 64, 78, 87,103,121,120,101,
    72, 92, 95, 98,112,100,103, 99 };

//色度量化表
int Q_CbCr[64]={
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99 };

int z[64]={
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63 };


/*construct a structure of BMP header*/
typedef struct Bmpheader {
	//Bitmap File Header
	char identifier[2]; //'BM'
	unsigned int filesize; //點陣圖檔大小(byte)
	unsigned short reserved; //保留欄位
	unsigned short reserved2;
	unsigned int bitmap_dataoffset; //資料開始前的偏移量
	//Bitmap Info Header
	unsigned int bitmap_headersize; //Bitmap Info Header的長度
	unsigned int width; //寬度(pixel)
	unsigned int height; //高度(pixel)
	unsigned short planes; //位元圖層數
	unsigned short bits_perpixel; //
	unsigned int compression; //壓縮方式
	unsigned int bitmap_datasize; //點陣圖資料的大小(byte)
	unsigned int hresolution; //水平解析度
	unsigned int vresolution; //垂直解析度
	unsigned int usedcolors; //使用的調色盤顏色數
	unsigned int importantcolors; //重要的顏色數
	//Pallette
	unsigned int palette; //調色盤資料
}Bitmap;

/*construct a structure of RGB*/
typedef struct RGB{
	int R;
	int G;
	int B;
}ImgRGB;

/*construct a structure of YCbCr*/
typedef struct YCbCr{
	float Y[8][8];
	float Cb[8][8];
	float Cr[8][8];
}ImgYCbCr;

/*construct a structure of DCT*/
typedef struct DCT{
    float Y[8][8];
    float Cb[8][8];
    float Cr[8][8];
}ImgDCT;

/*construct a structure of QUAN*/
typedef struct QUAN{
    int Y[8][8];
    int Cb[8][8];
    int Cr[8][8];
}ImgQUAN;

/*construct a structure of ZigZag*/
typedef struct ZigZag{
    int Y[64];
    int Cb[64];
    int Cr[64];
}ImgZigZag;

/*construct a structure of RLE*/
typedef struct RLE{
    int Y_value[64];
    int Cb_value[64];
    int Cr_value[64];
    int Y_zero_num[64];
    int Cb_zero_num[64];
    int Cr_zero_num[64];
}ImgRLE;

typedef struct HUFF{
    char code_length;
    short code_word;//huffman code(2 byte)
    char code_word_string[17];//16 + \0
}ImgHUFF;


/*ImgRGB 的 2*2 array 的 malloc*/
ImgRGB** malloc_2D_ImgRGB(int x, int y){
    ImgRGB **array, *data;
    array = (ImgRGB**)malloc(x*sizeof(ImgRGB *));
    data = (ImgRGB*)malloc(x*y*sizeof(ImgRGB));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgYCbCr 的 2*2 array 的 malloc*/
ImgYCbCr** malloc_2D_ImgYCbCr(int x, int y){
    ImgYCbCr **array, *data;
    array = (ImgYCbCr**)malloc(x*sizeof(ImgYCbCr *));
    data = (ImgYCbCr*)malloc(x*y*sizeof(ImgYCbCr));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgDCT 的 2*2 array 的 malloc*/
ImgDCT** malloc_2D_ImgDCT(int x, int y){
    ImgDCT **array, *data;
    array = (ImgDCT**)malloc(x*sizeof(ImgDCT *));
    data = (ImgDCT*)malloc(x*y*sizeof(ImgDCT));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgQUAN 的 2*2 array 的 malloc*/
ImgQUAN** malloc_2D_ImgQUAN(int x, int y){
    ImgQUAN **array, *data;
    array = (ImgQUAN**)malloc(x*sizeof(ImgQUAN *));
    data = (ImgQUAN*)malloc(x*y*sizeof(ImgQUAN));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgZigZag 的 2*2 array 的 malloc*/
ImgZigZag** malloc_2D_ImgZigZag(int x, int y){
    ImgZigZag **array, *data;
    array = (ImgZigZag**)malloc(x*sizeof(ImgZigZag *));
    data = (ImgQUAN*)malloc(x*y*sizeof(ImgZigZag));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}

/*ImgRLE 的 2*2 array 的 malloc*/
ImgRLE** malloc_2D_ImgRLE(int x, int y){
    ImgRLE **array, *data;
    array = (ImgRLE**)malloc(x*sizeof(ImgRLE *));
    data = (ImgRLE*)malloc(x*y*sizeof(ImgRLE));
    for(i=0;i<x;i++,data+=y){
        array[i] = data;
    }
    return array;
}


//亮度DC差值表
static ImgHUFF huffman_Y_DC[12] = {
    //length, code word, code word(string)  // category(差值位數)
	{ 2, 0x0000 , "00"},					//  0
	{ 3, 0x0002 , "010"},					//  1
	{ 3, 0x0003 , "011"},					//  2
	{ 3, 0x0004 , "100"},					//  3
	{ 3, 0x0005 , "101"},					//  4
	{ 3, 0x0006 , "110"},					//  5
	{ 4, 0x000e , "1110"},					//  6
	{ 5, 0x001e , "11110"},					//  7
	{ 6, 0x003e , "111110"},				//  8
	{ 7, 0x007e , "1111110"},				//  9
	{ 8, 0x00fe , "11111110"},				//  10
	{ 9, 0x01fe , "111111110"}				//  11
};

//色度DC差值表
static ImgHUFF huffman_C_DC[12] = {
    //length, code word, code word(string)  // category
	{ 2, 0x0000 , "00"},					//  0
	{ 2, 0x0001 , "01"},					//  1
	{ 2, 0x0002 , "10"},					//  2
	{ 3, 0x0006 , "110"},					//  3
	{ 4, 0x000e , "1110"},					//  4
	{ 5, 0x001e , "11110"},					//  5
	{ 6, 0x003e , "111110"},				//  6
	{ 7, 0x007e , "1111110"},				//  7
	{ 8, 0x00fe , "11111110"},				//  8
	{ 9, 0x01fe , "111111110"},				//  9
	{ 10, 0x03fe, "1111111110" },			//  10
	{ 11, 0x07fe, "11111111110" }			//  11
};

//亮度AC表
static ImgHUFF huffman_Y_AC[256] = {
    //length, code word, code word(string)
	{ 4, 0x000a , "1010"},
	{ 2, 0x0000 , "00"},
	{ 2, 0x0001 , "01"},
	{ 3, 0x0004 , "100"},
	{ 4, 0x000b , "1011"},
	{ 5, 0x001a , "11010"},
	{ 7, 0x0078 , "1111000"},
	{ 8, 0x00f8 ,"11111000"},
	{ 10, 0x03f6,"1111110110" },
	{ 16, 0xff82, "1111111110000010" },
	{ 16, 0xff83, "1111111110000011" },
	{ 4, 0x000c , "1100"},
	{ 5, 0x001b , "11011"},
	{ 7, 0x0079 , "1111001"},
	{ 9, 0x01f6 , "111110110"},
	{ 11, 0x07f6, "11111110110"},
	{ 16, 0xff84, "1111111110000100"},
	{ 16, 0xff85, "1111111110000101"},
	{ 16, 0xff86, "1111111110000110"},
	{ 16, 0xff87, "1111111110000111"},
	{ 16, 0xff88, "1111111110001000"},
	{ 5, 0x001c , "11100"},
	{ 8, 0x00f9 , "11111001"},
	{ 10, 0x03f7, "1111110111"},
	{ 12, 0x0ff4, "111111110100"},
	{ 16, 0xff89, "1111111110001001"},
	{ 16, 0xff8a, "1111111110001010"},
	{ 16, 0xff8b, "1111111110001011"},
	{ 16, 0xff8c, "1111111110001100"},
	{ 16, 0xff8d, "1111111110001101"},
	{ 16, 0xff8e, "1111111110001110"},
	{ 6, 0x003a , "111010"},
	{ 9, 0x01f7 , "111110111"},
	{ 12, 0x0ff5, "111111110101"},
	{ 16, 0xff8f, "1111111110001111"},
	{ 16, 0xff90, "1111111110010000"},
	{ 16, 0xff91, "1111111110010001"},
	{ 16, 0xff92, "1111111110010010"},
	{ 16, 0xff93, "1111111110010011"},
	{ 16, 0xff94, "1111111110010100"},
	{ 16, 0xff95, "1111111110010101"},
	{ 6, 0x003b , "111011"},
	{ 10, 0x03f8, "1111111000"},
	{ 16, 0xff96, "1111111110010110"},
	{ 16, 0xff97, "1111111110010111"},
	{ 16, 0xff98, "1111111110011000"},
	{ 16, 0xff99, "1111111110011001"},
	{ 16, 0xff9a, "1111111110011010"},
	{ 16, 0xff9b, "1111111110011011"},
	{ 16, 0xff9c, "1111111110011100"},
	{ 16, 0xff9d, "1111111110011101"},
	{ 7, 0x007a , "1111010"},
	{ 11, 0x07f7, "11111110111"},
	{ 16, 0xff9e, "1111111110011110"},
	{ 16, 0xff9f, "1111111110011111"},
	{ 16, 0xffa0, "1111111110100000"},
	{ 16, 0xffa1, "1111111110100001"},
	{ 16, 0xffa2, "1111111110100010"},
	{ 16, 0xffa3, "1111111110100011"},
	{ 16, 0xffa4, "1111111110100100"},
	{ 16, 0xffa5, "1111111110100101"},
	{ 7, 0x007b , "1111011"			},
	{ 12, 0x0ff6, "111111110110"	},
	{ 16, 0xffa6, "1111111110100110"},
	{ 16, 0xffa7, "1111111110100111"},
	{ 16, 0xffa8, "1111111110101000"},
	{ 16, 0xffa9, "1111111110101001"},
	{ 16, 0xffaa, "1111111110101010"},
	{ 16, 0xffab, "1111111110101011"},
	{ 16, 0xffac, "1111111110101100"},
	{ 16, 0xffad, "1111111110101101"},
	{ 8, 0x00fa , "11111010"},
	{ 12, 0x0ff7, "111111110111"},
	{ 16, 0xffae, "1111111110101110"},
	{ 16, 0xffaf, "1111111110101111"},
	{ 16, 0xffb0, "1111111110110000"},
	{ 16, 0xffb1, "1111111110110001"},
	{ 16, 0xffb2, "1111111110110010"},
	{ 16, 0xffb3, "1111111110110011"},
	{ 16, 0xffb4, "1111111110110100"},
	{ 16, 0xffb5, "1111111110110101"},
	{ 9, 0x01f8 , "111111000"},
	{ 15, 0x7fc0, "111111111000000"},
	{ 16, 0xffb6, "1111111110110110"},
	{ 16, 0xffb7, "1111111110110111"},
	{ 16, 0xffb8, "1111111110111000"},
	{ 16, 0xffb9, "1111111110111001"},
	{ 16, 0xffba, "1111111110111010"},
	{ 16, 0xffbb, "1111111110111011"},
	{ 16, 0xffbc, "1111111110111100"},
	{ 16, 0xffbd, "1111111110111101"},
	{ 9, 0x01f9 , "111111001"},
	{ 16, 0xffbe, "1111111110111110"},
	{ 16, 0xffbf, "1111111110111111"},
	{ 16, 0xffc0, "1111111111000000"},
	{ 16, 0xffc1, "1111111111000001"},
	{ 16, 0xffc2, "1111111111000010"},
	{ 16, 0xffc3, "1111111111000011"},
	{ 16, 0xffc4, "1111111111000100"},
	{ 16, 0xffc5, "1111111111000101"},
	{ 16, 0xffc6, "1111111111000110"},
	{ 9, 0x01fa , "111111010"		},
	{ 16, 0xffc7, "1111111111000111"},
	{ 16, 0xffc8, "1111111111001000"},
	{ 16, 0xffc9, "1111111111001001"},
	{ 16, 0xffca, "1111111111001010"},
	{ 16, 0xffcb, "1111111111001011"},
	{ 16, 0xffcc, "1111111111001100"},
	{ 16, 0xffcd, "1111111111001101"},
	{ 16, 0xffce, "1111111111001110"},
	{ 16, 0xffcf, "1111111111001111"},
	{ 10, 0x03f9,  "1111111001"},
	{ 16, 0xffd0,  "1111111111010000"},
	{ 16, 0xffd1,  "1111111111010001"},
	{ 16, 0xffd2,  "1111111111010010"},
	{ 16, 0xffd3,  "1111111111010011"},
	{ 16, 0xffd4,  "1111111111010100"},
	{ 16, 0xffd5,  "1111111111010101"},
	{ 16, 0xffd6,  "1111111111010110"},
	{ 16, 0xffd7,  "1111111111010111"},
	{ 16, 0xffd8,  "1111111111011000"},
	{ 10, 0x03fa,  "1111111010"},
	{ 16, 0xffd9,  "1111111111011001"},
	{ 16, 0xffda,  "1111111111011010"},
	{ 16, 0xffdb,  "1111111111011011"},
	{ 16, 0xffdc,  "1111111111011100"},
	{ 16, 0xffdd,  "1111111111011101"},
	{ 16, 0xffde,  "1111111111011110"},
	{ 16, 0xffdf,  "1111111111011111"},
	{ 16, 0xffe0,  "1111111111100000"},
	{ 16, 0xffe1,  "1111111111100001"},
	{ 11, 0x07f8,  "11111111000"},
	{ 16, 0xffe2,  "1111111111100010"},
	{ 16, 0xffe3,  "1111111111100011"},
	{ 16, 0xffe4,  "1111111111100100"},
	{ 16, 0xffe5,  "1111111111100101"},
	{ 16, 0xffe6,  "1111111111100110"},
	{ 16, 0xffe7,  "1111111111100111"},
	{ 16, 0xffe8,  "1111111111101000"},
	{ 16, 0xffe9,  "1111111111101001"},
	{ 16, 0xffea,  "1111111111101010"},
	{ 16, 0xffeb,  "1111111111101011"},
	{ 16, 0xffec,  "1111111111101100"},
	{ 16, 0xffed,  "1111111111101101"},
	{ 16, 0xffee,  "1111111111101110"},
	{ 16, 0xffef,  "1111111111101111"},
	{ 16, 0xfff0,  "1111111111110000"},
	{ 16, 0xfff1,  "1111111111110001"},
	{ 16, 0xfff2,  "1111111111110010"},
	{ 16, 0xfff3,  "1111111111110011"},
	{ 16, 0xfff4,  "1111111111110100"},
	{ 11, 0x07f9,  "11111111001"},
	{ 16, 0xfff5,  "1111111111110101"},
	{ 16, 0xfff6,  "1111111111110110"},
	{ 16, 0xfff7,  "1111111111110111"},
	{ 16, 0xfff8,  "1111111111111000"},
	{ 16, 0xfff9,  "1111111111111001"},
	{ 16, 0xfffa,  "1111111111111010"},
	{ 16, 0xfffb,  "1111111111111011"},
	{ 16, 0xfffc,  "1111111111111100"},
	{ 16, 0xfffd,  "1111111111111101"},
	{ 16, 0xfffe,  "1111111111111110"},
};

//色度AC表
static ImgHUFF huffman_C_AC[256] = {
//  { code length, code word, code word(binary)}
	{ 2, 0x0000 , "00"},
	{ 2, 0x0001 , "01"},
	{ 3, 0x0004 , "100"},
	{ 4, 0x000a , "1010"},
	{ 5, 0x0018 , "11000"},
	{ 5, 0x0019 , "11001"},
	{ 6, 0x0038 , "111000"},
	{ 7, 0x0078 , "1111000"},
	{ 9, 0x01f4 , "111110100"},
	{ 10, 0x03f6, "1111110110" },
	{ 12, 0x0ff4, "111111110100" },
	{ 4, 0x000b , "1011"},
	{ 6, 0x0039 , "111001"},
	{ 8, 0x00f6 , "11110110"},
	{ 9, 0x01f5 , "111110101"},
	{ 11, 0x07f6, "11111110110" },
	{ 12, 0x0ff5, "111111110101" },
	{ 16, 0xff88, "1111111110001000" },
	{ 16, 0xff89, "1111111110001001" },
	{ 16, 0xff8a, "1111111110001010" },
	{ 16, 0xff8b, "1111111110001011" },
	{ 5, 0x001a , "11010"},
	{ 8, 0x00f7 , "11110111"},
	{ 10, 0x03f7, "1111110111" },
	{ 12, 0x0ff6, "111111110110" },
	{ 15, 0x7fc2, "111111111000010" },
	{ 16, 0xff8c, "1111111110001100" },
	{ 16, 0xff8d, "1111111110001101" },
	{ 16, 0xff8e, "1111111110001110" },
	{ 16, 0xff8f, "1111111110001111" },
	{ 16, 0xff90, "1111111110010000" },
	{ 5, 0x001b , "11011"},
	{ 8, 0x00f8 , "11111000"},
	{ 10, 0x03f8, "1111111000" },
	{ 12, 0x0ff7, "111111110111" },
	{ 16, 0xff91, "1111111110010001" },
	{ 16, 0xff92, "1111111110010010" },
	{ 16, 0xff93, "1111111110010011" },
	{ 16, 0xff94, "1111111110010100" },
	{ 16, 0xff95, "1111111110010101" },
	{ 16, 0xff96, "1111111110010110" },
	{ 6, 0x003a , "111010"},
	{ 9, 0x01f6 , "111110110"},
	{ 16, 0xff97, "1111111110010111" },
	{ 16, 0xff98, "1111111110011000" },
	{ 16, 0xff99, "1111111110011001" },
	{ 16, 0xff9a, "1111111110011010" },
	{ 16, 0xff9b, "1111111110011011" },
	{ 16, 0xff9c, "1111111110011100" },
	{ 16, 0xff9d, "1111111110011101" },
	{ 16, 0xff9e, "1111111110011110" },
	{ 6, 0x003b , "111011"},
	{ 10, 0x03f9, "1111111001" },
	{ 16, 0xff9f, "1111111110011111" },
	{ 16, 0xffa0, "1111111110100000" },
	{ 16, 0xffa1, "1111111110100001" },
	{ 16, 0xffa2, "1111111110100010" },
	{ 16, 0xffa3, "1111111110100011" },
	{ 16, 0xffa4, "1111111110100100" },
	{ 16, 0xffa5, "1111111110100101" },
	{ 16, 0xffa6, "1111111110100110" },
	{ 7, 0x0079 , "1111001"},
	{ 11, 0x07f7, "11111110111" },
	{ 16, 0xffa7, "1111111110100111" },
	{ 16, 0xffa8, "1111111110101000" },
	{ 16, 0xffa9, "1111111110101001" },
	{ 16, 0xffaa, "1111111110101010" },
	{ 16, 0xffab, "1111111110101011" },
	{ 16, 0xffac, "1111111110101100" },
	{ 16, 0xffad, "1111111110101101" },
	{ 16, 0xffae, "1111111110101110" },
	{ 7, 0x007a , "1111010"},
	{ 11, 0x07f8, "11111111000" },
	{ 16, 0xffaf, "1111111110101111" },
	{ 16, 0xffb0, "1111111110110000" },
	{ 16, 0xffb1, "1111111110110001" },
	{ 16, 0xffb2, "1111111110110010" },
	{ 16, 0xffb3, "1111111110110011" },
	{ 16, 0xffb4, "1111111110110100" },
	{ 16, 0xffb5, "1111111110110101" },
	{ 16, 0xffb6, "1111111110110110" },
	{ 8, 0x00f9 , "11111001"},
	{ 16, 0xffb7, "1111111110110111" },
	{ 16, 0xffb8, "1111111110111000" },
	{ 16, 0xffb9, "1111111110111001" },
	{ 16, 0xffba, "1111111110111010" },
	{ 16, 0xffbb, "1111111110111011" },
	{ 16, 0xffbc, "1111111110111100" },
	{ 16, 0xffbd, "1111111110111101" },
	{ 16, 0xffbe, "1111111110111110" },
	{ 16, 0xffbf, "1111111110111111" },
	{ 9, 0x01f7 , "111110111"},
	{ 16, 0xffc0, "1111111111000000" },
	{ 16, 0xffc1, "1111111111000001" },
	{ 16, 0xffc2, "1111111111000010" },
	{ 16, 0xffc3, "1111111111000011" },
	{ 16, 0xffc4, "1111111111000100" },
	{ 16, 0xffc5, "1111111111000101" },
	{ 16, 0xffc6, "1111111111000110" },
	{ 16, 0xffc7, "1111111111000111" },
	{ 16, 0xffc8, "1111111111001000" },
	{ 9, 0x01f8 , "111111000"},
	{ 16, 0xffc9, "1111111111001001" },
	{ 16, 0xffca, "1111111111001010" },
	{ 16, 0xffcb, "1111111111001011" },
	{ 16, 0xffcc, "1111111111001100" },
	{ 16, 0xffcd, "1111111111001101" },
	{ 16, 0xffce, "1111111111001110" },
	{ 16, 0xffcf, "1111111111001111" },
	{ 16, 0xffd0, "1111111111010000" },
	{ 16, 0xffd1, "1111111111010001" },
	{ 9, 0x01f9 , "111111001"},
	{ 16, 0xffd2, "1111111111010010" },
	{ 16, 0xffd3, "1111111111010011" },
	{ 16, 0xffd4, "1111111111010100" },
	{ 16, 0xffd5, "1111111111010101" },
	{ 16, 0xffd6, "1111111111010110" },
	{ 16, 0xffd7, "1111111111010111" },
	{ 16, 0xffd8, "1111111111011000" },
	{ 16, 0xffd9, "1111111111011001" },
	{ 16, 0xffda, "1111111111011010" },
	{ 9, 0x01fa , "111111010"},
	{ 16, 0xffdb, "1111111111011011" },
	{ 16, 0xffdc, "1111111111011100" },
	{ 16, 0xffdd, "1111111111011101" },
	{ 16, 0xffde, "1111111111011110" },
	{ 16, 0xffdf, "1111111111011111" },
	{ 16, 0xffe0, "1111111111100000" },
	{ 16, 0xffe1, "1111111111100001" },
	{ 16, 0xffe2, "1111111111100010" },
	{ 16, 0xffe3, "1111111111100011" },
	{ 11, 0x07f9, "11111111001" },
	{ 16, 0xffe4, "1111111111100100" },
	{ 16, 0xffe5, "1111111111100101" },
	{ 16, 0xffe6, "1111111111100110" },
	{ 16, 0xffe7, "1111111111100111" },
	{ 16, 0xffe8, "1111111111101000" },
	{ 16, 0xffe9, "1111111111101001" },
	{ 16, 0xffea, "1111111111101010" },
	{ 16, 0xffeb, "1111111111101011" },
	{ 16, 0xffec, "1111111111101100" },
	{ 14, 0x3fe0, "11111111100000" },
	{ 16, 0xffed, "1111111111101101" },
	{ 16, 0xffee, "1111111111101110" },
	{ 16, 0xffef, "1111111111101111" },
	{ 16, 0xfff0, "1111111111110000" },
	{ 16, 0xfff1, "1111111111110001" },
	{ 16, 0xfff2, "1111111111110010" },
	{ 16, 0xfff3, "1111111111110011" },
	{ 16, 0xfff4, "1111111111110100" },
	{ 16, 0xfff5, "1111111111110101" },
	{ 10, 0x03fa, "1111111010" },
	{ 15, 0x7fc3, "111111111000011" },
	{ 16, 0xfff6, "1111111111110110" },
	{ 16, 0xfff7, "1111111111110111" },
	{ 16, 0xfff8, "1111111111111000" },
	{ 16, 0xfff9, "1111111111111001" },
	{ 16, 0xfffa, "1111111111111010" },
	{ 16, 0xfffb, "1111111111111011" },
	{ 16, 0xfffc, "1111111111111100" },
	{ 16, 0xfffd, "1111111111111101" },
	{ 16, 0xfffe, "1111111111111110" },
};

/*read header*/
void read_header(FILE *fp, Bitmap *x){
	fread(&x->identifier, sizeof(x->identifier), 1, fp);
	fread(&x->filesize, sizeof(x->filesize), 1, fp);
	fread(&x->reserved, sizeof(x->reserved), 1, fp);
	fread(&x->reserved2, sizeof(x->reserved2), 1, fp);
	fread(&x->bitmap_dataoffset, sizeof(x->bitmap_dataoffset), 1, fp);
	fread(&x->bitmap_headersize, sizeof(x->bitmap_headersize), 1, fp);
	fread(&x->width, sizeof(x->width), 1, fp);
	fread(&x->height, sizeof(x->height), 1, fp);
	fread(&x->planes, sizeof(x->planes), 1, fp);
	fread(&x->bits_perpixel, sizeof(x->bits_perpixel), 1, fp);
	fread(&x->compression, sizeof(x->compression), 1, fp);
	fread(&x->bitmap_datasize, sizeof(x->bitmap_datasize), 1, fp);
	fread(&x->hresolution, sizeof(x->hresolution), 1, fp);
	fread(&x->vresolution, sizeof(x->vresolution), 1, fp);
	fread(&x->usedcolors, sizeof(x->usedcolors), 1, fp);
	fread(&x->importantcolors, sizeof(x->importantcolors), 1, fp);
}

/*write header*/
void write_header(FILE *outfile, Bitmap *bmpheader){
	fwrite(&bmpheader->identifier, sizeof(short), 1, outfile);
	fwrite(&bmpheader->filesize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->reserved, sizeof(short), 1, outfile);
	fwrite(&bmpheader->reserved2, sizeof(short), 1, outfile);
	fwrite(&bmpheader->bitmap_dataoffset, sizeof(int), 1, outfile);
	fwrite(&bmpheader->bitmap_headersize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->width, sizeof(int), 1, outfile);
	fwrite(&bmpheader->height, sizeof(int), 1, outfile);
	fwrite(&bmpheader->planes, sizeof(short), 1, outfile);
	fwrite(&bmpheader->bits_perpixel, sizeof(short), 1, outfile);
	fwrite(&bmpheader->compression, sizeof(int), 1, outfile);
	fwrite(&bmpheader->bitmap_datasize, sizeof(int), 1, outfile);
	fwrite(&bmpheader->hresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader->vresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader->usedcolors, sizeof(int), 1, outfile);
	fwrite(&bmpheader->importantcolors, sizeof(int), 1, outfile);
}

/*read data*/
//寫進 RGB structure
void read_data(FILE *fp, ImgRGB **array, int H, int W, int skip){
    printf("\nread data...\n");
	int temp;
	char skip_buf[3];
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			temp = fgetc(fp);
			array[i][j].B = temp;
			temp = fgetc(fp);
			array[i][j].G = temp;
			temp = fgetc(fp);
			array[i][j].R = temp;
		}
		if (skip != 0) fread(skip_buf, skip, 1, fp);
	}
	printf("finish\n");
}

/*write data*/
//寫出RGB的值
void write_data(FILE *outfile, ImgRGB **RGB, int H, int W, int skip){
    printf("\nwrite data...\n");
    char skip_buf[3] = { 0, 0, 0 };
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
        	fwrite(&RGB[i][j].B, sizeof(char),1,outfile);
        	fwrite(&RGB[i][j].G, sizeof(char),1,outfile);
        	fwrite(&RGB[i][j].R, sizeof(char),1,outfile);
        }
        if (skip != 0) { fwrite(skip_buf, skip, 1, outfile); }
    }
    printf("finish");
}

/*色度轉換YCbCr*/
//Y是亮度 Cb,Cr是色度
void YCbCr(ImgRGB **data_RGB, ImgYCbCr **data_YCbCr, int H, int W){
    printf("\nYCbCr...\n");
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(k=0;k<8;k++){
				for(l=0;l<8;l++){
					data_YCbCr[i][j].Y[k][l]=0.299*data_RGB[i*8+k][j*8+l].R + 0.578*data_RGB[i*8+k][j*8+l].G + 0.114*data_RGB[i*8+k][j*8+l].B;
        			data_YCbCr[i][j].Cb[k][l]=0.564*(data_RGB[i*8+k][j*8+l].B - data_YCbCr[i][j].Y[k][l]);
        			data_YCbCr[i][j].Cr[k][l]=0.713*(data_RGB[i*8+k][j*8+l].R - data_YCbCr[i][j].Y[k][l]);
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
	for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_YCbCr[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*反色度轉換YCbCr*/
void IYCbCr(ImgRGB **data_RGB, ImgYCbCr **data_YCbCr, int H, int W){
    printf("\nIYCbCr...\n");
	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(k=0;k<8;k++){
				for(l=0;l<8;l++){
					data_RGB[i*8+k][j*8+l].R=round(data_YCbCr[i][j].Y[k][l] + 1.402*data_YCbCr[i][j].Cr[k][l]);
        			data_RGB[i*8+k][j*8+l].G=round(data_YCbCr[i][j].Y[k][l] - 0.344*data_YCbCr[i][j].Cb[k][l] - 0.714*data_YCbCr[i][j].Cr[k][l]);
        			data_RGB[i*8+k][j*8+l].B=round(data_YCbCr[i][j].Y[k][l] + 1.772*data_YCbCr[i][j].Cb[k][l]);
        			//限制RGB的值在0~255
        			if(data_RGB[8*i+k][8*j+l].R > 255) data_RGB[8*i+k][8*j+l].R = 255;
        			if(data_RGB[8*i+k][8*j+l].G > 255) data_RGB[8*i+k][8*j+l].G = 255;
        			if(data_RGB[8*i+k][8*j+l].B > 255) data_RGB[8*i+k][8*j+l].B = 255;
        			if(data_RGB[8*i+k][8*j+l].R < 0) data_RGB[8*i+k][8*j+l].R = 0;
        			if(data_RGB[8*i+k][8*j+l].G < 0) data_RGB[8*i+k][8*j+l].G = 0;
        			if(data_RGB[8*i+k][8*j+l].B < 0) data_RGB[8*i+k][8*j+l].B = 0;
 				}
			}
		}
	}
	printf("finish\n");
}

/*DCT*/
//值的範圍要在-128~127,所以要先-128
void DCT(ImgYCbCr **data_YCbCr, ImgDCT **data_DCT, int H, int W){
    printf("\nDCT...\n");
    float cos1=0, cos2=0;
    float cosA[8][8];
    float cosB[8][8];

    for(r=0;r<8;r++){
        for(u=0;u<8;u++){
            cosA[r][u]=cos((2*r+1)*u*PI/16);
        }
    }

    for(c=0;c<8;c++){
        for(v=0;v<8;v++){
            cosB[c][v]=cos((2*c+1)*v*PI/16);
        }
    }

 	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(u=0;u<8;u++){
				for(v=0;v<8;v++){
                    sum_Y = 0;
                    sum_Cb = 0;
                    sum_Cr = 0;
                    for(r=0;r<8;r++){
                        for(c=0;c<8;c++){
                            if(u==0 && v==0){
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v]/2;
                            }

                            else if(u==0 || v==0){
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v]/sqrt(2);
                            }

                            else{
                                sum_Y += (data_YCbCr[i][j].Y[r][c]-128)*cosA[r][u]*cosB[c][v];
                                sum_Cb += (data_YCbCr[i][j].Cb[r][c]-128)*cosA[r][u]*cosB[c][v];
                                sum_Cr += (data_YCbCr[i][j].Cr[r][c]-128)*cosA[r][u]*cosB[c][v];
                            }
                        }
                    }
                    data_DCT[i][j].Y[u][v]=sum_Y/4;
                    data_DCT[i][j].Cb[u][v]=sum_Cb/4;
                    data_DCT[i][j].Cr[u][v]=sum_Cr/4;
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_DCT[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*IDCT*/
void IDCT(ImgYCbCr **data_YCbCr, ImgDCT **data_DCT, int H, int W){
    printf("\nIDCT...\n");
    float cos1=0, cos2=0;
    float cosA[8][8];
    float cosB[8][8];

    for(r=0;r<8;r++){
        for(u=0;u<8;u++){
            cosA[r][u]=cos((2*r+1)*u*PI/16);
        }
    }

    for(c=0;c<8;c++){
        for(v=0;v<8;v++){
            cosB[c][v]=cos((2*c+1)*v*PI/16);
        }
    }

 	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			for(r=0;r<8;r++){
				for(c=0;c<8;c++){
                    sum_Y = 0;
                    sum_Cb = 0;
                    sum_Cr = 0;
                    for(u=0;u<8;u++){
                        for(v=0;v<8;v++){
                            if(u==0 && v==0){
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v]/2;
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v]/2;
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v]/2;
                            }

                            else if(u==0 || v==0){
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v]/sqrt(2);
                            }

                            else{
                                sum_Y += (data_DCT[i][j].Y[u][v])*cosA[r][u]*cosB[c][v];
                                sum_Cb += (data_DCT[i][j].Cb[u][v])*cosA[r][u]*cosB[c][v];
                                sum_Cr += (data_DCT[i][j].Cr[u][v])*cosA[r][u]*cosB[c][v];
                            }
                        }
                    }
                    //要把128加回來
                    data_YCbCr[i][j].Y[r][c]=sum_Y/4+128;
                    data_YCbCr[i][j].Cb[r][c]=sum_Cb/4+128;
                    data_YCbCr[i][j].Cr[r][c]=sum_Cr/4+128;
 				}
			}
		}
	}
	printf("finish\n");

	printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_YCbCr[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*量化*/
//DCT的值除以量化表
void QUAN(ImgQUAN **data_QUAN, ImgDCT ** data_DCT, int H, int W){
    printf("\nQUAN...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_QUAN[i][j].Y[k][l]=round(data_DCT[i][j].Y[k][l]/Q_Y[k*8+l]);
                    data_QUAN[i][j].Cb[k][l]=round(data_DCT[i][j].Cb[k][l]/Q_CbCr[k*8+l]);
                    data_QUAN[i][j].Cr[k][l]=round(data_DCT[i][j].Cr[k][l]/Q_CbCr[k*8+l]);
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_QUAN[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*反量化*/
void IQUAN(ImgQUAN **data_QUAN, ImgDCT ** data_DCT, int H, int W){
    printf("\nIQUAN...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_DCT[i][j].Y[k][l]=data_QUAN[i][j].Y[k][l]*Q_Y[k*8+l];
                    data_DCT[i][j].Cb[k][l]=data_QUAN[i][j].Cb[k][l]*Q_CbCr[k*8+l];
                    data_DCT[i][j].Cr[k][l]=data_QUAN[i][j].Cr[k][l]*Q_CbCr[k*8+l];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3.3f ",data_DCT[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*ZigZag*/
void ZigZag(ImgZigZag **data_ZigZag, ImgQUAN ** data_QUAN, int H, int W){
    printf("\nZigZag...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_ZigZag[i][j].Y[z[k*8+l]]=data_QUAN[i][j].Y[k][l];
                    data_ZigZag[i][j].Cb[z[k*8+l]]=data_QUAN[i][j].Cb[k][l];
                    data_ZigZag[i][j].Cr[z[k*8+l]]=data_QUAN[i][j].Cr[k][l];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_ZigZag[0][0].Y[k*8+l]);
 		}
 		printf("\n");
    }
}

/*IZigZag*/
void IZigZag(ImgZigZag **data_ZigZag, ImgQUAN ** data_QUAN, int H, int W){
    printf("\nIZigZag...\n");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=0;k<8;k++){
                for(l=0;l<8;l++){
                    data_QUAN[i][j].Y[k][l]=data_ZigZag[i][j].Y[z[k*8+l]];
                    data_QUAN[i][j].Cb[k][l]=data_ZigZag[i][j].Cb[z[k*8+l]];
                    data_QUAN[i][j].Cr[k][l]=data_ZigZag[i][j].Cr[z[k*8+l]];
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_QUAN[0][0].Y[k][l]);
 		}
 		printf("\n");
    }
}

/*DPCM*/
void DPCM(ImgQUAN **data_QUAN, int H, int W){
    printf("\nDPCM...\n");

    printf("Y:(original)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
            printf("%3d ",data_QUAN[k][l].Y[0][0]);
            if(l%8==7) printf("\n");
        }
        printf("\n");
    }

    //先做第一行以外的
    for(i=0;i<H;i++){
        for(j=W;j>0;j--){
            data_QUAN[i][j].Y[0][0]=data_QUAN[i][j].Y[0][0]-data_QUAN[i][j-1].Y[0][0];
            data_QUAN[i][j].Cb[0][0]=data_QUAN[i][j].Cb[0][0]-data_QUAN[i][j-1].Cb[0][0];
            data_QUAN[i][j].Cr[0][0]=data_QUAN[i][j].Cr[0][0]-data_QUAN[i][j-1].Cr[0][0];
        }
    }
    //第一行的另外做 要扣掉上一行的
    for(i=H-1;i>0;i--){
        data_QUAN[i][0].Y[0][0]=data_QUAN[i][0].Y[0][0]-data_QUAN[i-1][W-1].Y[0][0];
        data_QUAN[i][0].Cb[0][0]=data_QUAN[i][0].Cb[0][0]-data_QUAN[i-1][W-1].Cb[0][0];
        data_QUAN[i][0].Cr[0][0]=data_QUAN[i][0].Cr[0][0]-data_QUAN[i-1][W-1].Cr[0][0];
    }

    printf("finish\n");

    printf("Y:(Diff)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
			printf("%3d ",data_QUAN[k][l].Y[0][0]);
			if(l%8==7) printf("\n");
 		}
 		printf("\n");
    }
}

/*IDPCM*/
void IDPCM(ImgQUAN **data_QUAN, int H, int W){
    printf("\nIDPCM...\n");

    for(i=1;i<H;i++){
        data_QUAN[i][0].Y[0][0]=data_QUAN[i][0].Y[0][0]+data_QUAN[i-1][W-1].Y[0][0];
        data_QUAN[i][0].Cb[0][0]=data_QUAN[i][0].Cb[0][0]+data_QUAN[i-1][W-1].Cb[0][0];
        data_QUAN[i][0].Cr[0][0]=data_QUAN[i][0].Cr[0][0]+data_QUAN[i-1][W-1].Cr[0][0];
    }

    for(i=0;i<H;i++){
        for(j=1;j<W;j++){
            data_QUAN[i][j].Y[0][0]=data_QUAN[i][j].Y[0][0]+data_QUAN[i][j-1].Y[0][0];
            data_QUAN[i][j].Cb[0][0]=data_QUAN[i][j].Cb[0][0]+data_QUAN[i][j-1].Cb[0][0];
            data_QUAN[i][j].Cr[0][0]=data_QUAN[i][j].Cr[0][0]+data_QUAN[i][j-1].Cr[0][0];
        }
    }

    printf("finish\n");

    printf("Y:(original)\n");
    for(k=0;k<1;k++){
        for(l=0;l<64;l++){
			printf("%3d ",data_QUAN[k][l].Y[0][0]);
			if(l%8==7) printf("\n");
 		}
 		printf("\n");
    }
}

/*RLE*/
void RLE(ImgRLE **data_RLE, ImgZigZag ** data_ZigZag, int H, int W){
    printf("\nRLE...\n");
    int Y_zero, Cb_zero, Cr_zero;
    int n1,n2,n3;
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            Y_zero = 0;//計算連續幾個0
            Cb_zero = 0;
            Cr_zero = 0;
            n1=1;
            n2=1;
            n3=1;
            for(k=1;k<64;k++){
                //Y
                //值不為0時
                if((data_ZigZag[i][j].Y[k])!=0){
                    data_RLE[i][j].Y_value[n1]=data_ZigZag[i][j].Y[k];
                    data_RLE[i][j].Y_zero_num[n1]=Y_zero;
                    Y_zero=0;
                    n1++;
                }
                //值為0時Y_zero+1，計算連續多少個0
                else{ //data_ZigZag[i][j].Y[k]==0
                    Y_zero++;
                }

                //Cb
                if((data_ZigZag[i][j].Cb[k])!=0){
                    data_RLE[i][j].Cb_value[n2]=data_ZigZag[i][j].Cb[k];
                    data_RLE[i][j].Cb_zero_num[n2]=Cb_zero;
                    Cb_zero=0;
                    n2++;
                }
                else{
                    Cb_zero++;
                }

                //Cr
                if((data_ZigZag[i][j].Cr[k])!=0){
                    data_RLE[i][j].Cr_value[n3]=data_ZigZag[i][j].Cr[k];
                    data_RLE[i][j].Cr_zero_num[n3]=Cr_zero;
                    Cr_zero=0;
                    n3++;
                }
                else{
                    Cr_zero++;
                }
            }
            //最後都是0寫(0,0)
            data_RLE[i][j].Y_value[n1]=0;
            data_RLE[i][j].Y_zero_num[n1]=0;
            data_RLE[i][j].Cb_value[n2]=0;
            data_RLE[i][j].Cb_zero_num[n2]=0;
            data_RLE[i][j].Cr_value[n3]=0;
            data_RLE[i][j].Cr_zero_num[n3]=0;
        }
    }
    printf("finish\n");

    for(i=1;i<15;i++){
        printf("%3d ", data_RLE[0][0].Y_zero_num[i]);
        printf("%3d ", data_RLE[0][0].Y_value[i]);
    }
}

//middle product
void middle_product(ImgRLE **data_RLE, ImgQUAN ** data_QUAN, int H, int W){
    //寫出壓縮後的大小
    FILE *fp_RLE_txt = fopen("RLE.txt","wb");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Y[0][0]);//先寫DPCM的DC
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Y_zero_num[k], data_RLE[i][j].Y_value[k]);//再寫RLE的AC
                if(data_RLE[i][j].Y_value[k]==0) break;//會寫(0,0)後跳到下一個block
            }
        }
    }

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Cb[0][0]);
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Cb_zero_num[k], data_RLE[i][j].Cb_value[k]);
                if(data_RLE[i][j].Cb_value[k]==0) break;
            }
        }
    }

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RLE_txt,"%d",data_QUAN[i][j].Cr[0][0]);
            for(k=1;k<64;k++){
                fprintf(fp_RLE_txt,"(%d,%d)",data_RLE[i][j].Cr_zero_num[k], data_RLE[i][j].Cr_value[k]);
                if(data_RLE[i][j].Cr_value[k]==0) break;
            }
        }
    }
    fclose(fp_RLE_txt);
}

/*IRLE*/
void IRLE(ImgRLE **data_RLE, ImgZigZag ** data_ZigZag, int H, int W){
    printf("\nIRLE...\n");
    int n1,n2,n3;

    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            for(k=1;k<64;k++){
                data_ZigZag[i][j].Y[k]=0;
                data_ZigZag[i][j].Cb[k]=0;
                data_ZigZag[i][j].Cr[k]=0;
                n1=1;
                n2=1;
                n3=1;
                for(l=1;l<64;l++){
                    //Y
                    if(n1<64){
                        if(data_RLE[i][j].Y_zero_num[l]!=0){ //補上被壓縮的0
                        for(m=0;m<data_RLE[i][j].Y_zero_num[l];m++){
                            data_ZigZag[i][j].Y[n1]=0;
                            n1++;
                        }
                    }
                    data_ZigZag[i][j].Y[n1]=data_RLE[i][j].Y_value[l];
                    n1++;
                    }
                }

                for(l=1;l<64;l++){
                    //Cb
                    if(n2<64){
                        if(data_RLE[i][j].Cb_zero_num[l]!=0){
                        for(m=0;m<data_RLE[i][j].Cb_zero_num[l];m++){
                            data_ZigZag[i][j].Cb[n2]=0;
                            n2++;
                        }
                    }
                    data_ZigZag[i][j].Cb[n2]=data_RLE[i][j].Cb_value[l];
                    n2++;
                    }
                }

                 for(l=1;l<64;l++){
                    //Cr
                    if(n3<64){
                        if(data_RLE[i][j].Cr_zero_num[l]!=0){
                        for(m=0;m<data_RLE[i][j].Cr_zero_num[l];m++){
                            data_ZigZag[i][j].Cr[n3]=0;
                            n3++;
                        }
                    }
                    data_ZigZag[i][j].Cr[n3]=data_RLE[i][j].Cr_value[l];
                    n3++;
                    }
                }
            }
        }
    }
    printf("finish\n");

    printf("Y:\n");
    for(k=0;k<8;k++){
        for(l=0;l<8;l++){
			printf("%3d ",data_ZigZag[0][0].Y[k*8+l]);
 		}
 		printf("\n");
    }
}

/*huffman*/

// Category          DC/AC value
// -----------------------------------------
// 0 					   0
// 1 		  		   –1 , 1
// 2 				 3,–2 , 2,3
// 3 			  –7..–4 , 4..7
// 4		 	 –15..–8 , 8..15
// 5 			–31..–16 , 16..31
// 6 			–63..–32 , 32..63
// 7 		   –127..–64 , 64..127
// 8 		  –255..–128 , 128..255
// 9 		  –511..–256 , 256..511
// 10 		 –1023..–512 , 512..1023
// 11 		–2047..–1024 , 1024..2047


//category
int get_category(int value){
    //先把值轉正
    int category;
    int a[20];
    if(value<0){
        value = -value;
    }
    int num;
	for(num=0;value>0;num++){//轉成二進位
		a[num]=value%2;
		value=value/2;
	}
    category=num;//i即是category的數值
    return category;
}

void huffman(ImgRLE **data_RLE, ImgQUAN ** data_QUAN, int H, int W){
    int DC_string[256];
    int category =0;
    int value;
    int run;
    int runsize;

    FILE *fp_temp_txt = fopen("temp.txt","wb");

/*------------------Y------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Y
            category = get_category(data_QUAN[i][j].Y[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_Y_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Y[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }
                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Y
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Y_value[m]);
                run = data_RLE[i][j].Y_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_Y_AC[runsize].code_word_string);
                if(data_RLE[i][j].Y_value[m]==0) break;
            }
        }
    }
/*------------------Cb------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Cb
            category = get_category(data_QUAN[i][j].Cb[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_C_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Cb[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }

                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Cb
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Cb_value[m]);
                run = data_RLE[i][j].Cb_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_C_AC[runsize].code_word_string);
                if(data_RLE[i][j].Cb_value[m]==0) break;
            }
        }
    }
/*------------------Cr------------------*/
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            //DC_Cr
            category = get_category(data_QUAN[i][j].Cr[0][0]);
            //print Huffman code
            fprintf(fp_temp_txt,"%s",huffman_C_DC[category].code_word_string);

            //DC值轉二進位後print------
            value = data_QUAN[i][j].Cr[0][0];

            for(l=0;l<256;l++){
                DC_string[256] = 0;
            }

            if(value < 0){ //如果值是負的，先變成正的，再把0和1對調(1010->0101)
                value = -value;
                for(k=0;value>0;k++){
                    if(value%2==1){
                        DC_string[k]=0;
                    }
                    if(value%2==0){
                        DC_string[k]=1;
                    }
                    value=value/2;
                }

                for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }

            else{
                for(k=0;value>0;k++){
                    DC_string[k]=value%2;
                    value=value/2;
                }
                 for(k=k-1;k>=0;k--){
                    fprintf(fp_temp_txt,"%d",DC_string[k]);
                 }
            }
            //AC_Cr
            for(m=1;m<64;m++){
                category = get_category(data_RLE[i][j].Cr_value[m]);
                run = data_RLE[i][j].Cr_zero_num[m];
                runsize = run*10 + category;
                fprintf(fp_temp_txt,"%s",huffman_C_AC[runsize].code_word_string);
                if(data_RLE[i][j].Cr_value[m]==0) break;
            }
        }
    }
/*-------------------------------------------*/

    fclose(fp_temp_txt);
}

int main(int argc, char **argv){

	FILE *fp_in;
	FILE *fp_out;

/*open*/
    printf("open...\n");
	fp_in = fopen(argv[1],"rb");
	fp_out = fopen(argv[2],"wb");
    //fp_in = fopen("input.bmp","rb");
	//fp_out = fopen("output.bmp","wb");

/*read*/
	//read header
	printf("read header...\n");
	Bitmap bmpheader;
	read_header(fp_in, &bmpheader);

	//取出width和height
	int H = bmpheader.height;
	int W = bmpheader.width;

	ImgRGB **data_RGB = malloc_2D_ImgRGB(H,W);
    ImgYCbCr **data_YCbCr = malloc_2D_ImgYCbCr(H/8,W/8);
    ImgDCT **data_DCT = malloc_2D_ImgDCT(H/8,W/8);
    ImgQUAN **data_QUAN = malloc_2D_ImgQUAN(H/8,W/8);
    ImgZigZag **data_ZigZag = malloc_2D_ImgZigZag(H/8,W/8);
    ImgRLE **data_RLE = malloc_2D_ImgRLE(H/8,W/8);

	//skip paddings at the end of each image line
	int skip = (4-W*3%4);
	if (skip == 4) skip = 0;
	char skip_buf[3] = {0,0,0};

	// 8x8, if not multiples of 8, then bitmap padded, i.e. one more block
	int block_H = H / 8;
	if (H % 8 != 0) block_H++;
	int block_W = W / 8;
	if (W % 8 != 0) block_W++;

	printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W*H * 3);
	printf("Image line skip=%d bytes\n", skip);
	printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W*block_H);
	printf("\nLoading BMP data: RRGGBB RRGGBB ... (BGR format in BMP file)\n");

	//read data
	read_data(fp_in, data_RGB, H, W, skip);
	fclose(fp_in);

	//寫出原始大小
	FILE *fp_RGB_txt = fopen("RGB.txt","wb");
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].B);
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].G);
            fprintf(fp_RGB_txt,"%d,",data_RGB[i][j].R);
        }
    }
    fclose(fp_RGB_txt);

/*色度轉換*/
    YCbCr(data_RGB, data_YCbCr, H/8, W/8);

/*DCT*/
    DCT(data_YCbCr, data_DCT, H/8, W/8);

/*量化*/
    QUAN(data_QUAN, data_DCT, H/8, W/8);

/*ZigZag*/
    ZigZag(data_ZigZag, data_QUAN, H/8, W/8);

/*DPCM*/
    DPCM(data_QUAN, H/8, W/8);

/*RLE*/
    RLE(data_RLE, data_ZigZag, H/8, W/8);

    //middle product
    middle_product(data_RLE, data_QUAN, H/8, W/8);

/*Huffman*/
    huffman(data_RLE, data_QUAN, H/8, W/8);

/*----decoder----*/

/*IRLE*/
    IRLE(data_RLE, data_ZigZag, H/8, W/8);

/*IDPCM*/
    IDPCM(data_QUAN, H/8, W/8);

/*IZigZag*/
    IZigZag(data_ZigZag, data_QUAN, H/8, W/8);

/*反量化*/
    IQUAN(data_QUAN, data_DCT, H/8, W/8);

/*IDCT*/
    IDCT(data_YCbCr, data_DCT, H/8, W/8);

/*反色度轉換*/
    IYCbCr(data_RGB, data_YCbCr, H/8, W/8);

/*write*/
    write_header(fp_out, &bmpheader);
    write_data(fp_out, data_RGB, H, W, skip);

/*free pointer*/
	free(data_RGB);
	free(data_YCbCr);
	free(data_DCT);
	free(data_QUAN);
	free(data_ZigZag);
	free(data_RLE);
}


