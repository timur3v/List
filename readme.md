#List
Project contains an std-like implementation of double-linked list.

##Language standart
It is recommended to use List with C++17, also all references to standard library are to this version.

## Iterators
List has the following public member types which are bidirectional iterators.

* *iterator*
* *const_iterator*
* *reverse_iterator*
* *const_reverse_iterator* 

Behaviour is same as in std::list.

##Allocator 
List is an allocator-aware container. By default, it uses std::allocator. To successfully use Allocator in List it must meet the requirements of Allocator and the following of its member functions should not throw exceptions (if implemented): copy and move constructor, copy and move assignment operator, construct, destroy, deallocate.  

##Public methods of List class
###Constructors
* `List();`
* `List(const List& other);`
* `List(List&& other);`
* `List(size_t count, const T& value = T(), const Allocator& alloc = Allocator());`
* `List(const Allocator& alloc);`

###Destructor
* `~List() noexcept;`

###Assignment operators
* `List& operator=(const List& other);`
* `List& operator=(List&& other) noexcept(noexcept(MoveFromOther(std::move(other))));`

###Capacity
* `size_t size() const noexcept;`
* `bool empty() const noexcept;`

###Element access
* `const T& front() const noexcept;`
* `const T& back() const noexcept;`

* `T& front() noexcept;`
* `T& back() noexcept;`

###Modifiers
* `template <typename... Args_t>`
  
  `void emplace_back(Args_t&& ...args);`
* `template <typename... Args_t>`

  `void emplace_front(Args_t&& ...args);`

* `void push_back(const T& value);`
* `void push_back(T&& value);`

* `void push_front(const T& value);`
* `void push_front(T&& value);`

* `void pop_front() noexcept;`
* `void pop_back() noexcept;`

* `void reverse();`
* `void unique();`
* `void clear() noexcept;`

###Presentation
* `void Print() const;`
