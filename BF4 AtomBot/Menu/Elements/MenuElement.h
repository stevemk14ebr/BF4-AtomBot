#pragma once

typedef EventDispatcher<void(const Vector2f&)> ePositionChanged;
typedef EventDispatcher<void(const Vector2f&)> eSizeChanged;

typedef EventDispatcher<void(const MouseMessage&)> eMouseDown;
typedef EventDispatcher<void(const MouseMessage&)> eMouseUp;
typedef EventDispatcher<void(const MouseMessage&)> eMouseEnter;
typedef EventDispatcher<void(const MouseMessage&)> eMouseLeave;
typedef EventDispatcher<void(const MouseMessage&)> eMouseMove;
typedef EventDispatcher<void(const MouseMessage&)> eMouseScroll;

typedef EventDispatcher<void(const KeyboardMessage&)> eKeydown;
typedef EventDispatcher<void(const KeyboardMessage&)> eKeyUp;

typedef EventDispatcher<void(const uint32_t)> eZOrderChanged;
class MenuElement
{
public:
	friend class ElementManager;
	friend class WindowElement;
	friend class TabbedWindowElement;
	friend class TabbedWindowPageElement;

	MenuElement(const Vector2f& Pos, const Vector2f& Size);
	virtual ~MenuElement() = default;
	virtual void Draw(RenderInterface& Renderer) = 0;
	virtual ElementType GetType() = 0;

	const uint32_t GetId() const;
	Vector2f GetSize();
	Vector2f GetPosition();
	virtual void AddPosition(const Vector2f& NewAmount);
	virtual void SetPosition(const Vector2f& NewPos);
	virtual void SetSize(const Vector2f& NewSize);

	bool operator!=(const MenuElement& Element) const;
	bool operator==(const MenuElement& Element) const;

	/*Any actions that should be complete on a user action
	should be registered via these functions*/
	ePositionChanged& EventPositionChanged();
	eSizeChanged& EventSizeChanged();
	eMouseDown& EventMouseDown();
	eMouseUp& EventMouseUp();
	eMouseEnter& EventMouseEnter();
	eMouseLeave& EventMouseLeave();
	eMouseMove& EventMouseMove();
	eMouseScroll& EventMouseScroll();
	eKeydown& EventKeyDown();
	eKeyUp& EventKeyUp();

	//Should move this into window elements only
	eZOrderChanged& EventZOrderChanged();
protected:
	/*Any special actions a class should take on events should be
	executed by overloading these virtual methods
	(Think of these as callbacks for GUI elements)*/
	virtual void OnPositionChanged(const Vector2f& NewPosition);
	virtual void OnSizeChanged(const Vector2f& NewSize);

	virtual void OnMouseDown(const MouseMessage& Msg);
	virtual void OnMouseUp(const MouseMessage& Msg);
	virtual void OnMouseEnter(const MouseMessage& Msg);
	virtual void OnMouseLeave(const MouseMessage& Msg);
	virtual void OnMouseMove(const MouseMessage& Msg);
	virtual void OnMouseScroll(const MouseMessage& Msg);

	virtual void OnKeyDown(const KeyboardMessage& Msg);
	virtual void OnKeyUp(const KeyboardMessage& Msg);

	virtual void OnZOrderChanged();

	virtual bool IsPointInControl(const Vector2f& Point);
	virtual bool IsPointInMouseDownZone(const Vector2f& Point);

	virtual bool IsCursorInElement();

	Vector2f m_Position;
	Vector2f m_Size;
	uint32_t m_ID;

	bool m_CursorInElement;

	/*Callbacks to user defined functions*/
	ePositionChanged m_ePositionChanged;
	eSizeChanged m_eSizeChanged;
	eMouseDown m_eMouseDown;
	eMouseUp m_eMouseUp;
	eMouseEnter m_eMouseEnter;
	eMouseLeave m_eMouseLeave;
	eMouseMove m_eMouseMove;
	eMouseScroll m_eMouseScroll;
	eKeydown m_eKeyDown;
	eKeyUp m_eKeyUp;
	eZOrderChanged m_eZOrderChanged;
};

