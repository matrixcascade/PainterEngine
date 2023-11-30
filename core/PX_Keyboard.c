#include "PX_Keyboard.h"

static PX_KeyboardKey px_kbkeys[128];

static px_void PX_KeyboardBuildScanCodeMap()
{
	px_int i;
	px_int scan_code = 0;
	PX_memset(px_kbkeys, 0, sizeof(px_kbkeys));
	//no code
	px_kbkeys[scan_code].content1[0] = (px_char)0xff;
	scan_code++;

	px_kbkeys[scan_code].vk_code = PX_VK_ESCAPE;
	PX_strset(px_kbkeys[scan_code].content1, "esc");
	scan_code++;

	//1-9
	for (i=0;i<9;i++)
	{
		const px_char table1[] = "123456789";
		const px_char table2[] = "!@#$%^&*(";
		px_kbkeys[scan_code].content1[0] = table1[i];
		px_kbkeys[scan_code].content2[0] = table2[i];
		px_kbkeys[scan_code].vk_code = table1[i];
		scan_code++;
	}

	//0
	px_kbkeys[scan_code].vk_code = '0';
	PX_strset(px_kbkeys[scan_code].content1, "0");
	PX_strset(px_kbkeys[scan_code].content2, ")");
	scan_code++;

	//-
	px_kbkeys[scan_code].vk_code = 189;
	PX_strset(px_kbkeys[scan_code].content1, "-");
	PX_strset(px_kbkeys[scan_code].content2, "_");
	scan_code++;

	//=
	px_kbkeys[scan_code].vk_code = 187;
	PX_strset(px_kbkeys[scan_code].content1, "=");
	PX_strset(px_kbkeys[scan_code].content2, "+");
	scan_code++;

	//backspace
	px_kbkeys[scan_code].vk_code = PX_VK_BACK;
	PX_strset(px_kbkeys[scan_code].content1, "back");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//Tab
	px_kbkeys[scan_code].vk_code = PX_VK_TAB;
	PX_strset(px_kbkeys[scan_code].content1, "tab");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	for (i = 0; i < 10; i++)
	{
		const px_char table1[] = "qwertyuiop";
		const px_char table2[] = "QWERTYUIOP";
		px_kbkeys[scan_code].content1[0] = table1[i];
		px_kbkeys[scan_code].content2[0] = table2[i];
		px_kbkeys[scan_code].vk_code = table2[i];
		scan_code++;
	}

	//[
	px_kbkeys[scan_code].vk_code = 219;
	PX_strset(px_kbkeys[scan_code].content1, "[");
	PX_strset(px_kbkeys[scan_code].content2, "{");
	scan_code++;

	//]
	px_kbkeys[scan_code].vk_code = 221;
	PX_strset(px_kbkeys[scan_code].content1, "]");
	PX_strset(px_kbkeys[scan_code].content2, "}");
	scan_code++;

	//Enter
	px_kbkeys[scan_code].vk_code = PX_VK_RETURN;
	PX_strset(px_kbkeys[scan_code].content1, "enter");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//Ctrl
	px_kbkeys[scan_code].vk_code = PX_VK_CONTROL;
	PX_strset(px_kbkeys[scan_code].content1, "ctrl");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//a-l
	for (i = 0; i < 9; i++)
	{
		const px_char table1[] = "asdfghjkl";
		const px_char table2[] = "ASDFGHJKL";
		px_kbkeys[scan_code].content1[0] = table1[i];
		px_kbkeys[scan_code].content2[0] = table2[i];
		px_kbkeys[scan_code].vk_code = table2[i];
		scan_code++;
	}

	//;
	px_kbkeys[scan_code].vk_code = 186;
	PX_strset(px_kbkeys[scan_code].content1, ";");
	PX_strset(px_kbkeys[scan_code].content2, ":");
	scan_code++;

	//'
	px_kbkeys[scan_code].vk_code = 222;
	PX_strset(px_kbkeys[scan_code].content1, "'");
	PX_strset(px_kbkeys[scan_code].content2, "\"");
	scan_code++;

	//`
	px_kbkeys[scan_code].vk_code = 192;
	PX_strset(px_kbkeys[scan_code].content1, "`");
	PX_strset(px_kbkeys[scan_code].content2, "~");
	scan_code++;

	//LSHIFT
	px_kbkeys[scan_code].vk_code = PX_VK_LSHIFT;
	PX_strset(px_kbkeys[scan_code].content1, "lshift");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//
	px_kbkeys[scan_code].vk_code = 220;
	PX_strset(px_kbkeys[scan_code].content1, "\\");
	PX_strset(px_kbkeys[scan_code].content2, "|");
	scan_code++;

	//Z-M
	for (i = 0; i < 7; i++)
	{
		const px_char table1[] = "zxcvbnm";
		const px_char table2[] = "ZXCVBNM";
		px_kbkeys[scan_code].content1[0] = table1[i];
		px_kbkeys[scan_code].content2[0] = table2[i];
		px_kbkeys[scan_code].vk_code = table2[i];
		scan_code++;
	}

	//,
	px_kbkeys[scan_code].vk_code = 188;
	PX_strset(px_kbkeys[scan_code].content1, ",");
	PX_strset(px_kbkeys[scan_code].content2, "<");
	scan_code++;

	//.
	px_kbkeys[scan_code].vk_code = 190;
	PX_strset(px_kbkeys[scan_code].content1, ".");
	PX_strset(px_kbkeys[scan_code].content2, ">");
	scan_code++;

	//
	px_kbkeys[scan_code].vk_code = 191;
	PX_strset(px_kbkeys[scan_code].content1, "/");
	PX_strset(px_kbkeys[scan_code].content2, "?");
	scan_code++;

	//RSHIFT
	px_kbkeys[scan_code].vk_code = PX_VK_RSHIFT;
	PX_strset(px_kbkeys[scan_code].content1, "rshift");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//grey
	px_kbkeys[scan_code].vk_code = 0;
	PX_strset(px_kbkeys[scan_code].content1, "grey");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//alt
	px_kbkeys[scan_code].vk_code = 18;
	PX_strset(px_kbkeys[scan_code].content1, "alt");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//space
	px_kbkeys[scan_code].vk_code = PX_VK_SPACE;
	PX_strset(px_kbkeys[scan_code].content1, "space");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//capsl
	px_kbkeys[scan_code].vk_code = PX_VK_CAPITAL;
	PX_strset(px_kbkeys[scan_code].content1, "caps");
	PX_strset(px_kbkeys[scan_code].content2, "");
	scan_code++;

	//F1-F10
	for (i = 1; i < 10; i++)
	{
		PX_sprintf1(px_kbkeys[scan_code].content1,8,"F%1",PX_STRINGFORMAT_INT(i));
		px_kbkeys[scan_code].vk_code = PX_VK_F1+i-1;
		scan_code++;
	}

	scan_code = 0x48;
	px_kbkeys[scan_code].vk_code = PX_VK_UP;
	PX_strset(px_kbkeys[scan_code].content1, "up");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x4B;
	px_kbkeys[scan_code].vk_code = PX_VK_LEFT;
	PX_strset(px_kbkeys[scan_code].content1, "left");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x4D;
	px_kbkeys[scan_code].vk_code = PX_VK_RIGHT;
	PX_strset(px_kbkeys[scan_code].content1, "right");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x50;
	px_kbkeys[scan_code].vk_code = PX_VK_DOWN;
	PX_strset(px_kbkeys[scan_code].content1, "down");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x57;
	px_kbkeys[scan_code].vk_code = PX_VK_F11;
	PX_strset(px_kbkeys[scan_code].content1, "f11");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x58;
	px_kbkeys[scan_code].vk_code = PX_VK_F12;
	PX_strset(px_kbkeys[scan_code].content1, "f12");
	PX_strset(px_kbkeys[scan_code].content2, "");

	scan_code = 0x70;
	px_kbkeys[scan_code].vk_code = PX_VK_DELETE;
	PX_strset(px_kbkeys[scan_code].content1, "delete");
	PX_strset(px_kbkeys[scan_code].content2, "");

}

px_byte PX_KeyboardGetScanCode(const px_char content[])
{
	px_int i;
	if (!px_kbkeys[0].content1[0])
	{
		PX_KeyboardBuildScanCodeMap();
	}
	for (i=0;i<PX_COUNTOF(px_kbkeys);i++)
	{
		if (PX_strequ2(content,px_kbkeys[i].content1)|| PX_strequ2(content, px_kbkeys[i].content2))
		{
			return i;
		}
	}

	return 0;
}

px_byte PX_KeyboardScanCodeToVK(px_byte scan_code)
{
	if (!px_kbkeys[0].content1[0])
	{
		PX_KeyboardBuildScanCodeMap();
	}
	return px_kbkeys[scan_code].vk_code;
}

px_byte PX_KeyboardVKToScanCode(px_byte vk)
{
	px_int i;
	if (!px_kbkeys[0].content1[0])
	{
		PX_KeyboardBuildScanCodeMap();
	}
	for (i = 0; i < PX_COUNTOF(px_kbkeys); i++)
	{
		if (px_kbkeys[i].vk_code== vk)
		{
			return i;
		}
	}
	return 0;
}