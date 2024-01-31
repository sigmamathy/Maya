#pragma once

#include "./core.hpp"
#include "./math.hpp"

// List of all key code available
enum MayaKeyCode : int
{
	// ----------- The unknown key ------------ //
	MayaKeyUnknown			= -1,
	// ----------- Printable keys ------------- //
	MayaKeySpace			= 32,
	MayaKeyApostrophe		= 39,
	MayaKeyComma			= 44,
	MayaKeyMinus			= 45,
	MayaKeyPeriod			= 46,
	MayaKeySlash			= 47,
	MayaKey0				= 48,
	MayaKey1				= 49,
	MayaKey2				= 50,
	MayaKey3				= 51,
	MayaKey4				= 52,
	MayaKey5				= 53,
	MayaKey6				= 54,
	MayaKey7				= 55,
	MayaKey8				= 56,
	MayaKey9				= 57,
	MayaKeySemiColon		= 59,
	MayaKeyEqual			= 61,
	MayaKeyA				= 65,
	MayaKeyB				= 66,
	MayaKeyC				= 67,
	MayaKeyD				= 68,
	MayaKeyE				= 69,
	MayaKeyF				= 70,
	MayaKeyG				= 71,
	MayaKeyH				= 72,
	MayaKeyI				= 73,
	MayaKeyJ				= 74,
	MayaKeyK				= 75,
	MayaKeyL				= 76,
	MayaKeyM				= 77,
	MayaKeyN				= 78,
	MayaKeyO				= 79,
	MayaKeyP				= 80,
	MayaKeyQ				= 81,
	MayaKeyR				= 82,
	MayaKeyS				= 83,
	MayaKeyT				= 84,
	MayaKeyU				= 85,
	MayaKeyV				= 86,
	MayaKeyW				= 87,
	MayaKeyX				= 88,
	MayaKeyY				= 89,
	MayaKeyZ				= 90,
	MayaKeyLeftBracket		= 91,
	MayaKeyBackSlash		= 92,
	MayaKeyRightBracket		= 93,
	MayaKeyGraveAccent		= 96,
	MayaKeyWorld1			= 161,
	MayaKeyWorld2			= 162,
	// ----------- Function keys -------------- //
	MayaKeyEscape			= 256,
	MayaKeyEnter			= 257,
	MayaKeyTab				= 258,
	MayaKeyBackspace		= 259,
	MayaKeyInsert			= 260,
	MayaKeyDelete			= 261,
	MayaKeyRight			= 262,
	MayaKeyLeft				= 263,
	MayaKeyDown				= 264,
	MayaKeyUp				= 265,
	MayaKeyPageUp			= 266,
	MayaKeyPageDown			= 267,
	MayaKeyHome				= 268,
	MayaKeyEnd				= 269,
	MayaKeyCapsLock			= 280,
	MayaKeyScrollLock		= 281,
	MayaKeyNumLock			= 282,
	MayaKeyPrintScreen		= 283,
	MayaKeyPause			= 284,
	MayaKeyF1				= 290,
	MayaKeyF2				= 291,
	MayaKeyF3				= 292,
	MayaKeyF4				= 293,
	MayaKeyF5				= 294,
	MayaKeyF6				= 295,
	MayaKeyF7				= 296,
	MayaKeyF8				= 297,
	MayaKeyF9				= 298,
	MayaKeyF10				= 299,
	MayaKeyF11				= 300,
	MayaKeyF12				= 301,
	MayaKeyF13				= 302,
	MayaKeyF14				= 303,
	MayaKeyF15				= 304,
	MayaKeyF16				= 305,
	MayaKeyF17				= 306,
	MayaKeyF18				= 307,
	MayaKeyF19				= 308,
	MayaKeyF20				= 309,
	MayaKeyF21				= 310,
	MayaKeyF22				= 311,
	MayaKeyF23				= 312,
	MayaKeyF24				= 313,
	MayaKeyF25				= 314,
	MayaKeyKP0				= 320,
	MayaKeyKP1				= 321,
	MayaKeyKP2				= 322,
	MayaKeyKP3				= 323,
	MayaKeyKP4				= 324,
	MayaKeyKP5				= 325,
	MayaKeyKP6				= 326,
	MayaKeyKP7				= 327,
	MayaKeyKP8				= 328,
	MayaKeyKP9				= 329,
	MayaKeyKPDecimal		= 330,
	MayaKeyKPDivide			= 331,
	MayaKeyKPMultiply		= 332,
	MayaKeyKPSubtract		= 333,
	MayaKeyKPAdd			= 334,
	MayaKeyKPEnter			= 335,
	MayaKeyKPEqual			= 336,
	MayaKeyLeftShift		= 340,
	MayaKeyLeftControl		= 341,
	MayaKeyLeftAlt			= 342,
	MayaKeyLeftSuper		= 343,
	MayaKeyRightShift		= 344,
	MayaKeyRightControl		= 345,
	MayaKeyRightAlt			= 346,
	MayaKeyRightSuper		= 347,
	MayaKeyMenu				= 348
};

// List of all mouse button code available
enum MayaMouseButton : int
{
	MayaMouseButton1			= 0,
	MayaMouseButton2			= 1,
	MayaMouseButton3			= 2,
	MayaMouseButton4			= 3,
	MayaMouseButton5			= 4,
	MayaMouseButton6			= 5,
	MayaMouseButton7			= 6,
	MayaMouseButton8			= 7,
	MayaMouseButtonLast			= MayaMouseButton8,
	MayaMouseButtonLeft			= MayaMouseButton1,
	MayaMouseButtonRight		= MayaMouseButton2,
	MayaMouseButtonMiddle		= MayaMouseButton3
};

struct MayaEvent
{
	class MayaWindow* Window;
	virtual int GetEventID() const = 0; // 0-64 reserved
};

MAYA_TYPEDEF0(MayaEventCallback, MayaFunction<void(MayaEvent&)>);

#define MAYA_DEFINE_EVENT_ID(x) static constexpr int EventID = x; inline int GetEventID() const override { return x; }

struct MayaKeyEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(1)
	MayaKeyCode KeyCode; // the keycode
	bool Down; // true if key is pressed
	bool Repeat;
};

struct MayaMouseEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(2);
	MayaMouseButton Button; // the mouse button
	bool Down; // true if button is pressed
};

// When users moves the mouse
struct MayaMouseMovedEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(3);
	MayaIvec2 Position; // cursor position
};

// When users scroll the mouse (mouse wheel)
struct MayaMouseScrolledEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(4);
	MayaIvec2 Offset; // mouse offset
};

// When the window is focused or unfocused by the user
struct MayaWindowFocusedEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(5);
	bool Focused; // true if focused
};

// When the window is closed by the user
struct MayaWindowClosedEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(6);
};

// When the window is resized by the user
struct MayaWindowResizedEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(7);
	MayaIvec2 Size; // current window size
};

// When the window is moved by the user
struct MayaWindowMovedEvent : public MayaEvent
{
	MAYA_DEFINE_EVENT_ID(8);
	MayaIvec2 Position; // current window position
};
