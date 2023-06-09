#include "glew.h"
#include "glut.h"
#include "stb_image.h"
#pragma warning (disable : 4996)
#pragma pack(1)
typedef struct
{
	GLbyte identsize; //������ ���� ID,�������
	//������������� ���������(0)
		GLbyte colorMapType; // 0 = None, 1 =
	//paletted
		GLbyte imageType; // 0 = none,1 =
	//indexed, 2 = rgb, 3 = grey, +8 = rle
		unsigned short colorMapStart; //
	//������ ������ ������� �����
		unsigned short colorMapLength; // ���������� ������
	unsigned char colorMapBits; // ���������� ���
	unsigned short xstart; // ������ ���������� X
	unsigned short ystart; // ������ ���������� Y
	unsigned short width; // ������ � ��������
	unsigned short height; // ������ � ��������
	GLbyte bits; // ���������� ���
	//�� �������(8 16, 24, 32)
		GLbyte descriptor; // ����������
	//�����������
} TGAHEADER;
#pragma pack(8)
/////////////////////////////////////////////////////
////////////
// ������������ ������ � ��������� ���� ����� targa.
//����������
//// ��������� �� ����� �����, ������ � ������ ��������, ������
//// ������ OpenGL. �������� free() ��� ������������
//������
//// ����� ����������. �������� ������ � �������� ����������������
//// ������� targas � 8-, 24- ��� 32-������� ������,
//��� ������,
// ��� ���������� �����������
GLbyte* gltLoadTGA(const char* szFileName, GLint
	* iWidth, GLint* iHeight, GLint* iComponents, GLenum
	* eFormat)
{
	FILE* pFile; // ��������� �����
	TGAHEADER tgaHeader; // ��������� �����
	//TGA
		unsigned long lImageSize; // ������
	//����������� � ������
		short sDepth; // ������ �������
	;
	GLbyte* pBits = NULL; // ��������� ��
	//����
		// �������� �� ���������/�������� ��� ����
		* iWidth = 0;
	*iHeight = 0;
	*eFormat = GL_BGR_EXT;
	*iComponents = GL_RGB8;

	// �������� ������� ����
	pFile = fopen(szFileName, "rb");
	if (pFile == NULL)
		return NULL;

	// ��������� ��������� (��������)
	fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1,
		pFile);

	// ��������� ������ ��� �������� ����� �������� �
	//������
		// �������� �����
#ifdef __APPLE__
		BYTE_SWAP(tgaHeader.colorMapStart);
	BYTE_SWAP(tgaHeader.colorMapLength);
	BYTE_SWAP(tgaHeader.xstart);
	BYTE_SWAP(tgaHeader.ystart);
	BYTE_SWAP(tgaHeader.width);
	BYTE_SWAP(tgaHeader.height);
#endif

	// �������� ������, ������ � ������� ��������
	* iWidth = tgaHeader.width;
	*iHeight = tgaHeader.height;
	sDepth = tgaHeader.bits / 8;

	// �������� ������������. ����� ������: � �������
	//������
		// 8-, 24- ��� 32-������� ����� targa
		if (tgaHeader.bits != 8 && tgaHeader.bits != 24 &&
			tgaHeader.bits != 32)
			return NULL;
	// ������ ������� ������ �����������
	lImageSize = tgaHeader.width * tgaHeader.height *
		sDepth;

	// ������������� ������ � �������� ����������
	pBits = (GLbyte*)malloc(lImageSize *
		sizeof(GLbyte));
	if (pBits == NULL)
		return NULL;

	// ���������� �����
	// �������� �� ������� ������ ������. ����� ������ //
	//������������� ��������� ����������� ��� ������
		// �������, ������� �� ����� ������������
		if (fread(pBits, lImageSize, 1, pFile) != 1)
		{
			free(pBits);
			return NULL;
		}

		// ��������������� ������, ��������� OpenGL
		switch (sDepth)
		{
		case 3: // �������� ��������� ������
			*eFormat = GL_BGR_EXT;
			*iComponents = GL_RGB8;
			break;
		case 4:
			*eFormat = GL_BGRA_EXT;
			*iComponents = GL_RGBA8;
			break;
		case 1:
			*eFormat = GL_LUMINANCE;
			*iComponents = GL_LUMINANCE8;
			break;
		};


	// ������ � ������ ���������
	fclose(pFile);

	// ���������� ��������� �� ������ �����������
	return pBits;
}

GLint gltWriteTGA(const char* szFileName)
{
	FILE* pFile; // ��������� �����
	TGAHEADER tgaHeader; // ��������� �����
	//TGA
		unsigned long lImageSize; // ������ �����������
//	� ������
		GLbyte* pBits = NULL; // ��������� �� ����
	GLint iViewport[4]; // ������ ���� ��������� � ��������
	GLenum lastBuffer; // ������ ��� �������� ������� �������� ������ ������
	// �������� ������� ���� ���������
	glGetIntegerv(GL_VIEWPORT, iViewport);
	// ��������� ������� ����� ����������� (�����
	//targa ������ ���������)
		lImageSize = iViewport[2] * 3 * iViewport[3];
	// ������������ ����. ���� ��� �� ��������, ������������ �����
	pBits = (GLbyte*)malloc(lImageSize);
	if (pBits == NULL)
		return 0;
	// ��������� ���� �� ������ �����
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	// �������� ������� ��������� ������ ������ � ���������� ��.
   // ������������� �� �������� ����� � ��������� �������� ������.
   //� ����� ������ ��������������� ��������� ������
	//������
		glGetIntegerv(GL_READ_BUFFER,
			(GLint*)&lastBuffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, iViewport[2], iViewport[3],
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);
	glReadBuffer(lastBuffer);

	// �������������� ��������� ����� Targa
	tgaHeader.identsize = 0;
	tgaHeader.colorMapType = 0;
	tgaHeader.imageType = 2;
	tgaHeader.colorMapStart = 0;
	tgaHeader.colorMapLength = 0;
	tgaHeader.colorMapBits = 0;
	tgaHeader.xstart = 0;
	tgaHeader.ystart = 0;
	tgaHeader.width = iViewport[2];
	tgaHeader.height = iViewport[3];
	tgaHeader.bits = 24;
	tgaHeader.descriptor = 0;

	// ��������� ������ ��� �������� ����� �������� �
	//����� II �������� �����
#ifdef __APPLE__
		BYTE_SWAP(tgaHeader.colorMapStart);
	BYTE_SWAP(tgaHeader.colorMapLength);
	BYTE_SWAP(tgaHeader.xstart);
	BYTE_SWAP(tgaHeader.ystart);
	BYTE_SWAP(tgaHeader.width);
	BYTE_SWAP(tgaHeader.height);
#endif

	// �������� ������� ����
	pFile = fopen(szFileName, "wb");
	if (pFile == NULL)
	{
		free(pBits); // ����������� ����� � ���������� ������
		return 0;
	}

	// ���������� ���������
	fwrite(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// ���������� ���������
	fwrite(pBits, lImageSize, 1, pFile);

	// ����������� ��������� ����� � ��������� ����
	free(pBits);
	fclose(pFile);

	// �����!
	return 1;
}
