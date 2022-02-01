//
// Created by Timur Demchenko (https://github.com/timur3v) on 28.08.2020.
//

// This is a .ipp file for list.hpp. For more information check list.hpp.

//
// LIST CONSTRUCTORS
//

/**
 * Default constructor
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::NodeBase::NodeBase() {}

/**
 * Constructor, that constructs value in node from args.
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <typename... Args_t>
List<T, Allocator>::Node::Node(Args_t&& ...args) : value(std::forward<Args_t>(args)...) {
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::List() : alloc_(NodeAllocator()) {}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) : alloc_(NodeAllocator(alloc)) {
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) : alloc_(other.alloc_) {
  CopyFromOther(other);
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::List(List&& other) noexcept(noexcept(MoveFromOther(std::move(other)))
    && std::is_nothrow_move_constructible_v<NodeAllocator>) : alloc_(std::move(other.alloc_)) {
  MoveFromOther(std::move(other));
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc) : alloc_(NodeAllocator(alloc)) {}

//
// LIST ASSIGNMENT OPERATORS
//

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (&other == this) {
    return *this;
  }
  CopyFromOther(other);

  return *this;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept(noexcept(MoveFromOther(std::move(other)))) {
  if (&other == this) {
    return *this;
  }
  MoveFromOther(std::move(other));

  return *this;
}

//
// LIST DESTRUCTOR
//

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
List<T, Allocator>::~List() noexcept {
  clear();
}

//
// LIST FUNCTIONS
//

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  emplace_back(value);
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  emplace_back(std::move(value));
}

/**
 * Makes this a copy of other. May throw exceptions, for example, when allocating memory. Values of type T are copied
 * here.
 *
 * @tparam T Type of elements in container
 * @tparam Allocator Allocator type
 * @param other List to be copied from
*/
template <typename T, typename Allocator>
void List<T, Allocator>::CopyFromOther(const List<T, Allocator>& other) {
  NodeBase* other_pointer = other.end_.prev;
  NodeBase* this_pointer = end_.prev;

  CopyPartOfCommonSize(this_pointer, other_pointer, other);

  if (other.length_ >= length_) {
    CopyNewNodes(this_pointer, other_pointer, other);
  } else {
    end_.next = this_pointer;
    DeleteRedundantNodes(this_pointer, length_ - other.length_);
  }

  length_ = other.length_;
}

/**
 * Copies part of other list to this. Starts from the end of other, copies min(other.length_, length_) elements.
 * Changes this_pointer and other_pointer, at the end they point at the last copied nodes.
 *
 * @tparam T Type of elements in container
 * @tparam Allocator Allocator type
 * @param this_pointer Pointer to position in this class
 * @param other_pointer Pointer to position in other class
 * @param other List to be copied from
 */
template <typename T, typename Allocator>
void List<T, Allocator>::CopyPartOfCommonSize(NodeBase*& this_pointer,
                                              NodeBase*& other_pointer,
                                              const List<T, Allocator>& other) {
  for (size_t i = 0; i < std::min(other.length_, length_); ++i) {
    AsNode(this_pointer)->value = AsNode(other_pointer)->value;

    other_pointer = other_pointer->prev;
    this_pointer = this_pointer->prev;
  }
}

/**
 * Creates other.length - length nodes, which are copies of first other.length - length nodes of other.
 * Initially this_pointer and other_pointer must be pointers to nodes, where to start copying. At the end they point
 * to last copied nodes.
 *
 * @tparam T Type of elements in container
 * @tparam Allocator Allocator type
 * @param this_pointer Pointer to position in this class
 * @param other_pointer Pointer to position in other class
 * @param other List to be copied from
 */
template <typename T, typename Allocator>
void List<T, Allocator>::CopyNewNodes(NodeBase*& this_pointer,
                                      NodeBase*& other_pointer,
                                      const List<T, Allocator>& other) {
  size_t old_this_length = length_;
  for (size_t i = 0; i < other.length_ - old_this_length; ++i) {
    push_front(AsNode(other_pointer)->value);

    other_pointer = other_pointer->prev;
    this_pointer = this_pointer->prev;
  }
}

/**
 * Deletes count nodes, starting from this_pointer->prev and going to the "prev" side.
 *
 * @tparam T Type of elements in container
 * @tparam Allocator Allocator type
 * @param this_pointer Pointer to position in this class
 * @param count Number of elements to be deleted
 */
