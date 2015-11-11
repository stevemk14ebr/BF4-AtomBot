#pragma once

class ComboBoxElement :public MenuElement
{
public:
	typedef EventDispatcher<void(uint32_t, const std::string&)> eSelectionChanged;
	virtual ~ComboBoxElement();
	virtual void Draw(RenderInterface& Renderer) override;
	virtual ElementType GetType() override;

	struct Context
	{
		Vector2f m_Position;
		Vector2f m_Size;
		Color m_FillColor;
		Color m_FillColorMouseOver;
		Color m_FillColorMouseDown;

		//Not Required
		Color m_TextColor;
		Context()
		{
			m_TextColor = Color::Black();
		}
	};
	ComboBoxElement(const Context& Ctx);
	uint32_t AddComboButton(const std::string& Text);

	eSelectionChanged& EventSelectionChanged();
protected:
	virtual void AddPosition(const Vector2f& NewAmount) override;
	virtual void SetPosition(const Vector2f& NewPos) override;
	virtual void OnMouseDown(const MouseMessage& Msg) override;
	virtual void OnMouseUp(const MouseMessage& Msg) override;
	virtual void OnMouseMove(const MouseMessage& Msg) override;
	virtual void OnMouseLeave(const MouseMessage& Msg) override;
	virtual void OnMouseEnter(const MouseMessage& Msg) override;
	virtual bool IsPointInMouseDownZone(const Vector2f& Point) override;
	virtual bool IsPointInControl(const Vector2f& Point) override;
	virtual bool IsCursorInElement() override;
private:
	std::vector<ButtonElement*> m_ComboButtons;
	Context m_Ctx;
	uint32_t m_SelectedButton;
	uint32_t m_IdClickedButton;
	uint32_t m_IdCurrentSelection; //First main combo-button
	bool m_IsDroppedDown;

	eSelectionChanged m_eSelectionChanged;
};

ComboBoxElement::ComboBoxElement(const Context& Ctx) :
	MenuElement(Ctx.m_Position, Ctx.m_Size)
{
	m_Ctx = Ctx;
	m_IsDroppedDown = false;
	m_IdCurrentSelection = AddComboButton("");
}

ComboBoxElement::~ComboBoxElement()
{
	for (ButtonElement* Button : m_ComboButtons)
	{
		delete Button;
	}
	m_ComboButtons.clear();
}

void ComboBoxElement::Draw(RenderInterface& Renderer)
{
	m_ComboButtons[0]->Draw(Renderer);
	if (!m_IsDroppedDown)
		return;

	for (int i = 1; i < m_ComboButtons.size(); i++)
	{
		m_ComboButtons[i]->Draw(Renderer);
	}
}

uint32_t ComboBoxElement::AddComboButton(const std::string& Text)
{
	ButtonElement::Context Ctx;
	Ctx.m_ButtonText = Text;
	Ctx.m_FillColor = m_Ctx.m_FillColor;
	Ctx.m_FillColorMouseDown = m_Ctx.m_FillColorMouseDown;
	Ctx.m_FillColorMouseOver = m_Ctx.m_FillColorMouseOver;
	Ctx.m_TextColor = m_Ctx.m_TextColor;
	Ctx.m_Position = Vector2f(m_Position.x, m_Position.y + m_Size.y*m_ComboButtons.size());
	Ctx.m_Size = m_Ctx.m_Size;
	ButtonElement* Button = new ButtonElement(Ctx);
	m_ComboButtons.push_back(Button);
	return Button->GetId();
}

void ComboBoxElement::AddPosition(const Vector2f& NewAmount)
{
	MenuElement::AddPosition(NewAmount);
	for (MenuElement* Button : m_ComboButtons)
	{
		Button->AddPosition(NewAmount);
	}
}

void ComboBoxElement::SetPosition(const Vector2f& NewPos)
{
	Vector2f DeltaPos = m_Position - NewPos;
	MenuElement::SetPosition(NewPos);
	for (MenuElement* Button : m_ComboButtons)
	{
		Button->AddPosition(DeltaPos);
	}
}

