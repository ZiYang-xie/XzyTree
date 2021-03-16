/*
    Created BY 谢子飏 19307130037
*/
#pragma once
#ifndef Linear_Table
#define Linear_Table
#include "typedef.h"


namespace DS
{
#define RED false
#define BLACK true

#define BIN_CNT 9
#define NODE_BUF_SIZE 512

struct RB_Node
{
    RB_Node* parent;
    RB_Node* left;
    RB_Node* right;
    size_type size;
    size_type sz;
    node_type data;
    bool color; // RED = false (init state), BLACK = true

    RB_Node(){}    // 默认构造
    RB_Node(const RB_Node* node):   // 复制构造
    parent(NULL),
    left(NULL),
    right(NULL),
    data(node->data),
    color(node->color)
    {
        size = 1;
        sz = data.size();
    }
    template<typename _InputIter>
    RB_Node(_InputIter begin, _InputIter end):
    data(begin, end){}
    RB_Node& operator=(const RB_Node* node)   // 重载运算符 纯粹数据拷贝
    {
        parent = node->parent;
        left = node->left;
        right = node->right;
        size = node->size;
        data = node->data;   // deque深拷贝
        sz = node->sz;
        size = node->size;
        color = node->color;
        return *this;
    }
};

// -------------------------- Iterator --------------------------- //

class iterator : public
std::iterator<std::random_access_iterator_tag, value_type>
{
private:
    RB_Node* root;
    RB_Node* nil;
    key_type key;
    RB_Node* node_ptr;
    std::deque<value_type>::iterator inner_it;

    friend class LinearTable;
    friend difference_type operator-(const iterator& a, const iterator& b);

    void FixEnd()
    {
        RB_Node* cur = root;
        key_type cur_key = ++key;
        size_type rank_low, rank_high;
        while(cur != nil)
        {
            rank_low = cur->left->sz;
            rank_high = cur->left->sz + cur->data.size();
            if(cur_key > rank_low && cur_key <= rank_high)
            {
                node_ptr = cur;
                inner_it = cur->data.begin() + (cur_key - cur->left->sz - 1);
                break;
            }
            else if(cur_key <= rank_low)
                cur = cur->left;
            else
            {
                cur = cur->right;
                cur_key -= rank_high;
            }
        }
    }
public:
    iterator(): node_ptr(NULL), key(0)
    {
        nil = new RB_Node;
        nil->parent = NULL;
        nil->left = NULL;
        nil->right = NULL;
        nil->color = BLACK;
        nil->size = 0;
        nil->sz = 0;
        root = nil;
    };

    iterator(RB_Node* _root, 
    RB_Node* _nil, 
    RB_Node* _node_ptr, 
    const key_type& _key, 
    const std::deque<value_type>::iterator& _inner_it):

    root(_root), 
    nil(_nil), 
    node_ptr(_node_ptr), 
    key(_key) , 
    inner_it(_inner_it)
    {};

    ~iterator()
    {
        root = NULL;
        nil = NULL;
        node_ptr = NULL;
        key = 0;
        delete nil;
    }
    iterator& operator++()
    {
        if(inner_it == node_ptr->data.end() - 1)
        {
            // 找后继
            if(node_ptr->right != nil)  
            {
                node_ptr = node_ptr->right;
                while(node_ptr->left != nil)
                    node_ptr = node_ptr->left;
            }
            else 
            {
                RB_Node* parent = node_ptr->parent;
                while(parent != nil && node_ptr != parent->left)
                {
                    node_ptr = parent;
                    parent = node_ptr->parent;
                }
                node_ptr = parent;
            }
            inner_it = node_ptr->data.begin();
        }
        else
            ++inner_it;
        ++key;
        return *this;
    }
    iterator operator++(int n)
    {
        iterator origin_it = *this;
        this->operator++();
        return origin_it;
    }
    iterator& operator--()
    {
        if(inner_it == node_ptr->data.begin())
        {
            if(node_ptr->left != nil) // 有左子树，就是左子树的最右
            {
                node_ptr = node_ptr->left;
                while(node_ptr->right != nil)
                    node_ptr = node_ptr->right;
            }
            else 
            {
                RB_Node* parent = node_ptr->parent; // 寻祖先前驱
                while(parent != nil && node_ptr != parent->right)
                {
                    node_ptr = parent;
                    parent = node_ptr->parent;
                }
                node_ptr = parent;
            }
            inner_it = node_ptr->data.end() - 1;
        }
        else
            --inner_it;
        --key;
        return *this;
    }
    iterator operator--(int n)
    {
        iterator origin_it = *this;
        this->operator--();
        return origin_it;
    }
    iterator& operator+(const difference_type& diff)
    {
        if(diff == 0)
            return *this;
        key += diff;
        difference_type res_len = std::distance(inner_it, node_ptr->data.end());
        if(diff <= res_len)
            inner_it += diff;
        else 
        {
            RB_Node* cur = root;
            key_type cur_key = key;
            size_type rank_low, rank_high;
            while(cur != nil)
            {
                rank_low = cur->left->sz;
                rank_high = cur->left->sz + cur->data.size();
                if(cur_key > rank_low && cur_key <= rank_high)
                {
                    node_ptr = cur;
                    inner_it = cur->data.begin() + (cur_key - cur->left->sz);
                    break;
                }
                else if(cur_key <= rank_low)
                    cur = cur->left;
                else
                {
                    cur = cur->right;
                    cur_key -= rank_high;
                }
            }
        }
        return *this;
    }
    const iterator operator+(const difference_type& diff) const
    {
        iterator it = *this;
        return it += diff;
    }
    iterator& operator-(const difference_type& diff)
    {
        if(diff == 0)
            return *this;
        key -= diff;
        difference_type res_len = std::distance(node_ptr->data.begin(), inner_it);
        if(diff <= res_len)
            inner_it -= diff;
        else 
        {
            RB_Node* cur = root;
            key_type cur_key = key;
            size_type rank_low, rank_high;
            while(cur != nil)
            {
                rank_low = cur->left->sz;
                rank_high = cur->left->sz + cur->data.size();
                if(cur_key > rank_low && cur_key <= rank_high)
                {
                    node_ptr = cur;
                    inner_it = cur->data.begin() + (cur_key - cur->left->sz - 1);
                    break;
                }
                else if(cur_key <= rank_low)
                    cur = cur->left;
                else
                {
                    cur = cur->right;
                    cur_key -= rank_high;
                }
            }
        }
        return *this;
    }
    const iterator operator-(const difference_type& diff) const
    {
        iterator it = *this;
        return it -= diff;
    }
    iterator& operator+=(const difference_type& diff)
    {
        return operator+(diff);
    }
    iterator& operator-=(const difference_type& diff)
    {
        return operator-(diff);
    }
    void operator=(const iterator& it)
    {
        node_ptr = it.node_ptr;
        inner_it = it.inner_it;
        nil = it.nil;
        root = it.root;
        key = it.key;
    }
    value_type& operator*()
    {
        return *inner_it;
    }
    const value_type& operator*() const
    {
        return *inner_it;
    }
    bool operator==(const iterator& it)
    {
        return it.key == key;
    }
    bool operator!=(const iterator& it)
    {
        return it.key != key;
    }
};

// difference_type operator-(const iterator& a, const iterator& b)
// {
//     return std::abs(b.key - a.key);
// }

// -------------------------- LinearTable --------------------------- //

class LinearTable
{
private:
    RB_Node* root;
    RB_Node* nil;

