#ifndef RJSON_H

// Real Json Version 0.2-Alpha
// More verbose reading events like the SAX writing API (aka more granlurity)
// DOM Object (maybe we implement this during the course of this project)
// In place parsing (not going to be happen in the context of this project)
// Pretty Output and optionally a validator

// TODO(rajat):
// ROADMAP 0.2-Alpha
// 1. String Parsing functions !done
// 2. Granlur SAX API !done
// 3. Pretty Output
// A DOM Parser (A fancy in this version cycle)

#ifdef RJSON_STATIC
#define RJSON_DEF static
#else
#define RJSON_DEF
#endif

#ifndef RJSON_ASSERT
#define RJ_Assert(condition)
#endif

#include <stdint.h>
#include <stdio.h>

#ifndef RJSON_USE_USER_DEFINED_TYPES
typedef uint32_t u32;
typedef uint8_t u8;
typedef int32_t s32;
typedef uint64_t u64;
typedef float r32;
typedef double r64;
typedef u32 b32;
#endif

struct string
{
    u8 *Data;
    u32 Count;
};

struct tokenizer
{
    string Input;
    u32 LineNo;
    u32 ColumnNo;
    char At[2];
};

enum token_type
{
    TT_OpenBracket,
    TT_CloseBracket,
    TT_OpenSquareBracket,
    TT_CloseSquareBracket,
    TT_String,
    TT_Identifier,
    TT_Equals,
    TT_Colon,
    TT_Comma,
    TT_Number,
    TT_TrailingSlash,

    TT_EndOfStream,
    TT_Count,
    TT_None
};

struct token
{
    string Data;
    token_type Type;
    u32 LineNo;
    u32 ColumnNo;
};

RJSON_DEF void RJ_InitTokenizer(tokenizer *Tokenizer, u8 *String, u32 Count);
RJSON_DEF token RJ_GetToken(tokenizer *Tokenizer);

// SAX Api for Event based parsing

enum json_type
{
    JT_Array,
    JT_Float,
    JT_Int,
    JT_String,
    JT_Object,
    JT_Boolean,

    JT_Count
};

#define RJSON_ALLOC(name) void *name(void *AllocContext, u32 Count)
typedef RJSON_ALLOC(rjson_alloc);

#define RJSON_ARRAY_OBJECT_HANDLER(name) void name(u8 *Name, u32 NameCount, u32 ElementCount, json_type Type, void *UserData)
typedef RJSON_ARRAY_OBJECT_HANDLER(rjson_array_object_handler);

#define RJSON_NUMBER_STRING_HANDLER(name) void name(u8 *Key, u32 KeyCount, void *Value, u32 StringCount, json_type Type, void *UserData)
typedef RJSON_NUMBER_STRING_HANDLER(rjson_number_string_handler);

struct ReadHandlers
{
    rjson_number_string_handler *KeyHandler;
    rjson_array_object_handler *ArrayObjectHandler;

    void *UserData;
    void *TempBuffer;

    u8 *CurrentKey;
    u32 CurrentKeyStringCount;
};

RJSON_DEF void RJ_ParseJson(u8 *String, u32 StringCount, ReadHandlers Handlers);

// 0.2-Alpha SAX granular Api and string parsing

RJSON_DEF u32 RJ_GetParsedStringLength(u8 *String, u32 StringCount);
RJSON_DEF b32 RJ_ParseString(u8 *String, u32 StringCount, u8 *OutBuffer, u32 OutBufferSize);

#define RJSON_START_OBJECT_HANDLER(name) void name(u32 ElementCount, void *UserData)
typedef RJSON_START_OBJECT_HANDLER(rjson_start_object_handler);

#define RJSON_END_OBJECT_HANDLER(name) void name(void *UserData)
typedef RJSON_END_OBJECT_HANDLER(rjson_end_object_handler);

#define RJSON_START_ARRAY_HANDLER(name) void name(u32 ElementCount, void *UserData)
typedef RJSON_START_ARRAY_HANDLER(rjson_start_array_handler);

#define RJSON_END_ARRAY_HANDLER(name) void name(void *UserData)
typedef RJSON_END_ARRAY_HANDLER(rjson_end_array_handler);

#define RJSON_KEY_HANDLER(name) void name(u8 *Key, u32 KeyCount, void *UserData)
typedef RJSON_KEY_HANDLER(rjson_key_handler);

// NOTE(rajat): In future versions we may add a way to specify 64 bit values and 
// therefore 64 bit handlers for int and float and maybe unsigned values too with specifiers like f and u like in C
#define RJSON_STRING_HANDLER(name) void name(u8 *String, u32 StringCount, void *UserData)
typedef RJSON_STRING_HANDLER(rjson_string_handler);

