#pragma once

class WindowElement :public MenuElement
{
public:
	friend class ColorPickerElement;
	typedef EventDispatcher<void(const uint32_t)> eClosePressed;

	virtual ~WindowElement();
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;

	uint32_t AddSubElement(MenuElement* Element);

	void ReSortZOrder(const uint32_t Id);

	template<typename T>
	T* GetElementById(const uint32_t Id);

	struct Context
	{
		//Required
		std::string m_WindowName;
		Color m_FillColor;
		Color m_TitleFillColor;
		Vector2f m_Position;
		Vector2f m_Size;

		//Not Required to be set
		float m_TitleBarHeight;
		float m_BorderWidth;
		Color m_TextColor;
		Color m_DepthHighlightColor;
		Color m_DepthShadeColor;
		Context()
		{
			m_TitleBarHeight = 25;
			m_BorderWidth = 5;
			m_TextColor = Color::Black();
			m_DepthHighlightColor = Color::White();
			m_DepthShadeColor = Color::Black();
		}
	};

	WindowElement(const Context& Ctx);

	eClosePressed& EventClosePressed();
protected:
	virtual void OnMouseDown(const MouseMessage& Msg) override;
	virtual void OnMouseUp(const MouseMessage& Msg) override;
	virtual void OnMouseMove(const MouseMessage& Msg) override;
	virtual void OnPositionChanged(const Vector2f& NewPosition) override;
	virtual bool PointInRibbon(const Vector2f& Point);
	virtual bool PointInClient(const Vector2f& Point);
	virtual bool IsCursorInElement() override;
	virtual void AddPosition(const Vector2f& NewAmount);
	virtual void SetPosition(const Vector2f& NewPos);
	void OnClosePressed(const MouseMessage& Msg);

	Context m_Ctx;
	bool m_IsMouseDown;
	Vector2f m_DragOffsetFromPosition;
	std::vector<MenuElement*> m_SubElements;
	ButtonElement* m_CloseButton;
	eClosePressed m_eClosePressed;
};

WindowElement::~WindowElement()
{
	for (MenuElement* Element : m_SubElements)
	{
		delete Element;
	}
	m_SubElements.clear();
}

WindowElement::WindowElement(const WindowElement::Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_IsMouseDown = false;

	ButtonElement::Context CloseBtnCtx;
	CloseBtnCtx.m_ButtonText = "X";
	CloseBtnCtx.m_FillColor = Color(.5f, .5f, .5f);
	CloseBtnCtx.m_FillColorMouseOver = Color(.7f, .7f, .7f);
	CloseBtnCtx.m_FillColorMouseDown = Color(.4f, .4f, .4f);
	CloseBtnCtx.m_Size = Vector2f(m_Ctx.m_TitleBarHeight, m_Ctx.m_TitleBarHeight) * .75f;
	CloseBtnCtx.m_Position = Vector2f(m_Size.x, 0) -
		Vector2f(m_Ctx.m_TitleBarHeight * .75f + m_Ctx.m_BorderWidth, m_Ctx.m_TitleBarHeight *-.15f);
	m_CloseButton = new ButtonElement(CloseBtnCtx);
	m_CloseButton->EventMouseDown() +=
		std::bind(&WindowElement::OnClosePressed, this, std::placeholders::_1);
	m_CloseButton->AddPosition(m_Position); //make relative to window pos
	m_SubElements.push_back(m_CloseButton);
}

