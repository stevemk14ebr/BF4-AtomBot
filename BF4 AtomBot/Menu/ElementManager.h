#pragma once
#include <vector>

class ElementManager
{
public:
	~ElementManager();
	ElementManager(InputManagerInterface* InputManager,RenderInterface* Renderer);
	uint32_t AddElement(MenuElement* Element);
	void Render();
	void UpdateInput();
	void ProcessKeyboardMessage(const KeyboardMessage& Msg);
	void ProcessMouseMessage(const MouseMessage& Msg);

	template<typename T>
	T* GetElementById(const uint32_t Id);
private:
	void ReSortZOrder(const uint32_t Id);
	std::vector<MenuElement*> m_Elements;
	InputManagerInterface* m_InputManager;
	RenderInterface* m_Renderer;
};

ElementManager::~ElementManager()
{
	for (MenuElement* Element : m_Elements)
	{
		delete Element;
	}
	m_Elements.clear();
	delete m_InputManager;
	delete m_Renderer;
}

ElementManager::ElementManager(InputManagerInterface* InputManager, RenderInterface* Renderer)
{
	m_InputManager = InputManager;
	m_Renderer = Renderer;
	
	m_InputManager->EventKeyboardMessage() +=
		std::bind(&ElementManager::ProcessKeyboardMessage, this,
			std::placeholders::_1);
	m_InputManager->EventMouseMessage() +=
		std::bind(&ElementManager::ProcessMouseMessage, this,
			std::placeholders::_1);
}

uint32_t ElementManager::AddElement(MenuElement* Element)
{
	for (MenuElement* Element2 : m_Elements)
	{
		if (Element2->GetId() == Element->GetId())
			return Element2->GetId();
	}
	Element->EventZOrderChanged() +=
		std::bind(&ElementManager::ReSortZOrder, this, std::placeholders::_1);
	m_Elements.push_back(Element);
	return Element->GetId();
}

void ElementManager::Render()
{
	//Render in reverse order to allow first element to be on top
	m_Renderer->PreFrame();
	for (auto i = m_Elements.rbegin(); i != m_Elements.rend(); i++)
	{
		(*i)->Draw(*m_Renderer);
	}
	m_Renderer->Present();
}

void ElementManager::ProcessMouseMessage(const MouseMessage& Msg)
{
	switch (Msg.GetEvent())
	{
	case MouseMessage::MouseEvent::Move:
		for (MenuElement* Element : m_Elements)
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
		break;
	case MouseMessage::MouseEvent::BtnDown:
		for (MenuElement* Element : m_Elements)
		{
			if (!Element->IsPointInMouseDownZone(Msg.GetLocation()))
				continue;

			Element->OnMouseDown(Msg);
			return;
		}
		break;
	case MouseMessage::MouseEvent::BtnUp:
		for (MenuElement* Element : m_Elements)
		{
			//if (!Element->IsPointInMouseDownZone(Msg.GetLocation()))
			//	continue;

			Element->OnMouseUp(Msg);
		}
		break;
	default:
		break;
	}
}

void ElementManager::ProcessKeyboardMessage(const KeyboardMessage& Msg)
{

}

void ElementManager::UpdateInput()
{
	m_InputManager->PollKeyboard();
	m_InputManager->PollMouse();
}

void ElementManager::ReSortZOrder(const uint32_t Id)
{
	auto Pivot = std::find_if(m_Elements.begin(), m_Elements.end(), [&](MenuElement* Elem)->bool {
		return Elem->GetId() == Id;
	});
	std::vector<MenuElement*> Temp;
	Temp.push_back(*Pivot);
	for (MenuElement* Elem : m_Elements)
	{
		if (Elem->GetId() != Id)
			Temp.push_back(Elem);
	}
	m_Elements = Temp;
}

template<typename T>
T* ElementManager::GetElementById(const uint32_t Id)
{
	for (MenuElement* Element : m_Elements)
	{
		if (Element->GetId() == Id)
			return dynamic_cast<T*>(Element);

		//Search all windows for their elements also
		if (Element->GetType() == ElementType::Window)
		{
			WindowElement* WindowElem = dynamic_cast<WindowElement*>(Element);
			if (WindowElem == nullptr)
				continue;

			T* RetElement = WindowElem->GetElementById<T>(Id);
			if (RetElement == nullptr)
				continue;
			return RetElement;
		}
		if (Element->GetType() == ElementType::TabbedWindow)
		{
			TabbedWindowElement* TabbedWindowElem = dynamic_cast<TabbedWindowElement*>(Element);
			if (TabbedWindowElem == nullptr)
				continue;

			T* RetElement = TabbedWindowElem->GetElementById<T>(Id);
			if (RetElement == nullptr)
				continue;

			return RetElement;
		}
	}
	return nullptr;
}