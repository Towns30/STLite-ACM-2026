/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <cmath>
#include <cstddef>
#include <functional>

#include "exceptions.hpp"
#include "utility.hpp"

namespace sjtu
{

template <class Key, class T, class Compare = std::less<Key> >
class map
{
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  struct AVLNode
  {
    AVLNode *father_;
    AVLNode *left_;
    AVLNode *right_;
    value_type *data_;
    int height_;
    int size_;

    bool is_header_;

    AVLNode()
        : father_(nullptr),
          left_(nullptr),
          right_(nullptr),
          data_(nullptr),
          height_(1),
          size_(1),
          is_header_(false)
    {
    }
    AVLNode(AVLNode *father, AVLNode *left, AVLNode *right, value_type *data,
            int height = 1, int size = 1, bool is_header = false)
        : father_(father),
          left_(left),
          right_(right),
          data_(data),
          height_(height),
          size_(size),
          is_header_(is_header)
    {
    }
    AVLNode(const AVLNode &other)
        : father_(other.father_),
          left_(other.left_),
          right_(other.right_),
          data_(other.data_),
          height_(other.height_),
          size_(other.size_),
          is_header_(other.is_header_)
    {
    }
    ~AVLNode()
    {
      if (data_ != nullptr)
      {
        delete data_;
      }
    }
  };
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator
  {
   private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */

   public:
    AVLNode *node_;
    iterator()
    {
      node_ = nullptr;
    }

    iterator(const iterator &other)
    {
      node_ = other.node_;
    }

    iterator(AVLNode *node)
    {
      node_ = node;
    }

    /**
     * TODO iter++
     */
    iterator operator++(int)
    {
      if (node_->size_ == -1)  // it was deleted, ++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()++ is invalid
      {
        throw invalid_iterator();
      }
      iterator tmp = *this;
      if (node_->right_ ==
          nullptr)  // it doesn't have a right child, so we need to go up to
                    // find the node who is its father's left child.
                    // Particularly, if the node is the max node, the while
                    // circulaltion ends with father_node == header, which is
                    // actually what we want as end();
      {
        AVLNode *father_node = node_->father_, *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->left_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        node_ = father_node;
      }
      else  // it has right tree, so we need to find the first node who doesn't
            // have a left child
      {
        AVLNode *new_node = node_->right_;
        while (new_node->left_)
        {
          new_node = new_node->left_;
        }
        node_ = new_node;
      }
      return tmp;
    }

    /**
     * TODO ++iter
     */
    iterator &operator++()
    {
      if (node_->size_ == -1)  // it was deleted, ++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->right_ ==
          nullptr)  // it doesn't have a right child, so we need to go up to
                    // find the node who is its father's left child.
                    // Particularly, if the node is the max node, the while
                    // circulaltion ends with father_node == header, which is
                    // actually what we want as end();
      {
        AVLNode *father_node = node_->father_, *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->left_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        node_ = father_node;
      }
      else  // it has right tree, so we need to find the first node who doesn't
            // have a left child
      {
        AVLNode *new_node = node_->right_;
        while (new_node->left_)
        {
          new_node = new_node->left_;
        }
        node_ = new_node;
      }
      return *this;
    }

    /**
     * TODO iter--
     */
    iterator operator--(int)
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      iterator tmp = *this;
      if (node_->is_header_)  // end()-- means the max node, which is the right
                              // child of the header
      {
        if (node_->right_ ==
            node_)  // header's right child is itself, which means the map is
                    // empty, --end() is invalid
        {
          throw invalid_iterator();
        }
        node_ = node_->right_;
        return tmp;
      }
      if (node_->left_ == nullptr)
      {
        AVLNode *father_node = node_->father_;
        AVLNode *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->right_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        if (father_node->is_header_)  // we can't find a smaller one until we
                                      // arrive the header, which means it is
                                      // running begin()--, a invalid operation
        {
          throw invalid_iterator();
        }
        node_ = father_node;
      }
      else
      {
        AVLNode *new_node = node_->left_;
        while (new_node->right_)
        {
          new_node = new_node->right_;
        }
        node_ = new_node;
      }
      return tmp;
    }

