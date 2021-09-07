//
// Created by Timur Demchenko on 28.08.2020.
//

#pragma once

#include <memory>

// !!!NOTE!!!
// This is not final version
// TODO: add documentation
// TODO: finish with questions in comments
// TODO: test weird allocator cases

//
// DECLARATIONS
//

// ORGANIZE PUBLIC-PRIVATE
// NODE BASE does not depend on allocator
// exceptions?

template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  // Not so good - I don't use it
  // how many should i have?
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using size_type = size_t;

  List();
  List(const List& other);
  List(List&& other) noexcept(noexcept(MoveFromOther(std::move(other)))
      && std::is_nothrow_move_constructible_v<NodeAllocator>);
  explicit List(size_t count, const T& value = T(), const Allocator& alloc = Allocator());

  ~List() noexcept;

  List& operator=(const List& other);
  List& operator=(List&& other) noexcept(noexcept(MoveFromOther(std::move(other))));

  size_t size() const;

  const T& front() const;
  const T& back() const;

  T& front();
  T& back();

  void clear() noexcept;
  bool empty() const;

  template <typename... Args_t>
  void emplace_back(Args_t&& ...args);
  template <typename... Args_t>
  void emplace_front(Args_t&& ...args);

  void push_back(const T& value);
  void push_back(T&& value);

  void push_front(const T& value);
  void push_front(T&& value);

  void pop_front();
  void pop_back();

  void reverse();
  void unique();

  void Print() const;

 private:
  // To avoid copy-pasting code for const and not const versions, iterator is template class
  template <bool IsConst>
  class UnitedIterator;

  struct Node;
  struct NodeBase;

  void CopyFromOther(const List& other);
  void MoveFromOther(List&& other) noexcept(std::is_nothrow_move_assignable_v<NodeAllocator>);
  template <typename... Args_t>
  void InsertToEmpty(Args_t&& ...args);
  void CopyPartOfCommonSize(NodeBase*& this_pointer, NodeBase*& other_pointer, const List<T, Allocator>& other);
  void CopyNewNodes(NodeBase*& this_pointer, NodeBase*& other_pointer, const List<T, Allocator>& other);
  void DeleteRedundantNodes(NodeBase*& this_pointer, size_t count);

  static Node* AsNode(NodeBase* node_base); // node's method?

  void CheckStatus();

  using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using NodeAllocatorTraits = std::allocator_traits<NodeAllocator>;

  NodeBase end_;
  NodeAllocator alloc_;
  size_t length_ = 0;

 public:
  using const_iterator = UnitedIterator<true>;
  using iterator = UnitedIterator<false>;
  using reverse_iterator = std::reverse_iterator<UnitedIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<UnitedIterator<true>>;
  using difference_type = typename iterator::difference_type;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  template <bool IsConst, typename... Args_t>
  void emplace(UnitedIterator<IsConst> pos, Args_t&& ...args);

  template <bool IsConst>
  UnitedIterator<IsConst> erase(UnitedIterator<IsConst> pos);

  template <bool IsConst, bool IsConstOther>
  UnitedIterator<IsConst> erase(List::UnitedIterator<IsConst> first, List::UnitedIterator<IsConstOther> last);

  template <bool IsConst>
  UnitedIterator<IsConst> insert(List::UnitedIterator<IsConst> pos, const T& value);

  template <bool IsConst>
  UnitedIterator<IsConst> insert(List::UnitedIterator<IsConst> pos, T&& value);
};

template <typename T, typename Allocator>
struct List<T, Allocator>::NodeBase {
  NodeBase* next = this;
  NodeBase* prev = this;

  NodeBase();
};

template <typename T, typename Allocator>
struct List<T, Allocator>::Node : public NodeBase {
  T value;

  template <typename... Args_t>
  explicit Node(Args_t&& ...args);
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::UnitedIterator {
 public:
  explicit UnitedIterator(NodeBase* node);

  UnitedIterator operator++(int);
  UnitedIterator operator--(int);

  UnitedIterator& operator++();
  UnitedIterator& operator--();

  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using difference_type = int;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;

  reference operator*() const;
  pointer operator->() const;

  template <bool IsConstOther>
  bool operator==(const UnitedIterator<IsConstOther>& other);
  template <bool IsConstOther>
  bool operator!=(const UnitedIterator<IsConstOther>& other);

 private:
  NodeBase* current_node_;

  friend class List<T>;
};

#include "list.ipp"
