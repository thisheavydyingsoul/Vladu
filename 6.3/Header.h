#include "glew.h"
#include "glut.h"
#include "stb_image.h"
#pragma warning (disable : 4996)
#pragma pack(1)
typedef struct
{
	GLbyte identsize; //Размер поля ID,который
	//соответствует заголовку(0)
		GLbyte colorMapType; // 0 = None, 1 =
	//paletted
		GLbyte imageType; // 0 = none,1 =
	//indexed, 2 = rgb, 3 = grey, +8 = rle
		unsigned short colorMapStart; //
	//Первая позици цветной карты
		unsigned short colorMapLength; // Количество цветов
	unsigned char colorMapBits; // Количество бит
	unsigned short xstart; // начало координаты X
	unsigned short ystart; // начало координаты Y
	unsigned short width; // ширина в пикселях
	unsigned short height; // высота в пикселях
	GLbyte bits; // количество бит
	//на пикселе(8 16, 24, 32)
		GLbyte descriptor; // Дескриптор
	//изображения
} TGAHEADER;
#pragma pack(8)
/////////////////////////////////////////////////////
////////////
// Распределяет память и загружает биты файла targa.
//Возвращает
//// указатель на новые буфер, высоту и ширину текстуры, формат
//// данных OpenGL. Вызывает free() для освобождения
//буфера
//// после завершения. Работает только с простыми унифицированными
//// файлами targas с 8-, 24- или 32-битовым цветом,
//без палитр,
// без группового кодирования
GLbyte* gltLoadTGA(const char* szFileName, GLint
	* iWidth, GLint* iHeight, GLint* iComponents, GLenum
	* eFormat)
{
	FILE* pFile; // Указатель файла
	TGAHEADER tgaHeader; // Заголовок файла
	//TGA
		unsigned long lImageSize; // Размер
	//изображения в байтах
		short sDepth; // Размер пикселя
	;
	GLbyte* pBits = NULL; // Указатель на
	//биты
		// Значения по умолчанию/значения при сбое
		* iWidth = 0;
	*iHeight = 0;
	*eFormat = GL_BGR_EXT;
	*iComponents = GL_RGB8;

	// Пытаемся открыть файл
	pFile = fopen(szFileName, "rb");
	if (pFile == NULL)
		return NULL;

	// Считываем заголовок (двоичный)
	fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1,
		pFile);

	// Обращение байтов при переходе между обратным и
	//прямым
		// порядком битов
#ifdef __APPLE__
		BYTE_SWAP(tgaHeader.colorMapStart);
	BYTE_SWAP(tgaHeader.colorMapLength);
	BYTE_SWAP(tgaHeader.xstart);
	BYTE_SWAP(tgaHeader.ystart);
	BYTE_SWAP(tgaHeader.width);
	BYTE_SWAP(tgaHeader.height);
#endif

	// Получаем ширину, высоту и глубину текстуры
	* iWidth = tgaHeader.width;
	*iHeight = tgaHeader.height;
	sDepth = tgaHeader.bits / 8;

	// Проверки приемлемости. Очень просто: я понимаю
	//только
		// 8-, 24- или 32-битовые файлы targa
		if (tgaHeader.bits != 8 && tgaHeader.bits != 24 &&
			tgaHeader.bits != 32)
			return NULL;
	// Расчет размера буфера изображения
	lImageSize = tgaHeader.width * tgaHeader.height *
		sDepth;

	// Распределение памяти и проверка успешности
	pBits = (GLbyte*)malloc(lImageSize *
		sizeof(GLbyte));
	if (pBits == NULL)
		return NULL;

	// Считывание битов
	// Проверка на наличие ошибок чтения. Здесь должны //
	//отлавливаться групповое кодирование или другие
		// форматы, которые не нужно распознавать
		if (fread(pBits, lImageSize, 1, pFile) != 1)
		{
			free(pBits);
			return NULL;
		}

		// Устанавливается формат, ожидаемый OpenGL
		switch (sDepth)
		{
		case 3: // Наиболее вероятный случай
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


	// Работа с файлом закончена
	fclose(pFile);

	// Возвращает указатель на данные изображения
	return pBits;
}

GLint gltWriteTGA(const char* szFileName)
{
	FILE* pFile; // Указатель файла
	TGAHEADER tgaHeader; // Заголовок файла
	//TGA
		unsigned long lImageSize; // размер изображения
//	в байтах
		GLbyte* pBits = NULL; // Указатель на биты
	GLint iViewport[4]; // Размер поля просмотра в пикселях
	GLenum lastBuffer; // Память для хранения текущих настроек буфера чтения
	// Получает размеры поля просмотра
	glGetIntegerv(GL_VIEWPORT, iViewport);
	// Насколько большим будет изображение (файлы
	//targa плотно упакованы)
		lImageSize = iViewport[2] * 3 * iViewport[3];
	// Распределяет блок. Если это не работает, возвращаемся домой
	pBits = (GLbyte*)malloc(lImageSize);
	if (pBits == NULL)
		return 0;
	// Считывает биты из буфера цвета
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	// Получает текущие установки буфера чтения и записывает их.
   // Переключается на передний буфер и выполняет операцию чтения.
   //В конце концов восстанавливает состояние буфера
	//чтения
		glGetIntegerv(GL_READ_BUFFER,
			(GLint*)&lastBuffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, iViewport[2], iViewport[3],
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);
	glReadBuffer(lastBuffer);

	// Инициализирует заголовок файла Targa
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

	// Обращение байтов при переходе между обратным и
	//рямым II порядком битов
#ifdef __APPLE__
		BYTE_SWAP(tgaHeader.colorMapStart);
	BYTE_SWAP(tgaHeader.colorMapLength);
	BYTE_SWAP(tgaHeader.xstart);
	BYTE_SWAP(tgaHeader.ystart);
	BYTE_SWAP(tgaHeader.width);
	BYTE_SWAP(tgaHeader.height);
#endif

	// Пытается открыть файл
	pFile = fopen(szFileName, "wb");
	if (pFile == NULL)
	{
		free(pBits); // Освобождает буфер и возвращает ошибку
		return 0;
	}

	// Записывает заголовок
	fwrite(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// Записывает заголовок
	fwrite(pBits, lImageSize, 1, pFile);

	// Освобождает временный буфер и закрывает файл
	free(pBits);
	fclose(pFile);

	// Успех!
	return 1;
}
