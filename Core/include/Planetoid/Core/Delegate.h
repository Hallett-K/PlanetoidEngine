#pragma once

#include <iostream>
#include <iterator>
#include <functional>
#include <list>

namespace PlanetoidEngine
{
    template <typename... Args>
    class PE_API Delegate
    {
    public:

        Delegate() = default;

        ~Delegate()
        {
            UnbindAll();
        }

        using Func = std::function<void(Args...)>;

        class PE_API DelegateIterator
        {
        public:
            DelegateIterator() : m_id(0), delegate_(nullptr) {}

            explicit DelegateIterator(unsigned int id) : m_id(id) {}

            bool IsBound() const { return delegate_ != nullptr && m_id != 0; }

        private:
            friend class Delegate;

            unsigned int m_id;
            Delegate* delegate_;

            explicit DelegateIterator(unsigned int id, Delegate* delegate)
                : m_id(id), delegate_(delegate)
            {}
        };

        DelegateIterator Bind(Func func)
        {
            m_functions[m_nextId++] = func;
            return DelegateIterator(m_nextId - 1, this);
        }

        template <typename T>
        DelegateIterator Bind(void (T::*func)(Args...), T* instance)
        {
            //m_functions.emplace_back([instance, func](Args... args) {
            //    (instance->*func)(args...);
            //});

            m_functions[m_nextId++] = [instance, func](Args... args) {
                (instance->*func)(args...);
            };

            return DelegateIterator(m_nextId - 1, this);
        }

        void Unbind(DelegateIterator it)
        {
            if (it.delegate_ == this)
            {
                m_functions.erase(it.m_id);
            }
        }

        void UnbindAll()
        {
            m_functions.clear();
        }

        void Broadcast(Args... args)
        {
            for (auto& func : m_functions)
            {
                func.second(args...);
            }
        }

    private:
        std::unordered_map<unsigned int, Func> m_functions;
        unsigned int m_nextId = 1;
    };

// Macro to declare a delegate type with specific parameter types
#define DECLARE_DELEGATE(DelegateName, ...) \
    class PE_API DelegateName : public Delegate<__VA_ARGS__> {}
}