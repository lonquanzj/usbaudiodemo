
#include "time.h"
#include "myaes.h"
#include "stdlib.h"

//#define byte unsigned char 

#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16 //!< Block size in number of bytes.

#define KEYBITS 128 //!< Use AES128.
#define ROUNDS 10 //!< Number of rounds.
#define KEYLENGTH 16 //!< Key length in number of bytes.

typedef unsigned char  byte;
typedef unsigned char  uint8_t;
byte  block1[ 256 ]; //!< Workspace 1.
byte  block2[ 256 ]; //!< Worksapce 2.


byte  * powTbl; //!< Final location of exponentiation lookup table.
byte  * logTbl; //!< Final location of logarithm lookup table.
byte  * sBox; //!< Final location of s-box.
byte  * sBoxInv; //!< Final location of inverse s-box.
byte  * expandedKey; //!< Final location of expanded key.
bool g_IsVad = false;
//byte expandedKey[16]; //!< Final location of expanded key.

void CalcPowLog( byte * powTbl, byte * logTbl )
{
        byte  i = 0;
        byte  t = 1;

        do {
		// Use 0x03 as root for exponentiation and logarithms.
        	powTbl[i] = t;
       		logTbl[t] = i;
       		i++;

		// Muliply t by 3 in GF(2^8).
       		t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
       	} while( t != 1 ); // Cyclic properties ensure that i < 255.
       	
       	powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}

void CalcSBox( byte * sBox )
{
	byte  i, rot;
	byte  temp;
	byte  result;

	// Fill all entries of sBox[].
	i = 0;
	do 
	{
		// Inverse in GF(2^8).
		if( i > 0 ) 
		{
			temp = powTbl[ 255 - logTbl[i] ];
		} 
		else 
		{
			temp = 0;
		}

		// Affine transformation in GF(2).
		result = temp ^ 0x63; // Start with adding a vector in GF(2).
		for( rot = 0; rot < 4; rot++ ) 
		{
			// Rotate left.
			temp = (temp<<1) | (temp>>7);

			// Add rotated byte in GF(2).
			result ^= temp;
		}

		// Put result in table.
		sBox[i] = result;

	} while( ++i != 0 );
}	

void CalcSBoxInv( byte * sBox, byte * sBoxInv )
{
	byte  i = 0;
	byte  j = 0;

	// Iterate through all elements in sBoxInv using  i.
	do {
		// Search through sBox using j.
		//cleardog();
		do {
			// Check if current j is the inverse of current i.
			if( sBox[ j ] == i ) {
				// If so, set sBoxInc and indicate search finished.
				sBoxInv[ i ] = j;
				j = 255;
			}
		} while( ++j != 0 );
	} while( ++i != 0 );
}

void CycleLeft( byte * row )
{
	// Cycle 4 bytes in an array left once.
	byte  temp = row[0];
	row[0] = row[1];
	row[1] = row[2];
	row[2] = row[3];
	row[3] = temp;
}

void InvMixColumn( byte * column )
{
	byte  r0, r1, r2, r3;

	r0 = column[1] ^ column[2] ^ column[3];
	r1 = column[0] ^ column[2] ^ column[3];
	r2 = column[0] ^ column[1] ^ column[3];
	r3 = column[0] ^ column[1] ^ column[2];

 	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
 	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	r0 ^= column[0] ^ column[1];
	r1 ^= column[1] ^ column[2];
	r2 ^= column[2] ^ column[3];
	r3 ^= column[0] ^ column[3];

   	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
   	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	r0 ^= column[0] ^ column[2];
	r1 ^= column[1] ^ column[3];
	r2 ^= column[0] ^ column[2];
	r3 ^= column[1] ^ column[3];

   	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
   	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
   	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
   	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

	column[0] ^= column[1] ^ column[2] ^ column[3];
	r0 ^= column[0];
	r1 ^= column[0];
	r2 ^= column[0];
	r3 ^= column[0];

	column[0] = r0;
	column[1] = r1;
	column[2] = r2;
	column[3] = r3;
}

byte Multiply( uint8_t num, uint8_t factor )
{
	byte mask = 1;
	byte result = 0;

	while( mask != 0 ) {
		// Check bit of factor given by mask.
		if( mask & factor ) {
			// Add current multiple of num in GF(2).
			result ^= num;
		}

		// Shift mask to indicate next bit.
		mask <<= 1;

		// Double num.
       		num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);
	}

	return result;
}

byte DotProduct( uint8_t* vector1, uint8_t * vector2 )
{
	byte result = 0;

	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1  , *vector2   );

	return result;
}

