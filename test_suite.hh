/*! 
 * Test suite for the CS225 entity-component framework assigment.
 *
 * Contains the combined collection of tests for the assignment. 
 * Use this if you find it convenient to work with a single file that contains all the driver code.
 *
 * Modifications to this file are allowed only for development convenience (e.g. commenting out code
 * that fails to compile). This file will be used as provided during the grading process, so make sure
 * that your solution does not rely on any modification done on this file.
 *
 * If you want to disable a test, just comment it out. Bear in mind that the `TEST` macro makes it 
 * necessary to compile and run the test code, so both compilation and run time errors should
 * be taken into account.
 */
#include "test_helper.hh"
#include "testing.hh" // testing framework (ASSERT_THAT, etc.)
#include <chrono>
#include <thread>

using namespace testing;

#include <string>       // std::string
#include <exception>    // std::exception
#include <stdexcept>    // std::out_of_range, std::invalid_argument
#include <typeinfo>     // std::bad_cast

// ===========================================================================
// ===========================================================================
// ===========================================================================

// Events Callbacks
#include "event.hh" 
#include "event_callbacks.hh" 

/*********************************************************************
 *                        Test for Events Callbacks                  *
 *********************************************************************/

namespace Tests 
{ 
    namespace Events
    {
        #ifdef ENABLE_CALLBACK_TEST1
        // Dummy events for testing purpose
        class ConcreteEventA : public Event
        {
            public:
                ~ConcreteEventA(){};
        };

        class ConcreteEventB : public Event
        {
            public:
                ~ConcreteEventB(){};
        };
        
        //dummy global calback function 
        void dummyCallback(void* ctx, Event* evt){
            ctx = nullptr;
            evt = nullptr;
        }

        // Dummy object for callback testing
        class DummyObject
        {
            int id;
            public:
                DummyObject(int i):id(i){}                

                static void staticCallbackMethod(void *ctx, Event* evt){}
                
                void nonStaticCallbackMethod(void *ctx, Event *evt) {}
        };

        // [ Test #1 ] -------------------------------------------------------
        TEST( "Abstract Events",
            "All events inherit from an abstract class Event" )
        {        
             printf("TEST1 Abstract Event\n");

            ConcreteEventA eventA;
            ConcreteEventB eventB;

            printf("TEST1 Succeed!\n");
            SUCCEED();
        }
        #endif

        #ifdef ENABLE_CALLBACK_TEST2

        // [ Test #2 ] -------------------------------------------------------
        TEST( "Polymorphic Events",
            "Event type can be determined polymorphically" )
        {        
            printf("TEST2 Polymorphic events \n");

            ConcreteEventA* eventA = new ConcreteEventA();

            Event *event = static_cast<Event*>(eventA);
            ConcreteEventA *eventB = static_cast<ConcreteEventA*>(event);

            delete eventA;

            printf("TEST2 Succeed!\n");
            SUCCEED();
        }
        
        #endif

        #ifdef ENABLE_CALLBACK_TEST3

        // [ Test #3 ] -------------------------------------------------------
        TEST( "Callback function",
            "Callback Function Pointer defined as a datatype 'CallbackFuntion' that can be assigned to functions with the firm: void f(void*, Event *evt)" )
        {        
            printf("TEST3 Callback function \n");

            CallbackFunction function = dummyCallback;

            printf("TEST3 Succeed!\n");
            SUCCEED();
        }
        
        #endif
       
        #ifdef ENABLE_CALLBACK_TEST4

        // [ Test #4 ] -------------------------------------------------------
        TEST( "Callback functions can be assigned to static-methods",
            "Callback functions can be assigned to static-methods with the same firm" )
        {        
            printf("TEST4 Callback function can be assigned to static methods\n");

            CallbackFunction callbackFunction = DummyObject::staticCallbackMethod;
            callbackFunction(new DummyObject(0), new ConcreteEventA());

            printf("TEST4 Succeed!\n");
            SUCCEED();
        }
        
        #endif  
        
        #ifdef ENABLE_CALLBACK_TEST5

        // [ Test #5 ] -------------------------------------------------------
        TEST( "Event Listener",
            "Object that reacts to events and event handling callback method are stored in a EventListener struct/class")
        {        
            printf("TEST5 Event Listener\n");

            EventListener listener;
            DummyObject* dummy = new DummyObject(0);
            
            listener.contextObject = dummy;
            listener.callbackFunction = dummyCallback;
            
            listener.contextObject = nullptr;
            delete dummy;

            printf("TEST5 Succeed!\n");
            SUCCEED();
        }
        
        #endif  
        
        #ifdef ENABLE_CALLBACK_TEST6

        // [ Test #6 ] -------------------------------------------------------
        TEST( "Event Dispatcher for callbacks",
            "Listeners are stored in a EventDispatcher class, objects that implements the callback can subscribe to Events")
        {        
            printf("TEST6 Event Listener\n");

            EventListener listener;

            DummyObject dummyObject(0);

            EventDispatcher eventDispatcher;
            eventDispatcher.subscribe(&dummyObject, dummyObject.staticCallbackMethod);

            printf("TEST6 Succeed!\n");
            SUCCEED();
        }
        