void ComboBoxElement::OnMouseDown(const MouseMessage& Msg)
{
	MenuElement::OnMouseDown(Msg);
	if (Msg.GetButton() == MouseMessage::MouseButton::Left)
		m_IsDroppedDown = !m_IsDroppedDown;

	MenuElement::OnZOrderChanged();
	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->GetId() == m_IdClickedButton)
		{
			Button->OnMouseDown(Msg);
			if (Button->GetId() != m_IdCurrentSelection)
			{
				m_ComboButtons[0]->SetText(Button->m_Ctx.m_ButtonText);
				m_eSelectionChanged.Invoke(Button->GetId(), Button->m_Ctx.m_ButtonText);
			}
		}
	}
}

void ComboBoxElement::OnMouseUp(const MouseMessage& Msg)
{
	MenuElement::OnMouseUp(Msg);
	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->GetId() == m_IdClickedButton)
			Button->OnMouseUp(Msg);
	}
	/*ID can never be zero as all elements
	are sub elements of ComboBox which receives
	its id before all the sub elements, and ID is
	ascending by order, so at worst the lowest it could
	be is 1*/
	m_IdClickedButton = 0;
}

void ComboBoxElement::OnMouseMove(const MouseMessage& Msg)
{
	MenuElement::OnMouseMove(Msg);

	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->IsPointInControl(Msg.GetLocation()) &&
			!Button->IsCursorInElement())
		{
			Button->OnMouseEnter(Msg);
		}

		if (Button->IsCursorInElement())
		{
			Button->OnMouseMove(Msg);
		}
	}
}

void ComboBoxElement::OnMouseLeave(const MouseMessage& Msg)
{
	MenuElement::OnMouseLeave(Msg);
	/*Unlike window elements that have ample room to receive a mouse move event,
	ComboBox's size is exactly that of its sub elements, so mouse leave events
	would be missed without this section here*/
	for (ButtonElement* Button : m_ComboButtons)
	{
		if (!Button->IsPointInControl(Msg.GetLocation()) &&
			Button->IsCursorInElement())
		{
			Button->OnMouseLeave(Msg);
		}
	}
}

void ComboBoxElement::OnMouseEnter(const MouseMessage& Msg)
{
	MenuElement::OnMouseEnter(Msg);

	for (ButtonElement* Butoon : m_ComboButtons)
	{
		if (Butoon->IsPointInControl(Msg.GetLocation()))
			Butoon->OnMouseEnter(Msg);
	}
}

bool ComboBoxElement::IsPointInMouseDownZone(const Vector2f& Point)
{
	if (m_ComboButtons[0]->IsPointInMouseDownZone(Point))
	{
		m_IdClickedButton = m_ComboButtons[0]->GetId();
		return true;
	}
	if (!m_IsDroppedDown)
		return false;

	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->IsPointInMouseDownZone(Point))
		{
			m_IdClickedButton = Button->GetId();
			return true;
		}
	}
	return false;
}

bool ComboBoxElement::IsPointInControl(const Vector2f& Point)
{
	if (m_ComboButtons[0]->IsPointInControl(Point))
		return true;
	if (!m_IsDroppedDown)
		return false;

	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->IsPointInControl(Point))
			return true;
	}
	return false;
}

bool ComboBoxElement::IsCursorInElement()
{
	if (m_ComboButtons[0]->IsCursorInElement())
		return true;
	if (!m_IsDroppedDown)
		return false;

	for (ButtonElement* Button : m_ComboButtons)
	{
		if (Button->IsCursorInElement())
			return true;
	}
	return false;
}

ComboBoxElement::eSelectionChanged& ComboBoxElement::EventSelectionChanged()
{
	return m_eSelectionChanged;
}

ElementType ComboBoxElement::GetType()
{
	return ElementType::ComboBox;
}