template <typename T, typename Allocator>
void List<T, Allocator>::DeleteRedundantNodes(NodeBase*& this_pointer, size_t count) {
  this_pointer = this_pointer->prev;
  for (size_t i = 0; i < count; ++i) {
    NodeBase* this_pointer_prev = this_pointer->prev;

    NodeAllocatorTraits::destroy(alloc_, AsNode(this_pointer));
    NodeAllocatorTraits::deallocate(alloc_, AsNode(this_pointer), 1);

    this_pointer = this_pointer_prev;
  }
}

/**
 * Moves other into this. After usage other is an empty working list, if allocator is fine after moving. Otherwise,
 * other is not valid.
 *
 * @tparam T Type of elements in container
 * @tparam Allocator Allocator type
 * @param other List to be moved from
 */
template <typename T, typename Allocator>
void List<T, Allocator>::MoveFromOther(List&& other) noexcept(std::is_nothrow_move_assignable_v<NodeAllocator>) {
  if (&other == this) {
    return;
  }

  clear();

  if (!other.empty()) {
    alloc_ = std::move(other.alloc_);

    other.end_.next->prev = &end_;
    other.end_.prev->next = &end_;
    end_ = other.end_;
    length_ = other.length_;

    other.end_.next = &other.end_;
    other.end_.prev = &other.end_;
    other.length_ = 0;
  }
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
bool List<T, Allocator>::empty() const noexcept {
  return length_ == 0;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const noexcept {
  return length_;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
const T& List<T, Allocator>::front() const noexcept {
  return AsNode(end_.next)->value;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
const T& List<T, Allocator>::back() const noexcept {
  return AsNode(end_.prev)->value;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
T& List<T, Allocator>::front() noexcept {
  return AsNode(end_.next)->value;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
T& List<T, Allocator>::back() noexcept {
  return AsNode(end_.prev)->value;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::clear() noexcept {
  Node* current_node = AsNode(end_.next);
  Node* next_node = current_node;
  for (int i = 0; i < length_; ++i) {
    next_node = AsNode(current_node->next);
    NodeAllocatorTraits::destroy(alloc_, current_node);
    NodeAllocatorTraits::deallocate(alloc_, current_node, 1);
    current_node = next_node;
  }
  end_.prev = &end_;
  end_.next = &end_;
  length_ = 0;
}

/**
 * Inserts node, constructed from args, to an empty list. If list is not empty, behaviour is undefined.
 */
template <typename T, typename Allocator>
template <typename... Args_t>
void List<T, Allocator>::InsertToEmpty(Args_t&& ...args) {
  end_.prev = NodeAllocatorTraits::allocate(alloc_, 1);
  end_.next = end_.prev;

  NodeAllocatorTraits::construct(alloc_, AsNode(end_.prev), std::forward<Args_t>(args)...);

  end_.prev->next = &end_;
  end_.prev->prev = &end_;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <typename... Args_t>
void List<T, Allocator>::emplace_back(Args_t&& ...args) {
  if (empty()) {
    InsertToEmpty(std::forward<Args_t>(args)...);
  } else {
    NodeBase* old_last = end_.prev;
    old_last->next = NodeAllocatorTraits::allocate(alloc_, 1);
    NodeAllocatorTraits::construct(alloc_, AsNode(old_last->next), std::forward<Args_t>(args)...);
    old_last->next->prev = old_last;
    old_last->next->next = &end_;
    end_.prev = old_last->next;
  }
  length_++;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <typename... Args_t>
void List<T, Allocator>::emplace_front(Args_t&& ...args) {
  if (empty()) {
    InsertToEmpty(std::forward<Args_t>(args)...);
  } else {
    NodeBase* old_first = end_.next;
    old_first->prev = NodeAllocatorTraits::allocate(alloc_, 1);
    NodeAllocatorTraits::construct(alloc_, AsNode(old_first->prev), std::forward<Args_t>(args)...);
    old_first->prev->next = old_first;
    old_first->prev->prev = &end_;
    end_.next = old_first->prev;
  }
  length_++;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() noexcept {
  if (length_ == 1) {
    clear();
  } else {
    NodeBase* new_first = end_.next->next;
    NodeAllocatorTraits::destroy(alloc_, AsNode(end_.next));
    NodeAllocatorTraits::deallocate(alloc_, AsNode(end_.next), 1);
    end_.next = new_first;
    new_first->prev = &end_;
    length_--;
  }
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() noexcept {
  if (length_ == 1) {
    clear();
  } else {
    NodeBase* new_last = end_.prev->prev;
    NodeAllocatorTraits::destroy(alloc_, end_.prev);
    NodeAllocatorTraits::deallocate(alloc_, AsNode(end_.prev), 1);
    end_.prev = new_last;
    new_last->next = &end_;
    length_--;
  }
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  emplace_front(value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  emplace_front(std::move(value));
}

//
// ITERATOR CLASS
//

/**
 * Increments iterator. Returns reference to itself after incrementing.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst>& List<T,
                                                                    Allocator>::UnitedIterator<IsConst>::operator++() {
  current_node_ = current_node_->next;
  return *this;
}

/**
 * Decrements iterator. Returns reference to itself after decrementing.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst>& List<T,
                                                                    Allocator>::UnitedIterator<IsConst>::operator--() {
  current_node_ = current_node_->prev;
  return *this;
}

/**
 * Increments iterator. Returns copy of itself before incrementing.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T, Allocator>::UnitedIterator<IsConst>::operator++(
    int) {
  auto copy = *this;
  current_node_ = current_node_->next;
  return copy;
}

/**
 * Decrements iterator. Returns copy of itself before decrementing.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T, Allocator>::UnitedIterator<IsConst>::operator--(
    int) {
  auto copy = *this;
  current_node_ = current_node_->prev;
  return copy;
}

/**
 * Constructs iterator from node.
 */
template <typename T, typename Allocator>
template <bool IsConst>
List<T, Allocator>::UnitedIterator<IsConst>::UnitedIterator(NodeBase* node) : current_node_(node) {}

/**
 * Returns reference to a value in iterator. Reference is const when iterator is const, and not const otherwise.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst>::reference List<T,
                                                                              Allocator>::UnitedIterator<IsConst>::operator*() const {
  return AsNode(current_node_)->value;
}

/**
 * Returns pointer to a value in iterator. Pointer is const when iterator is const, and not const otherwise.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst>::pointer List<T,
                                                                            Allocator>::UnitedIterator<IsConst>::operator->() const {
  return &AsNode(current_node_)->value;
}

/**
 * Returns true if iterators point at the same node, false otherwise.
 */
template <typename T, typename Allocator>
template <bool IsConst>
template <bool IsConstOther>
bool List<T, Allocator>::UnitedIterator<IsConst>::operator==(const UnitedIterator<IsConstOther>& other) {
  return current_node_ == other.current_node_;
}

/**
 * Returns false if iterators point at the same node, true otherwise.
 */
template <typename T, typename Allocator>
template <bool IsConst>
template <bool IsConstOther>
bool List<T, Allocator>::UnitedIterator<IsConst>::operator!=(const UnitedIterator<IsConstOther>& other) {
  return current_node_ != other.current_node_;
}

//
// LIST ITERATORS
//

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
  auto it = UnitedIterator<false>(end_.next);
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
  auto it = UnitedIterator<false>(&end_);
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
  auto it = UnitedIterator<true>(end_.next);
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
  auto it = UnitedIterator<true>(const_cast<NodeBase*>(&end_));
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
  return cbegin();
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
  return cend();
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
  auto it = reverse_iterator(end());
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() {
  auto it = reverse_iterator(begin());
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const {
  auto it = const_reverse_iterator(cend());
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const {
  auto it = const_reverse_iterator(cbegin());
  return it;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const {
  return crbegin();
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const {
  return crend();
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <bool IsConst, typename... Args_t>
void List<T, Allocator>::emplace(List::UnitedIterator<IsConst> pos, Args_t&& ...args) {
  if (pos == begin()) {
    emplace_front(std::forward<Args_t>(args)...);
  } else if (pos == end()) {
    emplace_back(std::forward<Args_t>(args)...);
  } else {
    Node* new_node = NodeAllocatorTraits::allocate(alloc_, 1);
    NodeAllocatorTraits::construct(alloc_, new_node, std::forward<Args_t>(args)...);
    new_node->next = pos.current_node_;
    new_node->prev = pos.current_node_->prev;
    new_node->prev->next = new_node;
    pos.current_node_->prev = new_node;
    length_++;
  }
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T,
                                                                   Allocator>::insert(List::UnitedIterator<IsConst> pos,
                                                                                      const T& value) {
  emplace(pos, value);
  return std::prev(pos);
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T,
                                                                   Allocator>::insert(List::UnitedIterator<IsConst> pos,
                                                                                      T&& value) {
  emplace(pos, std::move(value));
  return std::prev(pos);
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T,
                                                                   Allocator>::erase(List::UnitedIterator<IsConst> pos) {
  auto next_pos = std::next(pos);
  if (pos == begin()) {
    pop_front();
  } else if (next_pos == end()) {
    pop_back();
  } else {
    pos.current_node_->prev->next = pos.current_node_->next;
    pos.current_node_->next->prev = pos.current_node_->prev;
    NodeAllocatorTraits::destroy(alloc_, AsNode(pos.current_node_));
    NodeAllocatorTraits::deallocate(alloc_, AsNode(pos.current_node_), 1);
    length_--;
  }
  return next_pos;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
template <bool IsConst, bool IsConstOther>
typename List<T, Allocator>::template UnitedIterator<IsConst> List<T,
                                                                   Allocator>::erase(List::UnitedIterator<IsConst> first,
                                                                                     List::UnitedIterator<IsConstOther> last) {
  for (auto it = first; it != last;) {
    it = erase(it);
  }
  return last;
}

//
// OTHER LIST FUNCTIONS
//

/**
 * Prints length and values in order they are in list.
 * For this function to work properly std::cout should be able to print value of type T.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::Print() const {
  std::cout << "length: " << length_ << "\n";
  std::cout << "values:\n";

  int counter = 0;
  NodeBase* current_node = end_.next;
  const NodeBase* prev_node = &end_;
  while (current_node != &end_) {
    counter++;
    std::cout << AsNode(current_node)->value << " ";

    prev_node = current_node;
    current_node = current_node->next;
  }
  std::cout << std::endl;
}

/**
 * Checks if list is not "broken".
 */
template <typename T, typename Allocator>
void List<T, Allocator>::CheckStatus() {
  int counter = 0;
  NodeBase* current_node = end_.next;
  const NodeBase* prev_node = &end_;
  while (current_node != &end_) {
    assert(current_node->prev == prev_node);
    counter++;
    AsNode(current_node)->value;

    prev_node = current_node;
    current_node = current_node->next;
  }
  assert(end_.prev == prev_node);
  assert(counter == length_);
}

/**
 * Unsafe cast from base class pointer NodeBase* to derived Node*.
 */
template <typename T, typename Allocator>
typename List<T, Allocator>::Node* List<T, Allocator>::AsNode(NodeBase* node_base) {
  return static_cast<Node*>(node_base);
}

template <typename T, typename Allocator>
void List<T, Allocator>::reverse() {
  NodeBase* new_last = end_.next;

  NodeBase* current_pos = end_.next;
  NodeBase* prev_current_pos = &end_;
  while (current_pos->next != &end_) {
    NodeBase* next_current_pos = current_pos->next;

    current_pos->prev = current_pos->next;
    current_pos->next = prev_current_pos;

    prev_current_pos = current_pos;
    current_pos = next_current_pos;
  }

  // now current_pos->next == &end_
  current_pos->next = prev_current_pos;
  current_pos->prev = &end_;
  end_.prev = new_last;
  end_.next = current_pos;
}

/**
 * Reference C++17 standard at https://en.cppreference.com/w/cpp/container/list.
 */
template <typename T, typename Allocator>
void List<T, Allocator>::unique() {
  // goes from back to front, leaving only closest to back element in a group of adjacent equal elements
  if (empty()) {
    return;
  }

  NodeBase* last_in_equal_group = end_.prev;
  NodeBase* first_not_equal = last_in_equal_group->prev;

  while (true) {
    size_t to_skip = 0;
    while (first_not_equal != &end_ && AsNode(first_not_equal)->value == AsNode(last_in_equal_group)->value) {
      ++to_skip;
      first_not_equal = first_not_equal->prev;
      NodeAllocatorTraits::destroy(alloc_, AsNode(first_not_equal->next));
      NodeAllocatorTraits::deallocate(alloc_, AsNode(first_not_equal->next), 1);
    }

    assert(length_ >= to_skip);
    length_ -= to_skip;

    if (first_not_equal == &end_) {
      last_in_equal_group->prev = &end_;
      end_.next = last_in_equal_group;
      break;
    } else {
      last_in_equal_group->prev = first_not_equal;
      first_not_equal->next = last_in_equal_group;
    }

    last_in_equal_group = first_not_equal;
    first_not_equal = first_not_equal->prev;
  }
}