    RB_Node* back_node;
    RB_Node* front_node;
public:
    LinearTable();  // 默认构造
    LinearTable(const LinearTable&); // 复制构造函数 深拷贝
    LinearTable(LinearTable&&); // 移动构造函数
    LinearTable(const int&, const int&); // 初始化 n 个 val 的构造函数
    template <typename _InputIter>
    LinearTable(_InputIter a, _InputIter b);   // 迭代器构造
    ~LinearTable();

private:    // 红黑树维护
    void RotateLeft(RB_Node*);
    void RotateRight(RB_Node*);
    RB_Node* lNode(RB_Node*);   // 树链最左
    RB_Node* rNode(RB_Node*);   // 树链最右
    RB_Node* Predecessor(RB_Node*);
    RB_Node* Successor(RB_Node*);

    void InsertFix(RB_Node*);
    void DelFix(RB_Node*);

    // 可考虑内存池优化
    RB_Node* CreateNode(const value_type&);
    RB_Node* CreateNode(const size_type&, const value_type&);

    void Transplant(RB_Node*, RB_Node*);    // 把第二个参数的子树种到第一个参数上去
    void PushBackNode(RB_Node*, const size_type&);
    void PushFrontNode(RB_Node*, const size_type&);
    void DelNode(RB_Node*);

    RB_Node* FindNode(key_type);    // 根据 key 找到对应节点
    RB_Node* backNode();
    RB_Node* frontNode();

    void DelTree(RB_Node*);

    // size和sz的维护为了保证数据不溢出，所以增减各写了一个
    void FixSizeInc(RB_Node*, const size_type&);
    void FixSizeDec(RB_Node*, const size_type&);
    void FixSzInc(RB_Node*, const size_type&);
    void FixSzDec(RB_Node*, const size_type&);

public:
    // 外部开放 api
    void push_back(const value_type&);
    void push_back(size_type, const value_type&);
    void push_front(const value_type&);
    void push_front(size_type, const value_type&);

    void pop_back();
    void pop_back(size_type);
    void pop_front();
    void pop_front(size_type);

    void clear();
    void swap(LinearTable&);
    
    size_type size();
    bool empty();

    value_type front();
    const value_type& front() const;
    value_type back();
    const value_type& back() const;
    

    value_type& operator[](key_type);
    const value_type& operator[](key_type) const;
    value_type& at(key_type);
    value_type modify(key_type, const value_type&);

    // ------- iterator extended -------- //
    
    iterator begin();
    const iterator begin() const;
    iterator end();
    const iterator end() const;

    iterator insert(iterator, const value_type&); // 在 iterator 前 插入一个 val
    iterator insert(iterator, size_type, const value_type&); // 在 iterator 前 插入 size 个 val

    iterator erase(iterator); // 删除迭代器位置的元素
};

}
#endif