#define RJSON_INT_HANDLER(name) void name(s32 Value, void *UserData) 
typedef RJSON_INT_HANDLER(rjson_int_handler);

#define RJSON_FLOAT_HANDLER(name) void name(r32 Value, void *UserData)
typedef RJSON_FLOAT_HANDLER(rjson_float_handler);

#define RJSON_BOOLEAN_HANDLER(name) void name(b32 Value, void *UserData)
typedef RJSON_BOOLEAN_HANDLER(rjson_boolean_handler);

struct ReadHandlersEx
{
    rjson_key_handler *Key;
    rjson_string_handler *String;
    rjson_int_handler *Int;
    rjson_float_handler *Float;
    rjson_boolean_handler *Boolean;
    rjson_start_object_handler *StartObject;
    rjson_end_object_handler *EndObject;
    rjson_start_array_handler *StartArray;
    rjson_end_array_handler *EndArray;

    void *TempBuffer;
    void *UserData;
};

RJSON_DEF void RJ_ParseJson(u8 *String, u32 StringCount, ReadHandlers HandlersEx);

// SAX wrtie API
enum json_write_style
{
    JWS_Standard,
    JWS_Simplified,

    JWS_Count
};

struct rj_write_context
{
    /* NOTE(rajat): Fill This yourself */
    json_write_style Style;
    u8 *Buffer; // NOTE(rajat): Give Us 1 MB buffer, can't gurantee if it fits, use the same again for subsquent writing
    u32 BufferSize;

    /* NOTE(rajat): This is what you will get */
    u32 BufferWritePos;
    u32 FinalBufferSize;

    u32 InObjectOrArrayCount;
};

RJSON_DEF void RJ_StartArray(rj_write_context *Context);
RJSON_DEF void RJ_EndArray(rj_write_context *Context);
RJSON_DEF void RJ_StartRootObject(rj_write_context *Context);
RJSON_DEF void RJ_EndRootObject(rj_write_context *Context);
RJSON_DEF void RJ_StartObject(rj_write_context *Context);
RJSON_DEF void RJ_EndObject(rj_write_context *Context);
RJSON_DEF void RJ_Key(rj_write_context *Context, u8 *Key, u32 Count);
RJSON_DEF void RJ_Float(rj_write_context *Context, r32 Value);
RJSON_DEF void RJ_Int(rj_write_context *Context, s32 Value);
RJSON_DEF void RJ_String(rj_write_context *Context, u8 *String, u32 Count);
RJSON_DEF void RJ_Boolean(rj_write_context *Context, b32 Value);

#ifdef RJSON_IMPLEMENTATION

RJSON_DEF void
RJ_Refill(tokenizer *Tokenizer)
{
    if (Tokenizer->Input.Count == 0)
    {
        Tokenizer->At[0] = 0;
        Tokenizer->At[1] = 0;
    }
    else if (Tokenizer->Input.Count == 1)
    {
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = 0;
    }
    else
    {
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = Tokenizer->Input.Data[1];
    }
}

RJSON_DEF void
RJ_AdvanceChars(tokenizer *Tokenizer, u32 Count)
{
    if (Tokenizer->Input.Count > Count)
    {
        Tokenizer->Input.Data += Count;
        Tokenizer->Input.Count -= Count;
    }
    else
    {
        Count = Tokenizer->Input.Count;

        Tokenizer->Input.Data += Count;
        Tokenizer->Input.Count = 0;
    }

    RJ_Refill(Tokenizer);
}

RJSON_DEF b32
RJ_IsSpace(u8 C)
{
    b32 Result = false;
    if ((C == ' ') ||
        (C == '\n') ||
        (C == '\r') ||
        (C == '\t'))
    {
        Result = true;
    }

    return (Result);
}

RJSON_DEF b32
RJ_IsAlpha(u8 C)
{
    b32 Result = false;
    if ((C >= 'a' && C <= 'z') ||
        (C >= 'A' && C <= 'Z'))
    {
        Result = true;
    }

    return (Result);
}

RJSON_DEF b32
RJ_IsNumeric(u8 C)
{
    b32 Result = false;
    if (C >= '0' && C <= '9')
    {
        Result = true;
    }

    return (Result);
}