void WindowElement::Draw(RenderInterface& Renderer)
{
	Renderer.BeginLine();
	//Outer Panel
	Renderer.DrawFilledBox(m_Position, m_Size, m_Ctx.m_TitleFillColor);
	Renderer.DrawLineBox(m_Position, m_Size, Color::Black());

	//Inner Panel
	Vector2f InnerPanelPosition = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TitleBarHeight);
	Vector2f InnerPanelSize = Vector2f(m_Size.x - (m_Ctx.m_BorderWidth * 2), m_Size.y - m_Ctx.m_TitleBarHeight - m_Ctx.m_BorderWidth);
	Renderer.DrawFilledBox(InnerPanelPosition, InnerPanelSize, m_Ctx.m_FillColor);
	Renderer.DrawLineBox(InnerPanelPosition, InnerPanelSize, Color::Black());

	//Start Inner Shadows
	Vector2f InnerTopLeft = InnerPanelPosition + Vector2f(1, 1);
	Vector2f InnerTopRight = Renderer.GetRectPoint(InnerPanelPosition, InnerPanelSize, RenderInterface::RectPoint::TopRight) + Vector2f(-1, 1);
	Vector2f InnerBottomRight = Renderer.GetRectPoint(InnerPanelPosition, InnerPanelSize, RenderInterface::RectPoint::BottomRight) + Vector2f(-1, -1);
	Vector2f InnerBottomLeft = Renderer.GetRectPoint(InnerPanelPosition, InnerPanelSize, RenderInterface::RectPoint::BottomLeft) + Vector2f(1, -1);
	Renderer.DrawLine(InnerTopLeft, InnerTopRight, m_Ctx.m_DepthShadeColor);
	Renderer.DrawLine(InnerTopLeft, InnerBottomLeft, m_Ctx.m_DepthShadeColor);
	Renderer.DrawLine(InnerTopRight, InnerBottomRight, m_Ctx.m_DepthHighlightColor);
	Renderer.DrawLine(InnerBottomLeft, InnerBottomRight, m_Ctx.m_DepthHighlightColor);
	//End Inner Shadows

	//Start Shadows
	Vector2f TopLeft = m_Position + Vector2f(1, 1);
	Vector2f TopRight = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::TopRight) + Vector2f(-1, 1);
	Vector2f BottomRight = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::BottomRight) + Vector2f(-1, -1);
	Vector2f BottomLeft = Renderer.GetRectPoint(m_Position, m_Size, RenderInterface::RectPoint::BottomLeft) + Vector2f(1, -1);
	Renderer.DrawLine(TopLeft, TopRight, m_Ctx.m_DepthHighlightColor);
	Renderer.DrawLine(TopLeft, BottomLeft, m_Ctx.m_DepthHighlightColor);
	Renderer.DrawLine(TopRight, BottomRight, m_Ctx.m_DepthShadeColor);
	Renderer.DrawLine(BottomLeft, BottomRight, m_Ctx.m_DepthShadeColor);
	//End Shadows
	Renderer.EndLine();

	Vector2f WndNameSz = Renderer.MeasureString("%s", m_Ctx.m_WindowName.c_str());
	float DeltaHeight = m_Ctx.m_TitleBarHeight - WndNameSz.y;
	DeltaHeight /= 2;

	Renderer.BeginText();
	Renderer.RenderText(Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + DeltaHeight), m_Ctx.m_TextColor, "%s", m_Ctx.m_WindowName.c_str());
	Renderer.EndText();
	for (auto i = m_SubElements.rbegin(); i != m_SubElements.rend(); i++)
	{
		(*i)->Draw(Renderer);
	}
}

void WindowElement::OnMouseDown(const MouseMessage& Msg)
{
	MenuElement::OnMouseDown(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left && PointInRibbon(Msg.GetLocation()))
	{
		m_IsMouseDown = true;
		m_DragOffsetFromPosition = m_Position - Msg.GetLocation();
		MenuElement::OnZOrderChanged();
	}

	for (MenuElement* Element : m_SubElements)
	{
		if (!Element->IsPointInMouseDownZone(Msg.GetLocation()))
			continue;

		Element->OnMouseDown(Msg);
		return;
	}
}

void WindowElement::OnMouseUp(const MouseMessage& Msg)
{
	MenuElement::OnMouseUp(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left)
		m_IsMouseDown = false;

	for (MenuElement* Element : m_SubElements)
	{
		Element->OnMouseUp(Msg);
	}
}

void WindowElement::OnMouseMove(const MouseMessage& Msg)
{
	MenuElement::OnMouseMove(Msg);
	if (m_IsMouseDown)
	{
		Vector2f NewPosition = Msg.GetLocation() + m_DragOffsetFromPosition;
		//Move to new spot
		OnPositionChanged(NewPosition);
	}

	for (MenuElement* Element : m_SubElements)
	{
		if (!Element->IsPointInControl(Msg.GetLocation()) &&
			Element->IsCursorInElement())
		{
			Element->OnMouseLeave(Msg);
			return;
		}
		else if (Element->IsPointInControl(Msg.GetLocation()) &&
			!Element->IsCursorInElement())
		{
			Element->OnMouseEnter(Msg);
			return;
		}

		if (Element->IsCursorInElement())
		{
			Element->OnMouseMove(Msg);
			return;
		}
	}
}

