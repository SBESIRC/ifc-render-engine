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
		glm::ivec2 Bearing;  // offsety from baseline to left/top of glyph
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
			FT_Set_Pixel_Sizes(face, 0, 48); // 设置字体大小
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制

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
				if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {  // 加载字符的字形
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// Generate texture // 生成纹理
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
				// Set texture options // 设置纹理选项
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// 存储字符供之后使用
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				Characters.insert(std::pair<Char, Character>(c, character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			FT_Done_Face(face); // 清理FreeType的资源
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

		bool notGenerated() { // 是否已经生成
			return x0 == 0 && y0 == 0 && x1 == 0 && y1 == 0;
		}
		// 存储当前字符在纹理上的坐标位置
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
	struct TextNewFromat {
		std::wstring content;
		glm::vec3 color;
		float size;
		glm::vec3 normal;
		glm::vec3 direction;
		//so the vertical direciton will be normal x direction in right-hand coordination
		glm::vec3 center;
	};
	struct TextureFont {
	protected:
		Character2 _character[1 << 16]; // one word = two Byte, total 65536 bit, can sove 4096 words, row & coloun = 64
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
	public:

		TextureFont(std::string fontfilepath, int fontSize) {
			_fontSize = fontSize;
			_xStart = 0;
			_yStart = 0;

			memset(_character, 0, sizeof(_character));
			memset(vert, 0, sizeof(vert));
			FT_Init_FreeType(&_library); // 初始化字体库
			assert(_library != 0);

			FT_New_Face(_library, fontfilepath.c_str(), 0, &_face); // load typestyle
			FT_Set_Char_Size(_face, fontSize << 6, fontSize << 6, 72, 72); // set size of typestyle
			//FT_Set_Pixel_Sizes(_face, 0, 48);

			assert(_face != 0);

			glGenTextures(1, &_textureId);
			glBindTexture(GL_TEXTURE_2D, _textureId); // bind/use this texture id

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 指定纹理的放大,缩小滤波，使用线性方式，即当图片放大的时候插值方式 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(
				GL_TEXTURE_2D,				//! 指定是二维图片
				0,							//! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
				GL_RED,						//! 纹理的使用的存储格式
				1024, 1024,					//! 纹理宽高，可容纳大约 1024/16 * 1024/16 = 4096个汉字
				0,							//! 是否的边
				GL_RED,					 	//! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
				GL_UNSIGNED_BYTE, 0		 	//! 数据是8bit数据
			);							 

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		Character2* getCharacter(wchar_t ch) {
			wchar_t& word = ch;
			if (_character[word].notGenerated()) { // 说明字符还没有绘制到纹理上，则进行绘制
				if (_xStart + _fontSize > 1024) { // 写满一行,重新开始
					//this line filled, enter next line
					_xStart = 0;
					_yStart += _fontSize; // y开始位置要增加
				}
				FT_Load_Glyph(_face, FT_Get_Char_Index(_face, word), FT_LOAD_DEFAULT); // get typeface information
				FT_Error error = 0;
				FT_Glyph glyph;
				error = FT_Get_Glyph(_face->glyph, &glyph);
				
				FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);// Convert the glyph to a bitmap.
				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
				FT_Bitmap& bitmap = bitmap_glyph->bitmap;//This reference will make accessing the bitmap easier

				/*if (FT_Load_Char(_face, ch, FT_LOAD_RENDER)) {
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				}*/

				//if no data, write a cube with _fontSize / 4 width & place _fontSize / 2 width in texture
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
					glTexSubImage2D( // 将bmp数据覆盖写到指定纹理上指定位置
						GL_TEXTURE_2D,		   // 活动纹理单元的目标纹理
						0,					   // 详细级别编号。 0级是基本图像级别。 级别n是第n个mipmap缩小图像。
						_xStart,			   // 纹理中x方向偏移
						_yStart,			   // 纹理中y方向偏移
						bitmap.width,		   // 像素宽度
						bitmap.rows,		   // 像素高度
						GL_RED,				   // 像素数据格式
						GL_UNSIGNED_BYTE,	   // 像素数据类型
						bitmap.buffer		   // 指向内存中图像数据的指针
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

		void drawText(const wchar_t* text, float text_scale) { //生成网格，贴上纹理绘制文字
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
					Character2* ch = getCharacter(text[i]); // 获得纹理中存的“字”

					float h = (ch->y1 - ch->y0) * text_scale;
					//int h = 30;
					float w = (ch->x1 - ch->x0) * text_scale;
					float offsety = float(ch->offsetY * text_scale);
					float offset2 = offsety - float(h);
					//float offsety = 0;
					float offsetx = float(ch->offsetX * text_scale);

					/*point 1*/
					vert[vertsize + 0][0] = xStart;
					vert[vertsize + 0][1] = yStart + offsety;
					//vert[index + 0][2] = zStart;
					vert[vertsize + 0][2] = ch->x0 / texWidth;
					vert[vertsize + 0][3] = ch->y0 / texHeight;


					/*point 2*/
					vert[vertsize + 1][0] = xStart + w;
					vert[vertsize + 1][1] = yStart + offsety;
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
					vert[vertsize + 4][1] = yStart + offsety;
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

			glEnable(GL_BLEND); // 启用blend
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

		vector<float> vert3dd;

		// 方案A: 一次性计算出所有字符串的位置，并使用一个VAO 优点：最低的draw call 实现简单 缺点：重复字符串多的情况下内存占用较大
		void drawText3D(UniquePtr<GLSLProgram>& m_text3d_shader, Vector<wstring>& texts, Vector<float>& text_data, glm::mat4 m_projection, glm::mat4 m_modelview, bool& text_first) {
			m_text3d_shader->use();
			m_text3d_shader->setMat4("projection", m_projection);
			m_text3d_shader->setMat4("modelview", m_modelview);

			//static bool text_first = true;
			static uint32_t textVAO, textVBO;
			static uint32_t cnt = 0;
			if (text_first) {
				text_first = false;

				for (int text = 0, j = 0; text < texts.size(); ++text, j += 14) { // 对每一串字符串
					wstring content = texts[text];
					glm::vec3 center = glm::vec3(text_data[j + 0], text_data[j + 1], text_data[j + 2]);
					glm::vec3 normal = glm::normalize(glm::vec3(text_data[j + 3], text_data[j + 4], text_data[j + 5]));
					glm::vec3 direction = glm::normalize(glm::vec3(text_data[j + 6], text_data[j + 7], text_data[j + 8]));
					m_text3d_shader->setVec3("textColor", glm::vec3(text_data[j + 9], text_data[j + 10], text_data[j + 11]));
					Real size = text_data[j + 13];

					float texWidth = 1024;
					float texHeight = 1024;
					glm::vec3 pStart = center;
					float totalW = 0;
					float maxH = 0;
					unsigned nSize = content.size();
					for (unsigned i = 0; i < nSize; i++) {
						Character2* ch = getCharacter(content[i]);
						maxH = std::max(maxH, (float)(ch->y1 - ch->y0));
						totalW += (ch->x1 - ch->x0);
					}
					maxH *= size;
					totalW *= size * 0.7; // 设置长宽比

					glm::vec3 verticalDirection = glm::cross(normal, direction); // 通过朝向和法向量计算出另一个坐标轴方向
					pStart -= (totalW / 2 * direction + maxH / 2 * verticalDirection); // pStart偏移至左下角正确位置

					for (unsigned i = 0; i < nSize; i++) {
						Character2* ch = getCharacter(content[i]);

						float h = (ch->y1 - ch->y0) * size;
						float w = (ch->x1 - ch->x0) * size * 0.7;
						float offsety = float(ch->offsetY * size);
						float offset2 = offsety - float(h);
						float offsetx = float(ch->offsetX * size);

						glm::vec3 curStart = pStart + offsety * verticalDirection;
						glm::vec3 temp1 = pStart + offset2 * verticalDirection;
						glm::vec3 temp2 = pStart + w * direction + offsety * verticalDirection;
						glm::vec3 temp3 = temp1 + w * direction;

						vert3dd.emplace_back(curStart.x);
						vert3dd.emplace_back(curStart.y);
						vert3dd.emplace_back(curStart.z);
						vert3dd.emplace_back(ch->x0 / texWidth); // get normalized coordinate
						vert3dd.emplace_back(ch->y0 / texHeight);

						vert3dd.emplace_back(temp2.x);
						vert3dd.emplace_back(temp2.y);
						vert3dd.emplace_back(temp2.z);
						vert3dd.emplace_back(ch->x1 / texWidth);
						vert3dd.emplace_back(ch->y0 / texHeight);

						vert3dd.emplace_back(temp3.x);
						vert3dd.emplace_back(temp3.y);
						vert3dd.emplace_back(temp3.z);
						vert3dd.emplace_back(ch->x1 / texWidth);
						vert3dd.emplace_back(ch->y1 / texHeight);


						vert3dd.emplace_back(temp1.x);
						vert3dd.emplace_back(temp1.y);
						vert3dd.emplace_back(temp1.z);
						vert3dd.emplace_back(ch->x0 / texWidth);
						vert3dd.emplace_back(ch->y1 / texHeight);

						vert3dd.emplace_back(curStart.x);
						vert3dd.emplace_back(curStart.y);
						vert3dd.emplace_back(curStart.z);
						vert3dd.emplace_back(ch->x0 / texWidth);
						vert3dd.emplace_back(ch->y0 / texHeight);

						vert3dd.emplace_back(temp3.x);
						vert3dd.emplace_back(temp3.y);
						vert3dd.emplace_back(temp3.z);
						vert3dd.emplace_back(ch->x1 / texWidth);
						vert3dd.emplace_back(ch->y1 / texHeight);

						pStart = pStart + (offsetx + w) * direction;
					}
				}

				cnt = vert3dd.size();
				//GLuint textVAO, textVBO;
				glGenVertexArrays(1, &textVAO);
				glBindVertexArray(textVAO);
				glBindTexture(GL_TEXTURE_2D, _textureId);
				glGenBuffers(1, &textVBO);
				glBindBuffer(GL_ARRAY_BUFFER, textVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cnt, vert3dd.data(), GL_DYNAMIC_DRAW);

				vector<float>().swap(vert3dd);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(textVAO);
			glBindTexture(GL_TEXTURE_2D, _textureId);
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

			glDrawArrays(GL_TRIANGLES, 0, cnt);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_BLEND);
		}

		// 方案B：使用unordered_map记录多个字符串对应的VAO，调用shader的时候传入对应的移动矩阵（最后乘MVP） 优点：省内存，数据变化灵活，缺点：增加draw call，较难实现
		void drawText3DProB(UniquePtr<GLSLProgram>& m_text3d_shader, Vector<wstring>& texts, Vector<float>& text_data, glm::mat4 m_projection, glm::mat4 m_modelview) {
			
		}
	};

}
