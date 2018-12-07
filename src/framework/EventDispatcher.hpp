#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

#include "framework/Common.hpp"

namespace Tobi
{

template <typename Event>
class Dispatcher
{
  public:
    class Listener
    {
      public:
        virtual ~Listener(){};
        virtual void onEvent(Event &event, Dispatcher &sender) = 0;
    };

  private:
    typedef std::shared_ptr<Listener> ListenerPtr;
    typedef std::vector<ListenerPtr> Listeners;
    Listeners listeners;

  public:
    virtual ~Dispatcher()
    {
        while (!listeners.empty())
        {
            listeners.erase(listeners.begin());
        }
    }
    void Reg(ListenerPtr listener)
    {
        if (listeners.end() != std::find(listeners.begin(), listeners.end(), listener))
            return;

        listeners.push_back(listener);
    }

    void Unreg(ListenerPtr listener)
    {
        typename Listeners::iterator iter = std::find(listeners.begin(), listeners.end(), listener);
        if (listeners.end() == iter)
            return;

        listeners.erase(iter);
    }

    void Dispatch(Event &event)
    {
        for (typename Listeners::iterator iter = listeners.begin(); iter != listeners.end(); ++iter)
            (*iter)->onEvent(event, *this);
    }
};

struct KeyPressEvent
{
    uint32_t key;
    KeyPressEvent(uint32_t key) : key(key) {}
};

class KeyPressDispatcher : public Dispatcher<KeyPressEvent>
{
  public:
    KeyPressDispatcher() { LOGI("CONSTRUCTING KeyPressDispatcher\n"); };
    ~KeyPressDispatcher() { LOGI("DECONSTRUCTING KeyPressDispatcher\n"); };
};

struct KeyReleaseEvent
{
    uint32_t key;
    KeyReleaseEvent(uint32_t key) : key(key) {}
};

class KeyReleaseDispatcher : public Dispatcher<KeyReleaseEvent>
{
  public:
    KeyReleaseDispatcher() { LOGI("CONSTRUCTING KeyReleaseDispatcher\n"); };
    ~KeyReleaseDispatcher() { LOGI("DECONSTRUCTING KeyReleaseDispatcher\n"); };
};

} // namespace Tobi
