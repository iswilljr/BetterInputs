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
#ifdef GEODE_IS_WINDOWS
			// Geode sends fake "a" inserts for arrow/home/end keys on Windows.
			// With a shortcut modifier held, KeyboardInputEvent may not receive them.
			if (BI::platform::hasShortcutModifier())
			{
				bool const isCtrl = BI::platform::keyDown(BI::PlatformKey::LEFT_CONTROL);
				bool const isShift = BI::platform::keyDown(BI::PlatformKey::LEFT_SHIFT);

				switch (keyCode)
				{
					case cocos2d::enumKeyCodes::KEY_Left:
						return g_selectedInput->onLeftArrowKey(isCtrl, isShift);

					case cocos2d::enumKeyCodes::KEY_Right:
						return g_selectedInput->onRightArrowKey(isCtrl, isShift);

					case cocos2d::enumKeyCodes::KEY_Up:
						return g_selectedInput->onUpArrowKey(isShift);

					case cocos2d::enumKeyCodes::KEY_Down:
						return g_selectedInput->onDownArrowKey(isShift);

					case cocos2d::enumKeyCodes::KEY_Home:
						return g_selectedInput->onHomeKey(isShift);

					case cocos2d::enumKeyCodes::KEY_End:
						return g_selectedInput->onEndKey(isShift);

					default:
						break;
				}

				return;
			}

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
#else
			// modifier combos are handled in platform key hooks
			if (BI::platform::hasShortcutModifier())
				return;
#endif

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