RJSON_DEF string
RJ_SkipWhitespace(tokenizer *Tokenizer)
{
    while (Tokenizer->At[0] == '#') {
        while (Tokenizer->At[0] != '\n') {
            RJ_AdvanceChars(Tokenizer, 1);
        }

        RJ_AdvanceChars(Tokenizer, 1);
    }

    while (RJ_IsSpace(Tokenizer->At[0]) ||
           Tokenizer->At[0] == '\\')
    {
        if (Tokenizer->At[0] == '\\' &&
            Tokenizer->At[1] == '\\')
        {
            RJ_AdvanceChars(Tokenizer, 2);
            while (Tokenizer->At[0] != '\n' &&
                   Tokenizer->At[0])
            {
                RJ_AdvanceChars(Tokenizer, 1);
            }
        }

        if (Tokenizer->At[0] != '\r')
        {
            Tokenizer->ColumnNo++;
        }

        if (Tokenizer->At[0] == '\n')
        {
            Tokenizer->LineNo++;
            Tokenizer->ColumnNo = 0;
        }

        RJ_AdvanceChars(Tokenizer, 1);
    }

    return (Tokenizer->Input);
}

RJSON_DEF void
RJ_InitTokenizer(tokenizer *Tokenizer, u8 *String, u32 Count)
{
    Tokenizer->Input = {String, Count};
    Tokenizer->LineNo = 0;
    Tokenizer->ColumnNo = 0;

    RJ_Refill(Tokenizer);
}

RJSON_DEF void
RJ_TokenizerSkipNumber(tokenizer *Tokenizer, token *Token)
{
    if (Tokenizer->At[0] == '-')
    {
        RJ_AdvanceChars(Tokenizer, 1);
        Token->Data.Count++;
        Tokenizer->ColumnNo++;
    }

    while (RJ_IsNumeric(Tokenizer->At[0]))
    {
        RJ_AdvanceChars(Tokenizer, 1);
        Token->Data.Count++;
        Tokenizer->ColumnNo++;
    }
}

RJSON_DEF token
RJ_GetToken(tokenizer *Tokenizer)
{
    token Token = {};

    Token.Data = RJ_SkipWhitespace(Tokenizer);
    Token.Data.Count = 1;
    Token.LineNo = Tokenizer->LineNo;
    Token.ColumnNo = Tokenizer->ColumnNo++;
    Token.Type = TT_None;

    u32 Advance = 1;

    switch (Tokenizer->At[0])
    {
    case '{':
    {
        Token.Type = TT_OpenBracket;
    }
    break;
    case '}':
    {
        Token.Type = TT_CloseBracket;
    }
    break;
    case '[':
    {
        Token.Type = TT_OpenSquareBracket;
    }
    break;
    case ']':
    {
        Token.Type = TT_CloseSquareBracket;
    }
    break;
    case ',':
    {
        Token.Type = TT_Comma;
    }
    break;
    case '=':
    {
        Token.Type = TT_Equals;
    }
    break;
    case ':':
    {
        Token.Type = TT_Colon;
    }
    break;
    case '/':
    {
        Token.Type = TT_TrailingSlash;
    }
    break;
    case 0:
    {
        Token.Type = TT_EndOfStream;
    }
    break;

    case '"':
    {
        Token.Type = TT_String;
        RJ_AdvanceChars(Tokenizer, 1);

        Token.Data.Data = Tokenizer->Input.Data;
        Token.Data.Count = 0;

        while (Tokenizer->At[0] != '"')
        {
            if (Tokenizer->At[0] == '\\' &&
                Tokenizer->At[1] == '"')
            {
                Token.Data.Count++;
                Tokenizer->ColumnNo++;
                RJ_AdvanceChars(Tokenizer, 1);
            }
            else if (Tokenizer->At[0] == '\0')
            {
                // TODO(rajat): Log error cannot find the string terminator
                break;
            }

            Token.Data.Count++;
            Tokenizer->ColumnNo++;
            RJ_AdvanceChars(Tokenizer, 1);
        }
    }
    break;

    default:
    {
        if (RJ_IsAlpha(Tokenizer->At[0]))
        {
            Token.Type = TT_Identifier;
            Token.Data.Count = 0;
            Advance = 0;

            while (RJ_IsAlpha(Tokenizer->At[0]) ||
                   RJ_IsNumeric(Tokenizer->At[0]) ||
                   Tokenizer->At[0] == '_')
            {
                Token.Data.Count++;
                Tokenizer->ColumnNo++;

                RJ_AdvanceChars(Tokenizer, 1);
            }

            Tokenizer->ColumnNo--;
        }
        else if (RJ_IsNumeric(Tokenizer->At[0]) ||
                 Tokenizer->At[0] == '-')
        {
            Token.Type = TT_Number;
            Token.Data.Count = 0;
            Advance = 0;

            RJ_TokenizerSkipNumber(Tokenizer, &Token);

            if (Tokenizer->At[0] == '.')
            {
                RJ_AdvanceChars(Tokenizer, 1);
                Token.Data.Count++;
                Tokenizer->ColumnNo++;

                if (!RJ_IsNumeric(Tokenizer->At[0]))
                {
                    // TODO(rajat): Log, Error Missing fractional part of the number or invalid
                    // fractional part
                }
                else
                {
                    RJ_TokenizerSkipNumber(Tokenizer, &Token);
                }
            }

            if (Tokenizer->At[0] == 'e' ||
                Tokenizer->At[0] == 'E')
            {
                RJ_AdvanceChars(Tokenizer, 1);
                Token.Data.Count++;

                RJ_TokenizerSkipNumber(Tokenizer, &Token);
            }
        }
    }
    break;
    }

    RJ_AdvanceChars(Tokenizer, Advance);

    return Token;
}

