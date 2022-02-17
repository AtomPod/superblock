#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H
#include <atomic>
namespace hfs {

template<class T>
class ObjectView
{
public:
    typedef std::atomic_uint64_t counter_t;
public:
    ObjectView();
    ObjectView(T &value);
    ObjectView(T *value, bool owner = true);

    ObjectView(const ObjectView &other);

    template<class U>
    ObjectView(const ObjectView<U> &other);
    ~ObjectView();
public:
    template<class U>
    ObjectView &operator=(const ObjectView<U> &other);
    T *operator->();
    const T *operator->() const;
public:
    T *ptr() const;
protected:
    void release();
private:
    bool       m_owner;
    counter_t *m_counter;
    T         *m_ptr;
};

template<class T>
ObjectView<T>::ObjectView()
    : m_owner(true),
      m_counter(nullptr),
      m_ptr(nullptr)
{
    m_counter = new counter_t(1);
}

template<class T>
ObjectView<T>::ObjectView(T &value)
    : ObjectView(&value, false)
{

}

template<class T>
ObjectView<T>::ObjectView(T *value, bool owner)
    : m_owner(owner),
      m_counter(nullptr),
      m_ptr(value)
{
  m_counter = new counter_t(1);
}

template<class T>
ObjectView<T>::ObjectView(const ObjectView &other)
  : m_owner(other.m_owner),
    m_counter(other.m_counter),
    m_ptr(other.m_ptr)
{
  m_counter->fetch_add(1, std::memory_order_relaxed);
}

template<class T>
template<class U>
ObjectView<T>::ObjectView(const ObjectView<U> &other)
    : m_owner(other.m_owner),
      m_counter(other.m_counter),
      m_ptr(other.m_ptr)
{
    m_counter->fetch_add(1, std::memory_order_relaxed);
}

template<class T>
ObjectView<T>::~ObjectView()
{
    release();
}

template<class T>
T *ObjectView<T>::operator->()
{
    return m_ptr;
}

template<class T>
const T *ObjectView<T>::operator->() const
{
    return m_ptr;
}

template<class T>
T *ObjectView<T>::ptr() const
{
    return m_ptr;
}

template<class T>
void ObjectView<T>::release()
{
    if (m_counter != nullptr && m_counter->fetch_add(-1, std::memory_order_relaxed) == 0) {
      delete m_counter;
      if (m_owner) {
        delete m_ptr;
      }
      m_ptr = nullptr;
      m_counter = nullptr;
    }
}

template<class T>
template<class U>
ObjectView<T> &ObjectView<T>::operator=(const ObjectView<U> &other)
{
    if (&other == this) {
        return *this;
    }

    this->m_ptr = other.m_ptr;
    this->m_owner = other.m_owner;
    this->m_counter = other.m_counter;
    this->m_counter->fetch_add(1, std::memory_order_relaxed);
    return *this;
}

}

#endif // OBJECTVIEW_H
