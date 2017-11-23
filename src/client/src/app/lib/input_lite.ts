

export enum KeyCodes {
    KEY_A = 4,
    KEY_B = 5,
    KEY_C = 6,
    KEY_D = 7,
    KEY_E = 8,
    KEY_F = 9,
    KEY_G = 10,
    KEY_H = 11,
    KEY_I = 12,
    KEY_J = 13,
    KEY_K = 14,
    KEY_L = 15,
    KEY_M = 16,
    KEY_N = 17,
    KEY_O = 18,
    KEY_P = 19,
    KEY_Q = 20,
    KEY_R = 21,
    KEY_S = 22,
    KEY_T = 23,
    KEY_U = 24,
    KEY_V = 25,
    KEY_W = 26,
    KEY_X = 27,
    KEY_Y = 28,
    KEY_Z = 29,
    KEY_1 = 30,
    KEY_2 = 31,
    KEY_3 = 32,
    KEY_4 = 33,
    KEY_5 = 34,
    KEY_6 = 35,
    KEY_7 = 36,
    KEY_8 = 37,
    KEY_9 = 38,
    KEY_0 = 39,
    KEY_Enter = 40,
    KEY_Escape = 41,
    KEY_Backspace = 42,
    KEY_Tab = 43,
    KEY_Space = 44,
    KEY_Minus = 45,
    KEY_Equals = 46,
    KEY_LeftBracket = 47,
    KEY_RightBracket = 48,
    KEY_Backslash = 49,
    KEY_Semicolon = 51,
    KEY_Quote = 52,
    KEY_Grave = 53,
    KEY_Comma = 54,
    KEY_Period = 55,
    KEY_Slash = 56,
    KEY_CapsLock = 57,
    KEY_F1 = 58,
    KEY_F2 = 59,
    KEY_F3 = 60,
    KEY_F4 = 61,
    KEY_F5 = 62,
    KEY_F6 = 63,
    KEY_F7 = 64,
    KEY_F8 = 65,
    KEY_F9 = 66,
    KEY_F10 = 67,
    KEY_F11 = 68,
    KEY_F12 = 69,
    KEY_PrintScreen = 70,
    KEY_ScrollLock = 71,
    KEY_Pause = 72,
    KEY_Insert = 73,
    KEY_Home = 74,
    KEY_PageUp = 75,
    KEY_Delete = 76,
    KEY_End = 77,
    KEY_PageDown = 78,
    KEY_Right = 79,
    KEY_Left = 80,
    KEY_Down = 81,
    KEY_Up = 82,
    KP_NumLock = 83,
    KP_Divide = 84,
    KP_Multiply = 85,
    KP_Subtract = 86,
    KP_Add = 87,
    KP_Enter = 88,
    KP_1 = 89,
    KP_2 = 90,
    KP_3 = 91,
    KP_4 = 92,
    KP_5 = 93,
    KP_6 = 94,
    KP_7 = 95,
    KP_8 = 96,
    KP_9 = 97,
    KP_0 = 98,
    KP_Point = 99,
    KEY_NonUSBackslash = 100,
    KP_Equals = 103,
    KEY_F13 = 104,
    KEY_F14 = 105,
    KEY_F15 = 106,
    KEY_F16 = 107,
    KEY_F17 = 108,
    KEY_F18 = 109,
    KEY_F19 = 110,
    KEY_F20 = 111,
    KEY_F21 = 112,
    KEY_F22 = 113,
    KEY_F23 = 114,
    KEY_F24 = 115,
    KEY_Help = 117,
    KEY_Menu = 118,
    KEY_LeftControl = 224,
    KEY_LeftShift = 225,
    KEY_LeftAlt = 226,
    KEY_LeftMeta = 227,
    KEY_RightControl = 228,
    KEY_RightShift = 229,
    KEY_RightAlt = 230,
    KEY_RightMeta = 231,
    INVALID = 255
};

