#include <iostream>
#include <stdexcept>

template <class Key>
struct Less
{
    bool operator()(Key a, Key b) const
    {
        return a < b;
    }
};

template <class Key, class T, class Compare=Less<Key>>
class map
{
private:

    struct Node
    {
        Node* Parent;
        Node* Left;
        Node* Right;
        std::pair<Key, T> Value;
    };

    Node * Root;

    void Delete(Node* rhs)
    {
        if (rhs != nullptr) {
            if (rhs->Left != nullptr) {
                Delete(rhs->Left);
            }
            if (rhs->Right != nullptr) {
                Delete(rhs->Right);
            }
            delete rhs;
        }
        return;
    }

    Node* Copy(Node* rhs)
    {
        Node* tmp = new Node;
        if (rhs != nullptr) {
            tmp->Parent = rhs->Parent;
            tmp->Value.first = rhs->Value.first;
            tmp->Value.second = rhs->Value.second;
            if (rhs->Left != nullptr)
                tmp->Left = Copy(rhs->Left);
            if (rhs->Right != nullptr)
                tmp->Right = Copy(rhs->Right);
        }
        return tmp;
    }

public:

    struct Iterator
            : public std::iterator<std::random_access_iterator_tag, std::pair<Key, T>>
    {

        Node * ptr;

        explicit Iterator(Node* p)
                : ptr(p)
        { }

        std::pair<Key, T>& operator*()
        {
            return ptr->Value;
        }

        std::pair<Key, T>* operator->()
        {
            std::pair<Key, T>* tmp = &(ptr->Value);
            return tmp;
        }

        Iterator& operator++()
        {
            if (ptr->Right != nullptr) {
                ptr = ptr->Right;
                while (ptr->Left != nullptr) {
                    ptr = ptr->Left;
                }
                return *this;
            } else {
                while(ptr->Parent != nullptr) {
                    if (ptr->Parent->Value.first > ptr->Value.first) {
                        ptr = ptr->Parent;
                        return *this;
                    } else {
                        if(ptr->Value.first == Key()){
                            return *this;
                        }
                        ptr = ptr->Parent;
                    }
                }
            }
        }

        Iterator operator++(int /*unused*/)
        {
            Iterator it(ptr);
            ++*this;
            return it;
        }

        Iterator& operator+=(size_t n)
        {
            for (size_t i = 0; i < n; ++i) {
                ++ptr;
            }
            return *this;
        }

        Iterator& operator-=(size_t n)
        {
            for (size_t i = 0; i < n; ++i) {
                --ptr;
            }
            return *this;
        }

        Iterator& operator--()
        {
            if (*this == end()) {
                while(ptr->Right != nullptr)
                    ptr =  ptr->Right;
            }
            if (ptr->Left != nullptr) {
                ptr = ptr->Left;
                while (ptr->Right != nullptr) {
                    ptr = ptr->Right;
                }
                return *this;
            } else {
                while(ptr->Parent != nullptr) {
                    if (ptr->Parent->Value.first > ptr->Value.first) {
                        ptr = ptr->Parent;
                    } else {
                        ptr = ptr->Parent;
                        return *this;
                    }
                }
            }
        }

        Iterator operator--(int /*unused*/)
        {
            Iterator it(ptr);
            --ptr;
            return it;
        }

        bool operator==(Iterator it)
        {
            return (ptr->Value.first == it.ptr->Value.first)
                   && (ptr->Value.second == it.ptr->Value.second);
        }

        bool operator<(Iterator it)
        {
            return ptr->Value.first < it.ptr->Value.first;
        }

        bool operator<=(Iterator it)
        {
            return this->operator==(it) || this->operator<(it);
        }

        bool operator>(Iterator it)
        {
            return !(this->operator==(it) || this->operator<(it));
        }

        bool operator>=(Iterator it)
        {
            return !this->operator<(it);
        }

        bool operator!=(Iterator it)
        {
            return !(this->operator==(it));
        }
    };

    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = Iterator;
    using const_iterator = const Iterator;

    map()
    {
        Root = new Node;
        Root->Parent = new Node;
        Root->Parent->Right = Root;
        Root->Parent->Parent = nullptr;
        Root->Parent->Left = nullptr;
        Root->Parent->Value.first = Key();
        Root->Parent->Value.second = T();
        Root->Right = nullptr;
        Root->Left = nullptr;
        Root->Value.first = Key();
        Root->Value.second = T();
    }

    ~map()
    {
        Delete(Root);
    }

    map(const map& rhs)
    {
        Root = Copy(rhs.Root);
    }

    map& operator=(const map& rhs)
    {
        if (this != &rhs) {
            Delete(Root);
            Root = Copy(rhs.Root);
        }
        return *this;
    }

    iterator begin()
    {
        Node* tmp = Root;
        while (tmp->Left != nullptr) {
            tmp = tmp->Left;
        }
        return Iterator(tmp);
    }

    const_iterator begin() const
    {
        Node* tmp = Root;
        while (tmp->Left != nullptr) {
            tmp = tmp->Left;
        }
        return const_iterator(tmp);
    }

    iterator end()
    {
        iterator it(Root->Parent);
        return it;
    }

    const_iterator end() const
    {
        iterator it(Root->Parent);
        return it;
    }

