#pragma once

#include <iostream>
#include <map>
#include <string>
#include <codecvt>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#include <GLFW/glfw3.h>
#define Char char

//#include "bmp.h"

namespace ifcre {
	static std::string convertUtf8ToGBK(std::string const& strUtf8)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
		std::wstring wTemp = cutf8.from_bytes(strUtf8);
#ifdef _MSC_VER
		std::locale loc("zh-CN");
#else
		std::locale loc("zh_CN.GB18030");
#endif
		const wchar_t* pwszNext = nullptr;
		char* pszNext = nullptr;
		mbstate_t state = {};

		std::vector<char> buff(wTemp.size() * 2);
		int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
			(loc).out(state,
				wTemp.data(), wTemp.data() + wTemp.size(), pwszNext,
				buff.data(), buff.data() + buff.size(), pszNext);

		if (std::codecvt_base::ok == res) {
			return std::string(buff.data(), pszNext);
		}
		return "";
	}
	struct Character {
		GLuint TextureID;
		glm::ivec2 Size;
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;
	};
	struct TextData {
		FT_Library ft;
		FT_Face face;
		std::map<Char, Character> Characters;
		GLuint textVAO, textVBO;
		GLfloat vertices[6][4];
		TextData(std::string fontfilepath) {
			if (FT_Init_FreeType(&ft))
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
			if (FT_New_Face(ft, fontfilepath.c_str(), 0, &face))
				std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
			// Set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 48); // ���������С
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // �����ֽڶ�������

			map_ascii_text();

			glGenVertexArrays(1, &textVAO);
			glGenBuffers(1, &textVBO);
			glBindVertexArray(textVAO);
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		~TextData()
		{
			Characters.clear();
		}
		void map_ascii_text() {
			for (GLubyte c = 0; c < 128; c++) {
				if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {  // �����ַ�������
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// Generate texture // ��������
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// Set texture options // ��������ѡ��
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// �洢�ַ���֮��ʹ��
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				Characters.insert(std::pair<Char, Character>(c, character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			FT_Done_Face(face); // ����FreeType����Դ
			FT_Done_FreeType(ft);
		}

		void draw_text(Char c, glm::vec3& pos, GLfloat scale) {
			Character& ch = Characters[c];
			GLfloat xpos = pos.x + ch.Bearing.x * scale;
			GLfloat ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			pos.x += (ch.Advance >> 6) * scale;
		}

		void render_text(std::string text, glm::vec3 pos, GLfloat scale) {
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(textVAO);
			for (auto c : text) {
				draw_text(c, pos, scale);
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_BLEND);
		}
	};

	struct Character2 {
		Character2() {
			x0 = 0;
			y0 = 0;
			x1 = 0;
			y1 = 0;
			offsetX = 0;
			offsetY = 0;
		}

		bool generated() { // �Ƿ��Ѿ�����
			return x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0;
		}
		// �洢��ǰ�ַ��������ϵ�����λ��
		uint32_t x0 : 10;
		uint32_t y0 : 10;
		uint32_t x1 : 10;
		uint32_t y1 : 10;
		uint32_t offsetX : 10;
		uint32_t offsetY : 10;
	};
	struct float5 {
		float x, y, z, u, v;
	};
	struct mappingStruct {
		std::pair<GLuint, GLuint> vaovbo;
		unsigned vertsize;
	};
	struct TextureFont {
	protected:
		Character2 _character[1 << 16]; // һ���������ֽ�,��Լ65536��
		FT_Library _library;
		FT_Face _face;
		unsigned _textureId;
		int _yStart;
		int _xStart;
		int _fontSize;
		bool vboflag = false;

		std::unordered_map<std::wstring, mappingStruct> text_handle;


		typedef float TextVertex[4];
		TextVertex vert[1024];
		//Vector<Vector<float>> vert;
	public:

		//void init_bmp() {
		//	BMP_BUFFER one_bmp_BUFFER;
		//	//��ʼ��buffer_k
		//	one_bmp_BUFFER.mbfType = 0X4d42;
		//	one_bmp_BUFFER.hand.bfOffBits = 54;
		//	one_bmp_BUFFER.hand.bfReserved1 = 0;
		//	one_bmp_BUFFER.hand.bfReserved2 = 0;
		//	one_bmp_BUFFER.hand.bfSize = 1032 * 672 * 3 + 40 + 14;
		//	one_bmp_BUFFER.BUFFER = new DATA[1032 * 672];
		//	one_bmp_BUFFER.info.biBitCount = 24;
		//	one_bmp_BUFFER.info.biClrImportant = 0;
		//	one_bmp_BUFFER.info.biClrUsed = 0;
		//	one_bmp_BUFFER.info.biCompression = 0;
		//	one_bmp_BUFFER.info.biHeight = 1032;
		//	one_bmp_BUFFER.info.biPlanes = 1;
		//	one_bmp_BUFFER.info.biSize = 40;
		//	one_bmp_BUFFER.info.biSizeImage = 1032 * 672 * 3;
		//	one_bmp_BUFFER.info.biWidth = 672;
		//	one_bmp_BUFFER.info.biXPelsPerMeter = 0;
		//	one_bmp_BUFFER.info.biYPelsPerMeter = 0;

		//	//ȫ��ʹ�ð�ɫ����
		//	int x = 0;
		//	int y = 0;
		//	for (x = 0; x < 1032; x++)
		//	{
		//		for (y = 0; y < 672; y++)
		//		{
		//			one_bmp_BUFFER.BUFFER[x * 672 + y].red = 255;
		//			one_bmp_BUFFER.BUFFER[x * 672 + y].blue = 255;
		//			one_bmp_BUFFER.BUFFER[x * 672 + y].green = 255;
		//		}
		//	}
		//	wchar_t wszString[] = L"������";
		//	WORD word;


		//	int bitmap_width_sum = -672;//������ͼ����X����ʼλ��
		//	int bitmap_high_sum = 50;//������ͼ����Y����ʼλ��
		//	int wszStringLen = wcslen(wszString);
		//	printf("wszStringLen: %d\n", wszStringLen);
		//	int k = 0;

		//	for (k = 0; k < wszStringLen; k++)
		//	{
		//		memcpy(&word, wszString + k, 2);
		//		//ת����λͼ
		//		FT_Load_Glyph(_face, FT_Get_Char_Index(_face, word), FT_LOAD_DEFAULT);

		//		FT_Error error = 0;
		//		FT_Glyph glyph;
		//		error = FT_Get_Glyph(_face->glyph, &glyph);
		//		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		//		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		//		//ȡλͼ����
		//		FT_Bitmap* bitmap = &bitmap_glyph->bitmap;

		//		int i = 0;
		//		int j = 0;

		//		int font_H = bitmap->rows;
		//		int font_W = bitmap->width;

		//		for (i = 0; i < font_H; ++i)
		//		{
		//			for (j = 0; j < font_W; ++j)
		//			{
		//				if (bitmap->buffer[i * font_W + j] != 0)
		//				{
		//					//arrayColor[(i+100)*strInfo.biWidth+j+bitmap_width_sum+100].green = 0;
		//					//arrayColor[(i+100)*strInfo.biWidth+j+bitmap_width_sum+100].blue = 0;

		//					//arrayColor[(strInfo.biHeight-i)*strInfo.biWidth+j+bitmap_width_sum].green = 0;
		//					//arrayColor[(strInfo.biHeight-i)*strInfo.biWidth+j+bitmap_width_sum].blue = 0;


		//		   //��������
		//		   // one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].red=0;
		//				   // one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].blue=0;
		//				   // one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].green=0;
		//			//���ֺ���
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].red = 0;
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].blue = 0;
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].green = 0;

		//				}
		//				/*else
		//				{
		//					//��������
		//					 // one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].red=255;
		//							  //one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].blue=255;
		//							 // one_bmp_BUFFER.BUFFER[(1032-i-bitmap_width_sum)*672+j].green=255;
		//					  //���ֺ���
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].red = 255;
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].blue = 255;
		//					one_bmp_BUFFER.BUFFER[(1032 - i - bitmap_high_sum) * 672 + j + bitmap_width_sum].green = 255;

		//				}*/
		//			}
		//		}
		//		// bitmap_width_sum += bitmap->width + 10;

		//		if (bitmap_width_sum + font_W > 1032) {
		//			bitmap_width_sum = -672;
		//			bitmap_high_sum += font_H;
		//		}
		//		else {
		//			bitmap_width_sum += font_W;
		//		}
		//	}
		//	FILE* fpo1;
		//	fopen_s(&fpo1, "./one_bmp_BUFFER.bmp", "wb");
		//	fwrite(&one_bmp_BUFFER.mbfType, 1, sizeof(WORD), fpo1);
		//	fwrite(&one_bmp_BUFFER.hand, 1, sizeof(tagBITMAPFILEHEADER), fpo1);
		//	fwrite(&one_bmp_BUFFER.info, 1, sizeof(tagBITMAPINFOHEADER), fpo1);
		//	fwrite(one_bmp_BUFFER.BUFFER, 1, sizeof(DATA) * (1032 * 672), fpo1);
		//	fclose(fpo1);

		//}

		TextureFont(std::string fontfilepath, int fontSize) {
			_fontSize = fontSize;
			_xStart = 0;
			_yStart = 0;

			memset(_character, 0, sizeof(_character));
			memset(vert, 0, sizeof(vert));
			FT_Init_FreeType(&_library); // ��ʼ�������
			assert(_library != 0);

			FT_New_Face(_library, fontfilepath.c_str(), 0, &_face); // ��������
			FT_Set_Char_Size(_face, fontSize << 6, fontSize << 6, 72, 72); // ���������С
			//FT_Set_Pixel_Sizes(_face, 0, 48);

			assert(_face != 0);

			glGenTextures(1, &_textureId);
			glBindTexture(GL_TEXTURE_2D, _textureId); // ʹ���������id,���߽а�(����)

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ָ�������ķŴ�,��С�˲���ʹ�����Է�ʽ������ͼƬ�Ŵ��ʱ���ֵ��ʽ 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(
				GL_TEXTURE_2D,				//! ָ���Ƕ�άͼƬ
				0,							//! ָ��Ϊ��һ��������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
				GL_RED,						//! ������ʹ�õĴ洢��ʽ
				1024, 1024,					//! �������ߣ������ɴ�Լ 1024/16 * 1024/16 = 4096������
				0,							//! �Ƿ�ı�
				GL_RED,					 	//! ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
				GL_UNSIGNED_BYTE, 0		 	//! ������8bit����
			);							 

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		Character2* getCharacter(wchar_t ch) {
			wchar_t& word = ch;
			if (_character[word].generated()) { // ˵���ַ���û�л��Ƶ������ϣ�����л���
				if (_xStart + _fontSize > 1024) { // д��һ��,���¿�ʼ
					//this line filled, enter next line
					_xStart = 0;
					_yStart += _fontSize; // y��ʼλ��Ҫ����
				}
				FT_Load_Glyph(_face, FT_Get_Char_Index(_face, word), FT_LOAD_DEFAULT); // ��ȡ�������Ϣ
				FT_Error error = 0;
				FT_Glyph glyph;
				error = FT_Get_Glyph(_face->glyph, &glyph);
				
				FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);// Convert the glyph to a bitmap.
				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
				FT_Bitmap& bitmap = bitmap_glyph->bitmap;//This reference will make accessing the bitmap easier

				/*if (FT_Load_Char(_face, ch, FT_LOAD_RENDER)) {
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				}*/


				//���û�����ݣ���д��ֱ�ӹ�ȥ
				if (_face->glyph->bitmap.width == 0 || _face->glyph->bitmap.rows == 0) {
					//no data of this character
					_xStart += _fontSize / 2;

					_character[word].x0 = _xStart;
					_character[word].y0 = _yStart;
					_character[word].x1 = _xStart + _fontSize / 4;
					_character[word].y1 = _yStart + _fontSize / 4;
					_character[word].offsetY = _fontSize / 4;
					_character[word].offsetX = _fontSize / 4;
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, _textureId);

					_character[word].x0 = _xStart;
					_character[word].y0 = _yStart;
					_character[word].x1 = _xStart + bitmap.width;
					_character[word].y1 = _yStart + bitmap.rows;

					_character[word].offsetY = bitmap_glyph->top;
					_character[word].offsetX = bitmap_glyph->left;

					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexSubImage2D( // ��bmp����д��������
						GL_TEXTURE_2D,
						0,
						_xStart,
						_yStart,
						bitmap.width,
						bitmap.rows,
						GL_RED,
						GL_UNSIGNED_BYTE,
						bitmap.buffer
					);/*
					int s = sizeof(bitmap.buffer) / sizeof(unsigned char);
					for (int k = 0; k < s; k++) {
						std::cout << bitmap.buffer[k] << " ";
					}*/
					_xStart += bitmap.width + 1;
				}
			}
			//std::wcout << ch << L" has been generated.\n";
			//std::cout << _character[ch].x0 << " " << _character[ch].y0 << " " << _character[ch].x1 << " " << _character[ch].y1 << "\n";
			return &_character[word];
		}

		void drawText(const wchar_t* text) { //������������������������
			auto wstringtemp = std::wstring(text);
			if (text_handle.find(wstringtemp) == text_handle.end()) {
				unsigned vertsize = 0;
				float texWidth = 1024;
				float texHeight = 1024;
				float xStart = 0;
				float yStart = 0;
				unsigned nSize = wcslen(text);
				float fHeight = 0;
				//WORD word;
				for (unsigned i = 0; i < nSize; i++) {
					//memcpy(&word, text + i, 2);
					Character2* ch = getCharacter(text[i]); // ��������д�ġ��֡�

					int h = ch->y1 - ch->y0;
					//int h = 30;
					int w = ch->x1 - ch->x0;
					float offset = float(ch->offsetY);
					float offset2 = offset - float(h);
					//float offset = 0;
					float offsetx = float(ch->offsetX);

					/*point 1*/
					vert[vertsize + 0][0] = xStart;
					vert[vertsize + 0][1] = yStart + offset;
					//vert[index + 0][2] = zStart;
					vert[vertsize + 0][2] = ch->x0 / texWidth;
					vert[vertsize + 0][3] = ch->y0 / texHeight;


					/*point 2*/
					vert[vertsize + 1][0] = xStart + w;
					vert[vertsize + 1][1] = yStart + offset;
					//vert[index + 1][2] = zStart;
					vert[vertsize + 1][2] = ch->x1 / texWidth;
					vert[vertsize + 1][3] = ch->y0 / texHeight;

					/*point 3*/
					vert[vertsize + 2][0] = xStart + w;
					vert[vertsize + 2][1] = yStart + offset2;
					//vert[index + 2][2] = zStart;
					vert[vertsize + 2][2] = ch->x1 / texWidth;
					vert[vertsize + 2][3] = ch->y1 / texHeight;

					/*point 4*/
					vert[vertsize + 3][0] = xStart;
					vert[vertsize + 3][1] = yStart + offset2;
					//vert[index + 3][2] = zStart;
					vert[vertsize + 3][2] = ch->x0 / texWidth;
					vert[vertsize + 3][3] = ch->y1 / texHeight;

					/*point 5*/
					vert[vertsize + 4][0] = xStart;
					vert[vertsize + 4][1] = yStart + offset;
					//vert[index + 0][2] = zStart;
					vert[vertsize + 4][2] = ch->x0 / texWidth;
					vert[vertsize + 4][3] = ch->y0 / texHeight;

					/*point 6*/
					vert[vertsize + 5][0] = xStart + w;
					vert[vertsize + 5][1] = yStart + offset2;
					//vert[index + 5][2] = zStart;
					vert[vertsize + 5][2] = ch->x1 / texWidth;
					vert[vertsize + 5][3] = ch->y1 / texHeight;

					vertsize += 6;
					xStart += w + offsetx;
				}

				GLuint textVAO, textVBO;

				glGenVertexArrays(1, &textVAO);
				glBindVertexArray(textVAO);
				glBindTexture(GL_TEXTURE_2D, _textureId);
				glGenBuffers(1, &textVBO);
				glBindBuffer(GL_ARRAY_BUFFER, textVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertsize * 4, vert, GL_DYNAMIC_DRAW);

				mappingStruct value = { std::pair<GLuint, GLuint>(textVAO, textVBO) ,vertsize };
				text_handle[wstringtemp] = value;

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
				//std::cout << index << std::endl;
			}

			auto the_value = text_handle[wstringtemp];
			auto [textVAO, textVBO] = the_value.vaovbo;

			glEnable(GL_BLEND); // ����blend
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(textVAO);
			glBindTexture(GL_TEXTURE_2D, _textureId);
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

			glDrawArrays(GL_TRIANGLES, 0, the_value.vertsize);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_BLEND);
		}
	};

}