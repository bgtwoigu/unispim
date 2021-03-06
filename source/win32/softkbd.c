/*	软键盘模块
 */

#include <config.h>
#include <utility.h>
#include <win32/softkbd.h>
#include <assert.h>

static int current_index = -1;	/* 当前软键盘索引 */

#pragma data_seg(HYPIM_SHARED_SEGMENT)

/* 软键盘对应键表 */
static const TCHAR softkbd_vkey_map[] =
{
	' ', '0', '1', '2', '3', '4', '5','6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xdb, 0xdc, 0xdd, 0xde
};

/* 正常字符 */
static TCHAR softkbd_char_map[SOFTKBD_NUMBER * 2][SOFTKBD_KEY_NUMBER + 1] =
{
/*  "　０１２３４５６７８９ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ；＝，－．／｀［＼］＇", */
	TEXT(" ⑽⑴⑵⑶⑷⑸⑹⑺⑻⑼■α∑▲⒊●★【⒏】『‘γβ⒐⒑⒈⒋◆⒌⒎§⒉‰⒍＄；＝，－。／｀［、］’"),
	TEXT(" ㈩㈠㈡㈢㈣㈤㈥㈦㈧㈨□±∏△Ⅲ○☆〖Ⅷ〗』“÷×ⅨⅩⅠⅣ◇ⅤⅦ※Ⅱ℃Ⅵ￥：＋《＿》？～｛…｝”"),
	TEXT(" ˉ，、；：？！…—·〔（【〈“〉《》∶「」『［）＂＇‘”〕々‖】’〗～〖』¨］ˇ｛｝。｀〃｜．"),
	TEXT("                                                "),
	TEXT("           ■＾＠▲☆※→←◇↑↓〓￣＿◆□§★△○◎＼№＆●＃           "),
	TEXT("                                                "),
	TEXT(" ┄┍┎┏┐┑┒┓ ─┬╀┾┮┞┯┰┱┣┲┳ ╂╁ │├┟┭┠┢┿┝┽┡┼  ╃┈  ┌┆ ┊ "),
	TEXT(" ┅┕┖┗┘┙┚┛ ━┴╈╆┶┦┷┸┹┫┺┻ ╊╉ ┃┤┧┵┨┪╇┥╅┩╄  ╋┉  └┇ ┋ "),
	TEXT(" ⅪⅡⅢⅣⅤⅥⅦⅧⅨⅩ㈠⑸⑶㈢⒊㈣㈤㈥⒏㈦㈧㈨⑺⑹⒐⒑⒈⒋㈡⒌⒎⑷⒉⑵⒍⑴㈩ ⑻Ⅻ⑼⑽Ⅰ    "),
	TEXT("           ①⒂⒀③⒔④⑤⑥⒙⑦⑧⑨⒄⒃⒚⒛⒒⒕②⒖⒘⒁⒓⑿⒗⑾⑩ ⒅ ⒆⒇     "),
	TEXT("  ≡≠＝≤≥＜＞≮≯∧⊙∠∑－∏∪∩∫∈ ∵∽≌∮∝±×∨÷ ⌒＋∥／⊥∴  ∷√ ≈∞   "),
	TEXT("                                                "),
	TEXT(" ¤°′″＄￡￥‰％℃百 毫万二亿兆吉七太拍艾  八九○三千四六微一厘五分   ￠   十   "),
	TEXT("           佰   贰   柒     捌玖零叁仟肆陆 壹 伍        拾   "),
	TEXT("           ē ǔěǎè īǒíǐìǘǖò āàé óùáúōū  ǚ ǜü ê   "),
	TEXT("                                                "),
	TEXT(" ㄢㄅㄉˇˋㄓˊ˙ㄚㄞㄇㄖㄏㄎㄍㄑㄕㄘㄛㄨㄜㄠㄩㄙㄟㄣㄆㄐㄋㄔㄧㄒㄊㄌㄗㄈㄤ ㄝㄦㄡㄥ    "),
	TEXT("                                                "),
	TEXT("  ぃぅぇぉかきくけこなもむぬすねのはつひふへゅゃってさせにそちめしみたまほ ょんゎをぁと ゐゑ"),
	TEXT("  いうえおがぎぐげごぱろるぷずぺぽばづびぶべゆや でざぜぴぞぢれじりだらぼ よ わ あど   "),
	TEXT(" ケィゥヴェォカヵキクナモムヌスネノハツヒフヘュャッテサセニソチメシミタマホコョヶヮヲァトンヰヱ"),
	TEXT(" ゲイウ エオガ ギグパロルプズペポバヅビブベユヤ デザゼピゾヂレジリダラボゴヨ ワ アド   "),
	TEXT("           κχυμγνξοθπρ ωψι αδλεηφβτζσ           "),
	TEXT("           ΚΧΥΜΓΝΞΟΘΠΡ ΩΨΙ ΑΔΛΕΗΦΒΤΖΣ           "),
	TEXT("           лъшнвопржстуьызиагмдёщбчецф э юя й кх"),
	TEXT("           ЛЪШНВОПРЖСТУЬЫЗИАГМДЁЩБЧЕЦФ Э ЮЯ Й КХ"),
    TEXT(" 0123456789abcdefghijklmnopqrstuvwxyz;=,-./`[\\]'"),
	TEXT(" )!@#$%^&*(ABCDEFGHIJKLMNOPQRSTUVWXYZ:+<_>?~{|}\""),
    TEXT(" 0123456789abcdefghijklmnopqrstuvwxyz;=,-./`[\\]'"),
	TEXT(" )!@#$%^&*(ABCDEFGHIJKLMNOPQRSTUVWXYZ:+<_>?~{|}\""),
};
#pragma data_seg()

