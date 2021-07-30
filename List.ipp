//
// Created by Timur Demchenko on 28.08.2020.
//

//
// CLASS NODE
//

#include "List.h"

template <typename T>
List<T>::NodeBase::NodeBase() = default;

template <typename T>
List<T>::Node::Node(const T& value) : value(value) {}

template <typename T>
List<T>::Node::Node(T&& value) : value(std::move(value)) {}

template <typename T>
template <typename... Args_t>
List<T>::Node::Node(Args_t&&... args) : value(std::forward<Args_t>(args)...) {
}

//
// LIST CONSTRUCTORS
//

template <typename T>
List<T>::List() {}

template <typename T>
List<T>::List(size_t count, const T& value) {
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

template <typename T>
List<T>::List(size_t count) {
  for (size_t i = 0; i < count; ++i) {
    push_back(T());
  }
}

template <typename T>
List<T>::List(const List& other) {
  CopyFromOther(other);
}

template <typename T>
List<T>::List(List&& other) noexcept {
  MoveFromOther(other);
}

//
// LIST ASSIGNMENT OPERATORS
//

template <typename T>
List<T>& List<T>::operator=(const List& other) {
  if (&other == this) {
    return *this;
  }
  CopyFromOther(other);

  return *this;
}

template <typename T>
List<T>& List<T>::operator=(List&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  MoveFromOther(other);

  return *this;
}

//
// LIST DESTRUCTOR
//

template <typename T>
List<T>::~List() = default;

//
// LIST FUNCTIONS
//

template <typename T>
void List<T>::push_back(const T& value) {
  emplace_back(value);
}

template <typename T>
void List<T>::push_back(T&& value) {
  emplace_back(std::move(value));
}

template <typename T>
void List<T>::CopyFromOther(const List<T>& other) {
  if (!other.empty()) {
    Node* other_pointer = other.end_->prev.get();

    end_->prev = std::make_unique<Node>(other.end_->prev->value);
    end_->prev->next = end_.get();
    Node* this_pointer = end_->prev.get();

    for (size_t i = 0; i < other.length_ - 1; ++i) {
      other_pointer = other_pointer->prev.get();

      this_pointer->prev = std::make_unique<Node>(other_pointer->value);
      this_pointer->prev->next = this_pointer;
      this_pointer = this_pointer->prev.get();
    }

    end_->next = this_pointer;
    length_ = other.length_;
  } else {
    clear();
  }
}

template <typename T>
void List<T>::MoveFromOther(List& other) { // &&
  end_ = std::move(other.end_);
  length_ = other.length_;

  other.end_ = std::make_unique<NodeBase>();
  other.length_ = 0;
}

template <typename T>
bool List<T>::empty() const {
  return length_ == 0;
}

template <typename T>
size_t List<T>::size() const {
  return length_;
}

template <typename T>
T& List<T>::front() const {
  return AsNode(end_->next)->value;
}

template <typename T>
T& List<T>::back() const {
  return end_->prev->value;
}

template <typename T>
void List<T>::clear() {
  end_->prev = nullptr;
  end_->next = end_.get(); // tricky, remove last only by clear
  length_ = 0;
}

template <typename T>
template <typename... Args_t>
void List<T>::InsertToEmpty(Args_t&& ...args) {
  end_->prev = std::make_unique<Node>(std::forward<Args_t>(args)...);
  end_->prev->next = end_.get();
  end_->next = end_->prev.get();
}

template <typename T>
template <typename... Args_t>
void List<T>::emplace_back(Args_t&& ... args) {
  if (empty()) {
    InsertToEmpty(std::forward<Args_t>(args)...);
  } else {
    auto old_last = std::move(end_->prev);
    end_->prev = std::make_unique<Node>(std::forward<Args_t>(args)...);
    end_->prev->prev = std::move(old_last);
    end_->prev->prev->next = end_->prev.get();
    end_->prev->next = end_.get();
  }
  length_++;
}

template <typename T>
template <typename... Args_t>
void List<T>::emplace_front(Args_t&& ... args) {
  if (empty()) {
    InsertToEmpty(std::forward<Args_t>(args)...);
  } else {
    end_->next->prev = std::make_unique<Node>(std::forward<Args_t>(args)...);
    end_->next->prev->next = end_->next;
    end_->next = end_->next->prev.get();
  }
  length_++;
}

template <typename T>
void List<T>::pop_front() {
  if (length_ == 1) {
    clear();
  } else {
    end_->next = end_->next->next;
    end_->next->prev = nullptr;
    length_--;
  }
}

template <typename T>
void List<T>::pop_back() {
  if (length_ == 1) {
    clear();
  } else {
    end_->prev = std::move(end_->prev->prev);
    end_->prev->next = end_.get();
    length_--;
  }
}

template <typename T>
void List<T>::push_front(const T& value) {
  emplace_front(value);
}

template <typename T>
void List<T>::push_front(T&& value) {
  emplace_front(std::move(value));
}

//
// ITERATOR CLASS
//

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>& List<T>::UnitedIterator<IsConst>::operator++() {
  current_node_ = current_node_->next;
  return *this;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>& List<T>::UnitedIterator<IsConst>::operator--() {
  current_node_ = current_node_->prev.get();
  return *this;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::UnitedIterator<IsConst>::operator++(int) {
  auto copy = *this;
  current_node_ = current_node_->next;
  return copy;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::UnitedIterator<IsConst>::operator--(int) {
  auto copy = *this;
  current_node_ = current_node_->prev.get();
  return copy;
}

template <typename T>
template <bool IsConst>
List<T>::UnitedIterator<IsConst>::UnitedIterator(NodeBase* node) : current_node_(node) {}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>::reference List<T>::UnitedIterator<IsConst>::operator*() const {
  return AsNode(current_node_)->value;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>::pointer List<T>::UnitedIterator<IsConst>::operator->() const {
  return &AsNode(current_node_)->value;
}

template <typename T>
template <bool IsConst>
template <bool IsConstOther>
bool List<T>::UnitedIterator<IsConst>::operator==(const UnitedIterator<IsConstOther>& other) {
  return current_node_ == other.current_node_;
}

template <typename T>
template <bool IsConst>
template <bool IsConstOther>
bool List<T>::UnitedIterator<IsConst>::operator!=(const UnitedIterator<IsConstOther>& other) {
  return !(*this == other);
}

//
// LIST ITERATORS
//

template <typename T>
typename List<T>::iterator List<T>::begin() const {
  auto it = UnitedIterator<false>(end_->next);
  return it;
}

template <typename T>
typename List<T>::iterator List<T>::end() const {
  auto it = UnitedIterator<false>(end_.get());
  return it;
}

template <typename T>
typename List<T>::const_iterator List<T>::cbegin() const {
  auto it = UnitedIterator<true>(end_->next);
  return it;
}

template <typename T>
typename List<T>::const_iterator List<T>::cend() const {
  auto it = UnitedIterator<true>(end_.get());
  return it;
}

template <typename T>
template <bool IsConst, typename... Args_t>
void List<T>::emplace(List::UnitedIterator<IsConst> pos, Args_t&& ... args) {
  // PrintStatus();

  if (pos == begin()) {
    emplace_front(std::forward<Args_t>(args)...);
  } else if (pos == end()) {
    emplace_back(std::forward<Args_t>(args)...);
  } else {
    auto new_node = std::make_unique<Node>(std::forward<Args_t>(args)...);
    new_node->next = pos.current_node_;
    new_node->prev = std::move(pos.current_node_->prev);
    new_node->prev->next = new_node.get();
    pos.current_node_->prev = std::move(new_node);
    length_++;
  }

  // PrintStatus();
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::insert(List::UnitedIterator<IsConst> pos, const T& value) {
  emplace(pos, value);
  return std::prev(pos);
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::insert(List::UnitedIterator<IsConst> pos, T&& value) {
  emplace(pos, std::move(value));
  return std::prev(pos);
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::erase(List::UnitedIterator<IsConst> pos) {
  auto next_pos = std::next(pos);
  if (pos == begin()) {
    pop_front();
  } else if (next_pos == end()) {
    pop_back();
  } else {
    pos.current_node_->prev->next = pos.current_node_->next;
    pos.current_node_->next->prev = std::move(pos.current_node_->prev);
    length_--;
  }
  return next_pos;
}

template <typename T>
template <bool IsConst, bool IsConstOther>
typename List<T>::template UnitedIterator<IsConst> List<T>::erase(List::UnitedIterator<IsConst> first, List::UnitedIterator<IsConstOther> last) {
  for (auto it = first; it != last; ) { // Not really optimal, but who cares
    it = erase(it);
  }
  return last;
}

//
// OTHER LIST FUNCTIONS
//

template <typename T>
void List<T>::PrintStatus() const {
  std::cout << "length: " << length_ << "\n";
  std::cout << "values:\n";

  int counter = 0;
  NodeBase* current_node = end_->next;
  NodeBase* prev_node = nullptr;
  while (current_node != end_.get()) {
    assert(current_node->prev.get() == prev_node);
    counter++;
    std::cout << AsNode(current_node)->value << " ";

    prev_node = current_node;
    current_node = current_node->next;
  }
  assert(end_->prev.get() == prev_node);
  assert(counter == length_);
  std::cout << std::endl;
}

template <typename T>
typename List<T>::Node* List<T>::AsNode(NodeBase* node_base) {
  return static_cast<Node*>(node_base);
}

template <typename T>
void List<T>::reverse() {
  if (length_ <= 1) {
    return;
  }

  // PrintStatus();

  auto new_last = std::move(end_->next->next->prev);

  NodeBase* current_pos = end_->next;
  NodeBase* prev_current_pos = end_.get();
  while (current_pos->next != end_.get()) {
    NodeBase* next_current_pos = current_pos->next;

    current_pos->prev = std::move(current_pos->next->next->prev);
    current_pos->next = prev_current_pos;

    prev_current_pos = current_pos;
    current_pos = next_current_pos;
  }

  // now current_pos->next == end_
  current_pos->next = prev_current_pos;
  current_pos->prev = nullptr;
  end_->prev = std::move(new_last);
  end_->next = current_pos;

  // PrintStatus();
}

template <typename T>
void List<T>::unique() {
  if (empty()) {
    return;
  }

  Node* last_in_equal_group = end_->prev.get();
  Node* first_not_equal = last_in_equal_group;

  while (true) {
    int to_skip = -1;
    while (first_not_equal && first_not_equal->value == last_in_equal_group->value) {
      to_skip++;
      first_not_equal = first_not_equal->prev.get();
    }

    length_ -= to_skip;

    if (first_not_equal == nullptr) {
      last_in_equal_group->prev = nullptr;
      end_->next = last_in_equal_group;
      break;
    } else {
      last_in_equal_group->prev = std::move(first_not_equal->next->prev);
      first_not_equal->next = last_in_equal_group;
    }

    last_in_equal_group = first_not_equal;
  }
  // PrintStatus();
}