    size_type size() const
    {
        if ((Root->Left == nullptr) && (Root->Right == nullptr)
                && (Root->Value.first == Key()) && (Root->Value.second == T())) {
            return 0;
        } else {
            size_t length = 0;
            for (iterator it = begin(); it != end(); ++it) {
                ++length;
            }
            return length;
        }
    }

    bool empty() const
    {
        return size() == 0;
    }

    T& operator[](const Key& key)
    {
        Iterator result = find(key);
        if (result == end()) {
            insert(key, T());
            return find(key).ptr->Value.second;
        }
        return result.ptr->Value.second;
    }

    T& operator[](Key&& key)
    {
        Iterator result = find(key);
        if (result == end()) {
            insert(key, T());
            return find(key).ptr->Value.second;
        }
        return result.ptr->Value.second;
    }

    T& at(const Key& key)
    {
        Iterator result = find(key);
        if (result.ptr == nullptr) {
            throw std::out_of_range("Error");
        } else {
            return result.ptr->Value.second;
        }
    }

    const T & at(const Key& key) const
    {
        Iterator result = find(key);
        if (result.ptr == nullptr) {
            throw std::out_of_range("Error");
        } else {
            return result.ptr->Value.second;
        }
    }

    void insert(const Key& key, const T& value)
    {
        if (find(key) != end()) {
            return;
        }
        if ((Root->Value.first == Key()) && (Root->Value.second == T())) {
            Root->Value.first = key;
            Root->Value.second = value;
            return;
        }
        Node * newBranch = new Node;
        Node * newNode = nullptr;
        Node * tmp = Root;
        newBranch->Value.first = key;
        newBranch->Value.second = value;
        newBranch->Left = nullptr;
        newBranch->Right = nullptr;
        while (tmp != nullptr) {
            newNode = tmp;
            if ((newBranch->Value.first) < (tmp->Value.first)) {
                tmp = tmp->Left;
            } else {
                tmp = tmp->Right;
            }
        }
        newBranch->Parent = newNode;
        if (newNode == nullptr) {
            Root = newBranch;
        } else {
            if ((newBranch->Value.first) < (newNode->Value.first)) {
                newNode->Left = newBranch;
            } else {
                newNode->Right = newBranch;
            }
        }
    }

    void insert(std::pair<Key, T> rhs)
    {
        Key key = rhs.first;
        T value = rhs.second;
        if (find(key) != end()) {
            return;
        }
        if ((Root->Value.first == Key()) && (Root->Value.second == T())) {
            Root->Value.first = key;
            Root->Value.second = value;
            return;
        }
        Node * newBranch = new Node;
        Node * newNode = nullptr;
        Node * tmp = Root;
        newBranch->Value.first = key;
        newBranch->Value.second = value;
        newBranch->Left = nullptr;
        newBranch->Right = nullptr;
        while (tmp != nullptr) {
            newNode = tmp;
            if ((newBranch->Value.first) < (tmp->Value.first)) {
                tmp = tmp->Left;
            } else {
                tmp = tmp->Right;
            }
        }
        newBranch->Parent = newNode;
        if (newNode == nullptr) {
            Root = newBranch;
        } else {
            if ((newBranch->Value.first) < (newNode->Value.first)) {
                newNode->Left = newBranch;
            } else {
                newNode->Right = newBranch;
            }
        }
    }

    void erase(Iterator it)
    {
        Iterator tmp = find(it->first);
        if (tmp == end()) {
            return;
        }
        if (tmp.ptr->Right == nullptr) {
            if (tmp.ptr->Parent->Left == tmp.ptr) {
                tmp.ptr->Parent->Left = tmp.ptr->Left;
            } else {
                tmp.ptr->Parent->Right = tmp.ptr->Left;
            }
            tmp.ptr->Left->Parent = tmp.ptr->Parent;
            return;
        } else if (tmp.ptr->Left == nullptr) {
            if (tmp.ptr->Parent->Left == tmp.ptr) {
                tmp.ptr->Parent->Left = tmp.ptr->Right;
            } else {
                tmp.ptr->Parent->Right = tmp.ptr->Right;
            }
            tmp.ptr->Right->Parent = tmp.ptr->Parent;
            return;
        } else {
            if (tmp.ptr == Root) {
                std::swap(tmp.ptr->Right->Value, Root->Value);
                tmp.ptr = tmp.ptr->Right;
            }
            Node* rhs = tmp.ptr->Right;
            while (rhs->Left != nullptr) {
                rhs = rhs->Left;
            }
            tmp.ptr->Value.first = rhs->Value.first;
            tmp.ptr->Value.second = rhs->Value.second;
            Node* tmp1 = rhs->Parent->Left;
            rhs->Parent->Left = nullptr;
            delete tmp1;
        }
    }

    void swap(map& other)
    {
        std::swap(Root, other.Root);
    }

    void clear()
    {
        delete Root->Parent;
        Delete(Root);
        Root = new Node;
        Root->Parent = new Node;
        Root->Parent->Right = Root;
        Root->Right = nullptr;
        Root->Left = nullptr;
        Root->Value.first = Key();
        Root->Value.second = T();
    }

    Iterator find(const Key& key) const
    {
        Node* tmp = Root;
        while (tmp->Value.first != key) {
            if (key < tmp->Value.first) {
                tmp = tmp->Left;
            } else {
                tmp = tmp->Right;
            }
            if (tmp == nullptr)
            {
                return end();
            }
        }
        iterator it(tmp);
        return it;
    }
};