RJSON_DEF void RJ_ParseElement(tokenizer *Tokenizer, ReadHandlersEx Handlers);
RJSON_DEF void RJ_ParseObject(tokenizer *Tokenizer, ReadHandlersEx Handlers);
RJSON_DEF void RJ_ParseArray(tokenizer *Tokenizer, ReadHandlersEx Handlers);

RJSON_DEF b32
RJ_IsFraction(string Data)
{
    b32 Result = 0;
    for(u32 i = 0; i < Data.Count; ++i)
    {
        if(Data.Data[i] == '.')
        {
            Result = 1;
        }
    }
    return(Result);
}

RJSON_DEF u8 
RJ_ToUpper(u8 C)
{
    u8 Result = C;
    if(C >= 97 && C <= 122)
    {
        return(Result - 32);
    }

    return(Result);
}

RJSON_DEF b32
RJ_TokenEqualsAnyCase(token Token, char *String)
{
    u32 StringCount = 0;

    char* s = String;
    while(*s)
    {
        s++;
        StringCount++;
    }

    b32 Result = 1;

    if(Token.Data.Count != StringCount)
    {
        return(0);
    }

    for(u32 i = 0; i < StringCount; ++i)
    {
        if(RJ_ToUpper(Token.Data.Data[i]) != RJ_ToUpper((u8)String[i]))
        {
            Result = 0;
        }
    }

    return(Result);
}

RJSON_DEF void
RJ_ParseElement(token Token, tokenizer *Tokenizer, ReadHandlersEx Handlers)
{
    switch (Token.Type)
    {
    case TT_OpenBracket:
    {
        RJ_ParseObject(Tokenizer, Handlers);
    }
    break;
    case TT_OpenSquareBracket:
    {
        RJ_ParseArray(Tokenizer, Handlers);
    }
    break;

    case TT_String:
    {
        Handlers.String(Token.Data.Data, Token.Data.Count, Handlers.UserData);
    }break;

    case TT_Number:
    {
        if(RJ_IsFraction(Token.Data))
        {
            r32 *Temp = (r32 *)Handlers.TempBuffer;
            _snscanf((char *)Token.Data.Data, Token.Data.Count, "%f", Temp);

            Handlers.Float(Temp[0], Handlers.UserData);
        }
        else
        {
            s32 *Temp = (s32 *)Handlers.TempBuffer;
            _snscanf((char *)Token.Data.Data, Token.Data.Count, "%i", Temp);

            Handlers.Int(Temp[0], Handlers.UserData);
        }
    }
    break;

    case TT_Identifier:
    {
        if(RJ_TokenEqualsAnyCase(Token, (char*)"true") &&
           RJ_TokenEqualsAnyCase(Token, (char*)"on"))
        {
            Handlers.Boolean(1, Handlers.UserData);
        }
        else if(RJ_TokenEqualsAnyCase(Token, (char*)"false") &&
                RJ_TokenEqualsAnyCase(Token, (char*)"off"))
        {
            Handlers.Boolean(0, Handlers.UserData);
        }
        else
        {
            // TODO(rajat): Log error, invalid identifier
        }
    }
    break;

    default:
    {
        // TODO(rajat): Log error, unexpected token
    }break;
    }
}

RJSON_DEF void
RJ_SkipArray(tokenizer *Tokenizer);

RJSON_DEF void
RJ_SkipObject(tokenizer *Tokenizer);

RJSON_DEF void
RJ_SkipElement(token Token, tokenizer *Tokenizer)
{
    switch (Token.Type)
    {
    case TT_OpenBracket:
    {
        RJ_SkipObject(Tokenizer);
    }
    break;
    case TT_OpenSquareBracket:
    {
        RJ_SkipArray(Tokenizer);
    }
    break;

    case TT_String:
    case TT_Number:
    case TT_Identifier:
        break;
    }
}

