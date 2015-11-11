#pragma once

typedef EventDispatcher<void(const MouseMessage&)> eProcessMouseMessage;
typedef EventDispatcher<void(const KeyboardMessage&)> eProcessKeyboardMessage;
class InputManagerInterface
{
public:
	/*Allow *ONLY* ElementManager to subscribe to callbacks events,
	via friendship*/
	friend class ElementManager;
	virtual void PollKeyboard()=0;
	virtual void PollMouse()=0;
	virtual Vector2f MapCursorToScreenSpace(const Vector2f& CursorPos) const =0;
protected:
	eProcessKeyboardMessage& EventKeyboardMessage();
	eProcessMouseMessage& EventMouseMessage();

	/*These callbacks inform the ElementManager of a new message*/
	eProcessMouseMessage m_eProcessMouseMessage;
	eProcessKeyboardMessage m_eProcessKeyboardMessage;
};

eProcessKeyboardMessage& InputManagerInterface::EventKeyboardMessage()
{
	return m_eProcessKeyboardMessage;
}

eProcessMouseMessage& InputManagerInterface::EventMouseMessage()
{
	return m_eProcessMouseMessage;
}

