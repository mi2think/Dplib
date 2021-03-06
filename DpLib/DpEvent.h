/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   20:50
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpEvent.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpEvent
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Event
*********************************************************************/
#ifndef __DP_EVENT__
#define __DP_EVENT__

#include "DoPixel.h"

namespace dopixel
{
	// Abstract class
	class Event
	{
	public:
		virtual ~Event() {}

		int GetEventType() const { return eventType_; }

		// subclass must override this and return their class type
		static int GetStaticEventType();

		virtual string ToString() const { return typeid(*this).name(); }

		typedef enum {
			UNDEFINED,

			// Key event type
			KEY_PRESS,
			KEY_RELEASE,

			// Mouse event type
			MOUSE_MOVE,
			MOUSE_PRESS,
			MOUSE_RELEASE,
			MOUSE_WHELL,

			// User define must begin at this ID
			USER = 100
		} CoreEventType;
	protected:
		Event(int eventType) : eventType_(eventType) {}
		int eventType_;
	};

	extern const char _BaseEventName[];

	// Base class
	template<int EventType, const char* const EventName = _BaseEventName>
	class BaseEvent : public Event
	{
	public:
		BaseEvent() : Event(EventType) {}

		static int GetStaticEventType() { return EventType; }

		virtual string ToString() const
		{
			if (*EventName)
				return EventName;
			else
				return Event::ToString();
		}
	};

	// down-cast an event reference
	// usage:
	//
	// Ref<MyEvent> me = event_cast<MyEvent>(e);
	template<typename EventType>
	inline Ref<EventType> event_cast(const EventRef& event)
	{
		if (event->GetEventType() != EventType::GetStaticEventType())
		{
			return Ref<EventType>();
		}
		return event.As<EventType>();
	}

	template<typename EventType>
	inline const EventType* event_cast(const Event& event)
	{
		if (event.GetEventType() != EventType::GetStaticEventType())
		{
			return nullptr;
		}
		return static_cast<const EventType*>(&event);
	}

	// Event listener
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}

		// handle event, return true if eat event, else false
		virtual bool OnEvent(const Event& event) = 0;
	};

	// Event broadcaster
	class EventBroadcaster
	{
	public:
		EventBroadcaster() {}

		void RegisterEventListener(IEventListener* listener);

		void UnregisterEventListener(IEventListener* listener);

		bool PostEvent(const Event& event);
	private:
		std::vector<IEventListener*> listeners_;
	};

	// Event dispatch
	class EventDispatch
	{
	public:
		EventDispatch(const Event& event)
			: event_(event)
			, handled_(false)
		{
		}

		template<typename EventType>
		void Dispatch(bool(*func)(const EventType&))
		{
			if (handled_)
				return;

			if (const EventType* staticEvent = event_cast<EventType>(event_))
			{
				handled_ |= (*func)(*staticEvent);
			}
		}

		template<typename T, typename EventType>
		void Dispatch(T* obj, bool (T::*method)(const EventType&))
		{
			if (handled_)
				return;

			if (const EventType* staticEvent = event_cast<EventType>(event_))
			{
				handled_ |= (obj->*method)(*staticEvent);
			}
		}

		void Dispatch(IEventListener* listener)
		{
			if (handled_)
				return;

			handled_ |= listener->OnEvent(event_);
		}

		void Dispatch(IEventListener& listener)
		{
			if (handled_)
				return;

			handled_ |= listener.OnEvent(event_);
		}

		bool GetResult() const { return handled_; }
		void SetResult(bool handled) { handled_ = handled; }
	private:
		const Event& event_;
		bool handled_;
	};
}

#endif // !__DP_EVENT__