        #endif  
        
        #ifdef ENABLE_CALLBACK_TEST7

         // [ Test #7 ] -------------------------------------------------------
        TEST( "Get event listener from event dispatcher",
            "Event dispatcher provides a method to obtain a listener subscribed in the array/container")
        {        
            printf("TEST7 Get event listener from Dispatcher \n");

            DummyObject dummyObject1(0);
            DummyObject dummyObject2(1);

            EventDispatcher eventDispatcher;
            eventDispatcher.subscribe(&dummyObject1, dummyObject1.staticCallbackMethod);
            eventDispatcher.subscribe(&dummyObject2, dummyObject2.staticCallbackMethod);

            EventListener* listener = eventDispatcher.getListenerAt(1);
            DummyObject *contextObject = static_cast<DummyObject*>(listener->contextObject);
            
            ASSERT_THAT( contextObject == &dummyObject2);

            ASSERT_THAT( contextObject->staticCallbackMethod == dummyObject2.staticCallbackMethod);

            printf("TEST7 Succeed!\n");
            
            SUCCEED();
        }
        
        #endif 

        #ifdef ENABLE_CALLBACK_TEST8

        const int WAIT_TIME = 1000000000;

        class TimerEvent : public Event { };

        class Timer
        {
            public:
                int number; 
                int seconds;

                static void handle_event(void* context, Event *event){
                    Timer* theTimer = static_cast<Timer*>(context);

                    if(static_cast<TimerEvent*>(event) != nullptr){
                        TimerEvent* tevent = static_cast<TimerEvent*>(event);
                        theTimer->showTime(tevent);
                    }
                }

                void showTime(TimerEvent* event){
                    seconds++;
                    printf("Event Timer %d! has passed exactly %d seconds \n", number, seconds);
                }
        };

         // [ Test #8 ] -------------------------------------------------------
        TEST( "Dispatch a event",
            "Event Dispatcher can send/dispatch events to their subscribed listeners")
        {        
            printf("TEST8 Event dispatcher can send events to their subscribed listeners  \n");
            
            Timer timer1 {1,0};
            Timer timer2 {2,0};

            EventDispatcher eventDispatcher;
            eventDispatcher.subscribe(&timer1, Timer::handle_event);

            TimerEvent event;
            for(int i=0; i < 3; i++){
                if(i==1)
                    eventDispatcher.subscribe(&timer2, Timer::handle_event);
                
                eventDispatcher.dispatchEvent(&event);          
                std::this_thread::sleep_for(std::chrono::nanoseconds(WAIT_TIME));  
            }
             
            ASSERT_THAT(timer1.seconds == 3);
            ASSERT_THAT(timer2.seconds == 2);

            printf("TEST8 Succeed!\n");
            
            SUCCEED();
        }
        
        #endif                   
    } 
} // namespace Test::Events


// ===========================================================================
// ===========================================================================
// ===========================================================================

// Event handlers tests
#include "event_handlers.hh" 

/*******************************************************************
 *                          Generic Event Handler tests            *
 *******************************************************************/

namespace Tests { 
    namespace EventHandlers
    {
        #ifdef ENABLE_EVENTHANDLER_TEST9
        
        class ConcreteEventHandler : public IEventHandler
        {
            void call(Event *event) override {}
        };

        class DummyEvent : public Event {};
        class OtherEvent : public Event {};
        class DummyObject 
        {
            public:
                bool eventCalled = false;
                void on_some_event(DummyEvent *event) {
                    eventCalled = true;
                };
                void on_other_event(OtherEvent *event) {

                };
            };

        // [ Test #9 ] -------------------------------------------------------
        TEST( "Event handler interface",
            "All events handlers implements an interface IEventHandler with a method 'call' to invoke the event" )
        {        
            printf("TEST9 Event handler interface\n");
            
            ConcreteEventHandler handler;
            
            printf("TEST9 Succeed!\n");
            SUCCEED();
        }
        #endif

        #ifdef ENABLE_EVENTHANDLER_TEST10

        // [ Test #10 ] -------------------------------------------------------
        TEST( "Generic event handler",
            "Generic event handlers can be compiled and instantiate using templates. All concrete event handlers must be create with a object intance and a function proint to an object method" )
        {        
            printf("TEST10 Generic event handler\n");
            
            DummyObject dummyObject;
            EventHandler<DummyObject, DummyEvent> eventHandler(&dummyObject, &DummyObject::on_some_event);
            
            printf("TEST10 Succeed!\n");
            SUCCEED();
        }
        
        #endif

        #ifdef ENABLE_EVENTHANDLER_TEST11

         // [ Test #11 ] -------------------------------------------------------
        TEST( "Invoking action method in event handler with a specific event",
            "Generic event handlers can invoque action method in object instance calling the 'call' method from the interface" )
        {        
            printf("TEST11 Invoking 'call' method in event handler from object instance and specific event\n");
            
            DummyObject dummyObject;
            DummyEvent event;
            
            EventHandler<DummyObject, DummyEvent> eventHandler(&dummyObject, &DummyObject::on_some_event);
            eventHandler.call(&event);

            ASSERT_THAT ( dummyObject.eventCalled );
            printf("TEST11 Succeed!\n");
            SUCCEED();
        }
        
