#include "stdafx.h"
#include "Keyboard.h"
#include <string>
#include "Logging.h"
#include <FL/Enumerations.H>

#define SL_MULTIPLY       0xf06A
#define SL_ADD            0xf06B
#define SL_SEPARATOR      0xf06C
#define SL_SUBTRACT       0xf06D
#define SL_DECIMAL        0xf06E
#define SL_DIVIDE         0xf06F


int KeyboardMap[] = {

#if _WIN32

	0, // [0]
	0, // [1]
	0, // [2]
	VK_CANCEL,//"CANCEL", // [3]
	0, // [4]
	0, // [5]
	VK_HELP,//"HELP", // [6]
	0, // [7]
	VK_BACK,//"BACK_SPACE", // [8]
	VK_TAB,//"TAB", // [9]
	0, // [10]
	0, // [11]
	VK_CLEAR,//"CLEAR", // [12]
	VK_RETURN,//"ENTER", // [13]
	0, // [14]
	0, // [15]
	VK_LSHIFT,//"SHIFT", // [16]
	VK_LCONTROL,//"CONTROL", // [17]
	VK_MENU,//"ALT", // [18]
	VK_PAUSE,//"PAUSE", // [19]
	VK_CAPITAL,//"CAPS_LOCK", // [20]
	VK_KANA,//"KANA", // [21]
	0,//"EISU", // [22]
	VK_JUNJA,//"JUNJA", // [23]
	VK_FINAL,//"FINAL", // [24]
	VK_HANJA,//"HANJA", // [25]
	0, // [26]
	VK_ESCAPE,//"ESCAPE", // [27]
	VK_CONVERT,//"CONVERT", // [28]
	VK_NONCONVERT,//"NONCONVERT", // [29]
	VK_ACCEPT,//"ACCEPT", // [30]
	VK_MODECHANGE,//"MODECHANGE", // [31]
	VK_SPACE,//"SPACE", // [32]
	VK_PRIOR,//"PAGE_UP", // [33]
	VK_NEXT,//"PAGE_DOWN", // [34]
	VK_END,//"END", // [35]
	VK_HOME,//"HOME", // [36]
	VK_LEFT,//"LEFT", // [37]
	VK_UP,//"UP", // [38]
	VK_RIGHT,//"RIGHT", // [39]
	VK_DOWN,//"DOWN", // [40]
	VK_SELECT,//"SELECT", // [41]
	VK_PRINT,//"PRINT", // [42]
	VK_EXECUTE,//"EXECUTE", // [43]
	VK_SNAPSHOT,//"PRINTSCREEN", // [44]
	VK_INSERT,//"INSERT", // [45]
	VK_DELETE,//"DELETE", // [46]
	0,//0, // [47]
	0x30, //"0", // [48]
	0x31, //"1", // [49]
	0x32, //"2", // [50]
	0x33, //"3", // [51]
	0x34, //"4", // [52]
	0x35, //"5", // [53]
	0x36, //"6", // [54]
	0x37, //"7", // [55]
	0x38, //"8", // [56]
	0x39, //"9", // [57]
	0x3A,//"COLON", // [58]
	0x3B, //"SEMICOLON", // [59]
	0x3C, //"LESS_THAN", // [60]
	0x3D,// "EQUALS", // [61]
	0x3E,//"GREATER_THAN", // [62]
	0x3F, //"QUESTION_MARK", // [63]
	0x40,//"AT", // [64]
	0x41,//"A", // [65]
	0x42,//"B", // [66]
	0x43,//"C", // [67]
	0x44,//"D", // [68]
	0x45,//"E", // [69]
	0x46,//"F", // [70]
	0x47,//"G", // [71]
	0x48,//"H", // [72]
	0x49,//"I", // [73]
	0x4A,//"J", // [74]
	0x4B,//"K", // [75]
	0x4C,//"L", // [76]
	0x4D,//"M", // [77]
	0x4E,//"N", // [78]
	0x4F,//"O", // [79]
	0x50,//"P", // [80]
	0x51,//"Q", // [81]
	0x52,//"R", // [82]
	0x53,//"S", // [83]
	0x54,//"T", // [84]
	0x55,//"U", // [85]
	0x56,//"V", // [86]
	0x57,//"W", // [87]
	0x58,//"X", // [88]
	0x59,//"Y", // [89]
	0x5A,//"Z", // [90]
	VK_LWIN,//"OS_KEY", // [91] Windows Key (Windows) or Command Key (Mac)
	0, // [92]
	VK_MENU,//"CONTEXT_MENU", // [93]
	0, // [94]
	0,//"SLEEP", // [95]
	VK_NUMPAD0,//"NUMPAD0", // [96]
	VK_NUMPAD1,//"NUMPAD1", // [97]
	VK_NUMPAD2,//"NUMPAD2", // [98]
	VK_NUMPAD3,//"NUMPAD3", // [99]
	VK_NUMPAD4,//"NUMPAD4", // [100]
	VK_NUMPAD5,//"NUMPAD5", // [101]
	VK_NUMPAD6,//"NUMPAD6", // [102]
	VK_NUMPAD7,//"NUMPAD8", // [104]
	VK_NUMPAD8,//"NUMPAD8", // [104]
	VK_NUMPAD9,//"NUMPAD9", // [105]
	VK_MULTIPLY,//"MULTIPLY", // [106]
	VK_ADD,//"ADD", // [107]
	VK_SEPARATOR,//"SEPARATOR", // [108]
	VK_SUBTRACT,//"SUBTRACT", // [109]
	VK_DECIMAL,//"DECIMAL", // [110]
	VK_DIVIDE,//"DIVIDE", // [111]
	VK_F1,//"F1", // [112]
	VK_F2,//"F2", // [113]
	VK_F3,//"F3", // [114]
	VK_F4,//"F4", // [115]
	VK_F5,//"F5", // [116]
	VK_F6,//"F6", // [117]
	VK_F7,//"F7", // [118]
	VK_F8,//"F8", // [119]
	VK_F9,//"F9", // [120]
	VK_F10,//"F10", // [121]
	VK_F11,//"F11", // [122]
	VK_F12,//"F12", // [123]
	VK_F13,//"F13", // [124]
	VK_F14,//"F14", // [125]
	VK_F15,//"F15", // [126]
	VK_F16,//"F16", // [127]
	VK_F17,//"F17", // [128]
	VK_F18,//"F18", // [129]
	VK_F19,//"F19", // [130]
	VK_F20,//"F20", // [131]
	VK_F21,//"F21", // [132]
	VK_F22,//"F22", // [133]
	VK_F23,//F23", // [134]
	VK_F24,//"F24", // [135]
	0, // [136]
	0, // [137]
	0, // [138]
	0, // [139]
	0, // [140]
	0, // [141]
	0, // [142]
	0, // [143]
	VK_NUMLOCK,//"NUM_LOCK", // [144]
	VK_SCROLL,//"SCROLL_LOCK", // [145]
	0,//"WIN_OEM_FJ_JISHO", // [146]
	0,//"WIN_OEM_FJ_MASSHOU", // [147]
	0,//"WIN_OEM_FJ_TOUROKU", // [148]
	0,//"WIN_OEM_FJ_LOYA", // [149]
	0,//"WIN_OEM_FJ_ROYA", // [150]
	0, // [151]
	0, // [152]
	0, // [153]
	0, // [154]
	0, // [155]
	0, // [156]
	0, // [157]
	0, // [158]
	0, // [159]
	0, //"CIRCUMFLEX", // [160]
	0,//"EXCLAMATION", // [161]
	0,//"DOUBLE_QUOTE", // [162]
	0,//"HASH", // [163]
	0,//"DOLLAR", // [164]
	0,//"PERCENT", // [165]
	0,//"AMPERSAND", // [166]
	0,//"UNDERSCORE", // [167]
	0,//"OPEN_PAREN", // [168]
	0,//"CLOSE_PAREN", // [169]
	0,//"ASTERISK", // [170]
	VK_OEM_PLUS,//"PLUS", // [171]
	VK_OEM_5,//"PIPE", // [172]
	VK_OEM_MINUS,//"HYPHEN_MINUS", // [173]
	VK_OEM_4,//"OPEN_CURLY_BRACKET", // [174]
	VK_OEM_6,//"CLOSE_CURLY_BRACKET", // [175]
	VK_OEM_3,//"TILDE", // [176]
	0, // [177]
	0, // [178]
	0, // [179]
	0, // [180]
	VK_VOLUME_MUTE,//"VOLUME_MUTE", // [181]
	VK_VOLUME_DOWN,//"VOLUME_DOWN", // [182]
	VK_VOLUME_UP,//"VOLUME_UP", // [183]
	0, // [184]
	0, // [185]
	VK_OEM_1,//"SEMICOLON", // [186]
	VK_OEM_NEC_EQUAL,//"EQUALS", // [187]
	VK_OEM_COMMA,//"COMMA", // [188]
	VK_OEM_MINUS,//"MINUS", // [189]
	VK_OEM_PERIOD,//"PERIOD", // [190]
	VK_OEM_2,//"SLASH", // [191]
	VK_OEM_3,//"BACK_QUOTE", // [192]
	0, // [193]
	0, // [194]
	0, // [195]
	0, // [196]
	0, // [197]
	0, // [198]
	0, // [199]
	0, // [200]
	0, // [201]
	0, // [202]
	0, // [203]
	0, // [204]
	0, // [205]
	0, // [206]
	0, // [207]
	0, // [208]
	0, // [209]
	0, // [210]
	0, // [211]
	0, // [212]
	0, // [213]
	0, // [214]
	0, // [215]
	0, // [216]
	0, // [217]
	0, // [218]
	VK_OEM_4,//"OPEN_BRACKET", // [219]
	VK_OEM_5,//"BACK_SLASH", // [220]
	VK_OEM_6,//"CLOSE_BRACKET", // [221]
	VK_OEM_7,//"QUOTE", // [222]
	0, // [223]
	0,//"META", // [224]
	0,//"ALTGR", // [225]
	0, // [226]
	0,//"WIN_ICO_HELP", // [227]
	0,//"WIN_ICO_00", // [228]
	0, // [229]
	0,//"WIN_ICO_CLEAR", // [230]
	0, // [231]
	0, // [232]
	0,//"WIN_OEM_RESET", // [233]
	0,//"WIN_OEM_JUMP", // [234]
	0,//"WIN_OEM_PA1", // [235]
	0,//"WIN_OEM_PA2", // [236]
	0,//"WIN_OEM_PA3", // [237]
	0,//"WIN_OEM_WSCTRL", // [238]
	0,//"WIN_OEM_CUSEL", // [239]
	0,//"WIN_OEM_ATTN", // [240]
	0,//"WIN_OEM_FINISH", // [241]
	0,//"WIN_OEM_COPY", // [242]
	0,//"WIN_OEM_AUTO", // [243]
	0,//"WIN_OEM_ENLW", // [244]
	0,//"WIN_OEM_BACKTAB", // [245]
	0,//"ATTN", // [246]
	0,//"CRSEL", // [247]
	0,//"EXSEL", // [248]
	0,//"EREOF", // [249]
	0,//"PLAY", // [250]
	0,//"ZOOM", // [251]
	0, // [252]
	0,//"PA1", // [253]
	0,//"WIN_OEM_CLEAR", // [254]
	0 // [255]

#elif __APPLE__
0
#elif __ANDROID__
0
#elif __linux__

	0, // [0]
	0, // [1]
	0, // [2]
	XK_Cancel,//"CANCEL", // [3]
	0, // [4]
	0, // [5]
	XK_Help,//"HELP", // [6]
	0, // [7]
	XK_BackSpace,//"BACK_SPACE", // [8]
	XK_Tab,//"TAB", // [9]
	0, // [10]
	0, // [11]
	XK_Clear,//"CLEAR", // [12]
	XK_Return,//"ENTER", // [13]
	0, // [14]
	0, // [15]
	XK_Shift_L,//"SHIFT", // [16]
	XK_Control_L,//"CONTROL", // [17]
	XK_Alt_L,//"ALT", // [18]
	XK_Pause,//"PAUSE", // [19]
	XK_Caps_Lock,//"CAPS_LOCK", // [20]
	0,//"KANA", // [21]
	0,//"EISU", // [22]
	0,//"JUNJA", // [23]
	0,//"FINAL", // [24]
	0,//"HANJA", // [25]
	0, // [26]
	XK_Escape,//"ESCAPE", // [27]
	0,//"CONVERT", // [28]
	0,//"NONCONVERT", // [29]
	0,//"ACCEPT", // [30]
	0,//"MODECHANGE", // [31]
	XK_space,//"SPACE", // [32]
	XK_Page_Up,//"PAGE_UP", // [33]
	XK_Page_Down,//"PAGE_DOWN", // [34]
	XK_End,//"END", // [35]
	XK_KP_Home,//"HOME", // [36]
	XK_Left,//"LEFT", // [37]
	XK_Up,//"UP", // [38]
	XK_Right,//"RIGHT", // [39]
	XK_Down,//"DOWN", // [40]
	XK_Select,//"SELECT", // [41]
	XK_Print,//"PRINT", // [42]
	XK_Execute,//"EXECUTE", // [43]
	XK_Print,//"PRINTSCREEN", // [44]
	XK_Insert,//"INSERT", // [45]
	XK_Delete,//"DELETE", // [46]
	0,//0, // [47]
	XK_0, //"0", // [48]
	XK_1, //"1", // [49]
	XK_2, //"2", // [50]
	XK_3, //"3", // [51]
	XK_4, //"4", // [52]
	XK_5, //"5", // [53]
	XK_6, //"6", // [54]
	XK_7, //"7", // [55]
	XK_8, //"8", // [56]
	XK_9, //"9", // [57]
	XK_colon,//"COLON", // [58]
	XK_semicolon, //"SEMICOLON", // [59]
	XK_less, //"LESS_THAN", // [60]
	XK_equal,// "EQUALS", // [61]
	XK_greater,//"GREATER_THAN", // [62]
	XK_question, //"QUESTION_MARK", // [63]
	XK_at,//"AT", // [64]
	XK_A,//"A", // [65]
	XK_B,//"B", // [66]
	XK_C,//"C", // [67]
	XK_D,//"D", // [68]
	XK_E,//"E", // [69]
	XK_F,//"F", // [70]
	XK_G,//"G", // [71]
	XK_H,//"H", // [72]
	XK_I,//"I", // [73]
	XK_J,//"J", // [74]
	XK_K,//"K", // [75]
	XK_L,//"L", // [76]
	XK_M,//"M", // [77]
	XK_N,//"N", // [78]
	XK_O,//"O", // [79]
	XK_P,//"P", // [80]
	XK_Q,//"Q", // [81]
	XK_R,//"R", // [82]
	XK_S,//"S", // [83]
	XK_T,//"T", // [84]
	XK_U,//"U", // [85]
	XK_V,//"V", // [86]
	XK_W,//"W", // [87]
	XK_X,//"X", // [88]
	XK_Y,//"Y", // [89]
	XK_Z,//"Z", // [90]
	0,//"OS_KEY", // [91] Windows Key (Windows) or Command Key (Mac)
	0, // [92]
	XK_Menu,//"CONTEXT_MENU", // [93]
	0, // [94]
	0,//"SLEEP", // [95]
	XK_KP_0,//"NUMPAD0", // [96]
	XK_KP_1,//"NUMPAD1", // [97]
	XK_KP_2,//"NUMPAD2", // [98]
	XK_KP_3,//"NUMPAD3", // [99]
	XK_KP_4,//"NUMPAD4", // [100]
	XK_KP_5,//"NUMPAD5", // [101]
	XK_KP_6,//"NUMPAD6", // [102]
	XK_KP_7,//"NUMPAD7", // [103]
	XK_KP_8,//"NUMPAD8", // [104]
	XK_KP_9,//"NUMPAD9", // [105]
	XK_KP_Multiply,//"MULTIPLY", // [106]
	XK_KP_Add,//"ADD", // [107]
	XK_KP_Separator,//"SEPARATOR", // [108]
	XK_KP_Subtract,//"SUBTRACT", // [109]
	XK_KP_Decimal,//"DECIMAL", // [110]
	XK_KP_Divide,//"DIVIDE", // [111]
	XK_F1,//"F1", // [112]
	XK_F2,//"F2", // [113]
	XK_F3,//"F3", // [114]
	XK_F4,//"F4", // [115]
	XK_F5,//"F5", // [116]
	XK_F6,//"F6", // [117]
	XK_F7,//"F7", // [118]
	XK_F8,//"F8", // [119]
	XK_F9,//"F9", // [120]
	XK_F10,//"F10", // [121]
	XK_F11,//"F11", // [122]
	XK_F12,//"F12", // [123]
	XK_F13,//"F13", // [124]
	XK_F14,//"F14", // [125]
	XK_F15,//"F15", // [126]
	XK_F16,//"F16", // [127]
	XK_F17,//"F17", // [128]
	XK_F18,//"F18", // [129]
	XK_F19,//"F19", // [130]
	XK_F20,//"F20", // [131]
	XK_F21,//"F21", // [132]
	XK_F22,//"F22", // [133]
	XK_F23,//F23", // [134]
	XK_F24,//"F24", // [135]
	0, // [136]
	0, // [137]
	0, // [138]
	0, // [139]
	0, // [140]
	0, // [141]
	0, // [142]
	0, // [143]
	XK_Num_Lock,//"NUM_LOCK", // [144]
	XK_Scroll_Lock,//"SCROLL_LOCK", // [145]
	0,//"WIN_OEM_FJ_JISHO", // [146]
	0,//"WIN_OEM_FJ_MASSHOU", // [147]
	0,//"WIN_OEM_FJ_TOUROKU", // [148]
	0,//"WIN_OEM_FJ_LOYA", // [149]
	0,//"WIN_OEM_FJ_ROYA", // [150]
	0, // [151]
	0, // [152]
	0, // [153]
	0, // [154]
	0, // [155]
	0, // [156]
	0, // [157]
	0, // [158]
	0, // [159]
	0, //"CIRCUMFLEX", // [160]
	XK_exclam,//"EXCLAMATION", // [161]
	XK_quotedbl,//"DOUBLE_QUOTE", // [162]
	XK_numbersign,//"HASH", // [163]
	XK_dollar,//"DOLLAR", // [164]
	XK_percent,//"PERCENT", // [165]
	XK_ampersand,//"AMPERSAND", // [166]
	XK_underscore,//"UNDERSCORE", // [167]
	XK_parenleft,//"OPEN_PAREN", // [168]
	XK_parenright,//"CLOSE_PAREN", // [169]
	XK_asterisk,//"ASTERISK", // [170]
	XK_plus,//"PLUS", // [171]
	XK_bar,//"PIPE", // [172]
	XK_minus,//"HYPHEN_MINUS", // [173]
	XK_braceleft,//"OPEN_CURLY_BRACKET", // [174]
	XK_braceright,//"CLOSE_CURLY_BRACKET", // [175]
	XK_asciitilde,//"TILDE", // [176]
	0, // [177]
	0, // [178]
	0, // [179]
	0, // [180]
	0,//"VOLUME_MUTE", // [181]
	0,//"VOLUME_DOWN", // [182]
	0,//"VOLUME_UP", // [183]
	0, // [184]
	0, // [185]
	XK_semicolon,//"SEMICOLON", // [186]
	XK_equal,//"EQUALS", // [187]
	XK_comma,//"COMMA", // [188]
	XK_minus,//"MINUS", // [189]
	XK_period,//"PERIOD", // [190]
	XK_slash,//"SLASH", // [191]
	XK_apostrophe,//"BACK_QUOTE", // [192]
	0, // [193]
	0, // [194]
	0, // [195]
	0, // [196]
	0, // [197]
	0, // [198]
	0, // [199]
	0, // [200]
	0, // [201]
	0, // [202]
	0, // [203]
	0, // [204]
	0, // [205]
	0, // [206]
	0, // [207]
	0, // [208]
	0, // [209]
	0, // [210]
	0, // [211]
	0, // [212]
	0, // [213]
	0, // [214]
	0, // [215]
	0, // [216]
	0, // [217]
	0, // [218]
	XK_bracketleft,//"OPEN_BRACKET", // [219]
	XK_backslash,//"BACK_SLASH", // [220]
	XK_bracketright,//"CLOSE_BRACKET", // [221]
	XK_grave,//"QUOTE", // [222]
	0, // [223]
	XK_Meta_L,//"META", // [224]
	0,//"ALTGR", // [225]
	0, // [226]
	0,//"WIN_ICO_HELP", // [227]
	0,//"WIN_ICO_00", // [228]
	0, // [229]
	0,//"WIN_ICO_CLEAR", // [230]
	0, // [231]
	0, // [232]
	0,//"WIN_OEM_RESET", // [233]
	0,//"WIN_OEM_JUMP", // [234]
	0,//"WIN_OEM_PA1", // [235]
	0,//"WIN_OEM_PA2", // [236]
	0,//"WIN_OEM_PA3", // [237]
	0,//"WIN_OEM_WSCTRL", // [238]
	0,//"WIN_OEM_CUSEL", // [239]
	0,//"WIN_OEM_ATTN", // [240]
	0,//"WIN_OEM_FINISH", // [241]
	0,//"WIN_OEM_COPY", // [242]
	0,//"WIN_OEM_AUTO", // [243]
	0,//"WIN_OEM_ENLW", // [244]
	0,//"WIN_OEM_BACKTAB", // [245]
	0,//"ATTN", // [246]
	0,//"CRSEL", // [247]
	0,//"EXSEL", // [248]
	0,//"EREOF", // [249]
	0,//"PLAY", // [250]
	0,//"ZOOM", // [251]
	0, // [252]
	0,//"PA1", // [253]
	0,//"WIN_OEM_CLEAR", // [254]
	0 // [255]

#endif

};