void MixColumn( byte * column )
{
	byte  row[8] = {
		0x02, 0x03, 0x01, 0x01,
		0x02, 0x03, 0x01, 0x01
	}; // Prepare first row of matrix twice, to eliminate need for cycling.

	byte  result[4];
	
	// Take dot products of each matrix row and the column vector.
	result[0] = DotProduct( row+0, column );
	result[1] = DotProduct( row+3, column );
	result[2] = DotProduct( row+2, column );
	result[3] = DotProduct( row+1, column );

	// Copy temporary result to original column.
	column[0] = result[0];
	column[1] = result[1];
	column[2] = result[2];
	column[3] = result[3];
}

void SubBytes( byte * bytes, byte count )
{
	do {
		*bytes = sBox[ *bytes ]; // Substitute every byte in state.
		bytes++;
	} while( --count );
}

void InvSubBytesAndXOR( byte * bytes, byte * key, byte count )
{
	do {
//		*bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
		*bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
		bytes++;
		key++;
	} while( --count );
}

void InvShiftRows( byte * state )
{
	byte temp;

	// Note: State is arranged column by column.

	// Cycle second row right one time.
	temp = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = temp;

	// Cycle third row right two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;

	// Cycle fourth row right three times, ie. left once.
	temp = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = temp;
}

void ShiftRows( byte * state )
{
	byte temp;

	// Note: State is arranged column by column.

	// Cycle second row left one time.
	temp = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = temp;

	// Cycle third row left two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;

	// Cycle fourth row left three times, ie. right once.
	temp = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = temp;
}

void InvMixColumns( byte * state )
{
	InvMixColumn( state + 0*4 );
	InvMixColumn( state + 1*4 );
	InvMixColumn( state + 2*4 );
	InvMixColumn( state + 3*4 );
}

void MixColumns( byte * state )
{
	MixColumn( state + 0*4 );
	MixColumn( state + 1*4 );
	MixColumn( state + 2*4 );
	MixColumn( state + 3*4 );
}

void XORBytes( byte * bytes1, byte * bytes2, byte count )
{
	do {
		*bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
		bytes1++;
		bytes2++;
	} while( --count );
}

void CopyBytes( byte * to, byte * from, byte count )
{
	do {
		*to = *from;
		to++;
		from++;
	} while( --count );
}

void KeyExpansion( byte * expandedKey )
{
	byte  temp[4];
	byte i;
	byte  Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.
	
	uint8_t  *key;
	uint8_t  a[16];
	key=a;
	//以下为加解密密码，共16字节。可以选择任意值
	/*key[0]=0x30;
	key[1]=0x30;
	key[2]=0x30;
	key[3]=0x30;
	key[4]=0x30;
	key[5]=0x30;
	key[6]=0x30;
	key[7]=0x30;
	key[8]=0x30;
	key[9]=0x30;
	key[10]=0x30;
	key[11]=0x30;
	key[12]=0x30;
	key[13]=0x30;
	key[14]=0x30;
	key[15]=0x30;*/

	if (g_IsVad)  //虚拟声卡上用的，不用的Key值。
	{
		for( i=0; i<16; i++)
		{
			key[i] = 0x68 + i * 5;
		}
	}
	else   //真正声卡上用的。
	{
		for( i=0; i<16; i++)
		{
			key[i] = 0x30 + i * 3;
		}
	}
	////////////////////////////////////////////

	// Copy key to start of expanded key.
	i = KEYLENGTH;
	do {
		*expandedKey = *key;
		expandedKey++;
		key++;
	} while( --i );

	// Prepare last 4 bytes of key in temp.
	expandedKey -= 4;
	temp[0] = *(expandedKey++);
	temp[1] = *(expandedKey++);
	temp[2] = *(expandedKey++);
	temp[3] = *(expandedKey++);

	// Expand key.
	i = KEYLENGTH;
	while( i < BLOCKSIZE*(ROUNDS+1) ) {
		// Are we at the start of a multiple of the key size?
		if( (i % KEYLENGTH) == 0 ) {
			CycleLeft( temp ); // Cycle left once.
			SubBytes( temp, 4 ); // Substitute each byte.
			XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
			*Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
		}

		// Keysize larger than 24 bytes, ie. larger that 192 bits?
		#if KEYLENGTH > 24
		// Are we right past a block size?
		else if( (i % KEYLENGTH) == BLOCKSIZE ) {
			SubBytes( temp, 4 ); // Substitute each byte.
		}
		#endif

		// Add bytes in GF(2) one KEYLENGTH away.
		XORBytes( temp, expandedKey - KEYLENGTH, 4 );

		// Copy result to current 4 bytes.
		*(expandedKey++) = temp[ 0 ];
		*(expandedKey++) = temp[ 1 ];
		*(expandedKey++) = temp[ 2 ];
		*(expandedKey++) = temp[ 3 ];

		i += 4; // Next 4 bytes.
	}	
}

