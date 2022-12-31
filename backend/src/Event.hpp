
#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <vector>
#include <cstdlib>
#include <mutex>
#include <iostream>

enum class GLEvent {
    NONE, REQUEST_FRAME, RESIZE_FRAME,
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


#endif