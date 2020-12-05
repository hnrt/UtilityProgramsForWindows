using System;

namespace hnrt.HttpRelay.Net.Http
{
    /// <summary>
    /// Base on RFC 2616 (HTTP/1.1)
    /// </summary>
    internal static class HttpChar
    {
        public const int CR = 13; // '\r'
        public const int LF = 10; // '\n'
        public const int SP = 32; // ' '
        public const int HT = 9; // '\t'

        private const int A______ = 0;
        private const int A_C____ = 1; // Control
        private const int A__S___ = 2; // Separator
        private const int A_CS___ = A_C____ | A__S___;
        private const int A___D__ = 4; // Digit
        private const int A___X__ = 8; // Hexadecimal
        private const int A___DX_ = A___D__ | A___X__;
        private const int A____U_ = 16; // Uppercase Alphabet
        private const int A___XU_ = A___X__ | A____U_;
        private const int A____L_ = 32; // Lowercase Alphabet
        private const int A___XL_ = A___X__ | A____L_;
        private const int A___UL_ = A____U_ | A____L_;
        private const int A___ULD = A____U_ | A____L_ | A___D__;
        private const int A_____M = 64; // Mark (RFC 2396)
        private const int A_____R = 128; // Reserved (RFC 2396)
        private const int A__S__M = A__S___ | A_____M;
        private const int A__S__R = A__S___ | A_____R;
        private const int A__ULDM = A____U_ | A____L_ | A___D__ | A_____M;