RJSON_DEF void
RJ_SkipArray(tokenizer *Tokenizer)
{
    token Token = RJ_GetToken(Tokenizer);

    if (Token.Type == TT_CloseSquareBracket)
    {
        return;
    }

    while (Token.Type != TT_CloseSquareBracket)
    {
        switch (Token.Type)
        {

        case TT_EndOfStream:
        {
        }
        break;

        case TT_Comma:
        {
        }
        break;

        default:
        {
            RJ_SkipElement(Token, Tokenizer);
        }
        break;
        }

        Token = RJ_GetToken(Tokenizer);
    }
}

RJSON_DEF void
RJ_SkipObject(tokenizer *Tokenizer)
{
    token Token = RJ_GetToken(Tokenizer);

    if (Token.Type == TT_CloseBracket)
    {
        return;
    }

    while (Token.Type != TT_CloseBracket)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Identifier:
        case TT_String:
        {
            string Key = Token.Data;

            Token = RJ_GetToken(Tokenizer);
            if (Token.Type == TT_Colon ||
                Token.Type == TT_Equals)
            {
                Token = RJ_GetToken(Tokenizer);
                RJ_SkipElement(Token, Tokenizer);
            }
            else
            {
                // TODO(rajat): Log error, invalid key declartion
            }
        }
        break;
        }
        Token = RJ_GetToken(Tokenizer);
    }
}

RJSON_DEF u32
RJ_GetArrayElementCount(tokenizer *Tokenizer)
{
    b32 Result = 0;

    tokenizer Temp = *Tokenizer;
    token Token = RJ_GetToken(&Temp);

    if (Token.Type == TT_CloseSquareBracket)
    {
        return (Result);
    }

    while (Token.Type != TT_CloseSquareBracket)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Comma:
        case TT_OpenSquareBracket:
        {
        }
        break;

        default:
        {
            RJ_SkipElement(Token, &Temp);
            Result++;
        }
        break;
        }

        Token = RJ_GetToken(&Temp);
    }

    return (Result);
}

RJSON_DEF void
RJ_ParseArray(tokenizer *Tokenizer, ReadHandlersEx Handlers)
{
    u32 ElementCount = RJ_GetArrayElementCount(Tokenizer);
    token Token = RJ_GetToken(Tokenizer);

    Handlers.StartArray(ElementCount, Handlers.UserData);

    if (Token.Type == TT_CloseSquareBracket)
    {
        Handlers.EndArray(Handlers.UserData);
        return;
    }

    while (Token.Type != TT_CloseSquareBracket)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Comma:
        {
        }
        break;

        case TT_OpenSquareBracket:
        {
        }
        break;

        default:
        {
            RJ_ParseElement(Token, Tokenizer, Handlers);
        }
        break;
        }

        Token = RJ_GetToken(Tokenizer);
    }
    Handlers.EndArray(Handlers.UserData);
}

RJSON_DEF u32
RJ_GetObjectMemberCount(tokenizer *Tokenizer)
{
    u32 Result = 0;
    tokenizer Temp = *Tokenizer;
    token Token = RJ_GetToken(&Temp);

    if (Token.Type == TT_CloseBracket)
    {
        return(Result);
    }

    while (Token.Type != TT_CloseBracket)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Identifier:
        case TT_String:
        {
            string Key = Token.Data;

            Token = RJ_GetToken(&Temp);
            if (Token.Type == TT_Colon ||
                Token.Type == TT_Equals)
            {
                Token = RJ_GetToken(&Temp);
                RJ_SkipElement(Token, &Temp);
                Result++;
            }
            else
            {
                // TODO(rajat): Log error, invalid key declartion
            }
        }
        break;
        }
        Token = RJ_GetToken(&Temp);
    }

    return(Result);
}

RJSON_DEF void
RJ_ParseObject(tokenizer *Tokenizer, ReadHandlersEx Handlers)
{
    u32 MemberCount = RJ_GetObjectMemberCount(Tokenizer);
    token Token = RJ_GetToken(Tokenizer);

    Handlers.StartObject(MemberCount, Handlers.UserData);
    if (Token.Type == TT_CloseBracket)
    {
        Handlers.EndObject(Handlers.UserData);
        return;
    }

    while (Token.Type != TT_CloseBracket)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Identifier:
        case TT_String:
        {
            string Key = Token.Data;

            Token = RJ_GetToken(Tokenizer);
            if (Token.Type == TT_Colon ||
                Token.Type == TT_Equals)
            {
                Handlers.Key(Key.Data, Key.Count, Handlers.UserData);
                Token = RJ_GetToken(Tokenizer);
                RJ_ParseElement(Token, Tokenizer, Handlers);
            }
            else
            {
                // TODO(rajat): Log error, invalid key declartion
            }
        }
        break;
        }
        Token = RJ_GetToken(Tokenizer);
    }
    Handlers.EndObject(Handlers.UserData);
}