MenuElement::MenuElement(const Vector2f& Pos, const Vector2f& Size)
{
	m_ID = IDGenerator::Instance().Next();
	m_Position = Pos;
	m_Size = Size;
	m_CursorInElement = false;
}

const uint32_t MenuElement::GetId() const
{
	return m_ID;
}

Vector2f MenuElement::GetSize()
{
	return m_Size;
}

Vector2f MenuElement::GetPosition()
{
	return m_Position;
}

bool MenuElement::operator!=(const MenuElement& Element) const
{
	return m_ID != Element.GetId();
}

bool MenuElement::operator==(const MenuElement& Element) const
{
	return m_ID == Element.GetId();
}

void MenuElement::OnPositionChanged(const Vector2f& NewPosition)
{
	m_ePositionChanged.Invoke(NewPosition);
}

void MenuElement::OnSizeChanged(const Vector2f& NewSize)
{
	m_eSizeChanged.Invoke(NewSize);
}

void MenuElement::OnMouseDown(const MouseMessage& Msg)
{
	m_eMouseDown.Invoke(Msg);
}

void MenuElement::OnMouseUp(const MouseMessage& Msg)
{
	m_eMouseUp.Invoke(Msg);
}

void MenuElement::OnMouseEnter(const MouseMessage& Msg)
{
	m_CursorInElement = true;
	m_eMouseEnter.Invoke(Msg);
}

void MenuElement::OnMouseLeave(const MouseMessage& Msg)
{
	m_CursorInElement = false;
	m_eMouseLeave.Invoke(Msg);
}

void MenuElement::OnMouseMove(const MouseMessage& Msg)
{
	m_eMouseMove.Invoke(Msg);
}

void MenuElement::OnMouseScroll(const MouseMessage& Msg)
{
	m_eMouseScroll.Invoke(Msg);
}

void MenuElement::OnKeyDown(const KeyboardMessage& Msg)
{
	m_eKeyDown.Invoke(Msg);
}

void MenuElement::OnKeyUp(const KeyboardMessage& Msg)
{
	m_eKeyUp.Invoke(Msg);
}

void MenuElement::OnZOrderChanged()
{
	m_eZOrderChanged.Invoke(GetId());
}

ePositionChanged& MenuElement::EventPositionChanged()
{
	return m_ePositionChanged;
}
eSizeChanged& MenuElement::EventSizeChanged()
{
	return m_eSizeChanged;
}
eMouseDown& MenuElement::EventMouseDown()
{
	return m_eMouseDown;
}
eMouseUp& MenuElement::EventMouseUp()
{
	return m_eMouseUp;
}
eMouseEnter& MenuElement::EventMouseEnter()
{
	return m_eMouseEnter;
}
eMouseLeave& MenuElement::EventMouseLeave()
{
	return m_eMouseLeave;
}
eMouseMove& MenuElement::EventMouseMove()
{
	return m_eMouseMove;
}
eMouseScroll& MenuElement::EventMouseScroll()
{
	return m_eMouseScroll;
}
eKeydown& MenuElement::EventKeyDown()
{
	return m_eKeyDown;
}
eKeyUp& MenuElement::EventKeyUp()
{
	return m_eKeyUp;
}
eZOrderChanged& MenuElement::EventZOrderChanged()
{
	return m_eZOrderChanged;
}

bool MenuElement::IsPointInControl(const Vector2f& Point)
{
	if (Point.x > m_Position.x && Point.x < (m_Position.x + m_Size.x) &&
		Point.y > m_Position.y && Point.y < (m_Position.y + m_Size.y))
		return true;
	return false;
}

bool MenuElement::IsPointInMouseDownZone(const Vector2f& Point)
{
	//This is a safe default case for most controls
	return IsPointInControl(Point);
}

bool MenuElement::IsCursorInElement()
{
	return m_CursorInElement;
}

void MenuElement::AddPosition(const Vector2f& NewAmount)
{
	m_Position += NewAmount;
}

void MenuElement::SetPosition(const Vector2f& NewPos)
{
	m_Position = NewPos;
}

void MenuElement::SetSize(const Vector2f& NewSize)
{
	m_Size = NewSize;
}