export function ConvertToKeyCode(key: KeyboardEvent): KeyCodes {
  
    switch (key.key) {
        case "0":
            return KeyCodes.KEY_0;
        case "1":
        case "2":
        case "3":
        case "4":
        case "5":
        case "6":
        case "7":
        case "8":
        case "9":
            return KeyCodes.KEY_1 + (key.key.charCodeAt(0) - 1);
        case "A":
        case "B":
        case "C":
        case "D":
        case "E":
        case "F":
        case "G":
        case "H":
        case "I":
        case "J":
        case "K":
        case "L":
        case "M":
        case "N":
        case "O":
        case "P":
        case "Q":
        case "R":
        case "S":
        case "T":
        case "U":
        case "V":
        case "W":
        case "X":
        case "Y":
        case "Z":
            return KeyCodes.KEY_A + (+key.key - +"A");
        case "a":
        case "b":
        case "c":
        case "d":
        case "e":
        case "f":
        case "g":
        case "h":
        case "i":
        case "j":
        case "k":
        case "l":
        case "m":
        case "n":
        case "o":
        case "p":
        case "q":
        case "r":
        case "s":
        case "t":
        case "u":
        case "v":
        case "w":
        case "x":
        case "y":
        case "z":
            return KeyCodes.KEY_A + (key.key.charCodeAt(0) - "a".charCodeAt(0));
        case "Enter":
            return KeyCodes.KEY_Enter;
        case "Escape":
            return KeyCodes.KEY_Escape;
        case "Backspace":
            return KeyCodes.KEY_Backspace;
        case "Tab":
            return KeyCodes.KEY_Tab;
        case " ":
            return KeyCodes.KEY_Space;
        case "-":
            return KeyCodes.KEY_Minus;
        case "=":
            return KeyCodes.KEY_Equals; // this is correct and not a mistype
        case "[":
            return KeyCodes.KEY_LeftBracket;
        case "[":
            return KeyCodes.KEY_RightBracket;
        case "\\":
            return KeyCodes.KEY_Backslash;
        case ";":
            return KeyCodes.KEY_Semicolon;
        case "'":
            return KeyCodes.KEY_Quote;
        case "`":
            return KeyCodes.KEY_Grave;
        case ",":
            return KeyCodes.KEY_Comma;
        case ".":
            return KeyCodes.KEY_Period;
        case "/":
            return KeyCodes.KEY_Slash;
        case "CapsLock":
            return KeyCodes.KEY_CapsLock;
        case "F1":
            return KeyCodes.KEY_F1;
        case "F2":
            return KeyCodes.KEY_F2;
        case "F3":
            return KeyCodes.KEY_F3;
        case "F4":
            return KeyCodes.KEY_F4;
        case "F5":
            return KeyCodes.KEY_F5;
        case "F6":
            return KeyCodes.KEY_F6;
        case "F7":
            return KeyCodes.KEY_F7;
        case "F8":
            return KeyCodes.KEY_F8;
        case "F9":
            return KeyCodes.KEY_F9;
        case "F10":
            return KeyCodes.KEY_F10;
        case "F11":
            return KeyCodes.KEY_F11;
        case "F12":
            return KeyCodes.KEY_F12;
        case "F13":
            return KeyCodes.KEY_F13;
        case "F14":
            return KeyCodes.KEY_F14;
        case "F15":
            return KeyCodes.KEY_F15;
        case "F16":
            return KeyCodes.KEY_F1;
        case "F17":
            return KeyCodes.KEY_F1;
        case "F18":
            return KeyCodes.KEY_F1;
        case "F19":
            return KeyCodes.KEY_F1;
        case "F20":
            return KeyCodes.KEY_F1;
        case "Print":
            return KeyCodes.KEY_PrintScreen;
        case "ScrollLock":
            return KeyCodes.KEY_ScrollLock;
        case "Pause":
            return KeyCodes.KEY_Pause;
        case "Insert":
            return KeyCodes.KEY_Insert;
        case "Home":
            return KeyCodes.KEY_Home;
        case "PageUp":
            return KeyCodes.KEY_PageUp;
        case "Delete":
            return KeyCodes.KEY_Delete;
        case "End":
            return KeyCodes.KEY_End;
        case "PageDown":
            return KeyCodes.KEY_PageDown;
        case "ArrowRight":
            return KeyCodes.KEY_Right;
        case "ArrowLeft":
            return KeyCodes.KEY_Left;
        case "ArrowDown":
            return KeyCodes.KEY_Down;
        case "ArrowUp":
            return KeyCodes.KEY_Up;
        case "NumLock":
            return KeyCodes.KP_NumLock;
        case "/":
            return KeyCodes.KP_Divide;
        case "*":
            return KeyCodes.KP_Multiply;
        case "-":
            return KeyCodes.KP_Subtract;
        case "+":
            return KeyCodes.KP_Add;
        case ".":
            return KeyCodes.KP_Point;
        case "Help":
            return KeyCodes.KEY_Help;
        case "Alt":
            return KeyCodes.KEY_Menu;
        case "Control":
            return KeyCodes.KEY_LeftControl;
        case "Shift":
            return KeyCodes.KEY_LeftShift;
        case "Meta":
            return KeyCodes.KEY_LeftMeta;
        default:
            return KeyCodes.INVALID;
    }

}

export enum MouseButtons { LEFT, MIDDLE, RIGHT };

export class KeyEvent {
    Pressed: boolean;
    Key: KeyCodes;
};
export class MouseButtonEvent {
    Pressed: boolean;
    Button: MouseButtons;
};
export class MouseScrollEvent {
    Offset = 0;
};
export class MousePositionOffsetEvent {
    X = 0;
    Y = 0;
};
export class MousePositionAbsoluteEvent {
    X = 0;
    Y = 0;
};