        #endif       
     
        #ifdef ENABLE_EVENTHANDLER_TEST12

         // [ Test #12 ] -------------------------------------------------------
        TEST( "Invoking action method in event handler with a not supported event",
            "Generic event handlers must not invoque action method in object instance if they receive an event that is not defined in the template definition" )
        {        
            printf("TEST12 Generic event handler must not call action with not supported event \n");
            
            DummyObject dummyObject;
            OtherEvent notSupportEvent;
            
            EventHandler<DummyObject, DummyEvent> eventHandler(&dummyObject, &DummyObject::on_some_event);
            eventHandler.call(&notSupportEvent);

            ASSERT_THAT(!dummyObject.eventCalled)
            
            printf("TEST12 Succeed!\n");
            SUCCEED();
        }
        #endif  
   
        #ifdef ENABLE_EVENTHANDLER_TEST13

         // [ Test #13 ] -------------------------------------------------------
        TEST( "Event manager for event handlers",
            "Event manager can register even handlers so that they can be called during an event" )
        {        
            printf("TEST13 Event manager for event handlers \n");
            
            DummyObject dummyObject;
            DummyEvent dummyEvent;
            
            EventHandler<DummyObject, DummyEvent> eventHandler(&dummyObject, &DummyObject::on_some_event);
            eventHandler.call(&dummyEvent);

            EventManager eventManager;
            eventManager.registerEventHandler(&eventHandler);
            
            printf("TEST13 Succeed!\n");
            SUCCEED();
        }
        #endif        
        
        #ifdef ENABLE_EVENTHANDLER_TEST14

         // [ Test #14 ] -------------------------------------------------------
        TEST( "Get handler from event manager",
            "We can obtain an event handler from the even handler based on its position in the array" )
        {        
            printf("TEST14 Get event handler from event manager \n");
            
            DummyObject dummyObject;
            DummyEvent dummyEvent;
            
            EventHandler<DummyObject, DummyEvent> eventHandler1(&dummyObject, &DummyObject::on_some_event);
            EventHandler<DummyObject, OtherEvent> eventHandler2(&dummyObject, &DummyObject::on_other_event);

            EventManager eventManager;
            eventManager.registerEventHandler(&eventHandler1);
            eventManager.registerEventHandler(&eventHandler2);
                     
            IEventHandler* handler = eventManager.getEventHandlerAt(1);
            
            ASSERT_THAT(handler == &eventHandler2);

            printf("TEST14 Succeed!\n");
            SUCCEED();
        }
        #endif                
        
        #ifdef ENABLE_EVENTHANDLER_TEST15

        const int WAIT_TIME = 1000000000;

        class TimerEvent : public Event { };
        class AnotherEvent : public Event { };
        
        class Timer
        {
            public:
                int number; 
                int seconds;
                bool other_event_called = false;

                EventHandler<Timer, TimerEvent> timerEventHandler;
                EventHandler<Timer, AnotherEvent> otherEventHandler;

                Timer(int n, int s) : number(n), seconds(s), 
                timerEventHandler(this, &Timer::on_timer_event), 
                otherEventHandler(this, &Timer::on_other_event) {}

                void on_timer_event(TimerEvent *event){
                    seconds++;
                    printf("Event Timer %d! has passed exactly %d seconds \n", number, seconds);
                }
                
                void on_other_event(AnotherEvent *event){
                    other_event_called = true;
                    printf("Timer %d! other event called \n", number); 
                }
        };

         // [ Test #15 ] -------------------------------------------------------
        TEST( "Event manager can notify events to registered event handlers",
            "Event manager can notify events to the resgistered event handlers calling the action method 'call'" )
        {        
            printf("TEST15 Event manager can notify events to registered event handlers \n");
            
            Timer timer1(1,0);
            Timer timer2(2,0);

            EventManager eventManager;
            eventManager.registerEventHandler(&timer1.timerEventHandler);
            eventManager.registerEventHandler(&timer1.otherEventHandler);

            TimerEvent event;
            for(int i=0; i < 3; i++){
                if(i==1)
                    eventManager.registerEventHandler(&timer2.timerEventHandler);
                
                eventManager.notifyEvent(&event);          
                std::this_thread::sleep_for(std::chrono::nanoseconds(WAIT_TIME));  
            }

            AnotherEvent otherEvent;
            eventManager.notifyEvent(&otherEvent);

            ASSERT_THAT(timer1.seconds == 3);
            ASSERT_THAT(timer2.seconds == 2);
            ASSERT_THAT(timer1.other_event_called);
            ASSERT_THAT(!timer2.other_event_called);

            printf("TEST15 Succeed!\n");
            SUCCEED();
        }
        #endif         
    }     
} // namespace Tests::EventHandlers