RJSON_DEF u32
RJ_GetRootObjectMemberCount(token Token, tokenizer *Tokenizer)
{
    u32 Result = 0;
    tokenizer Temp = *Tokenizer;

    if (Token.Type == TT_EndOfStream)
    {
        return(Result);
    }

    while (Token.Type != TT_EndOfStream)
    {
        switch (Token.Type)
        {
        case TT_EndOfStream:
        {
            // TODO(rajat): Log error, missing object end
        }
        break;

        case TT_Identifier:
        case TT_String:
        {
            string Key = Token.Data;

            Token = RJ_GetToken(&Temp);
            if (Token.Type == TT_Colon ||
                Token.Type == TT_Equals)
            {
                Token = RJ_GetToken(&Temp);
                RJ_SkipElement(Token, &Temp);
                Result++;
            }
            else
            {
                // TODO(rajat): Log error, invalid key declartion
            }
        }
        break;
        }
        Token = RJ_GetToken(&Temp);
    }

    return(Result);
}

RJSON_DEF void
RJ_ParseRootObject(token Token, tokenizer *Tokenizer, ReadHandlersEx Handlers)
{
    u32 MemberCount = RJ_GetRootObjectMemberCount(Token, Tokenizer);

    Handlers.StartObject(MemberCount, Handlers.UserData);
    if (Token.Type == TT_EndOfStream)
    {
        Handlers.EndObject(Handlers.UserData);
        return;
    }

    while (Token.Type != TT_EndOfStream)
    {
        switch (Token.Type)
        {
        case TT_Identifier:
        case TT_String:
        {
            string Key = Token.Data;

            Token = RJ_GetToken(Tokenizer);
            if (Token.Type == TT_Colon ||
                Token.Type == TT_Equals)
            {
                Handlers.Key(Key.Data, Key.Count, Handlers.UserData);
                Token = RJ_GetToken(Tokenizer);
                RJ_ParseElement(Token, Tokenizer, Handlers);
            }
            else
            {
                // TODO(rajat): Log error, invalid key declartion
            }
        }
        break;
        }
        Token = RJ_GetToken(Tokenizer);
    }

    Handlers.EndObject(Handlers.UserData);
}

RJSON_DEF void RJ_ParseJson(u8 *String, u32 StringCount, ReadHandlersEx HandlersEx)
{
    tokenizer Tokenizer = {};
    RJ_InitTokenizer(&Tokenizer, String, StringCount);

    token Token = RJ_GetToken(&Tokenizer);

    while (Token.Type != TT_EndOfStream)
    {
        switch (Token.Type)
        {
        case TT_OpenBracket:
        {
            RJ_ParseObject(&Tokenizer, HandlersEx);
        }
        break;
        case TT_OpenSquareBracket:
        {
            RJ_ParseArray(&Tokenizer, HandlersEx);
        }
        break;
        default:
        {
            HandlersEx.Key((u8 *)"Root", 5, HandlersEx.UserData);
            RJ_ParseRootObject(Token, &Tokenizer, HandlersEx);
        }
        break;
        }
        Token = RJ_GetToken(&Tokenizer);
    }
}

RJSON_DEF 
RJSON_START_OBJECT_HANDLER(_RJ_StartObject)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->ArrayObjectHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, ElementCount, JT_Object, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_END_OBJECT_HANDLER(_RJ_EndObject)
{
}

RJSON_DEF
RJSON_START_ARRAY_HANDLER(_RJ_StartArray)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->ArrayObjectHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, ElementCount, JT_Array, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_END_ARRAY_HANDLER(_RJ_EndArray)
{
}

RJSON_DEF
RJSON_INT_HANDLER(_RJ_Int)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->KeyHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, &Value, 0, JT_Int, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_FLOAT_HANDLER(_RJ_Float)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->KeyHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, &Value, 0, JT_Float, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_BOOLEAN_HANDLER(_RJ_Boolean)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->KeyHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, &Value, 0, JT_Boolean, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_STRING_HANDLER(_RJ_String)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->KeyHandler(Handler->CurrentKey, Handler->CurrentKeyStringCount, (void *)String, StringCount, JT_String, Handler->UserData);

    Handler->CurrentKey = 0;
    Handler->CurrentKeyStringCount = 0;
}

