#include <Geode/modify/CCIMEDispatcher.hpp>

#include "BetterTextInputNode.hpp"

// backspace and del is handled in dispatchDeleteBackward/dispatchDeleteForward then in [EAGLView keyDownExec:] (macOS) / KeyboardInputEvent (Windows)
// same goes for all the other characters (dispatchInsertText then [EAGLView keyDownExec:] / KeyboardInputEvent)
// except for ctrl and shift, which are only handled in [EAGLView keyDownExec:] / KeyboardInputEvent
struct BetterCCIMEDispatcher : geode::Modify<BetterCCIMEDispatcher, cocos2d::CCIMEDispatcher>
{
	void dispatchDeleteBackward()
	{
		if (g_selectedInput) return;

		CCIMEDispatcher::dispatchDeleteBackward();
	}

	void dispatchDeleteForward()
	{
		if (g_selectedInput) return;

		CCIMEDispatcher::dispatchDeleteForward();
	}

	void dispatchInsertText(const char* text, int len, cocos2d::enumKeyCodes keyCode)
	{
		if (g_selectedInput)
		{
			// modifier combos are handled in platform key hooks
			if (BI::platform::hasShortcutModifier())
				return;

			// Geode sends fake "a" inserts for arrow/home/end keys on Windows
			switch (keyCode)
			{
				case cocos2d::enumKeyCodes::KEY_Left:
				case cocos2d::enumKeyCodes::KEY_Right:
				case cocos2d::enumKeyCodes::KEY_Up:
				case cocos2d::enumKeyCodes::KEY_Down:
				case cocos2d::enumKeyCodes::KEY_Home:
				case cocos2d::enumKeyCodes::KEY_End:
					return;

				default:
					break;
			}

			// enter/newline and other control chars must use vanilla IME handling;
			// our insert path updates labels asynchronously and crashes in text areas
			if (len <= 0)
				return;

			char const c = text[0];
			if (c == '\n' || c == '\r' || c == '\t' || static_cast<unsigned char>(c) < 0x20)
				return CCIMEDispatcher::dispatchInsertText(text, len, keyCode);

			return g_selectedInput->insertCharAtPos(g_selectedInput->getCursorPos(), c);
		}

		CCIMEDispatcher::dispatchInsertText(text, len, keyCode);
	}
};