static HWND	softkbd_window;

/*	定义软键盘字符
 *	参数：
 *		index		当前软键盘的索引
 */
static void SetSoftKBDData(PIMCONTEXT *context, int index)
{
	int i;
	SOFTKBDDATA softkbd_data[2];
	LPSOFTKBDDATA data;

	if (current_index == index)
		return;

	current_index = index;

	data = &softkbd_data[0];
	data->uCount = 2;
    for (i = 0; i < SOFTKBD_KEY_NUMBER; i++)
	{
		data->wCode[0][softkbd_vkey_map[i]] = softkbd_char_map[index * 2][i];
		data->wCode[1][softkbd_vkey_map[i]] = softkbd_char_map[index * 2 + 1][i];
    }

	pim_config->soft_kbd_index = index;
	SaveConfigInternal(pim_config);
    SendMessage(softkbd_window, WM_IME_CONTROL, IMC_SETSOFTKBDDATA, (LPARAM) data);
}

/*	创建并软键盘窗口
 *	参数：
 *		ui_window		UI窗口句柄
 */
void CreateSoftKBDWindow(PIMCONTEXT *context)
{
	RECT softkbd_rect, status_rect, work_rect;
	POINT point;
	int	 x, y;

	softkbd_window = ImmCreateSoftKeyboard(SOFTKEYBOARD_TYPE_C1, context->ui_context->ui_window, 0, 0);

	GetWindowRect(softkbd_window, &softkbd_rect);
	GetWindowRect(context->ui_context->status_window, &status_rect);

	point.x = status_rect.left;
	point.y = status_rect.top;

	work_rect = GetMonitorRectFromPoint(point);

	x = work_rect.right - (softkbd_rect.right - softkbd_rect.left);
	y = status_rect.top - (softkbd_rect.bottom - softkbd_rect.top);

	if (y < work_rect.top)
		y = status_rect.bottom;

	SetWindowPos(softkbd_window, 0, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

/* 显示软键盘
 */
void ShowSoftKBDWindow()
{
	if (softkbd_window)
		ImmShowSoftKeyboard(softkbd_window, SW_SHOWNOACTIVATE);
}

/*	隐藏软键盘
 */
void HideSoftKBDWindow()
{
	if (softkbd_window)
		ImmShowSoftKeyboard(softkbd_window, SW_HIDE);
}

/*	不选择软键盘
 */
void DeSelectSoftKBD()
{
	if (softkbd_window)
		ImmDestroySoftKeyboard(softkbd_window);

	softkbd_window = 0;
	current_index  = -1;
}

/* 选择软键盘 */
void SelectSoftKBD(PIMCONTEXT *context, HIMC hIMC, int index)
{
	if (current_index != index)
	{
		if (softkbd_window)
			DeSelectSoftKBD();
	}

	if (!softkbd_window)
		CreateSoftKBDWindow(context);

	SetSoftKBDData(context, index);
}

/*	检索软键盘的符号
 */
int GetSoftKeyboardSymbol(PIMCONTEXT *context, int virtual_key, int key_flag, TCHAR *symbol)
{
	int i;

	if (key_flag & KEY_CONTROL)
		return 0;

	for (i = 0; i < SOFTKBD_KEY_NUMBER; i++)
		if ((BYTE)virtual_key == softkbd_vkey_map[i])
			break;

	if (i == SOFTKBD_KEY_NUMBER)
		return 0;

	if (((key_flag & KEY_SHIFT) && !(key_flag & KEY_CAPITAL)) ||
		(!(key_flag & KEY_SHIFT) && (key_flag & KEY_CAPITAL)))
	{
		symbol[0] = softkbd_char_map[context->softkbd_index * 2 + 1][i];
		symbol[1] = 0;
	}
	else
	{
		symbol[0] = softkbd_char_map[context->softkbd_index * 2][i];
		symbol[1] = 0;
	}

	if (symbol[0] == ' ' && symbol[1] == ' ')
		symbol[0] = 0;

	//英文键盘
	if (context->softkbd_index == SOFTKBD_NUMBER - 2)
		symbol[1] = 0;

	return 1;
}