RJSON_DEF
RJSON_KEY_HANDLER(_RJ_Key)
{
    ReadHandlers *Handler = (ReadHandlers *)UserData;

    Handler->CurrentKey = Key;
    Handler->CurrentKeyStringCount = KeyCount;
}

RJSON_DEF void
RJ_ParseJson(u8 *String, u32 StringCount, ReadHandlers Handlers)
{
    ReadHandlersEx Default = {};
    Default.UserData = &Handlers;
    Default.TempBuffer = Handlers.TempBuffer;

    Default.StartObject = _RJ_StartObject;
    Default.StartArray = _RJ_StartArray;
    Default.EndObject = _RJ_EndObject;
    Default.EndArray = _RJ_EndArray;
    Default.Key = _RJ_Key;
    Default.Int = _RJ_Int;
    Default.Float = _RJ_Float;
    Default.String = _RJ_String;
    Default.Boolean = _RJ_Boolean;

    Handlers.CurrentKey = 0;
    Handlers.CurrentKeyStringCount = 0;

    RJ_ParseJson(String, StringCount, Default);
}

RJSON_DEF void 
RJ_StartArray(rj_write_context *Context)
{
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 1);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos++;

    WriteBuffer[0] = '[';

    Context->InObjectOrArrayCount++;

    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void 
RJ_EndArray(rj_write_context *Context)
{
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 1);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos++;

    Context->InObjectOrArrayCount--;

    if(Context->Style == JWS_Standard &&
       WriteBuffer[-1] == ',')
    {
        WriteBuffer[-1] = ']';

        if(Context->InObjectOrArrayCount)
        {
            WriteBuffer[0] = ',';
        }
        else
        {
            Context->BufferWritePos--;
        }
    }
    else
    {
        WriteBuffer[0] = ']';
    }

    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void 
RJ_StartObject(rj_write_context *Context)
{
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 1);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos++;

    WriteBuffer[0] = '{';

    Context->InObjectOrArrayCount++;

    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void 
RJ_EndObject(rj_write_context *Context)
{    
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 1);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos++;

    Context->InObjectOrArrayCount--;

    if(Context->Style == JWS_Standard &&
       WriteBuffer[-1] == ',')
    {
        WriteBuffer[-1] = '}';

        if(Context->InObjectOrArrayCount)
        {
            WriteBuffer[0] = ',';
        }
        else
        {
            Context->BufferWritePos--;
        }
    }
    else
    {
        WriteBuffer[0] = '}';
    }
    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF b32
RJ_HasSpace(u8 *String, u32 Count)
{
    b32 Result = 0;
    for(u32 i = 0; i < Count; ++i)
    {
        if(RJ_IsSpace(String[i]))
        {
            Result++;
        }
    }

    return(Result);
}

RJSON_DEF void 
RJ_Key(rj_write_context *Context, u8 *Key, u32 Count)
{
    u32 WriteSize = Count + ((Context->Style == JWS_Standard) ? 3 : 1);
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + WriteSize);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos;

    switch(Context->Style)
    {
        case JWS_Standard:
        {
            WriteBuffer[0] = '"';
            for(u32 i = 1; i < (Count + 1); ++i)
            {
                WriteBuffer[i] = Key[i - 1];
            }
            WriteBuffer[Count + 1] = '"';
            WriteBuffer[Count + 2] = ':';
        }break;
        case JWS_Simplified:
        {
            if(RJ_HasSpace(Key, Count) ||
               RJ_IsNumeric(Key[0]))
            {
                WriteBuffer[0] = '"';
                for(u32 i = 1; i < (Count + 1); ++i)
                {
                    WriteBuffer[i] = Key[i - 1];
                }
                WriteBuffer[Count + 1] = '"';

                WriteSize += 2;
            }
            else
            {
                for(u32 i = 0; i < Count; ++i)
                {
                    WriteBuffer[i] = Key[i];
                }

            }

            WriteBuffer[WriteSize - 1] = '=';
        }break;
    }

    Context->BufferWritePos += WriteSize;
    Context->FinalBufferSize = Context->BufferWritePos;
}

// TODO(rajat): Add a API function for variable floating point precision.
RJSON_DEF void 
RJ_Float(rj_write_context *Context, r32 Value)
{
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 15);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos;

    s32 Advance = snprintf((char*)WriteBuffer, 15, "%.2f", Value);

    switch(Context->Style)
    {
        case JWS_Standard:
        {
            WriteBuffer[Advance] = ',';
        }break;
        case JWS_Simplified:
        {
            WriteBuffer[Advance] = ' ';
        }break;
    }

    Context->BufferWritePos += Advance + 1;
    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void 
