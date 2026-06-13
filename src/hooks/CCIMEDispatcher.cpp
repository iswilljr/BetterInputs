#include <Geode/modify/CCIMEDispatcher.hpp>

#include "BetterTextInputNode.hpp"

// backspace and del is handled in dispatchDeleteBackward/dispatchDeleteForward then in CCEGLView::onGLFWKeyCallback/[EAGLView keyDownExec:]
// same goes for all the other characters (dispatchInsertText then CCEGLView::onGLFWKeyCallback/[EAGLView keyDownExec:])
// except for ctrl and shift, which are only handled in CCEGLView::onGLFWKeyCallback/[EAGLView keyDownExec:]
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
			// modifier combos (ctrl/cmd+a, etc.) are handled in platform key hooks;
			// swallow the IME event so the character is not also inserted
			if (BI::platform::hasShortcutModifier())
				return;

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
