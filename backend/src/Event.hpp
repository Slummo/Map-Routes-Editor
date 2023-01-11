
#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <vector>
#include <cstdlib>
#include <mutex>
#include <iostream>
#include <thread>
#include <memory>

enum class GLEvent {
    NONE, REQUEST_FRAME, RESIZE_FRAME, MOVE_CAMERA, LOAD_MAP
};

class GLEventQueue {
    private:
        std::mutex mut;
        std::vector<GLEvent> _queue;

    public:
        GLEventQueue() {};

        inline GLEvent pop() {
            std::lock_guard<std::mutex> lock(mut);

            if (_queue.size() == 0) {
                return GLEvent::NONE;
            }

            GLEvent event = _queue.back();
            _queue.pop_back();
            
            return event;
        };

        inline void push(GLEvent event){
            std::lock_guard<std::mutex> lock(mut);
            
            _queue.push_back(event);
        };

        inline size_t size(){
            std::lock_guard<std::mutex> lock(mut);

            return _queue.size();
        };
};

// Still have to test these


class Event {
    protected:
        GLEvent type = GLEvent::NONE;

        std::mutex hMutex;
        std::shared_ptr<bool> isHandled;
        std::shared_ptr<void> data;

    public:
        Event() {

        };

        Event(const Event& other) : type(other.type), isHandled(other.isHandled), data(other.data) {};

        Event(GLEvent _type, void* _data) : type(_type), data(_data) {
            isHandled = std::make_shared<bool>();
           *isHandled = false; 
        };

        Event(GLEvent _type, std::shared_ptr<void> _data) : type(_type), data(_data) {
            isHandled = std::make_shared<bool>();
           *isHandled = false; 
        };

        void setHandled() {
            if (!isHandled) {
                return;
            }

            hMutex.lock();
            *isHandled = true;
            hMutex.unlock();
        };

        void waitUntilHandled() {
            if (!isHandled) {
                return;
            }

            while (1) {
                hMutex.lock();
                if (*isHandled) break;
                hMutex.unlock();
            }
        };
};

class EventQueue {
    private:
        std::mutex mut;
        std::vector<Event> _queue;

    public:
        EventQueue() {};

        inline Event pop() {
            std::lock_guard<std::mutex> lock(mut);

            if (_queue.size() == 0) {
                return Event();
            }

            Event event = _queue.back();
            _queue.pop_back();
            
            return event;
        };

        inline void push(Event event){
            std::lock_guard<std::mutex> lock(mut);
            
            _queue.push_back(event);
        };

        inline size_t size(){
            std::lock_guard<std::mutex> lock(mut);

            return _queue.size();
        };
};

#endif