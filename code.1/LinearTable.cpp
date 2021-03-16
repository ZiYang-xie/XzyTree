/*
    Created BY 谢子飏 19307130037
*/
#include "LinearTable.h"
using namespace DS;

LinearTable::LinearTable() // 默认构造函数
{
    nil = new RB_Node;
    nil->parent = NULL;
    nil->left = NULL;
    nil->right = NULL;
    nil->color = BLACK;
    nil->size = 0;
    nil->sz = 0;
    root = nil;
    back_node = root;
    front_node = root;
}

LinearTable::LinearTable(const LinearTable& lt) // 深拷贝
{
    // 初始化
    nil = new RB_Node;
    nil->parent = NULL;
    nil->left = NULL;
    nil->right = NULL;
    nil->color = BLACK;
    nil->size = 0;
    nil->sz = 0;
    root = nil;
    back_node = root;
    front_node = root;

    size_type node_sz = lt.root->size;
    RB_Node* cur_node = lt.front_node;
    while(node_sz--)
    {
        RB_Node* newNode = new RB_Node(cur_node);
        newNode->left = nil; newNode->right = nil; newNode->parent = nil;
        if(root == nil)
        {
            root = newNode;
            root->color = BLACK;
            front_node = newNode;
            back_node = newNode;
        }
        else
            PushBackNode(newNode, newNode->sz);
        
        // 找后继，之后可以重构 Successor 函数缩减一下代码
        if(cur_node->right != lt.nil)
        {
            cur_node = cur_node->right;
            while(cur_node->left != lt.nil)
                cur_node = cur_node->left;
        }
        else 
        {
            RB_Node* parent = cur_node->parent;
            while(parent != lt.nil && cur_node != parent->left)
            {
                cur_node = parent;
                parent = cur_node->parent;
            }
            cur_node = parent;
        }
    }
}

// 移动构造函数
LinearTable::LinearTable(LinearTable&& lt)
{
    // 内部浅拷贝
    root = lt.root;
    nil = lt.nil;
    front_node = lt.front_node;
    back_node = lt.back_node;
    lt.root = NULL; // 防止二次析构
    lt.nil = NULL;
    lt.back_node = NULL;
    lt.front_node = NULL;
}

LinearTable::LinearTable(const int& n, const int& val)
{
    nil = new RB_Node;
    nil->parent = NULL;
    nil->left = NULL;
    nil->right = NULL;
    nil->color = BLACK;
    nil->size = 0;
    nil->sz = 0;
    root = nil;
    back_node = root;
    front_node = root;
    
    push_back(n, val);
}

// 从迭代器构造
template<typename _InputIter>
LinearTable::LinearTable(_InputIter begin, _InputIter end)
{
    // 初始化 nil
    nil = new RB_Node;
    nil->parent = NULL;
    nil->left = NULL;
    nil->right = NULL;
    nil->color = BLACK;
    nil->size = 0;
    nil->sz = 0;

    difference_type diff = std::distance(begin, end);

    // 在构造函数中初始化root以减少 PushBackNode 中的特判，提升性能
    size_type root_sz = diff >= NODE_BUF_SIZE ? NODE_BUF_SIZE : diff;
    root = new RB_Node(begin, begin + root_sz);
    root->sz = root_sz;
    root->size = 1;
    root->parent = nil; root->left = nil; root->right = nil;
    root->color = BLACK;
    back_node = root; front_node = root;
    diff -= root_sz;
    begin += root_sz;

    if(diff)
    {
        while(diff > NODE_BUF_SIZE)
        {
            RB_Node* newNode = new RB_Node(begin, begin + NODE_BUF_SIZE);
            newNode->parent = nil;
            newNode->left = nil; newNode->right = nil;
            begin += NODE_BUF_SIZE;
            PushBackNode(newNode, NODE_BUF_SIZE);
            diff -= NODE_BUF_SIZE;
        }
        RB_Node* newNode = new RB_Node(begin, begin + diff);
        newNode->parent = nil;
        newNode->left = nil; newNode->right = nil;
        PushBackNode(newNode, diff);
    }
}

