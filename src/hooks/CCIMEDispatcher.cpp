#include <Geode/modify/CCIMEDispatcher.hpp>

#include "BetterTextInputNode.hpp"

// backspace and del is handled in dispatchDeleteBackward/dispatchDeleteForward then in KeyboardInputEvent/[EAGLView keyDownExec:]
// same goes for all the other characters (dispatchInsertText then KeyboardInputEvent/[EAGLView keyDownExec:])
// except for ctrl and shift, which are only handled in KeyboardInputEvent/[EAGLView keyDownExec:]
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
			return g_selectedInput->insertCharAtPos(g_selectedInput->getCursorPos(), text[0]);

		CCIMEDispatcher::dispatchInsertText(text, len, keyCode);
	}
};
