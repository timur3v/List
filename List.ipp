//
// Created by Timur Demchenko on 28.08.2020.
//

//
// CLASS NODE
//

template <typename T>
List<T>::Node::Node(const T& value) : value(value), next(nullptr), prev(nullptr) {}

template <typename T>
List<T>::Node::Node(T&& value) : value(std::move(value)), next(nullptr), prev(nullptr) {}

template <typename T>
template <typename... Args_t>
List<T>::Node::Node(Args_t&&... args) : value(std::forward<Args_t>(args)...), next(nullptr), prev(nullptr) {
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
List<T>::List(List&& other) noexcept = default;

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
List<T>& List<T>::operator=(List&& other) noexcept = default;

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

//  if (length_ == 0) {
//    first_ = std::make_unique<Node>(std::move(value));
//    last_ = first_.get();
//  } else {
//    last_->next = std::make_unique<Node>(std::move(value));
//    last_->next->prev = last_;
//    last_ = last_->next;
//  }
//
//  ++length_;
}

template <typename T>
void List<T>::CopyFromOther(const List<T>& other) {
  if (!other.empty()) {
    Node* other_pointer = other.first_.get();

    first_ = std::make_unique<Node>(other.first_->value);
    Node* this_pointer = first_.get();

    for (size_t i = 0; i < other.length_ - 1; ++i) {
      other_pointer = other_pointer->next.get();

      this_pointer->next = std::make_unique<Node>(other_pointer->value);
      this_pointer->next->prev = this_pointer;
      this_pointer = this_pointer->next.get();
    }

    last_ = this_pointer;
    length_ = other.length_;
  } else {
    clear();
  }
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
  return first_->value;
}

template <typename T>
T& List<T>::back() const {
  return last_->value;
}

template <typename T>
void List<T>::clear() {
  first_ = nullptr;
  last_ = nullptr;
  length_ = 0;
}

template <typename T>
template <typename... Args_t>
void List<T>::emplace_back(Args_t&& ... args) {
  if (empty()) {
    first_ = std::make_unique<Node>(std::forward<Args_t>(args)...);
    last_ = first_.get();
  } else {
    last_->next = std::make_unique<Node>(std::forward<Args_t>(args)...);
    last_->next->prev = last_;
    last_ = last_->next.get();
  }
  length_++;
}

template <typename T>
template <typename... Args_t>
void List<T>::emplace_front(Args_t&& ... args) {
  if (empty()) {
    first_ = std::make_unique<Node>(std::forward<Args_t>(args)...);
    last_ = first_.get();
  } else {
    std::unique_ptr<Node> new_first = std::make_unique<Node>(std::forward<Args_t>(args)...);
    first_->prev = new_first.get();
    new_first->next = std::move(first_);
    first_ = std::move(new_first);
  }
  length_++;
}

template <typename T>
void List<T>::pop_front() {
  if (length_ == 1) {
    clear();
  } else {
    first_ = std::move(first_->next);
    first_->prev = nullptr;
    length_--;
  }
}

template <typename T>
void List<T>::pop_back() {
  if (length_ == 1) {
    clear();
  } else {
    last_ = last_->prev;
    last_->next = nullptr;
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
  current_node_ = current_node_->next.get();
  return *this;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>& List<T>::UnitedIterator<IsConst>::operator--() {
  current_node_ = current_node_->prev;
  return *this;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::UnitedIterator<IsConst>::operator++(int) {
  auto copy = *this;
  current_node_ = current_node_->next.get();
  return copy;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::UnitedIterator<IsConst>::operator--(int) {
  auto copy = *this;
  current_node_ = current_node_->prev;
  return copy;
}

template <typename T>
template <bool IsConst>
List<T>::UnitedIterator<IsConst>::UnitedIterator(Node* node) : current_node_(node) {}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>::reference List<T>::UnitedIterator<IsConst>::operator*() const {
  return current_node_->value;
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst>::pointer List<T>::UnitedIterator<IsConst>::operator->() const {
  return &current_node_->value;
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
  auto it = UnitedIterator<false>(first_.get());
  return it;
}

template <typename T>
typename List<T>::iterator List<T>::end() const {
  auto it = UnitedIterator<false>(nullptr);
  return it;
}

template <typename T>
typename List<T>::const_iterator List<T>::cbegin() const {
  auto it = UnitedIterator<true>(first_.get());
  return it;
}

template <typename T>
typename List<T>::const_iterator List<T>::cend() const {
  auto it = UnitedIterator<true>(nullptr);
  return it;
}

template <typename T>
template <bool IsConst, typename... Args_t>
void List<T>::emplace(List::UnitedIterator<IsConst> pos, Args_t&& ... args) {
  if (pos == begin()) {
    emplace_front(std::forward<Args_t>(args)...);
  } else if (pos == end()) {
    emplace_back(std::forward<Args_t>(args)...);
  } else {
    auto new_node = std::make_unique<Node>(std::forward<Args_t>(args)...);
    new_node->next = std::move(pos.current_node_->prev->next);
    new_node->prev = pos.current_node_->prev;
    Node* new_node_ptr = new_node.get();
    pos.current_node_->prev->next = std::move(new_node);
    pos.current_node_->prev = new_node_ptr;
    length_++;
  }
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::insert(List::UnitedIterator<IsConst> pos, const T& value) {
  emplace(pos, value);

  if (!pos.current_node_) {
    return List::UnitedIterator<IsConst>(last_);
  }
  return std::prev(pos);
}

template <typename T>
template <bool IsConst>
typename List<T>::template UnitedIterator<IsConst> List<T>::insert(List::UnitedIterator<IsConst> pos, T&& value) {
  emplace(pos, std::move(value));

  if (!pos.current_node_) {
    return List::UnitedIterator<IsConst>(last_);
  }
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
    pos.current_node_->next->prev = pos.current_node_->prev;
    pos.current_node_->prev->next = std::move(pos.current_node_->next);
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
void List<T>::reverse() {
  if (length_ <= 1) {
    return;
  }

  auto new_first = std::move(last_->prev->next);

  Node* current_pos = last_;
  Node* prev_current_pos = nullptr;
  while (current_pos->prev->prev) {
    Node* next_current_pos = current_pos->prev;

    Node* prev_prev = current_pos->prev->prev;
    current_pos->next = std::move(prev_prev->next);
    current_pos->prev = prev_current_pos;

    prev_current_pos = current_pos;
    current_pos = next_current_pos;
  }

  // now current_pos->prev == first_
  first_->prev = current_pos;
  first_->next = nullptr;
  current_pos->next = std::move(first_);
  current_pos->prev = prev_current_pos;

  first_ = std::move(new_first);
  last_ = current_pos->next.get();
}

template <typename T>
void List<T>::unique() {
  Node* first_in_equal_group = first_.get();
  Node* first_not_equal = first_.get();

  while (true) {
    int to_skip = -1;
    while (first_not_equal && first_not_equal->value == first_in_equal_group->value) {
      to_skip++;
      first_not_equal = first_not_equal->next.get();
    }

    length_ -= to_skip;

    if (first_not_equal == nullptr) {
      first_in_equal_group->next = nullptr;
      last_ = first_in_equal_group;
      break;
    } else {
      first_in_equal_group->next = std::move(first_not_equal->prev->next);
      first_not_equal->prev = first_in_equal_group;
    }

    first_in_equal_group = first_not_equal;
  }
}