LinearTable::~LinearTable() // 析构函数
{
    DelTree(root);
    if(nil) // 防止诡异的内存泄漏
        delete nil;
}

//左旋
void LinearTable::RotateLeft(RB_Node* x)
{
    RB_Node* y = x->right; // 左旋节点右子节点将成为改节点的父亲
    x->right = y->left; // 令 now->right 子树等于未来父节点的左子树，维持LinearTable顺序不变

    if(y->left != nil)
        y->left->parent = x; // 维护未来父节点左子节点的 parent
    y->parent = x->parent; // 让未来父节点的父节点为当前节点的父节点
    
    // 维护x父节点的子节点指向
    if(x->parent == nil)
        root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;
    
    // 最后连接
    y->left = x;
    x->parent = y;

    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;

    y->sz = x->sz;
    x->sz = x->left->sz + x->right->sz + x->data.size();
}

//右旋和左旋对称
void LinearTable::RotateRight(RB_Node* x)
{
    RB_Node* y = x->left;
    x->left = y->right;

    if(y->right != nil)
        y->right->parent = x;
    y->parent = x->parent;
    
    if(x->parent == nil)
        root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;
    
    y->right = x;
    x->parent = y;

    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;

    y->sz = x->sz;
    x->sz = x->left->sz + x->right->sz + x->data.size();
}

inline RB_Node* LinearTable::lNode(RB_Node* TreeNode)
{
    if(TreeNode == nil)
        return nil;
    while(TreeNode->left != nil)
        TreeNode = TreeNode->left;
    return TreeNode;
}

inline RB_Node* LinearTable::rNode(RB_Node* TreeNode)
{
    while(TreeNode->right != nil)
        TreeNode = TreeNode->right;
    return TreeNode;
}

inline RB_Node* LinearTable::Predecessor(RB_Node* TreeNode) // 找前驱
{
    if(TreeNode->left != nil) // 有左子树，就是左子树的最右
        return rNode(TreeNode->left);
    
    RB_Node* parent = TreeNode->parent; // 寻祖先前驱
    while(parent != nil && TreeNode != parent->right)
    {
        TreeNode = parent;
        parent = TreeNode->parent;
    }

    return TreeNode;
}

inline RB_Node* LinearTable::Successor(RB_Node* TreeNode) // 找后继
{
    if(TreeNode->right != nil)  // 有右子树，就是右子树的最左
        return lNode(TreeNode->right);
    
    RB_Node* parent = TreeNode->parent; // 寻祖先后继
    while(parent != nil && TreeNode != parent->left)
    {
        TreeNode = parent;
        parent = TreeNode->parent;
    }

    return TreeNode;
}