void WindowElement::OnPositionChanged(const Vector2f& NewPosition)
{
	Vector2f DeltaPosition = NewPosition - m_Position;
	m_Position = NewPosition;
	m_ePositionChanged.Invoke(m_Position);
	for (MenuElement* Element : m_SubElements)
	{
		Element->AddPosition(DeltaPosition);
	}
}

bool WindowElement::PointInRibbon(const Vector2f& Point)
{
	Vector2f InnerPanelPosition = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TitleBarHeight);
	if (Point.x > m_Position.x && Point.x < (m_Position.x + m_Size.x) &&
		Point.y > m_Position.y && Point.y < InnerPanelPosition.y)
		return true;
	return false;
}

bool WindowElement::PointInClient(const Vector2f& Point)
{
	//Is point in the are where controls are added
	Vector2f InnerPanelPosition = Vector2f(m_Position.x + m_Ctx.m_BorderWidth, m_Position.y + m_Ctx.m_TitleBarHeight);
	Vector2f InnerPanelSize = Vector2f(m_Size.x - (m_Ctx.m_BorderWidth * 2), m_Size.y - m_Ctx.m_TitleBarHeight - m_Ctx.m_BorderWidth);
	if (Point.x > InnerPanelPosition.x  && Point.x < (InnerPanelPosition.x + InnerPanelSize.x) &&
		Point.y > InnerPanelPosition.y && Point.y < (InnerPanelPosition.y + InnerPanelSize.y))
		return true;
	return false;
}

bool WindowElement::IsCursorInElement()
{
	//Mouse can go out of element when moved fast, 
	//this lets us still receive mouse move messages if cursor is down
	if (m_IsMouseDown)
		return true;
	return MenuElement::IsCursorInElement();
}

uint32_t WindowElement::AddSubElement(MenuElement* Element)
{
	for (MenuElement* Element2 : m_SubElements)
	{
		if (Element2->GetId() == Element->GetId())
			return Element2->GetId();
	}
	//Make sub element relative to window
	Element->AddPosition(m_Position);
	Element->AddPosition(Vector2f(m_Ctx.m_BorderWidth, m_Ctx.m_TitleBarHeight));
	Element->EventZOrderChanged() += std::bind(&WindowElement::ReSortZOrder,
		this, std::placeholders::_1);
	m_SubElements.push_back(Element);
	return Element->GetId();
}

void WindowElement::ReSortZOrder(const uint32_t Id)
{
	//Find the element with Id
	auto Pivot = std::find_if(m_SubElements.begin(), m_SubElements.end(), [&](MenuElement* Elem)->bool {
		return Elem->GetId() == Id;
	});
	//Push found Element to front
	std::vector<MenuElement*> Temp;
	Temp.push_back(*Pivot);
	//Add Rest
	for (MenuElement* Elem : m_SubElements)
	{
		if (Elem->GetId() != Id)
			Temp.push_back(Elem);
	}
	//Set Re-Ordered Array
	m_SubElements = Temp;
}

void WindowElement::OnClosePressed(const MouseMessage& Msg)
{
	m_eClosePressed.Invoke(GetId());
}

WindowElement::eClosePressed& WindowElement::EventClosePressed()
{
	return m_eClosePressed;
}

ElementType WindowElement::GetType()
{
	return ElementType::Window;
}

void WindowElement::AddPosition(const Vector2f& NewAmount)
{
	MenuElement::AddPosition(NewAmount);
	for (MenuElement* Elem : m_SubElements)
	{
		Elem->AddPosition(NewAmount);
	}
}

void WindowElement::SetPosition(const Vector2f& NewPos)
{
	Vector2f Delta = m_Position - NewPos;
	MenuElement::SetPosition(NewPos);
	for (MenuElement* Elem : m_SubElements)
	{
		Elem->AddPosition(Delta);
	}
}

template<typename T>
T* WindowElement::GetElementById(const uint32_t Id)
{
	for (MenuElement* Element : m_SubElements)
	{
		if (Element->GetId() == Id)
			return dynamic_cast<T*>(Element);
	}
	return nullptr;
}
