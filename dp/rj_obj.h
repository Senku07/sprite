#pragma once

#include "json_parser.h"

struct obj {
	float* Vertex;
	unsigned int VertexCount;
	float* Normals;
	unsigned int NormalCount;
	float* TexCoords;
	unsigned int TexCount;
	unsigned int FaceCount;
};

obj ParseObj(u8* FileData, u32 Count) {
	tokenizer TokenizerA = {};
	tokenizer TokenizerB = {};
	RJ_InitTokenizer(&TokenizerA, FileData, Count);

	TokenizerB = TokenizerA;

	token Token = RJ_GetToken(&TokenizerA);
	obj TempObj = {};

	while (Token.Type != TT_EndOfStream) {
		if (Token.Type == TT_Identifier) {
			if (RJ_TokenEqualsAnyCase(Token, (char*)"v")) {
				TempObj.VertexCount++;
			} else if (RJ_TokenEqualsAnyCase(Token, (char*)"vn")) {
				TempObj.NormalCount++;
			}
			else if (RJ_TokenEqualsAnyCase(Token, (char*)"vt")) {
				TempObj.TexCount++;
			}
			else if (RJ_TokenEqualsAnyCase(Token, (char*)"f")) {
				TempObj.FaceCount++;
			}
		}
		Token = RJ_GetToken(&TokenizerA);
	}

	TempObj.Vertex = (float*)malloc(sizeof(float) * TempObj.VertexCount * 3);
	TempObj.Normals = (float*)malloc(sizeof(float) * TempObj.NormalCount * 3);
	TempObj.TexCoords = (float*)malloc(sizeof(float) * TempObj.TexCount * 2);

	Token = RJ_GetToken(&TokenizerB);
	obj Obj = {};

	Obj.Vertex = (float*)malloc(sizeof(float) * TempObj.FaceCount * 3 * 3);
	Obj.Normals = (float*)malloc(sizeof(float) * TempObj.FaceCount * 3 * 3);
	Obj.TexCoords = (float*)malloc(sizeof(float) * TempObj.FaceCount * 3 * 2);

	Obj.VertexCount = Obj.NormalCount = TempObj.FaceCount * 3;
	Obj.TexCount = TempObj.FaceCount * 3;

	void* TempBuffer = malloc(sizeof(char) * 1024 * 1024);

	u32 v = {}, vn = {}, vt = {};
	u32 rv = {}, rvn = {}, rvt = {};
	while (Token.Type != TT_EndOfStream) {
		if (Token.Type == TT_Identifier) {
			if (RJ_TokenEqualsAnyCase(Token, (char*)"v")) {
				Token = RJ_GetToken(&TokenizerB);

				while (Token.Type == TT_Number) {
					if (RJ_IsFraction(Token.Data))
					{
						r32* Temp = (r32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%f", Temp);
						TempObj.Vertex[v++] = *Temp;
					}
					else
					{
						s32* Temp = (s32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);
						TempObj.Vertex[v++] = *Temp;
					}

					Token = RJ_GetToken(&TokenizerB);
				}
			}
			else if (RJ_TokenEqualsAnyCase(Token, (char*)"vn")) {
				Token = RJ_GetToken(&TokenizerB);

				while (Token.Type == TT_Number) {
					if (RJ_IsFraction(Token.Data))
					{
						r32* Temp = (r32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%f", Temp);
						TempObj.Normals[vn++] = *Temp;
					}
					else
					{
						s32* Temp = (s32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);
						TempObj.Normals[vn++] = *Temp;
					}

					Token = RJ_GetToken(&TokenizerB);
				}
			}
			else if (RJ_TokenEqualsAnyCase(Token, (char*)"vt")) {
				Token = RJ_GetToken(&TokenizerB);

				while (Token.Type == TT_Number) {
					if (RJ_IsFraction(Token.Data))
					{
						r32* Temp = (r32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%f", Temp);
						TempObj.TexCoords[vt++] = *Temp;
					}
					else
					{
						s32* Temp = (s32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);
						TempObj.TexCoords[vt++] = *Temp;
					}

					Token = RJ_GetToken(&TokenizerB);
				}
			}
			else if (RJ_TokenEqualsAnyCase(Token, (char*)"f")) {
				Token = RJ_GetToken(&TokenizerB);

				while (Token.Type == TT_Number) {
					{
						s32* Temp = (s32*)TempBuffer;
						_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);

						*Temp = *Temp - 1;
						
						Obj.Vertex[rv++] = TempObj.Vertex[*Temp * 3];
						Obj.Vertex[rv++] = TempObj.Vertex[*Temp * 3 + 1];
						Obj.Vertex[rv++] = TempObj.Vertex[*Temp * 3 + 2];
					}

					Token = RJ_GetToken(&TokenizerB);

					if (Token.Type == TT_TrailingSlash) {
						Token = RJ_GetToken(&TokenizerB);

						if(Token.Type == TT_Number)
						{
							s32* Temp = (s32*)TempBuffer;
							_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);

							*Temp = *Temp - 1;

							Obj.TexCoords[rvt++] = TempObj.TexCoords[*Temp * 2];
							Obj.TexCoords[rvt++] = TempObj.TexCoords[*Temp * 2 + 1];

							Token = RJ_GetToken(&TokenizerB);

							if (Token.Type == TT_TrailingSlash) {
								Token = RJ_GetToken(&TokenizerB);

								if (Token.Type == TT_Number)
								{
									s32* Temp = (s32*)TempBuffer;
									_snscanf((char*)Token.Data.Data, Token.Data.Count, "%i", Temp);

									*Temp = *Temp - 1;

									Obj.Normals[rvn++] = TempObj.Normals[*Temp * 3];
									Obj.Normals[rvn++] = TempObj.Normals[*Temp * 3 + 1];
									Obj.Normals[rvn++] = TempObj.Normals[*Temp * 3 + 2];

									Token = RJ_GetToken(&TokenizerB);
								}
							}
						}
					}
				}
			}
			else {
			Token = RJ_GetToken(&TokenizerB);
}
	
		}
		else {
		Token = RJ_GetToken(&TokenizerB);
}
	}

	return Obj;
}