void LinearTable::InsertFix(RB_Node* z)
{
    // 四种情况
    while(z->parent->color == RED) // RED == false 为了可读性直接这样写了
    {
        if(z->parent == z->parent->parent->left)
        {
            RB_Node* y = z->parent->parent->right;
            if(y->color == RED) // case 1 RED uncle (recolor)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // up
            }
            else 
            {
                if(z == z->parent->right) // case 2 BLACK uncle(triangle) rotate x->parent
                {
                    z = z->parent;
                    RotateLeft(z);
                }
                z->parent->color = BLACK;   // case 3 BLACK uncle(line) rotate x->grandparent && recolor
                z->parent->parent->color = RED;
                RotateRight(z->parent->parent);
            }
        }
        else // 对称的
        {
            RB_Node* y = z->parent->parent->left;
            if(y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else 
            {
                if(z == z->parent->left)
                {
                    z = z->parent;
                    RotateRight(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RotateLeft(z->parent->parent);
            }
        }
    }
    root->color = BLACK; // case 0
}

void LinearTable::DelFix(RB_Node* x)
{
    while(x != root && x->color == BLACK)
    {
        if(x == x->parent->left)
        {
            RB_Node* y = x->parent->right;
            
            if(y->color == RED) // case 1
            {   
                y->color = BLACK;
                x->parent->color = RED;

                RotateLeft(x->parent);
                y = x->parent->right;
            }
            if(y == nil)
                break;
            if(y->left->color == BLACK && y->right->color == BLACK) //case 2
            {
                y->color = RED;
                x = x->parent;
            }
            else //case 3
            {
                if(y->right->color == BLACK)
                {
                    y->left->color = BLACK;
                    y->color = RED;

                    RotateRight(y);
                    y = x->parent->right;
                }
                // case 4
                y->color = x->parent->color;
                x->parent->color = BLACK;
                y->right->color = BLACK;

                RotateLeft(x->parent);
                x = root;
            }
        }
        else if(x == x->parent->right)// 对称的
        {
            RB_Node* y = x->parent->left;

            if(y->color == RED) // case 1
            {   
                y->color = BLACK;
                x->parent->color = RED;

                RotateLeft(x->parent);
                y = x->parent->left;
            }
            if(x == root)
                break;
            if(y->left == NULL)
                break;
            if(y->left->color == BLACK && y->right->color == BLACK) //case 2
            {
                y->color = RED;
                x = x->parent;
            }
            else 
            {
                if(y->left->color == BLACK) //case 3
                {
                    y->right->color = BLACK;
                    y->color = RED;

                    RotateLeft(y);
                    y = x->parent->left;
                }
                y->color = x->parent->color;
                x->parent->color = BLACK;
                y->left->color = BLACK;

                RotateRight(x->parent);
                x = root;
            }

        }
    }

    x->color = BLACK;
}

// 创建节点
inline RB_Node* LinearTable::CreateNode(const value_type& val)
{
    RB_Node* ptr = new RB_Node;

    ptr->parent = nil;
    ptr->left = nil;
    ptr->right = nil;
    ptr->data.push_back(val);
    ptr->size = 1;
    ptr->sz = 1;
    ptr->color = RED; // init by RED

    return ptr;
}

inline RB_Node* LinearTable::CreateNode(const size_type& init_sz, const value_type& val)
{
    RB_Node* ptr = new RB_Node;
    ptr->parent = nil;
    ptr->left = nil;
    ptr->right = nil;
    ptr->sz = init_sz;
    ptr->size = 1;
    ptr->data.insert(ptr->data.begin(), init_sz, val); // 可以用初始化构造函数进一步优化 
    ptr->color = RED; // init by RED

    return ptr;
}

inline void LinearTable::Transplant(RB_Node* u, RB_Node* v)
{
    if(u->parent == nil) // 根节点
        root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else if(u == u->parent->right)
        u->parent->right = v;
    if(v != nil)
        v->parent = u->parent;
}

inline void LinearTable::PushBackNode(RB_Node* node, const size_type& node_sz)
{
    FixSizeInc(back_node, node_sz);
    back_node->right = node;
    node->parent = back_node;
    InsertFix(node);
    back_node = node;
}

inline void LinearTable::PushFrontNode(RB_Node* node, const size_type& node_sz)
{
    FixSizeInc(front_node, node_sz);
    front_node->left = node;
    node->parent = front_node;
    InsertFix(node);
    front_node = node;
}

inline void LinearTable::DelNode(RB_Node* TreeNode)
{
    RB_Node* x;
    RB_Node* y = TreeNode;

    bool OriginalColor = y->color;
    if(TreeNode->left == nil) // 没有左孩子 or 两个都没有
    {
        x = TreeNode->right;
        // 维护frontNode
        Transplant(TreeNode, x);
        if(TreeNode == front_node)
        {
            if(TreeNode->right == nil)
                front_node = TreeNode->parent;
            else 
                front_node = Successor(TreeNode);
        }
        if(TreeNode == back_node)
            back_node = TreeNode->parent;
    }
    else if(TreeNode->right == nil) // 没有右孩子但有左孩子
    {
        x = TreeNode->left;
        Transplant(TreeNode, x);
        if(TreeNode == back_node)
            back_node = Predecessor(TreeNode);
    }
    else 
    {
        y = Successor(TreeNode);
        RB_Node* sz_fixer = y;
        while(sz_fixer != TreeNode)
        {
            sz_fixer = sz_fixer->parent;
            sz_fixer->size--;
            sz_fixer->sz -= y->data.size();
        }
        OriginalColor = y->color;
        x = y->right;
        if(y->parent == TreeNode)
            x->parent = y;
        else 
        {
            
            Transplant(y, y->right);
            y->right = TreeNode->right;
            y->right->parent = y;
        }
        Transplant(TreeNode, y);
        y->left = TreeNode->left;
        y->left->parent = y;
        y->color = TreeNode->color;
        // Fix SZ
        y->size = y->left->size + y->right->size + 1;
        y->sz = y->left->sz + y->right->sz + y->data.size();
    }
    if(OriginalColor == BLACK && x != nil)
        DelFix(x);
    
    delete TreeNode;
}

inline RB_Node* LinearTable::FindNode(key_type key)
{
    key++;
    RB_Node* cur = root;
    size_type rank_low, rank_high;
    while(cur != nil)
    {
        rank_low = cur->left->sz;
        rank_high = cur->left->sz + cur->data.size();
        if(key > rank_low && key <= rank_high)
            return cur;
        else if(key <= rank_low)
            cur = cur->left;
        else
        {
            cur = cur->right;
            key -= rank_high;
        }
    }
    return nil;
}

inline RB_Node* LinearTable::backNode()
{
    return back_node;
}

inline RB_Node* LinearTable::frontNode()
{
    return front_node;
}

// 递归删除
void LinearTable::DelTree(RB_Node* TreeNode)
{
    if(TreeNode == nil)
        return;
    DelTree(TreeNode->left);
    DelTree(TreeNode->right);
    DelNode(TreeNode);
}

// -----------------------Push Back & Front---------------------------- //

void LinearTable::push_back(const value_type& val)
{
    if(empty())
    {
        root = CreateNode(val);
        root->color = BLACK;
        front_node = root;
        back_node = root;
    }
    else if(back_node->data.size() >= NODE_BUF_SIZE) // 如果超节点上限了就新创一个节点
    {
        RB_Node* newNode = CreateNode(val);
        PushBackNode(newNode, 1);
    }
    else // 如果没超上限直接后端插入
    {
        FixSzInc(back_node, 1);
        back_node->data.push_back(val);
    }
}

void LinearTable::push_back(size_type num, const value_type& val)
{
    size_type res = NODE_BUF_SIZE - back_node->data.size();
    if(num <= res) // 如果直接能够放下
    {
        if(back_node != nil)
        {
            back_node->data.insert(back_node->data.end(), num, val);
            FixSzInc(back_node, num);
        }
        else 
        {
            root = CreateNode(num, val);
            root->color = BLACK;
            back_node = root;
            front_node = root;
        }
    }
    else // 如果要分成几个节点放
    {
        if(res)
        {
            if(back_node != nil)
            {
                back_node->data.insert(back_node->data.end(), res, val); // 先填满
                FixSzInc(back_node, res);
            }
            else 
            {
                root = CreateNode(res, val);
                root->color = BLACK;
                back_node = root;
                front_node = root;
            }
            num -= res;
        }
        // 算出要几个节点，分别填满插入, 可能还有插入子树优化，后续可以继续优化
        size_type full_node_num = num >> BIN_CNT;
        size_type back_res = num - (full_node_num << BIN_CNT);
        RB_Node* newNode;
        for(size_type i = 0; i < full_node_num; ++i)
        {
            newNode = CreateNode(NODE_BUF_SIZE, val);
            PushBackNode(newNode, NODE_BUF_SIZE);
        }
        if(back_res)
        {
            newNode = CreateNode(back_res, val);
            PushBackNode(newNode, back_res);
        }
    }
}

void LinearTable::push_front(const value_type& val)
{
    if(empty())
    {
        root = CreateNode(val);
        root->color = BLACK;
        front_node = root;
        back_node = root;
    }
    else if(front_node->data.size() >= NODE_BUF_SIZE)
    {
        RB_Node* newNode = CreateNode(val);
        PushFrontNode(newNode, 1);
    }
    else 
    {
        FixSzInc(front_node, 1);
        front_node->data.push_front(val);
    }
}

void LinearTable::push_front(size_type num, const value_type& val)
{
    size_type res = NODE_BUF_SIZE - front_node->data.size();
    if(num <= res) // 如果直接能够放下
    {
        if(front_node != nil)
        {
            front_node->data.insert(front_node->data.begin(), num, val);
            FixSzInc(front_node, num);
        }
        else 
        {
            root = CreateNode(num, val);
            root->color = BLACK;
            back_node = root;
            front_node = root;
        }
    }
    else // 如果要分成几个节点放
    {
        if(res)
        {
            if(front_node != nil)
            {
                front_node->data.insert(front_node->data.begin(), res, val); // 先填满
                FixSzInc(front_node, res);
            }
            else 
            {
                root = CreateNode(res, val);
                root->color = BLACK;
                back_node = root;
                front_node = root;
            }
            num -= res;
        }
        // 算出要几个节点，分别填满插入, 可能还有插入子树优化，后续可以继续优化
        size_type full_node_num = num >> BIN_CNT;
        size_type back_res = num - (full_node_num << BIN_CNT);
        RB_Node* newNode;
        for(size_type i = 0; i < full_node_num; ++i)
        {
            newNode = CreateNode(NODE_BUF_SIZE, val);
            PushFrontNode(newNode, NODE_BUF_SIZE);
        }
        if(back_res)
        {
            newNode = CreateNode(back_res, val);
            PushFrontNode(newNode, back_res);
        }
    }
}

inline void LinearTable::pop_back()
{
    if(back_node->data.size() > 1)
    {
        FixSzDec(back_node, 1);
        back_node->data.pop_back();
    }
    else
    {
        FixSizeDec(back_node, 1);
        back_node->data.pop_back();
        DelNode(back_node);
    }
}

void LinearTable::pop_back(size_type num)
{
    size_type res = back_node->data.size();
    if(num <= res) // 如果直接能够直接删除
    {
        back_node->data.erase(back_node->data.end() - num, back_node->data.end());
        FixSzDec(back_node, num);
    }
    else // 如果要分成几个节点删
    {
        while(num > res) // 循环删除直到不足一个节点
        {
            num -= res;
            FixSizeDec(back_node, res);
            DelNode(back_node);
            res = back_node->data.size();
        }
        back_node->data.erase(back_node->data.end() - num, back_node->data.end());
        FixSzDec(back_node, num);
    }
}

inline void LinearTable::pop_front()
{
    if(front_node->data.size() > 1)
    {
        FixSzDec(front_node, 1);
        front_node->data.pop_front();
    }
    else
    {
        FixSizeDec(front_node, 1);
        front_node->data.pop_front();
        DelNode(front_node);
    }
}

void LinearTable::pop_front(size_type num)
{
    size_type res = front_node->data.size();
    if(num <= res) // 如果直接能够直接删除
    {
        front_node->data.erase(front_node->data.begin(), front_node->data.begin() + num);
        FixSzDec(front_node, num);
    }
    else // 如果要分成几个节点删
    {
        while(num > res) // 循环删除直到不足一个节点
        {
            num -= res;
            FixSizeDec(front_node, res);
            DelNode(front_node);
            res = front_node->data.size();
        }
        front_node->data.erase(front_node->data.begin(), front_node->data.begin() + num);
        FixSzDec(front_node, num);
    }
}


// ------------------ END ----------------------- //


inline void LinearTable::clear()
{
    DelTree(root);
    root = nil;
    back_node = root;
    front_node = root;
}

inline void LinearTable::swap(LinearTable& B)
{
    std::swap(root, B.root);
    std::swap(nil, B.nil);
    std::swap(back_node, B.back_node);
    std::swap(front_node, B.front_node);
}

inline size_type LinearTable::size()
{
    return root->sz;
}

inline bool LinearTable::empty()
{
    return root->sz == 0;
}

inline value_type LinearTable::front()
{
    return front_node->data.front();
}

inline const value_type& LinearTable::front() const
{
    return front_node->data.front();
}

inline value_type LinearTable::back()
{
    return back_node->data.back();
}

inline const value_type& LinearTable::back() const
{
    return back_node->data.back();
}

inline value_type& LinearTable::operator[](key_type key)
{
    key++;
    RB_Node* cur = root;
    size_type rank_low, rank_high;
    while(cur != nil)
    {
        rank_low = cur->left->sz;
        rank_high = cur->left->sz + cur->data.size();
        if(key > rank_low && key <= rank_high)
            return cur->data[key - cur->left->sz - 1];
        else if(key <= rank_low)
            cur = cur->left;
        else
        {
            cur = cur->right;
            key -= rank_high;
        }
    }
    throw std::domain_error("Segmentation Fault, Cannot find the key!\n");
}

inline const value_type& LinearTable::operator[](key_type key) const
{
    key++;
    RB_Node* cur = root;
    size_type rank_low, rank_high;
    while(cur != nil)
    {
        rank_low = cur->left->sz;
        rank_high = cur->left->sz + cur->data.size();
        if(key > rank_low && key <= rank_high)
            return cur->data[key - cur->left->sz - 1];
        else if(key <= rank_low)
            cur = cur->left;
        else
        {
            cur = cur->right;
            key -= rank_high;
        }
    }
    throw std::domain_error("Segmentation Fault, Cannot find the key!\n");
}


inline value_type& LinearTable::at(key_type key)
{
    key++;
    RB_Node* cur = root;
    size_type rank_low, rank_high;
    while(cur != nil)
    {
        rank_low = cur->left->sz;
        rank_high = cur->left->sz + cur->data.size();
        if(key > rank_low && key <= rank_high)
            return cur->data[key - cur->left->sz - 1];
        else if(key <= rank_low)
            cur = cur->left;
        else
        {
            cur = cur->right;
            key -= rank_high;
        }
    }
    // at 边界检查
   throw std::domain_error("Segmentation Fault, Cannot find the key!\n");
}

value_type LinearTable::modify(key_type key, const value_type& y)
{
    key++;
    RB_Node* cur = root;
    size_type rank_low, rank_high;
    while(cur != nil)
    {
        rank_low = cur->left->sz;
        rank_high = cur->left->sz + cur->data.size();
        if(key > rank_low && key <= rank_high)
        {   
            value_type prev_val = cur->data[key - cur->left->sz - 1];
            cur->data[key - cur->left->sz - 1] += y;
            return prev_val;
        }
        else if(key <= rank_low)
            cur = cur->left;
        else
        {
            cur = cur->right;
            key -= rank_high;
        }
    }
    throw std::domain_error("Segmentation Fault, Cannot find the key!\n");
}

// --------------------------- Extended --------------------------- //

inline iterator LinearTable::begin()
{
    if(root == nil)
        return iterator();
    return iterator(root, nil, front_node, 0, front_node->data.begin());
}

inline const iterator LinearTable::begin() const
{
    if(root == nil)
        return iterator();
    return iterator(root, nil, front_node, 0, front_node->data.begin());
}

inline iterator LinearTable::end()
{
    if(root == nil)
        return iterator();
    return iterator(root, nil, back_node, root->sz, back_node->data.end());
}

inline const iterator LinearTable::end() const
{
    if(root == nil)
        return iterator();
    return iterator(root, nil, back_node, root->sz, back_node->data.end());
}

// -------------------- Size Fix Util -------------------------- //

inline void LinearTable::FixSizeInc(RB_Node* node, const size_type& num)
{
    while(node != nil)
    {
        node->size++;
        node->sz += num;
        node = node->parent;
    }
}

inline void LinearTable::FixSizeDec(RB_Node* node, const size_type& num)
{
    while(node != nil)
    {
        node->size--;
        node->sz -= num;
        node = node->parent;
    }
}

inline void LinearTable::FixSzInc(RB_Node* node, const size_type& num)
{
    while(node != nil)
    {
        node->sz += num;
        node = node->parent;
    }
}

inline void LinearTable::FixSzDec(RB_Node* node, const size_type& num)
{
    while(node != nil)
    {
        node->sz -= num;
        node = node->parent;
    }
}

// -------------------- Insert -------------------------- //

iterator LinearTable::insert(iterator it, const value_type& val)
{
    if(root == nil)  //无节点
    {
        push_back(val);
        return end();
    }
    
    if(it.node_ptr->data.size() < NODE_BUF_SIZE)
    {
        it.inner_it = it.node_ptr->data.insert(it.inner_it, val);
        FixSzInc(it.node_ptr, 1);
    }
    else
    {
        it.inner_it = it.node_ptr->data.insert(it.inner_it, val);
        value_type new_val = it.node_ptr->data.front();
        it.node_ptr->data.pop_front();
        it.inner_it++;
        
        if(it.node_ptr->left == nil)
        {
            RB_Node* newNode = CreateNode(new_val);
            it.node_ptr->left = newNode;
            newNode->parent = it.node_ptr;
            FixSizeInc(it.node_ptr, 1);
            InsertFix(newNode);
            if(it.node_ptr == front_node)
                front_node = newNode;
            return it;
        }
        RB_Node* succ = rNode(it.node_ptr->left); // 找前驱
        if(succ->data.size() < NODE_BUF_SIZE) // 如果前驱有空就直接插入
        {
            succ->data.push_back(new_val);
            FixSzInc(succ, 1);
        }
        else // 如果没空就直接挂一个节点，不找了
        {
            RB_Node* newNode = CreateNode(new_val);
            succ->right = newNode;
            newNode->parent = succ;
            FixSizeInc(succ, 1);
            InsertFix(newNode);
        }
    }
    return it;
}

// batch-insert 批量插入
iterator LinearTable::insert(iterator it, size_type num, const value_type& val)
{
    if(root == nil)
    {
        push_back(num, val);
        return end();
    }
    size_type res = NODE_BUF_SIZE - it.node_ptr->data.size();
    if(num <= res)
    {
        it.inner_it = it.node_ptr->data.insert(it.inner_it, num, val);
        FixSzInc(it.node_ptr, num);
        return it;
    }
    else 
    {
        it.inner_it = it.node_ptr->data.insert(it.inner_it, res, val);
        FixSzInc(it.node_ptr, res);
        num -= res;
        for(size_type i = 0; i < num; ++i)
            it = insert(it, val);
    }
    return it;
}

// -------------------------------  Erase --------------------------------- //

iterator LinearTable::erase(iterator it)
{
    if(it.inner_it == it.node_ptr->data.end())
        it.FixEnd(); // 修了一个bug
    RB_Node* cur_node = it.node_ptr;
    if(cur_node->data.size() > 1)
    {
        it.inner_it = cur_node->data.erase(it.inner_it);
        FixSzDec(cur_node, 1);
    }
    else
    {
        cur_node->data.pop_back(); // 删除元素
        // 下一迭代器位置
        if(cur_node != back_node)
        {
            RB_Node* nex_node = Successor(cur_node);
            it.node_ptr = nex_node;
            it.inner_it = nex_node->data.begin();
        }
        else 
        {
            RB_Node* nex_node = Predecessor(cur_node);
            it.node_ptr = nex_node;
            it.inner_it = nex_node->data.end() - 1;
        }
        FixSizeDec(cur_node, 1);
        DelNode(cur_node);
    }
    return it;
}