RJ_Int(rj_write_context *Context, s32 Value)
{
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + 15);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos;

    // TODO(rajat): Implement this ourself
    s32 Advance = snprintf((char*)WriteBuffer, 15, "%i", Value);

    switch(Context->Style)
    {
        case JWS_Standard:
        {
            WriteBuffer[Advance] = ',';
        }break;
        case JWS_Simplified:
        {
            WriteBuffer[Advance] = ' ';
        }break;
    }

    Context->BufferWritePos += Advance + 1;
    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void 
RJ_String(rj_write_context *Context, u8 *String, u32 Count)
{
    u32 WriteSize = Count + 3;
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + WriteSize);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos;

    WriteBuffer[0] = '"';
    for(u32 i = 1; i < (Count + 1); ++i)
    {
        WriteBuffer[i] = String[i - 1];
    }
    WriteBuffer[Count + 1] = '"';

    switch(Context->Style)
    {
        case JWS_Standard:
        {
            WriteBuffer[Count + 2] = ',';
        }break;
        case JWS_Simplified:
        {
            WriteBuffer[Count + 2] = ' ';
        }break;
    }

    Context->BufferWritePos += WriteSize;
    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF void RJ_StartRootObject(rj_write_context *Context)
{
    if(Context->Style == JWS_Standard)
    {
        RJ_StartObject(Context);
    }
}

RJSON_DEF void RJ_EndRootObject(rj_write_context *Context)
{
    if(Context->Style == JWS_Standard)
    {
        RJ_EndObject(Context);
    }
}

RJSON_DEF void 
RJ_Boolean(rj_write_context *Context, b32 Value)
{
    b32 WriteSize = (Value == 0) ? 6 : 5;
    RJ_Assert(Context->BufferSize > Context->BufferWritePos + WriteSize);

    u8 *WriteBuffer = Context->Buffer + Context->BufferWritePos;

    if(Value > 0)
    {
        WriteBuffer[0] = 't';
        WriteBuffer[1] = 'r';
        WriteBuffer[2] = 'u';
        WriteBuffer[3] = 'e';
    }
    else
    {
        WriteBuffer[0] = 'f';
        WriteBuffer[1] = 'a';
        WriteBuffer[2] = 'l';
        WriteBuffer[3] = 's';
        WriteBuffer[4] = 'e';
    }

    switch(Context->Style)
    {
        case JWS_Standard:
        {
            WriteBuffer[WriteSize - 1] = ',';
        }break;
        case JWS_Simplified:
        {
            WriteBuffer[WriteSize - 1] = ' ';
        }break;
    }

    Context->BufferWritePos += WriteSize;
    Context->FinalBufferSize = Context->BufferWritePos;
}

RJSON_DEF u32 
RJ_GetParsedStringLength(u8 *String, u32 StringCount)
{
    u32 Count = 0;
    for(u32 i = 0; i < StringCount; ++i)
    {
        if(String[i] == '\\')
        {
            switch(String[i + 1])
            {
                case '\\':
                case 'n':
                case 'f':
                case 'r':
                case 't':
                case '"':
                i++;
                break;
            }
        }

        Count++;
    }

    return(Count);
}

RJSON_DEF b32
RJ_ParseString(u8 *String, u32 StringCount, u8 *OutBuffer, u32 OutBufferSize)
{
    b32 Result = 1;
    for(u32 i = 0; (i < OutBufferSize && i < StringCount); ++i)
    {
        if(String[i] == '\\')
        {
            switch(String[i + 1])
            {
                case '\\':
                {
                    OutBuffer[i] = '\\';
                    String++;
                    StringCount--;
                }break;
                case 'n':
                {
                    OutBuffer[i] = '\n';
                    String++;
                    StringCount--;
                }break;
                case 'f':
                {
                    OutBuffer[i] = '\f';
                    String++;
                    StringCount--;
                }break;
                case 'r':
                {
                    OutBuffer[i] = '\r';
                    String++;
                    StringCount--;
                }break;
                case 't':
                {
                    OutBuffer[i] = '\t';
                    String++;
                    StringCount--;
                }break;
                case '"':
                {
                    OutBuffer[i] = '"';
                    String++;
                    StringCount--;
                }
                break;
                default:
                {
                    // TODO(rajat): Invalid escape sequence.
                    Result = 0;
                }
                break;
            }
        }
        else
        {
            OutBuffer[i] = String[i];
        }
    }

    return(Result);
}

#endif

#define RJSON_H
#endif