    /**
     * TODO --iter
     */
    iterator &operator--()
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()-- means the max node, which is the right
                              // child of the header
      {
        if (node_->right_ ==
            node_)  // header's right child is itself, which means the map is
                    // empty, --end() is invalid
        {
          throw invalid_iterator();
        }
        node_ = node_->right_;
        return *this;
      }
      if (node_->left_ == nullptr)
      {
        AVLNode *father_node = node_->father_;
        AVLNode *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->right_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        if (father_node->is_header_)  // we can't find a smaller one until we
                                      // arrive the header, which means it is
                                      // running begin()--, a invalid operation
        {
          throw invalid_iterator();
        }
        node_ = father_node;
      }
      else
      {
        AVLNode *new_node = node_->left_;
        while (new_node->right_)
        {
          new_node = new_node->right_;
        }
        node_ = new_node;
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    value_type &operator*() const
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return *(node_->data_);
    }

    bool operator==(const iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ == rhs.node_);
    }

    bool operator==(const const_iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ == rhs.node_);
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ != rhs.node_);
    }

    bool operator!=(const const_iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ != rhs.node_);
    }

    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    value_type *operator->() const noexcept
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_->data_);
    }
  };
  class const_iterator
  {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.

   public:
    const AVLNode *node_;
    const_iterator()
    {
      node_ = nullptr;
    }

    const_iterator(const const_iterator &other)
    {
      node_ = other.node_;
    }

    const_iterator(const iterator &other)
    {
      node_ = (const AVLNode *)(other.node_);
    }

    const_iterator(const AVLNode *node)
    {
      node_ = node;
    }

    const_iterator operator++(int)
    {
      if (node_->size_ == -1)  // it was deleted, ++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()++ is invalid
      {
        throw invalid_iterator();
      }
      const_iterator tmp = *this;
      if (node_->right_ ==
          nullptr)  // it doesn't have a right child, so we need to go up to
                    // find the node who is its father's left child.
                    // Particularly, if the node is the max node, the while
                    // circulaltion ends with father_node == header, which is
                    // actually what we want as end();
      {
        const AVLNode *father_node = node_->father_, *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->left_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        node_ = father_node;
      }
      else  // it has right tree, so we need to find the first node who doesn't
            // have a left child
      {
        const AVLNode *new_node = node_->right_;
        while (new_node->left_)
        {
          new_node = new_node->left_;
        }
        node_ = new_node;
      }
      return tmp;
    }

    const_iterator &operator++()
    {
      if (node_->size_ == -1)  // it was deleted, ++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()++ is invalid
      {
        throw invalid_iterator();
      }
      if (node_->right_ ==
          nullptr)  // it doesn't have a right child, so we need to go up to
                    // find the node who is its father's left child.
                    // Particularly, if the node is the max node, the while
                    // circulaltion ends with father_node == header, which is
                    // actually what we want as end();
      {
        const AVLNode *father_node = node_->father_, *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->left_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        node_ = father_node;
      }
      else  // it has right tree, so we need to find the first node who doesn't
            // have a left child
      {
        const AVLNode *new_node = node_->right_;
        while (new_node->left_)
        {
          new_node = new_node->left_;
        }
        node_ = new_node;
      }
      return *this;
    }

    const_iterator operator--(int)
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      const_iterator tmp = *this;
      if (node_->is_header_)  // end()-- means the max node, which is the right
                              // child of the header
      {
        if (node_->right_ ==
            node_)  // header's right child is itself, which means the map is
                    // empty, --end() is invalid
        {
          throw invalid_iterator();
        }
        node_ = node_->right_;
        return tmp;
      }
      if (node_->left_ == nullptr)
      {
        const AVLNode *father_node = node_->father_;
        const AVLNode *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->right_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        if (father_node->is_header_)  // we can't find a smaller one until we
                                      // arrive the header, which means it is
                                      // running begin()--, a invalid operation
        {
          throw invalid_iterator();
        }
        node_ = father_node;
      }
      else
      {
        const AVLNode *new_node = node_->left_;
        while (new_node->right_)
        {
          new_node = new_node->right_;
        }
        node_ = new_node;
      }
      return tmp;
    }

    const_iterator &operator--()
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      if (node_->is_header_)  // end()-- means the max node, which is the right
                              // child of the header
      {
        if (node_->right_ ==
            node_)  // header's right child is itself, which means the map is
                    // empty, --end() is invalid
        {
          throw invalid_iterator();
        }
        node_ = node_->right_;
        return *this;
      }
      if (node_->left_ == nullptr)
      {
        const AVLNode *father_node = node_->father_;
        const AVLNode *pre_node = node_;
        while (father_node->is_header_ == false &&
               father_node->right_ != pre_node)
        {
          pre_node = father_node;
          father_node = father_node->father_;
        }
        if (father_node->is_header_)  // we can't find a smaller one until we
                                      // arrive the header, which means it is
                                      // running begin()--, a invalid operation
        {
          throw invalid_iterator();
        }
        node_ = father_node;
      }
      else
      {
        const AVLNode *new_node = node_->left_;
        while (new_node->right_)
        {
          new_node = new_node->right_;
        }
        node_ = new_node;
      }
      return *this;
    }

    const value_type &operator*() const
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return *(node_->data_);
    }

    bool operator==(const iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ == rhs.node_);
    }

    bool operator==(const const_iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ == rhs.node_);
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ != rhs.node_);
    }

    bool operator!=(const const_iterator &rhs) const
    {
      if (node_->size_ == -1 ||
          rhs.node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return (node_ != rhs.node_);
    }

    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    const value_type *operator->() const noexcept
    {
      if (node_->size_ == -1)  // it was deleted, use is invalid
      {
        throw invalid_iterator();
      }
      return node_->data_;
    }
  };

  /**
   * TODO two constructors
   */
  map() : delete_list_(nullptr)
  {
    header_ = new AVLNode();
    resetHeader();
  }

  map(const map &other)
  {
    delete_list_ = nullptr;  // for deleted nodes, we have to make sure that
                             // only one map controls the deletion of this nodes
    // we first construct an empty map
    header_ = new AVLNode();
    resetHeader();
    if (other.header_->father_ == nullptr)  // other map is an empty map!
    {
      return;
    }

    // now other map is not an empty map
    header_->father_ =
        clone(other.header_->father_,
              header_);  // we clone other map's AVL tree to this map

    // update header_->left(min) and header_->right(max);
    updateHeader();
  }

  /**
   * TODO assignment operator
   */
  map &operator=(const map &other)
  {
    if (this == &other)
    {
      return *this;
    }
    clear();
    // we first construct an empty map
    resetHeader();
    if (other.header_->father_ == nullptr)  // other map is an empty map!
    {
      return *this;
    }

    // now other map is not an empty map
    header_->father_ =
        clone(other.header_->father_,
              header_);  // we clone other map's AVL tree to this map

    // update header_->left(min) and header_->right(max);
    updateHeader();
    return *this;
  }

  /**
   * TODO Destructors
   */
  ~map()
  {
    clear();
    delete header_;
    while (delete_list_ != nullptr)  // delete nodes which were already deleted
    {
      AVLNode *tmp = delete_list_->left_;
      delete delete_list_;
      delete_list_ = tmp;
    }
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent
   * to key. If no such element exists, an exception of type
   * `index_out_of_bound'
   */
  T &at(const Key &key)
  {
    AVLNode *tar_node = findNode(key);
    if (tar_node == header_)
    {
      throw index_out_of_bound();
    }
    return (tar_node->data_->second);
  }

  const T &at(const Key &key) const
  {
    AVLNode *tar_node = findNode(key);
    if (tar_node == header_)
    {
      throw index_out_of_bound();
    }
    return (tar_node->data_->second);
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key)
  {
    AVLNode *tar_node = findNode(key);
    if (tar_node == header_)
    {
      insert(sjtu::pair<Key, T>(key, T()));
      tar_node = findNode(key);
    }
    return tar_node->data_->second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const
  {
    AVLNode *tar_node = findNode(key);
    if (tar_node == header_)
    {
      throw index_out_of_bound();
    }
    return (tar_node->data_->second);
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin()
  {
    if (header_->father_ == nullptr)  // empty map
    {
      return iterator(header_);
    }
    return iterator(header_->left_);
  }

  const_iterator cbegin() const
  {
    if (header_->father_ == nullptr)  // empty map
    {
      return const_iterator(header_);
    }
    return const_iterator(header_->left_);
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end()
  {
    return iterator(header_);
  }

  const_iterator cend() const
  {
    return const_iterator(header_);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const
  {
    if (header_->father_ == nullptr)
    {
      return true;
    }
    return false;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const
  {
    if (header_->father_ == nullptr)
    {
      return 0;
    }
    return header_->father_->size_;
  }

  /**
   * clears the contents
   */
  void clear()
  {
    if (header_->father_ == nullptr)  // empty map
    {
      return;
    }
    throwTreeIntoList(header_->father_);
    // reset header_
    resetHeader();
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the
   * insertion), the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value)
  {
    int pre_size = size();
    bool flag = true;
    AVLNode *new_node = insertNode(value, header_->father_, header_, flag);
    int after_size = size();
    bool is_inserted = true;
    if (pre_size == after_size)
    {
      is_inserted = false;
    }
    updateHeader();
    return sjtu::pair<iterator, bool>(iterator(new_node), is_inserted);
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an
   * element out of this)
   */
  void erase(iterator pos)
  {
    if (pos.node_->size_ == -1 || pos.node_->is_header_)
    {
      throw invalid_iterator();
    }
    // check if pos is belong to this map
    AVLNode *node = pos.node_;
    while (node->is_header_ != true)
    {
      node = node->father_;
    }
    if (node != header_)
    {
      throw invalid_iterator();
    }
    AVLNode *earse_node = pos.node_;
    earseNode(earse_node);
    updateHeader();
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const
  {
    if (findNode(key) == header_)
    {
      return 0;
    }
    return 1;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is
   * returned.
   */
  iterator find(const Key &key)
  {
    return iterator(findNode(key));
  }

  const_iterator find(const Key &key) const
  {
    return const_iterator(findNode(key));
  }

 private:
  AVLNode *delete_list_;
  AVLNode
      *header_;  // we use header_ as a special structural node to be the
                 // role of end(), and the header_->left_ is the min
                 // node(begin()), the header_->right_ is the max node(end()
                 // - 1), the header_->father_ is the real root of the AVL
                 // tree also, to manage end()++ problem, we let
                 // root->father_ = header_
                 // by the way, if it is an empty map, the header_->father_ =
                 // nullptr, header_->left_ = header_, header_->right_ = header_
  Compare comp_;
  AVLNode *clone(AVLNode *node,
                 AVLNode *father_node)  // we need father_node to set the
                                        // new_node's father
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    value_type *new_data = new value_type(*(node->data_));
    AVLNode *new_node =
        new AVLNode(father_node, nullptr, nullptr, new_data, node->height_,
                    node->size_, node->is_header_);
    new_node->left_ = clone(node->left_, new_node);
    new_node->right_ = clone(node->right_, new_node);
    return new_node;
  }
  AVLNode *findNode(
      const Key &key) const  // return header_ if the key is not found
  {
    AVLNode *node = header_->father_;
    while (node != nullptr &&
           (comp_(node->data_->first, key) || comp_(key, node->data_->first)))
    {
      if (comp_(node->data_->first, key))
      {
        node = node->right_;
      }
      else
      {
        node = node->left_;
      }
    }
    if (node == nullptr)
    {
      return header_;
    }
    return node;
  }
  void throwTreeIntoList(AVLNode *node)
  {
    if (node == nullptr)
    {
      return;
    }
    throwTreeIntoList(node->left_);
    throwTreeIntoList(node->right_);
    AddNodeIntoList(node);
  }
  void AddNodeIntoList(AVLNode *node)
  {
    node->size_ = -1;
    node->right_ = nullptr;
    node->father_ = nullptr;
    if (delete_list_ == nullptr)  // delete_list_ is empty
    {
      delete_list_ = node;
      delete_list_->left_ = nullptr;
      return;
    }
    node->left_ = delete_list_;
    delete_list_ = node;
  }
  void updateHeader()  // update min and max in header
  {
    AVLNode *tar_node = header_->father_;
    if (tar_node == nullptr)  // empty map
    {
      resetHeader();
      return;
    }
    while (tar_node->left_)
    {
      tar_node = tar_node->left_;
    }
    header_->left_ = tar_node;
    tar_node = header_->father_;
    while (tar_node->right_)
    {
      tar_node = tar_node->right_;
    }
    header_->right_ = tar_node;
    header_->is_header_ = true;
  }
  void resetHeader()
  {
    header_->is_header_ = true;
    header_->father_ = nullptr;
    header_->left_ = header_;
    header_->right_ = header_;
  }
  void LL(AVLNode *&node, AVLNode *father_node)
  {
    AVLNode *new_root = node->left_;
    new_root->father_ = father_node;
    node->left_ = new_root->right_;
    new_root->right_ = node;
    node->father_ = new_root;
    if (node->left_ != nullptr)
    {
      node->left_->father_ = node;
    }
    node->size_ = treeSize(node);
    node->height_ = treeHeight(node);
    new_root->size_ = treeSize(new_root);
    new_root->height_ = treeHeight(new_root);
    node = new_root;
  }
  void RR(AVLNode *&node, AVLNode *father_node)
  {
    AVLNode *new_root = node->right_;
    new_root->father_ = father_node;
    node->right_ = new_root->left_;
    new_root->left_ = node;
    node->father_ = new_root;
    if (node->right_ != nullptr)
    {
      node->right_->father_ = node;
    }
    node->size_ = treeSize(node);
    node->height_ = treeHeight(node);
    new_root->size_ = treeSize(new_root);
    new_root->height_ = treeHeight(new_root);
    node = new_root;
  }
  void LR(AVLNode *&node, AVLNode *father_node)
  {
    RR(node->left_, node);
    LL(node, father_node);
  }
  void RL(AVLNode *&node, AVLNode *father_node)
  {
    LL(node->right_, node);
    RR(node, father_node);
  }
  int treeHeight(AVLNode *node)
  {
    if (node == nullptr)
    {
      return 0;
    }
    return std::max(((node->left_ != nullptr) ? node->left_->height_ : 0),
                    ((node->right_ != nullptr) ? node->right_->height_ : 0)) +
           1;
  }
  int treeSize(AVLNode *node)
  {
    if (node == nullptr)
    {
      return 0;
    }
    return ((node->left_ != nullptr) ? node->left_->size_ : 0) +
           ((node->right_ != nullptr) ? node->right_->size_ : 0) + 1;
  }
  AVLNode *insertNode(const value_type &value, AVLNode *&node,
                      AVLNode *father_node, bool &flag)
  {
    bool new_flag = true;  // true means still need adjust
    if (node == nullptr)   // insert into an empty tree
    {
      value_type *new_data = new value_type(value);
      node = new AVLNode(father_node, nullptr, nullptr, new_data);
      return node;
    }
    if (!comp_(node->data_->first, value.first) &&
        !comp_(value.first, node->data_->first))  // same key, which means it
                                                  // doesn't allow insert
    {
      flag = false;  // we don't need insert, so we also don't need adjust
      return node;
    }
    if (comp_(node->data_->first,
              value.first))  // insert into the right child tree
    {
      AVLNode *returned_node = insertNode(value, node->right_, node, new_flag);
      if (new_flag == false)  // upper nodes don't need adjust
      {
        node->size_ = treeSize(node);
        flag = false;
      }
      else
      {
        if (treeHeight(node->right_) - treeHeight(node->left_) ==
            2)  // need adjust, which means the upper nodes don't need adjust
        {
          if (comp_(node->right_->data_->first, value.first))
          {
            RR(node, node->father_);
          }
          else
          {
            RL(node, node->father_);
          }
          flag = false;
        }
        node->size_ = treeSize(node);
        node->height_ = treeHeight(node);
      }
      return returned_node;
    }
    else  // insert into the left child tree
    {
      AVLNode *returned_node = insertNode(value, node->left_, node, new_flag);
      if (new_flag == false)  // upper nodes don't need adjust
      {
        node->size_ = treeSize(node);
        flag = false;
      }
      else
      {
        if (treeHeight(node->left_) - treeHeight(node->right_) ==
            2)  // need adjust, which means the upper nodes don't need adjust
        {
          if (comp_(node->left_->data_->first, value.first))
          {
            LR(node, node->father_);
          }
          else
          {
            LL(node, node->father_);
          }
          flag = false;
        }
        node->size_ = treeSize(node);
        node->height_ = treeHeight(node);
      }
      return returned_node;
    }
  }
  void earseNode(AVLNode *earse_node)
  {
    if (earse_node->left_ == nullptr &&
        earse_node->right_ == nullptr)  // leaf node, delete immediately
    {
      int dir = 0;  // 0 stands for left, 1 stands for right
      AVLNode *father_node = earse_node->father_;
      if (father_node != header_)  // not root node
      {
        if (father_node->right_ == earse_node)
        {
          father_node->right_ = nullptr;
          dir = 1;
        }
        else
        {
          father_node->left_ = nullptr;
          dir = 0;
        }
        bool flag = false;  // to record whether we need to adjust upper nodes
        while (father_node->is_header_ == false)
        {
          if (flag == false)
          {
            AVLNode *old_root = father_node;  // record old root
            if (adjust(father_node,
                       dir))  // we don't need to adjust upper nodes
            {
              flag = true;
              // update father_node->father_
              if (old_root != father_node)  // spin happened
              {
                AVLNode *father_father_node = father_node->father_;
                if (father_father_node == header_)
                {
                  header_->father_ = father_node;
                }
                else if (father_father_node->left_ == old_root)
                {
                  father_father_node->left_ = father_node;
                }
                else
                {
                  father_father_node->right_ = father_node;
                }
              }
            }
            else  // update dir, we need to assure what child father_node is for
                  // its father node
            {
              // update father_node->father_
              if (old_root != father_node)  // spin happened
              {
                AVLNode *father_father_node = father_node->father_;
                if (father_father_node == header_)
                {
                  header_->father_ = father_node;
                }
                else if (father_father_node->left_ == old_root)
                {
                  father_father_node->left_ = father_node;
                }
                else
                {
                  father_father_node->right_ = father_node;
                }
              }
              if (father_node->father_ !=
                  header_)  // if father_node is root, we don't
                            // need to update as while circulation will end
              {
                if (father_node->father_->right_ == father_node)
                {
                  dir = 1;
                }
                else
                {
                  dir = 0;
                }
              }
            }
            father_node->height_ = treeHeight(
                father_node);  // only flag = false we need to update height
          }
          father_node->size_ = treeSize(father_node);
          father_node = father_node->father_;
        }
      }
      else  // root node, now the map is an empty map, we don't need adjust
      {
        resetHeader();
      }
      AddNodeIntoList(earse_node);
    }
    else if (earse_node->left_ != nullptr &&
             earse_node->right_ == nullptr)  // has left child
    {
      int dir = 0;  // 0 stands for left, 1 stands for right
      AVLNode *father_node = earse_node->father_;
      earse_node->left_->father_ = earse_node->father_;  // update left child
      // update father node
      if (earse_node->father_ != header_)  // not root node
      {
        if (earse_node->father_->right_ == earse_node)
        {
          earse_node->father_->right_ = earse_node->left_;
          dir = 1;
        }
        else
        {
          earse_node->father_->left_ = earse_node->left_;
          dir = 0;
        }
      }
      else  // root node
      {
        header_->father_ = earse_node->left_;
      }
      bool flag = false;  // to record whether we need to adjust upper nodes
      while (father_node->is_header_ == false)
      {
        if (flag == false)
        {
          AVLNode *old_root = father_node;  // record old root
          if (adjust(father_node,
                     dir))  // we don't need to adjust upper nodes
          {
            flag = true;
            // update father_node->father_
            if (old_root != father_node)  // spin happened
            {
              AVLNode *father_father_node = father_node->father_;
              if (father_father_node == header_)
              {
                header_->father_ = father_node;
              }
              else if (father_father_node->left_ == old_root)
              {
                father_father_node->left_ = father_node;
              }
              else
              {
                father_father_node->right_ = father_node;
              }
            }
          }
          else  // update dir, we need to assure what child father_node is for
                // its father node
          {
            // update father_node->father_
            if (old_root != father_node)  // spin happened
            {
              AVLNode *father_father_node = father_node->father_;
              if (father_father_node == header_)
              {
                header_->father_ = father_node;
              }
              else if (father_father_node->left_ == old_root)
              {
                father_father_node->left_ = father_node;
              }
              else
              {
                father_father_node->right_ = father_node;
              }
            }
            if (father_node->father_ !=
                header_)  // if father_node is root, we don't
                          // need to update as while circulation will end
            {
              if (father_node->father_->right_ == father_node)
              {
                dir = 1;
              }
              else
              {
                dir = 0;
              }
            }
          }
          father_node->height_ = treeHeight(
              father_node);  // only flag = false we need to update height
        }
        father_node->size_ = treeSize(father_node);
        father_node = father_node->father_;
      }
      AddNodeIntoList(earse_node);
    }
    else if (earse_node->left_ == nullptr &&
             earse_node->right_ != nullptr)  // has right child
    {
      int dir = 0;  // 0 stands for left, 1 stands for right
      AVLNode *father_node = earse_node->father_;
      earse_node->right_->father_ = earse_node->father_;  // update right child
      // update father node
      if (earse_node->father_ != header_)  // not root node
      {
        if (earse_node->father_->right_ == earse_node)
        {
          earse_node->father_->right_ = earse_node->right_;
          dir = 1;
        }
        else
        {
          earse_node->father_->left_ = earse_node->right_;
          dir = 0;
        }
      }
      else  // root node
      {
        header_->father_ = earse_node->right_;
      }
      bool flag = false;  // to record whether we need to adjust upper nodes
      while (father_node->is_header_ == false)
      {
        if (flag == false)
        {
          AVLNode *old_root = father_node;  // record old root
          if (adjust(father_node,
                     dir))  // we don't need to adjust upper nodes
          {
            flag = true;
            // update father_node->father_
            if (old_root != father_node)  // spin happened
            {
              AVLNode *father_father_node = father_node->father_;
              if (father_father_node == header_)
              {
                header_->father_ = father_node;
              }
              else if (father_father_node->left_ == old_root)
              {
                father_father_node->left_ = father_node;
              }
              else
              {
                father_father_node->right_ = father_node;
              }
            }
          }
          else  // update dir, we need to assure what child father_node is for
                // its father node
          {
            // update father_node->father_
            if (old_root != father_node)  // spin happened
            {
              AVLNode *father_father_node = father_node->father_;
              if (father_father_node == header_)
              {
                header_->father_ = father_node;
              }
              else if (father_father_node->left_ == old_root)
              {
                father_father_node->left_ = father_node;
              }
              else
              {
                father_father_node->right_ = father_node;
              }
            }
            if (father_node->father_ !=
                header_)  // if father_node is root, we don't
                          // need to update as while circulation will end
            {
              if (father_node->father_->right_ == father_node)
              {
                dir = 1;
              }
              else
              {
                dir = 0;
              }
            }
          }
          father_node->height_ = treeHeight(
              father_node);  // only flag = false we need to update height
        }
        father_node->size_ = treeSize(father_node);
        father_node = father_node->father_;
      }
      AddNodeIntoList(earse_node);
    }
    else  // two child
    {
      AVLNode *sub_node = earse_node->right_;
      while (sub_node->left_ != nullptr)
      {
        sub_node = sub_node->left_;
      }
      // swap two node
      AVLNode *A = earse_node;
      AVLNode *B = sub_node;
      AVLNode *A_father = A->father_;
      AVLNode *A_left = A->left_;
      AVLNode *A_right = A->right_;
      AVLNode *B_father = B->father_;
      AVLNode *B_right = B->right_;
      if(A_right == B) // A is B's right child
      {
        B->father_ = A_father;
        if(A_father == header_) // A is root
        {
          header_->father_ = B;
        }
        else if(A_father->left_ == A)
        {
          A_father->left_ = B;
        }
        else
        {
          A_father->right_ = B;
        }
        B->left_ = A_left;
        if(A_left != nullptr)
        {
          A_left->father_ = B; 
        }
        B->right_ = A;
        A->father_ = B;
        A->left_ = nullptr;
        A->right_ = B_right;
        if(B_right != nullptr)
        {
          B_right->father_ = A; 
        }
      }
      else // B is more deeper
      {
        B->father_ = A_father;
        if(A_father == header_) // A is root
        {
          header_->father_ = B;
        }
        else if(A_father->left_ == A)
        {
          A_father->left_ = B;
        }
        else
        {
          A_father->right_ = B;
        }
        B->left_ = A_left;
        if(A_left != nullptr)
        {
          A_left->father_ = B;
        }
        B->right_ = A_right;
        if(A_right != nullptr)
        {
          A_right->father_ = B;
        }
        A->father_ = B_father;
        B_father->left_ = A;
        A->left_ = nullptr;
        A->right_ = B_right;
        if(B_right != nullptr)
        {
          B_right->father_ = A;
        }
      }
      // swap height and size
      int tmp_height = A->height_;
      A->height_ = B->height_;
      B->height_ = tmp_height;
      int tmp_size = A->size_;
      A->size_ = B->size_;
      B->size_ = tmp_size;
      earseNode(A);
    }
  }
  bool adjust(AVLNode *&node,
              int dir)  // return true if we don't need to check anymore
  {
    if (dir == 0)  // earse in the left tree
    {
      int left_height = (node->left_ != nullptr) ? node->left_->height_ : 0,
          right_height = (node->right_ != nullptr) ? node->right_->height_ : 0;
      // two height have been updated after last adjust
      if (right_height - left_height == 1)
      {
        return true;
      }
      if (right_height == left_height)
      {
        return false;
      }
      // right_height - left_height = 2
      int right_right_height = (node->right_->right_ != nullptr)
                                   ? node->right_->right_->height_
                                   : 0,
          right_left_height = (node->right_->left_ != nullptr)
                                  ? node->right_->left_->height_
                                  : 0;
      int diff = right_right_height - right_left_height;
      if (diff == 1)
      {
        RR(node, node->father_);
        return false;
      }
      if (diff == -1)
      {
        RL(node, node->father_);
        return false;
      }
      // diff = 0
      RR(node, node->father_);
      return true;
    }
    else  // earse in the right tree
    {
      int left_height = (node->left_ != nullptr) ? node->left_->height_ : 0,
          right_height = (node->right_ != nullptr) ? node->right_->height_ : 0;
      // two height have been updated after last adjust
      if (left_height - right_height == 1)
      {
        return true;
      }
      if (right_height == left_height)
      {
        return false;
      }
      // left_height - right_height = 2
      int left_right_height = (node->left_->right_ != nullptr)
                                  ? node->left_->right_->height_
                                  : 0,
          left_left_height =
              (node->left_->left_ != nullptr) ? node->left_->left_->height_ : 0;
      int diff = left_left_height - left_right_height;
      if (diff == 1)
      {
        LL(node, node->father_);
        return false;
      }
      if (diff == -1)
      {
        LR(node, node->father_);
        return false;
      }
      // diff = 0
      LL(node, node->father_);
      return true;
    }
  }
};

}  // namespace sjtu

#endif