void InvCipher( byte * block, byte * expandedKey )
{
	byte round = ROUNDS-1;
	expandedKey += BLOCKSIZE * ROUNDS;

	XORBytes( block, expandedKey, 16 );
	expandedKey -= BLOCKSIZE;

	do {
		InvShiftRows( block );
		InvSubBytesAndXOR( block, expandedKey, 16 );
		expandedKey -= BLOCKSIZE;
		InvMixColumns( block );
	} while( --round );

	InvShiftRows( block );
	InvSubBytesAndXOR( block, expandedKey, 16 );
}

void Cipher( byte * block, byte * expandedKey )	   //完成一个块(16字节，128bit)的加密
{
	byte round = ROUNDS-1;

	XORBytes( block, expandedKey, 16 );
	expandedKey += BLOCKSIZE;

	do {
		SubBytes( block, 16 );
		ShiftRows( block );
		MixColumns( block );
		XORBytes( block, expandedKey, 16 );
		expandedKey += BLOCKSIZE;
	} while( --round );

	SubBytes( block, 16 );
	ShiftRows( block );
	XORBytes( block, expandedKey, 16 );
}

void aesInit( uint8_t * tempbuf )
{
	powTbl = block1;
	logTbl = block2;
	CalcPowLog( powTbl, logTbl );

	sBox = tempbuf;
	CalcSBox( sBox );

	expandedKey = block1;  //至此block1用来存贮密码表
	KeyExpansion( expandedKey );
	
	sBoxInv = block2; // Must be block2. block2至此开始只用来存贮SBOXINV
	CalcSBoxInv( sBox, sBoxInv );
}	

//对一个16字节块解密,参数buffer是解密密缓存，chainBlock是要解密的块
void aesDecrypt(uint8_t * chainBlock )
{
	//byte  temp[ BLOCKSIZE ];
	uint8_t buffer[16]; 

	//CopyBytes( temp, buffer, BLOCKSIZE );
	CopyBytes(buffer,chainBlock,BLOCKSIZE);
	InvCipher( buffer, expandedKey );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}

//对一个16字节块完成加密，参数buffer是加密缓存，chainBlock是要加密的块
void aesEncrypt(uint8_t * chainBlock )
{
    uint8_t buffer[16]; 
	CopyBytes( buffer, chainBlock, BLOCKSIZE );
	//XORBytes( buffer, chainBlock, BLOCKSIZE );
	Cipher( buffer, expandedKey );
	CopyBytes( chainBlock, buffer, BLOCKSIZE );
}


//取出加密数据。
void GetAESData(byte * outbuf, const byte * inbuf/*, bool isVad*/)
{
	byte  inTemp[16];
	byte  initTemp[256];
	byte     i, MaxD, MinD, nIndex1, nIndex2;

	//先取原值
	for( i=0; i<4; i++)
	{
		inTemp[i] = inbuf[i];
	}

	//倒序
	for( i=0; i<4; i++)
	{
		inTemp[i+4] = inbuf[4-i-1];
	}

	//以前8个的数值对8求模，做为下标
	for(i=0; i<8; i++)
	{
		byte j = inTemp[i] % 8;
		inTemp[i+8] = inTemp[j];
	}

//	g_IsVad = isVad;

	//初始化。
	aesInit(initTemp);

	//取出加密数据
	aesEncrypt(inTemp);

	//byte outTemp[16];
	//AES_Encrypt(inTemp);


	//取出最大值与最小值
	 MaxD =inTemp[0];
	 MinD =inTemp[0];

	for( i=1; i<16; i++)
	{
		if (MaxD < inTemp[i])
			MaxD = inTemp[i];
		else if (MinD > inTemp[i])
			MinD = inTemp[i];
	}

	//返回值中间值的下标
	 nIndex1 = MinD % 8;
	 nIndex2 = 8 + MaxD % 8;

	/* CString ss;
	 ss.Format(_T("Max=%d, Min=%d"),  MinD, MaxD);
	 AfxMessageBox(ss);*/

	//组成返回值
	//以最小值与最大值填充，由最大与最小取得下标，返回。
	outbuf[0] = MinD * 2 + 1;
	outbuf[1] = inTemp[nIndex1] + 3;
	outbuf[2] = inTemp[nIndex2] - 3;
	outbuf[3] = MaxD / 2;

}

unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

//获取随机的四个byte
void GetDataNow(byte * buf)
{
	time_t nowt;
	time(&nowt);

	int nCount = GetTickCount();

	int i = rand();

	//byte* pNow = (byte*)&nowt;
	
	byte inData[4];
	inData[0] = nowt & 0xFF;
	inData[1] = nCount & 0xFF;
	inData[2] = i & 0xFF;

    int idata = inData[0] + inData[2] + inData[1];
	inData[3] = idata & 0xFF;

	GetAESData(buf, (byte*)&idata);
}

void GetDataBySize(byte* buf, int size)
{
	int i;
	for(i = 0; i < size-size%4; i+=4){
		GetDataNow(buf+i);
	}
	int temp = size%4;
	if(temp){
		while(temp--){
			buf[i++]  = GetTickCount();
		}
	}
}
