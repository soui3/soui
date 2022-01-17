#pragma once

#include "SEventSlot.h"

namespace SOUI
{

    class SOUI_EXP SEvent
    {
    public:
        SEvent(DWORD dwEventID,LPCWSTR pszEventName);

        virtual ~SEvent();

        DWORD GetID();

        SStringW GetName() const;

        SStringA GetScriptHandler() const;

        void SetScriptHandler(const SStringA & strScriptHandler);

        BOOL subscribe(const IEvtSlot* slot);

        BOOL unsubscribe(const IEvtSlot* slot);

        void fire(IEvtArgs* args);

    protected:
        int findSlotFunctor(const IEvtSlot* slot);

        DWORD    m_dwEventID;
        SStringW m_strEventName;
        SStringA m_strScriptHandler;

        SArray<IEvtSlot *> m_evtSlots;
    };

    class SOUI_EXP SEventSet
    {
        friend class SWindow;
    public:
        SEventSet(void);
        virtual ~SEventSet(void);

        /*!
        \brief
            Add a new Event to the EventSet with the given name.

        \param name
            String object containing the name to give the new Event.  The name must be unique for the EventSet.

        \return
            Nothing

        \exception AlreadyExistsException    Thrown if an Event already exists named \a name.
        */
        void    addEvent(DWORD dwEventID,LPCWSTR pszEventHandlerName);


        /*!
        \brief
            Removes the Event with the given name.  All connections to the event are disconnected.

        \param name
            String object containing the name of the Event to remove.  If no such Event exists, nothing happens.

        \return
            Nothing.
        */
        void    removeEvent(DWORD dwEventID);


        /*!
        \brief
            Remove all Event objects from the EventSet

        \return
            Nothing
        */
        void    removeAllEvents(void);


        /*!
        \brief
            Checks to see if an Event with the given name is present in the EventSet.

        \return
            true if an Event named \a name was found, or false if the Event was not found
        */
        BOOL    isEventPresent(DWORD dwEventID);

        BOOL    setEventScriptHandler(const SStringW &  strEventName,const SStringA strScriptHandler);

        SStringA getEventScriptHandler(const SStringW &  strEventName) const;

        /*!
        \brief
            Subscribes a handler to Event. .
        \param dwEventID
            Event ID to subscribe to.
        \param ISlotFunctor
            Function or object that is to be subscribed to the Event.
        \return BOOL
        */
		BOOL subscribeEvent(DWORD dwEventID, const IEvtSlot & subscriber){
			return subscribeEvent(dwEventID,&subscriber);
		}

		BOOL subscribeEvent(DWORD dwEventID, const IEvtSlot * subscriber);
#if _MSC_VER >= 1700	//VS2012
		BOOL subscribeEvent(DWORD dwEventID, const StdFunCallback & eventCallback);
#endif
        template<typename T, typename A>
        BOOL subscribeEvent(BOOL (T::* pFn)(A *), T* pObject) {
            return subscribeEvent(A::EventID, Subscriber(pFn, pObject));
        }
        template<typename A>
        BOOL subscribeEvent(BOOL(*pFn)(A *)) {
            return subscribeEvent(A::EventID, Subscriber(pFn));
        }
		//add by 2017.2.25 008
		template<typename T>
		BOOL subscribeEvent(DWORD dwEventID, BOOL(T::*pFn)(IEvtArgs*),T *pObject) {
			return subscribeEvent(dwEventID, Subscriber(pFn,this));
		}

        BOOL unsubscribeEvent(DWORD dwEventID, const IEvtSlot * subscriber );

		BOOL unsubscribeEvent(DWORD dwEventID, const IEvtSlot & subscriber )
		{
			return unsubscribeEvent(dwEventID,&subscriber);
		}

        template<typename T, typename A>
        BOOL unsubscribeEvent(BOOL (T::* pFn)(A *), T* pObject) {
            return unsubscribeEvent(A::EventID, Subscriber(pFn, pObject));
        }
        template<typename A>
        BOOL unsubscribeEvent(BOOL(*pFn)(A *)) {
            return unsubscribeEvent(A::EventID, Subscriber(pFn));
        }
		//add by 2017.2.25 008
		template<typename T>
		BOOL unsubscribeEvent(DWORD dwEventID, BOOL(T::*pFn)(IEvtArgs*), T *pObject) {
			return unsubscribeEvent(dwEventID, Subscriber(pFn, this));
		}
        void FireEvent(IEvtArgs* args);

            /*!
    \brief
        Return whether the EventSet is muted or not.

    \return
        - true if the EventSet is muted.  All requests to fire events will be ignored.
        - false if the EventSet is not muted.  All requests to fire events are processed as normal.
    */
    BOOL    isMuted(void) const
    {
        return m_nMuted>0;
    }


    /*!
    \brief
        Set the mute state for this EventSet.

    \param setting
        - true if the EventSet is to be muted (no further event firing requests will be honoured until EventSet is unmuted).
        - false if the EventSet is not to be muted and all events should fired as requested.

    \return
        Nothing.
    */
	void    setMutedState(BOOL setting);

    protected:
        SEvent * GetEventObject(const DWORD dwEventID);
        SArray<SEvent *> m_evtArr;
        int      m_nMuted;
    };


}

