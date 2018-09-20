#include <iostream>
#include <memory>
#include <assert.h>

template <typename T>
class Stack {
    std::unique_ptr<T[]> buffer;
    size_t size, buffer_size;

    void change_buffer_size(size_t new_size) {
        std::unique_ptr <T[]> new_buffer(new T[new_size]);
        for(int i = 0; i < std::min(new_size, buffer_size); i++)
            new_buffer[i] = buffer[i];
        buffer.swap(new_buffer);
        new_buffer.release();
        buffer_size = new_size;
    }

public:

    explicit Stack(size_t new_size) {
        size = new_size;
        buffer_size = std::max(new_size, static_cast<size_t >(1));
        buffer.reset(new T[buffer_size]);
    }

    explicit Stack() {
        size = 0;
        buffer_size = 1;
        buffer.reset(new T[buffer_size]);
    }

    Stack(Stack& old) {
        buffer_size = old.buffer_size;
        size = old.size;
        buffer.reset(new T[buffer_size]);
        for(int i = 0; i < size; i++)
            buffer[i] = old.buffer[i];
    }

    void swap(Stack& one, Stack& two) {
        one.buffer.swap(two.buffer);
        std::swap(one.size, two.size);
        std::swap(one.buffer_size, two.buffer_size);
    };

    Stack&operator=(Stack that) {
        swap(*this, that);
    }

    T top() {
        assert(!empty());
        return buffer[size - 1];
    }

    void push(T &element) {
        buffer[size++] = element;
        if(size == buffer_size)
            change_buffer_size(buffer_size << 1);
    }

    void push(T &&element) {
        push(element);
    }

    void pop() {
        assert(!empty());
        size--;
        if(size < (buffer_size >> 2))
            change_buffer_size(buffer_size >> 1);
    }

    bool empty() {
        return size == 0;
    }

    inline size_t get_size() const {
        return size;
    }

    ~Stack() {
        buffer.release();
    }
};

template <typename T>
class Queue{
    Stack <T> in, out;

    void relocate() {
        while(!in.empty()) {
            out.push(in.top());
            in.pop();
        }
    }

public:

    bool empty() {
        return in.empty() && out.empty();
    }
    void push(T& element) {
        in.push(element);
    }

    T front() {
        assert(!empty());
        if(out.empty())
            relocate();
        return out.top();
    }

    void pop() {
        assert(!empty());
        if(out.empty())
            relocate();
        out.pop();
    }

    void push(T&& element) {
        push(element);
    }
};

int main() {
    return 0;
}