unsigned int Map_ToPlatformKey(unsigned int key) {
	switch (key) {
	case (FL_Help):
		return VK_HELP;
	case (FL_Shift_L):
		return VK_LSHIFT;
	case (FL_Shift_R):
		return VK_RSHIFT;
	case (FL_Control_L):
		return VK_LCONTROL;
	case (FL_Control_R):
		return VK_RCONTROL;
	case (FL_Alt_L):
		return VK_LMENU;
	case (FL_Alt_R):
		return VK_RMENU;
	case (FL_Pause):
		return VK_PAUSE;
	case (FL_Caps_Lock):
		return VK_CAPITAL;
	case (FL_Kana):
		return VK_KANA;
	case (FL_Escape):
		return VK_ESCAPE;

	case (FL_Home):
		return VK_HOME;
	case (FL_Left):
		return VK_LEFT;
	case (FL_Up):
		return VK_UP;
	case (FL_Right):
		return VK_RIGHT;
	case (FL_Down):
		return VK_DOWN;
	case (FL_Page_Up):
		return VK_PRIOR;
	case (FL_Page_Down):
		return VK_NEXT;
	case (FL_End):
		return VK_END;
	case (FL_Home_Page):
		return VK_HOME;

	case (FL_Print):
		return VK_PRINT;
	case (FL_Insert):
		return VK_INSERT;
	case (FL_Delete):
		return VK_DELETE;

	case (FL_Meta_L):
		return VK_LWIN;
	case (FL_Meta_R):
		return VK_LWIN;

	case (FL_Menu):
		return VK_LMENU;


	case (SL_MULTIPLY):
		return SL_MULTIPLY;

	case (SL_ADD):
		return VK_ADD;
	case (SL_SEPARATOR):
		return VK_SEPARATOR;
	case (SL_SUBTRACT):
		return VK_SUBTRACT;
	case (SL_DECIMAL):
		return VK_DECIMAL;
	case (SL_DIVIDE):
		return VK_DIVIDE;

	case (FL_F + 1):
		return VK_F1;
	case (FL_F + 2):
		return VK_F2;
	case (FL_F + 3):
		return VK_F3;
	case (FL_F + 4):
		return VK_F4;
	case (FL_F + 5):
		return VK_F5;
	case (FL_F + 6):
		return VK_F6;
	case (FL_F + 7):
		return VK_F7;
	case (FL_F + 8):
		return VK_F8;
	case (FL_F + 9):
		return VK_F9;
	case (FL_F + 10):
		return VK_F10;
	case (FL_F + 11):
		return VK_F11;
	case (FL_F + 12):
		return VK_F12;
	case (FL_F + 13):
		return VK_F13;
	case (FL_F + 14):
		return VK_F14;
	case (FL_F + 15):
		return VK_F15;
	case (FL_F + 16):
		return VK_F16;
	case (FL_F + 17):
		return VK_F17;
	case (FL_F + 18):
		return VK_F18;
	case (FL_F + 19):
		return VK_F19;
	case (FL_F + 20):
		return VK_F20;
	case (FL_F + 21):
		return VK_F21;
	case (FL_F + 22):
		return VK_F22;
	case (FL_F + 23):
		return VK_F23;
	case (FL_F + 24):
		return VK_F24;
	case (FL_Num_Lock):
		return VK_NUMLOCK;
	case (FL_Scroll_Lock):
		return VK_SCROLL;
	case (FL_Volume_Down):
		return VK_VOLUME_DOWN;
	case (FL_Volume_Mute):
		return VK_VOLUME_MUTE;
	case (FL_Volume_Up):
		return VK_VOLUME_UP;
	default:
		return 0;
	}
}