        private static readonly int[] USASCIIMap = new int[128]
        {
            A_C____, // 000;<control>;Cc;0;BN;;;;;N;NULL;;;;
            A_C____, // 001;<control>;Cc;0;BN;;;;;N;START OF HEADING;;;;
            A_C____, // 002;<control>;Cc;0;BN;;;;;N;START OF TEXT;;;;
            A_C____, // 003;<control>;Cc;0;BN;;;;;N;END OF TEXT;;;;
            A_C____, // 004;<control>;Cc;0;BN;;;;;N;END OF TRANSMISSION;;;;
            A_C____, // 005;<control>;Cc;0;BN;;;;;N;ENQUIRY;;;;
            A_C____, // 006;<control>;Cc;0;BN;;;;;N;ACKNOWLEDGE;;;;
            A_C____, // 007;<control>;Cc;0;BN;;;;;N;BELL;;;;
            A_C____, // 008;<control>;Cc;0;BN;;;;;N;BACKSPACE;;;;
            A_CS___, // 009;<control>;Cc;0;S;;;;;N;CHARACTER TABULATION;;;;
            A_C____, // 00A;<control>;Cc;0;B;;;;;N;LINE FEED(LF);;;;
            A_C____, // 00B;<control>;Cc;0;S;;;;;N;LINE TABULATION;;;;
            A_C____, // 00C;<control>;Cc;0;WS;;;;;N;FORM FEED(FF);;;;
            A_C____, // 00D;<control>;Cc;0;B;;;;;N;CARRIAGE RETURN(CR);;;;
            A_C____, // 00E;<control>;Cc;0;BN;;;;;N;SHIFT OUT;;;;
            A_C____, // 00F;<control>;Cc;0;BN;;;;;N;SHIFT IN;;;;
            A_C____, // 010;<control>;Cc;0;BN;;;;;N;DATA LINK ESCAPE;;;;
            A_C____, // 011;<control>;Cc;0;BN;;;;;N;DEVICE CONTROL ONE;;;;
            A_C____, // 012;<control>;Cc;0;BN;;;;;N;DEVICE CONTROL TWO;;;;
            A_C____, // 013;<control>;Cc;0;BN;;;;;N;DEVICE CONTROL THREE;;;;
            A_C____, // 014;<control>;Cc;0;BN;;;;;N;DEVICE CONTROL FOUR;;;;
            A_C____, // 015;<control>;Cc;0;BN;;;;;N;NEGATIVE ACKNOWLEDGE;;;;
            A_C____, // 016;<control>;Cc;0;BN;;;;;N;SYNCHRONOUS IDLE;;;;
            A_C____, // 017;<control>;Cc;0;BN;;;;;N;END OF TRANSMISSION BLOCK;;;;
            A_C____, // 018;<control>;Cc;0;BN;;;;;N;CANCEL;;;;
            A_C____, // 019;<control>;Cc;0;BN;;;;;N;END OF MEDIUM;;;;
            A_C____, // 01A;<control>;Cc;0;BN;;;;;N;SUBSTITUTE;;;;
            A_C____, // 01B;<control>;Cc;0;BN;;;;;N;ESCAPE;;;;
            A_C____, // 01C;<control>;Cc;0;B;;;;;N;INFORMATION SEPARATOR FOUR;;;;
            A_C____, // 01D;<control>;Cc;0;B;;;;;N;INFORMATION SEPARATOR THREE;;;;
            A_C____, // 01E;<control>;Cc;0;B;;;;;N;INFORMATION SEPARATOR TWO;;;;
            A_C____, // 01F;<control>;Cc;0;S;;;;;N;INFORMATION SEPARATOR ONE;;;;
            A__S___, // 0020; SPACE;Zs;0;WS;;;;;N;;;;;
            A_____M, // 021;EXCLAMATION MARK; Po;0;ON;;;;;N;;;;;
            A__S___, // 022;QUOTATION MARK; Po;0;ON;;;;;N;;;;;
            A______, // 023;NUMBER SIGN; Po;0;ET;;;;;N;;;;;
            A_____R, // 024;DOLLAR SIGN; Sc;0;ET;;;;;N;;;;;
            A______, // 025;PERCENT SIGN; Po;0;ET;;;;;N;;;;;
            A_____R, // 026;AMPERSAND;Po;0;ON;;;;;N;;;;;
            A_____M, // 027;APOSTROPHE;Po;0;ON;;;;;N;APOSTROPHE-QUOTE;;;;
            A__S__M, // 028;LEFT PARENTHESIS; Ps;0;ON;;;;;Y;OPENING PARENTHESIS;;;;
            A__S__M, // 029;RIGHT PARENTHESIS; Pe;0;ON;;;;;Y;CLOSING PARENTHESIS;;;;
            A_____M, // 02A;ASTERISK;Po;0;ON;;;;;N;;;;;
            A_____R, // 02B;PLUS SIGN; Sm;0;ES;;;;;N;;;;;
            A__S__R, // 02C;COMMA;Po;0;CS;;;;;N;;;;;
            A_____M, // 02D;HYPHEN-MINUS;Pd;0;ES;;;;;N;;;;;
            A_____M, // 02E;FULL STOP; Po;0;CS;;;;;N;PERIOD;;;;
            A__S__R, // 02F;SOLIDUS;Po;0;CS;;;;;N;SLASH;;;;
            A___D__, // 030;DIGIT ZERO; Nd;0;EN;;0;0;0;N;;;;;
            A___D__, // 031;DIGIT ONE; Nd;0;EN;;1;1;1;N;;;;;
            A___D__, // 032;DIGIT TWO; Nd;0;EN;;2;2;2;N;;;;;
            A___D__, // 033;DIGIT THREE; Nd;0;EN;;3;3;3;N;;;;;
            A___D__, // 034;DIGIT FOUR; Nd;0;EN;;4;4;4;N;;;;;
            A___D__, // 035;DIGIT FIVE; Nd;0;EN;;5;5;5;N;;;;;
            A___D__, // 036;DIGIT SIX; Nd;0;EN;;6;6;6;N;;;;;
            A___D__, // 037;DIGIT SEVEN; Nd;0;EN;;7;7;7;N;;;;;
            A___D__, // 038;DIGIT EIGHT; Nd;0;EN;;8;8;8;N;;;;;
            A___D__, // 039;DIGIT NINE; Nd;0;EN;;9;9;9;N;;;;;
            A__S__R, // 03A;COLON;Po;0;CS;;;;;N;;;;;
            A__S__R, // 03B;SEMICOLON;Po;0;ON;;;;;N;;;;;
            A__S___, // 03C;LESS-THAN SIGN; Sm;0;ON;;;;;Y;;;;;
            A__S__R, // 03D;EQUALS SIGN; Sm;0;ON;;;;;N;;;;;
            A__S___, // 03E;GREATER-THAN SIGN; Sm;0;ON;;;;;Y;;;;;
            A__S__R, // 03F;QUESTION MARK; Po;0;ON;;;;;N;;;;;
            A__S__R, // 040;COMMERCIAL AT; Po;0;ON;;;;;N;;;;;
            A___XU_, // 041;LATIN CAPITAL LETTER A; Lu;0;L;;;;;N;;;;0061;
            A___XU_, // 042;LATIN CAPITAL LETTER B; Lu;0;L;;;;;N;;;;0062;
            A___XU_, // 043;LATIN CAPITAL LETTER C; Lu;0;L;;;;;N;;;;0063;
            A___XU_, // 044;LATIN CAPITAL LETTER D; Lu;0;L;;;;;N;;;;0064;
            A___XU_, // 045;LATIN CAPITAL LETTER E; Lu;0;L;;;;;N;;;;0065;
            A___XU_, // 046;LATIN CAPITAL LETTER F; Lu;0;L;;;;;N;;;;0066;
            A____U_, // 047;LATIN CAPITAL LETTER G; Lu;0;L;;;;;N;;;;0067;
            A____U_, // 048;LATIN CAPITAL LETTER H; Lu;0;L;;;;;N;;;;0068;
            A____U_, // 049;LATIN CAPITAL LETTER I; Lu;0;L;;;;;N;;;;0069;
            A____U_, // 04A;LATIN CAPITAL LETTER J; Lu;0;L;;;;;N;;;;006A;
            A____U_, // 04B;LATIN CAPITAL LETTER K; Lu;0;L;;;;;N;;;;006B;
            A____U_, // 04C;LATIN CAPITAL LETTER L; Lu;0;L;;;;;N;;;;006C;
            A____U_, // 04D;LATIN CAPITAL LETTER M; Lu;0;L;;;;;N;;;;006D;
            A____U_, // 04E;LATIN CAPITAL LETTER N; Lu;0;L;;;;;N;;;;006E;
            A____U_, // 04F;LATIN CAPITAL LETTER O; Lu;0;L;;;;;N;;;;006F;
            A____U_, // 050;LATIN CAPITAL LETTER P; Lu;0;L;;;;;N;;;;0070;
            A____U_, // 051;LATIN CAPITAL LETTER Q; Lu;0;L;;;;;N;;;;0071;
            A____U_, // 052;LATIN CAPITAL LETTER R; Lu;0;L;;;;;N;;;;0072;
            A____U_, // 053;LATIN CAPITAL LETTER S; Lu;0;L;;;;;N;;;;0073;
            A____U_, // 054;LATIN CAPITAL LETTER T; Lu;0;L;;;;;N;;;;0074;
            A____U_, // 055;LATIN CAPITAL LETTER U; Lu;0;L;;;;;N;;;;0075;
            A____U_, // 056;LATIN CAPITAL LETTER V; Lu;0;L;;;;;N;;;;0076;
            A____U_, // 057;LATIN CAPITAL LETTER W; Lu;0;L;;;;;N;;;;0077;
            A____U_, // 058;LATIN CAPITAL LETTER X; Lu;0;L;;;;;N;;;;0078;
            A____U_, // 059;LATIN CAPITAL LETTER Y; Lu;0;L;;;;;N;;;;0079;
            A____U_, // 05A;LATIN CAPITAL LETTER Z; Lu;0;L;;;;;N;;;;007A;
            A__S___, // 05B;LEFT SQUARE BRACKET;Ps;0;ON;;;;;Y;OPENING SQUARE BRACKET;;;;
            A__S___, // 05C;REVERSE SOLIDUS; Po;0;ON;;;;;N;BACKSLASH;;;;
            A__S___, // 05D;RIGHT SQUARE BRACKET;Pe;0;ON;;;;;Y;CLOSING SQUARE BRACKET;;;;
            A______, // 05E;CIRCUMFLEX ACCENT; Sk;0;ON;;;;;N;SPACING CIRCUMFLEX;;;;
            A_____M, // 05F;LOW LINE; Pc;0;ON;;;;;N;SPACING UNDERSCORE;;;;
            A______, // 060;GRAVE ACCENT; Sk;0;ON;;;;;N;SPACING GRAVE;;;;
            A___XL_, // 061;LATIN SMALL LETTER A; Ll;0;L;;;;;N;;;0041;;0041
            A___XL_, // 062;LATIN SMALL LETTER B; Ll;0;L;;;;;N;;;0042;;0042
            A___XL_, // 063;LATIN SMALL LETTER C; Ll;0;L;;;;;N;;;0043;;0043
            A___XL_, // 064;LATIN SMALL LETTER D; Ll;0;L;;;;;N;;;0044;;0044
            A___XL_, // 065;LATIN SMALL LETTER E; Ll;0;L;;;;;N;;;0045;;0045
            A___XL_, // 066;LATIN SMALL LETTER F; Ll;0;L;;;;;N;;;0046;;0046
            A____L_, // 067;LATIN SMALL LETTER G; Ll;0;L;;;;;N;;;0047;;0047
            A____L_, // 068;LATIN SMALL LETTER H; Ll;0;L;;;;;N;;;0048;;0048
            A____L_, // 069;LATIN SMALL LETTER I; Ll;0;L;;;;;N;;;0049;;0049
            A____L_, // 06A;LATIN SMALL LETTER J; Ll;0;L;;;;;N;;;004A;;004A
            A____L_, // 06B;LATIN SMALL LETTER K; Ll;0;L;;;;;N;;;004B;;004B
            A____L_, // 06C;LATIN SMALL LETTER L; Ll;0;L;;;;;N;;;004C;;004C
            A____L_, // 06D;LATIN SMALL LETTER M; Ll;0;L;;;;;N;;;004D;;004D
            A____L_, // 06E;LATIN SMALL LETTER N; Ll;0;L;;;;;N;;;004E;;004E
            A____L_, // 06F;LATIN SMALL LETTER O; Ll;0;L;;;;;N;;;004F;;004F
            A____L_, // 070;LATIN SMALL LETTER P; Ll;0;L;;;;;N;;;0050;;0050
            A____L_, // 071;LATIN SMALL LETTER Q; Ll;0;L;;;;;N;;;0051;;0051
            A____L_, // 072;LATIN SMALL LETTER R; Ll;0;L;;;;;N;;;0052;;0052
            A____L_, // 073;LATIN SMALL LETTER S; Ll;0;L;;;;;N;;;0053;;0053
            A____L_, // 074;LATIN SMALL LETTER T; Ll;0;L;;;;;N;;;0054;;0054
            A____L_, // 075;LATIN SMALL LETTER U; Ll;0;L;;;;;N;;;0055;;0055
            A____L_, // 076;LATIN SMALL LETTER V; Ll;0;L;;;;;N;;;0056;;0056
            A____L_, // 077;LATIN SMALL LETTER W; Ll;0;L;;;;;N;;;0057;;0057
            A____L_, // 078;LATIN SMALL LETTER X; Ll;0;L;;;;;N;;;0058;;0058
            A____L_, // 079;LATIN SMALL LETTER Y; Ll;0;L;;;;;N;;;0059;;0059
            A____L_, // 07A;LATIN SMALL LETTER Z; Ll;0;L;;;;;N;;;005A;;005A
            A__S___, // 07B;LEFT CURLY BRACKET;Ps;0;ON;;;;;Y;OPENING CURLY BRACKET;;;;
            A______, // 07C;VERTICAL LINE; Sm;0;ON;;;;;N;VERTICAL BAR;;;;
            A__S___, // 07D;RIGHT CURLY BRACKET;Pe;0;ON;;;;;Y;CLOSING CURLY BRACKET;;;;
            A_____M, // 07E;TILDE;Sm;0;ON;;;;;N;;;;;
            A_C____, // 07F;<control>;Cc;0;BN;;;;;N;DELETE;;;;
        };

        public static bool IsChar(int c)
        {
            return 0 <= c && c <= 127;
        }

        public static bool IsControl(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A_C____) != 0;
        }

        public static bool IsSeparator(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A__S___) != 0;
        }

        public static bool IsDigit(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A___D__) != 0;
        }

        public static bool IsHexDigit(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A___DX_) != 0;
        }

        public static bool IsUpperAlpha(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A____U_) != 0;
        }

        public static bool IsLowerAlpha(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A____L_) != 0;
        }

        public static bool IsAlpha(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A___UL_) != 0;
        }

        public static bool IsAlphaNum(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A___ULD) != 0;
        }

        public static bool IsMark(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A_____M) != 0;
        }

        public static bool IsReserved(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A_____R) != 0;
        }

        public static bool IsUnreserved(int c)
        {
            return 0 <= c && c < USASCIIMap.Length && (USASCIIMap[c] & A__ULDM) != 0;
        }
    }
}
