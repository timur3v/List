//
// Created by Timur Demchenko on 28.08.2020.
//

#pragma once

#include <memory>

// !!!NOTE!!!
// This version seems to work fine on small-sized lists, but on big ones it fails with stack overflow,
// presumably due to extremely big chain of unique_ptr destructor calls.
// Also some features are still not implemented

//
// DECLARATIONS
//

template <typename T>
class List {
 public:
  using value_type = T;

  List();
  List(const List& other);
  List(List&& other) noexcept;
  List(size_t count, const T& value);
  explicit List(size_t count);

  ~List();

  List& operator=(const List& other);
  List& operator=(List&& other) noexcept;

  size_t size() const;

  T& front() const;
  T& back() const;

  void clear();
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

 private:
  // To avoid copying, iterator is template class
  template <bool IsConst>
  class UnitedIterator;

  struct Node;
  struct NodeBase;

  void CopyFromOther(const List& other);
  void MoveFromOther(List& other);
  template <typename... Args_t>
  void InsertToEmpty(Args_t&& ...args);

  static Node* AsNode(NodeBase* node_base);

  void PrintStatus() const; // DEBUG

  // List is double-linked, owning of objects is performed as "previous owns next" and first_ owns first node, which is
  // not containing a real value, needed for technical reasons (to differ begin and end iterators)
  // std::unique_ptr<Node> first_ = nullptr;
  // Node* last_ = nullptr;

  std::unique_ptr<NodeBase> end_ = std::make_unique<NodeBase>();
  size_t length_ = 0;

 public:
  using const_iterator = UnitedIterator<true>;
  using iterator = UnitedIterator<false>;
  using reverse_iterator = std::reverse_iterator<UnitedIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<UnitedIterator<true>>;

  iterator begin() const;
  iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

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

  // reverse iterator???
};

template <typename T>
struct List<T>::NodeBase {
  NodeBase* next = this;
  std::unique_ptr<Node> prev = nullptr;

  NodeBase();
};

template <typename T>
struct List<T>::Node : public NodeBase {
  T value;

  explicit Node(const T& value); // WHY DO I NEED THAT???
  explicit Node(T&& value);
  template <typename... Args_t>
  explicit Node(Args_t&& ... args);
};

template <typename T>
template <bool IsConst>
class List<T>::UnitedIterator {
 public:
  UnitedIterator(NodeBase* node);

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

  friend class List<T>; ///////FUFUFUFUFUFFFFFUFU
};

#include "List.ipp"