void SL::Remote_Access_Library::Input::SimulateKeyboardEvent(KeyEvent ev)
{
	SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Received SetKeyEvent Key:" << ev.Key << " SpecialKey: " << ev.SpecialKey << " PressData: " << ev.PressData);
	if (ev.Key == 0) return;//unmapped key

	if (ev.Key < 0) {//special key, needs to be mapped to platform specific code
		ev.Key = Map_ToPlatformKey(ev.Key);
	}//must be ascii char, do a check to make sure it is actually an ascii char. Below are the hex values of ascii chars
	else if ( !(ev.Key == 0x0008 || ev.Key == 0x0009 || ev.Key == 0x000d || (ev.Key >= 0x0020 && ev.Key >= 0x007f)) ) {
		SL_RAT_LOG(Utilities::Logging_Levels::Debug_log_level, "Recevied a key event which is outside of the asci char set");
		return;
	}
	if (ev.Key == 0) {//no mapping available
		SL_RAT_LOG(Utilities::Logging_Levels::Debug_log_level, "No Mapping Available for key");
		return;
	}
#if  _WIN32

	INPUT input;
	memset(&input, 0, sizeof(input));
	input.type = INPUT_KEYBOARD;

	input.ki.dwFlags = ev.PressData == Keyboard::Press::UP ? KEYEVENTF_KEYUP : 0;
	input.ki.wVk = ev.Key;
	//SendInput(1, &input, sizeof(input));

#elif __APPLE__

#elif __ANDROID__


#elif __linux__


	auto display = XOpenDisplay(NULL);
	auto keycode = XKeysymToKeycode(display, KeyboardMap[ev.Key]);
	if (keycode == 0) return;
	XTestGrabControl(display, True);

	//XTestFakeKeyEvent(display, keycode, ev.PressData == Keyboard::Press::DOWN ? True : False, 0);
	XSync(display, False);
	XTestGrabControl(display, False);
	XCloseDisplay(display);

#endif

}


