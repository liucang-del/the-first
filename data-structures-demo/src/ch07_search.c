/*
 * ch07_search.c —— 第 7 章 查找
 *
 * 覆盖目录：
 *   7.1 查找的基本概念
 *   7.2 线性表的查找（顺序、折半、分块）
 *   7.3 树表的查找（二叉排序树、平衡二叉树、B-树、B+树）
 *   7.4 散列表的查找（基本概念、散列函数、冲突处理、查找）
 *   7.5 小结
 */
#include "common.h"

/* ==================== 7.1 基本概念 ==================== */

typedef struct {
    int key;   /* 关键字 */
    int value; /* 记录中的其他数据 */
} Record;

static void demo_concepts(void)
{
    ds_section("7.1", "查找的基本概念");
    ds_note("查找表：由同一类型记录构成的集合；关键字：能唯一标识记录的数据项。");
    ds_note("平均查找长度 ASL = Σ(pi * ci)，pi 为查找第 i 个记录的概率，");
    ds_note("ci 为找到它所需的比较次数。ASL 是衡量查找算法优劣的主要指标。");
    ds_note("静态查找表只查找；动态查找表还要插入和删除。");
}

/* ==================== 7.2.1 顺序查找 ==================== */

/* 监视哨放在 0 号单元，从后向前查找；返回 1 基位序，0 表示未找到 */
static int seq_search(int *a, int n, int key, int *cmp)
{
    int i = n;
    a[0] = key;
    *cmp = 0;
    while (a[i] != key) {
        i--;
        (*cmp)++;
    }
    (*cmp)++;
    return i;
}

static void demo_seq_search(void)
{
    int a[12];
    int i;
    int cmp = 0;
    int pos;

    ds_section("7.2", "线性表的查找");
    ds_section("7.2.1", "顺序查找");
    ds_note("从表的一端逐个比较；设置监视哨可以省去循环内的越界判断。");
    ds_note("成功查找 ASL = (n+1)/2，不成功需 n+1 次比较，时间 O(n)。");
    for (i = 1; i <= 10; i++) {
        a[i] = i;
    }
    pos = seq_search(a, 10, 7, &cmp);
    ds_ok("在 1..10 中顺序查找 7：位置=%d，比较次数=%d", pos, cmp);
    ds_check_ll(pos, 7, "顺序查找返回位置 7");
    ds_check_ll(cmp, 4, "查找 7 需比较 4 次（10,9,8,7）");
    pos = seq_search(a, 10, 99, &cmp);
    ds_check_ll(pos, 0, "查找不存在的 99 返回 0");
    ds_check_ll(cmp, 11, "不成功时比较 11 次（含监视哨）");
    ds_note("顺序查找不要求表有序，但 ASL 较大。");
}

/* ==================== 7.2.2 折半查找 ==================== */

static int binary_search(const int *a, int n, int key, int *cmp)
{
    int low = 1;
    int high = n;
    *cmp = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        (*cmp)++;
        if (a[mid] == key) {
            return mid;
        }
        if (a[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return 0;
}

static void demo_binary_search(void)
{
    int a[11];
    int i;
    int cmp;
    int pos;

    ds_section("7.2.2", "折半查找");
    ds_note("要求表按关键字有序；每次比较后排除一半元素，时间 O(log n)。");
    ds_note("判定树是一棵平衡二叉树，成功查找最多比较 ⌊log2 n⌋+1 次。");
    for (i = 1; i <= 10; i++) {
        a[i] = i;
    }
    pos = binary_search(a, 10, 7, &cmp);
    ds_ok("在 1..10 中折半查找 7：位置=%d，比较次数=%d", pos, cmp);
    ds_check_ll(pos, 7, "折半查找返回位置 7");
    ds_check_true(cmp <= 4, "10 个元素的折半查找不超过 4 次比较");
    pos = binary_search(a, 10, 11, &cmp);
    ds_check_ll(pos, 0, "查找不存在的 11 返回 0");
    ds_check_true(cmp <= 4, "不成功查找也不超过 4 次比较");
    pos = binary_search(a, 10, 1, &cmp);
    ds_check_ll(pos, 1, "查找边界值 1 成功");
    pos = binary_search(a, 10, 10, &cmp);
    ds_check_ll(pos, 10, "查找边界值 10 成功");
}

/* ==================== 7.2.3 分块查找 ==================== */

typedef struct {
    int max_key;
    int start; /* 块内第一个元素的下标（0 基） */
    int count;
} BlockIndex;

static int block_search(const int *a, const BlockIndex *idx, int blocks,
                        int key, int *cmp)
{
    int b;
    int i;
    *cmp = 0;
    for (b = 0; b < blocks; b++) {
        (*cmp)++;
        if (key <= idx[b].max_key) {
            for (i = 0; i < idx[b].count; i++) {
                (*cmp)++;
                if (a[idx[b].start + i] == key) {
                    return idx[b].start + i + 1;
                }
            }
            return 0;
        }
    }
    return 0;
}

static void demo_block_search(void)
{
    int a[18];
    BlockIndex idx[3];
    int i;
    int cmp;
    int pos;

    ds_section("7.2.3", "分块查找");
    ds_note("块间有序、块内无序；先查索引表确定块，再在块内顺序查找。");
    ds_note("理想块长约为 sqrt(n)，ASL 介于顺序查找与折半查找之间。");
    for (i = 0; i < 18; i++) {
        a[i] = i + 1;
    }
    for (i = 0; i < 3; i++) {
        idx[i].start = i * 6;
        idx[i].count = 6;
        idx[i].max_key = (i + 1) * 6;
    }
    pos = block_search(a, idx, 3, 14, &cmp);
    ds_ok("在 1..18 中分块查找 14：位置=%d，比较次数=%d", pos, cmp);
    ds_check_ll(pos, 14, "分块查找返回位置 14");
    ds_check_ll(cmp, 5, "先比较 3 个索引项，再在块内比较 2 次");
    pos = block_search(a, idx, 3, 99, &cmp);
    ds_check_ll(pos, 0, "查找不存在的 99 返回 0");
}

/* ==================== 7.3.1 二叉排序树 ==================== */

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode, *BSTree;

static BSTree bst_insert(BSTree t, int key)
{
    if (t == NULL) {
        BSTree node = (BSTree)malloc(sizeof(BSTNode));
        node->key = key;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    if (key < t->key) {
        t->left = bst_insert(t->left, key);
    } else if (key > t->key) {
        t->right = bst_insert(t->right, key);
    }
    return t; /* 相等时不重复插入 */
}

static bool bst_search(BSTree t, int key)
{
    while (t != NULL) {
        if (key == t->key) {
            return true;
        }
        t = key < t->key ? t->left : t->right;
    }
    return false;
}

static BSTNode *bst_min(BSTree t)
{
    while (t != NULL && t->left != NULL) {
        t = t->left;
    }
    return t;
}

static BSTree bst_delete(BSTree t, int key)
{
    if (t == NULL) {
        return NULL;
    }
    if (key < t->key) {
        t->left = bst_delete(t->left, key);
    } else if (key > t->key) {
        t->right = bst_delete(t->right, key);
    } else {
        if (t->left == NULL) {
            BSTree r = t->right;
            free(t);
            return r;
        }
        if (t->right == NULL) {
            BSTree l = t->left;
            free(t);
            return l;
        }
        {
            BSTNode *succ = bst_min(t->right);
            t->key = succ->key; /* 用中序后继替换，再删后继 */
            t->right = bst_delete(t->right, succ->key);
        }
    }
    return t;
}

static void bst_inorder(BSTree t, int *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    bst_inorder(t->left, buf, k);
    buf[(*k)++] = t->key;
    bst_inorder(t->right, buf, k);
}

static void bst_free(BSTree t)
{
    if (t == NULL) {
        return;
    }
    bst_free(t->left);
    bst_free(t->right);
    free(t);
}

static void demo_bst(void)
{
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    BSTree t = NULL;
    int buf[16];
    int k = 0;
    int i;

    ds_section("7.3", "树表的查找");
    ds_section("7.3.1", "二叉排序树");
    ds_note("左子树所有结点小于根，右子树所有结点大于根；中序遍历得到有序序列。");
    ds_note("查找、插入平均 O(log n)，最坏退化成单支树 O(n)。");
    for (i = 0; i < 7; i++) {
        t = bst_insert(t, keys[i]);
    }
    bst_inorder(t, buf, &k);
    printf("  中序遍历：");
    for (i = 0; i < k; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
    ds_check_ll(k, 7, "二叉排序树有 7 个结点");
    for (i = 1; i < k; i++) {
        ds_check_true(buf[i - 1] < buf[i], "中序序列严格递增");
    }
    ds_check_true(bst_search(t, 60), "查找 60 成功");
    ds_check_true(!bst_search(t, 65), "查找 65 失败");
    ds_check_true(bst_search(t, 20), "查找最小值 20 成功");
    ds_check_true(bst_search(t, 80), "查找最大值 80 成功");

    t = bst_delete(t, 30); /* 有两个孩子：用中序后继 40 替换 */
    ds_check_true(!bst_search(t, 30), "删除 30 后查找失败");
    ds_check_true(bst_search(t, 40), "删除 30 后 40 仍在树中");
    k = 0;
    bst_inorder(t, buf, &k);
    ds_check_ll(k, 6, "删除后剩 6 个结点");
    ds_check_ll(buf[1], 40, "中序第二个元素为 40");
    bst_free(t);
}

/* ==================== 7.3.2 平衡二叉树 ==================== */

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode, *AVLTree;

static int avl_height(const AVLNode *t)
{
    return t == NULL ? 0 : t->height;
}

static int max_int(int a, int b)
{
    return a > b ? a : b;
}

static void avl_update(AVLNode *t)
{
    t->height = 1 + max_int(avl_height(t->left), avl_height(t->right));
}

static int avl_balance(const AVLNode *t)
{
    return t == NULL ? 0 : avl_height(t->left) - avl_height(t->right);
}

static AVLNode *rotate_right(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *t2 = x->right;
    x->right = y;
    y->left = t2;
    avl_update(y);
    avl_update(x);
    return x;
}

static AVLNode *rotate_left(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *t2 = y->left;
    y->left = x;
    x->right = t2;
    avl_update(x);
    avl_update(y);
    return y;
}

static AVLTree avl_insert(AVLTree t, int key)
{
    int bf;
    if (t == NULL) {
        AVLTree node = (AVLTree)malloc(sizeof(AVLNode));
        node->key = key;
        node->height = 1;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    if (key < t->key) {
        t->left = avl_insert(t->left, key);
    } else if (key > t->key) {
        t->right = avl_insert(t->right, key);
    } else {
        return t;
    }
    avl_update(t);
    bf = avl_balance(t);
    if (bf > 1 && key < t->left->key) { /* LL */
        return rotate_right(t);
    }
    if (bf < -1 && key > t->right->key) { /* RR */
        return rotate_left(t);
    }
    if (bf > 1 && key > t->left->key) { /* LR */
        t->left = rotate_left(t->left);
        return rotate_right(t);
    }
    if (bf < -1 && key < t->right->key) { /* RL */
        t->right = rotate_right(t->right);
        return rotate_left(t);
    }
    return t;
}

static bool avl_check(const AVLNode *t, int min, int max, int *count)
{
    int bf;
    if (t == NULL) {
        return true;
    }
    if (t->key <= min || t->key >= max) {
        return false;
    }
    bf = avl_balance(t);
    if (bf < -1 || bf > 1) {
        return false;
    }
    (*count)++;
    return avl_check(t->left, min, t->key, count) &&
           avl_check(t->right, t->key, max, count);
}

static void avl_free(AVLTree t)
{
    if (t == NULL) {
        return;
    }
    avl_free(t->left);
    avl_free(t->right);
    free(t);
}

static void demo_avl(void)
{
    int keys[] = {16, 3, 7, 11, 9, 26, 18, 14, 15};
    AVLTree t = NULL;
    int i;
    int count = 0;

    ds_section("7.3.2", "平衡二叉树");
    ds_note("平衡因子 = 左子树高度 - 右子树高度，绝对值不超过 1。");
    ds_note("插入后失衡分 LL、RR、LR、RL 四种，用单旋或双旋恢复平衡。");
    ds_note("删除也可能引起失衡，且可能要沿路径连续旋转；本演示聚焦插入与旋转。");
    for (i = 0; i < 9; i++) {
        t = avl_insert(t, keys[i]);
    }
    ds_check_true(avl_check(t, -10000, 10000, &count), "AVL 树平衡条件成立");
    ds_check_ll(count, 9, "AVL 树含全部 9 个结点");
    ds_check_true(avl_height(t) <= 4, "9 个结点的 AVL 树高度不超过 4");
    ds_ok("AVL 树高度：%d", avl_height(t));
    avl_free(t);
}

/* ==================== 7.3.3 B-树 ==================== */

#define BT_ORDER 4
#define BT_MAX_KEYS (BT_ORDER - 1)

typedef struct BTreeNode {
    int n;                               /* 当前关键字个数 */
    int keys[BT_MAX_KEYS];               /* 最多 3 个关键字 */
    struct BTreeNode *child[BT_ORDER];   /* 最多 4 个孩子 */
    bool leaf;
} BTreeNode;

static BTreeNode *bt_new(bool leaf)
{
    BTreeNode *p = (BTreeNode *)calloc(1, sizeof(BTreeNode));
    p->leaf = leaf;
    return p;
}

static bool bt_search(BTreeNode *t, int key, int *cmp)
{
    while (t != NULL) {
        int i = 0;
        while (i < t->n) {
            (*cmp)++;
            if (key == t->keys[i]) {
                return true;
            }
            if (key < t->keys[i]) {
                break;
            }
            i++;
        }
        if (t->leaf) {
            return false;
        }
        t = t->child[i];
    }
    return false;
}

/* 分裂 parent 的第 i 个孩子（该孩子已有 3 个关键字） */
static void bt_split_child(BTreeNode *parent, int i)
{
    BTreeNode *y = parent->child[i];
    BTreeNode *z = bt_new(y->leaf);
    int j;

    z->n = 1;
    z->keys[0] = y->keys[2];
    if (!y->leaf) {
        z->child[0] = y->child[2];
        z->child[1] = y->child[3];
    }
    for (j = parent->n; j > i; j--) {
        parent->keys[j] = parent->keys[j - 1];
        parent->child[j + 1] = parent->child[j];
    }
    parent->keys[i] = y->keys[1];
    parent->child[i + 1] = z;
    parent->n++;
    y->n = 1; /* 左半保留 keys[0]，孩子 0、1 */
}

static void bt_insert_nonfull(BTreeNode *t, int key)
{
    int i = t->n - 1;
    if (t->leaf) {
        while (i >= 0 && key < t->keys[i]) {
            t->keys[i + 1] = t->keys[i];
            i--;
        }
        t->keys[i + 1] = key;
        t->n++;
        return;
    }
    while (i >= 0 && key < t->keys[i]) {
        i--;
    }
    i++;
    if (t->child[i]->n == BT_MAX_KEYS) {
        bt_split_child(t, i);
        if (key > t->keys[i]) {
            i++;
        }
    }
    bt_insert_nonfull(t->child[i], key);
}

static BTreeNode *bt_insert(BTreeNode *root, int key)
{
    if (root == NULL) {
        root = bt_new(true);
        root->keys[0] = key;
        root->n = 1;
        return root;
    }
    if (root->n == BT_MAX_KEYS) {
        BTreeNode *newroot = bt_new(false);
        newroot->child[0] = root;
        bt_split_child(newroot, 0);
        root = newroot;
    }
    bt_insert_nonfull(root, key);
    return root;
}

static void bt_traverse(BTreeNode *t, int *buf, int *k)
{
    int i;
    if (t == NULL) {
        return;
    }
    for (i = 0; i < t->n; i++) {
        if (!t->leaf) {
            bt_traverse(t->child[i], buf, k);
        }
        buf[(*k)++] = t->keys[i];
    }
    if (!t->leaf) {
        bt_traverse(t->child[t->n], buf, k);
    }
}

static bool bt_validate(BTreeNode *t, int depth, int *leaf_depth, int *nodes)
{
    int i;
    if (t == NULL) {
        return true;
    }
    (*nodes)++;
    for (i = 1; i < t->n; i++) {
        if (t->keys[i - 1] >= t->keys[i]) {
            return false;
        }
    }
    if (t->n < 1 || t->n > BT_MAX_KEYS) {
        return false;
    }
    if (t->leaf) {
        if (*leaf_depth == -1) {
            *leaf_depth = depth;
        }
        return *leaf_depth == depth;
    }
    for (i = 0; i <= t->n; i++) {
        if (t->child[i] == NULL) {
            return false;
        }
        if (!bt_validate(t->child[i], depth + 1, leaf_depth, nodes)) {
            return false;
        }
    }
    return true;
}

static void bt_free(BTreeNode *t)
{
    int i;
    if (t == NULL) {
        return;
    }
    if (!t->leaf) {
        for (i = 0; i <= t->n; i++) {
            bt_free(t->child[i]);
        }
    }
    free(t);
}

static void demo_btree(void)
{
    BTreeNode *root = NULL;
    int buf[32];
    int k = 0;
    int i;
    int cmp = 0;
    int leaf_depth = -1;
    int nodes = 0;

    ds_section("7.3.3", "B-树");
    ds_note("B-树是平衡的多路查找树：每个结点最多 m 棵子树、m-1 个关键字，");
    ds_note("非根结点至少 ceil(m/2)-1 个关键字，所有叶子在同一层。");
    ds_note("本演示取 4 阶 B-树（每个结点最多 3 个关键字），插入 1..12。");
    ds_note("删除时若关键字不足，要向兄弟借或与兄弟合并；思路与插入对称。");
    for (i = 1; i <= 12; i++) {
        root = bt_insert(root, i);
    }
    ds_ok("B-树根结点关键字个数：%d", root->n);
    ds_check_true(root->n >= 1 && root->n <= BT_MAX_KEYS,
                  "根结点关键字个数合法");
    ds_check_true(bt_validate(root, 0, &leaf_depth, &nodes),
                  "B-树结构合法（关键字有序、叶同层、结点不空）");
    k = 0;
    bt_traverse(root, buf, &k);
    ds_check_ll(k, 12, "B-树中序输出 12 个关键字");
    for (i = 1; i < k; i++) {
        ds_check_true(buf[i - 1] < buf[i], "B-树关键字有序");
    }
    for (i = 1; i <= 12; i++) {
        cmp = 0;
        ds_check_true(bt_search(root, i, &cmp), "B-树能查到已插入的关键字");
    }
    cmp = 0;
    ds_check_true(!bt_search(root, 99, &cmp), "B-树查不到 99");
    bt_free(root);
}

/* ==================== 7.3.4 B+树 ==================== */

typedef struct BPNode {
    int n;
    int keys[BT_MAX_KEYS + 1];       /* 分裂时临时容纳第 4 个关键字 */
    struct BPNode *child[BT_ORDER + 1]; /* 分裂时临时容纳第 5 个孩子 */
    struct BPNode *next;             /* 叶子链表 */
    bool leaf;
} BPNode;

static BPNode *bp_new(bool leaf)
{
    BPNode *p = (BPNode *)calloc(1, sizeof(BPNode));
    p->leaf = leaf;
    return p;
}

static BPNode *bp_leaf_of(BPNode *t, int key)
{
    while (t != NULL && !t->leaf) {
        int i = 0;
        while (i < t->n && key >= t->keys[i]) {
            i++;
        }
        t = t->child[i];
    }
    return t;
}

static bool bp_search(BPNode *t, int key)
{
    BPNode *leaf = bp_leaf_of(t, key);
    int i;
    if (leaf == NULL) {
        return false;
    }
    for (i = 0; i < leaf->n; i++) {
        if (leaf->keys[i] == key) {
            return true;
        }
    }
    return false;
}

/* 在 node 的第 child_index 个孩子处插入提升关键字 promote 和右孩子 right */
static bool bp_insert_internal(BPNode *node, int child_index, int promote,
                               BPNode *right, int *out_promote,
                               BPNode **out_right)
{
    int i;
    for (i = node->n; i > child_index; i--) {
        node->keys[i] = node->keys[i - 1];
        node->child[i + 1] = node->child[i];
    }
    node->keys[child_index] = promote;
    node->child[child_index + 1] = right;
    node->n++;
    if (node->n <= BT_MAX_KEYS) {
        return false;
    }
    {
        BPNode *z = bp_new(false);
        /* 左半保留 keys[0..1] 与孩子 0..2；提升 keys[2]；右半只有 keys[3] */
        z->keys[0] = node->keys[3];
        z->child[0] = node->child[3];
        z->child[1] = node->child[4];
        z->n = 1;
        node->n = 2;
        *out_promote = node->keys[2];
        *out_right = z;
    }
    return true;
}

static bool bp_insert_rec(BPNode *node, int key, int *promote, BPNode **right)
{
    int i;
    if (node->leaf) {
        i = node->n - 1;
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n++;
        if (node->n <= BT_MAX_KEYS) {
            return false;
        }
        {
            BPNode *z = bp_new(true);
            z->keys[0] = node->keys[2];
            z->keys[1] = node->keys[3];
            z->n = 2;
            node->n = 2; /* 左叶保留前两个关键字 */
            z->next = node->next;
            node->next = z;
            *promote = z->keys[0];
            *right = z;
        }
        return true;
    }
    i = 0;
    while (i < node->n && key >= node->keys[i]) {
        i++;
    }
    if (bp_insert_rec(node->child[i], key, promote, right)) {
        return bp_insert_internal(node, i, *promote, *right, promote, right);
    }
    return false;
}

static BPNode *bp_insert(BPNode *root, int key)
{
    int promote = 0;
    BPNode *right = NULL;
    if (root == NULL) {
        root = bp_new(true);
        root->keys[0] = key;
        root->n = 1;
        return root;
    }
    if (bp_insert_rec(root, key, &promote, &right)) {
        BPNode *newroot = bp_new(false);
        newroot->keys[0] = promote;
        newroot->child[0] = root;
        newroot->child[1] = right;
        newroot->n = 1;
        root = newroot;
    }
    return root;
}

static void bp_collect(BPNode *t, int *buf, int *k)
{
    int i;
    if (t->leaf) {
        for (i = 0; i < t->n; i++) {
            buf[(*k)++] = t->keys[i];
        }
        return;
    }
    for (i = 0; i <= t->n; i++) {
        bp_collect(t->child[i], buf, k);
    }
}

static bool bp_validate(BPNode *t, int depth, int *leaf_depth, int *leaves)
{
    int i;
    if (t == NULL) {
        return false;
    }
    if (t->n < 1 || t->n > BT_MAX_KEYS) {
        return false;
    }
    for (i = 1; i < t->n; i++) {
        if (t->keys[i - 1] >= t->keys[i]) {
            return false;
        }
    }
    if (t->leaf) {
        if (*leaf_depth == -1) {
            *leaf_depth = depth;
        }
        (*leaves)++;
        return *leaf_depth == depth;
    }
    for (i = 0; i <= t->n; i++) {
        if (t->child[i] == NULL ||
            !bp_validate(t->child[i], depth + 1, leaf_depth, leaves)) {
            return false;
        }
    }
    return true;
}

static void bp_range(BPNode *root, int low, int high, int *out, int *n)
{
    BPNode *leaf = bp_leaf_of(root, low);
    int i;
    *n = 0;
    if (leaf == NULL) {
        return;
    }
    while (leaf != NULL) {
        for (i = 0; i < leaf->n; i++) {
            if (leaf->keys[i] >= low && leaf->keys[i] <= high) {
                out[(*n)++] = leaf->keys[i];
            }
            if (leaf->keys[i] > high) {
                return;
            }
        }
        leaf = leaf->next;
    }
}

static void bp_free(BPNode *t)
{
    int i;
    if (t == NULL) {
        return;
    }
    if (!t->leaf) {
        for (i = 0; i <= t->n; i++) {
            bp_free(t->child[i]);
        }
    }
    free(t);
}

static void demo_bplus_tree(void)
{
    BPNode *root = NULL;
    int buf[32];
    int k = 0;
    int i;
    int leaf_depth = -1;
    int leaves = 0;

    ds_section("7.3.4", "B+树");
    ds_note("B+树所有关键字都出现在叶子，叶子用链表串成有序序列；");
    ds_note("内部结点只作索引，因此支持范围查询和顺序扫描。");
    ds_note("删除同样需要借位或合并；本演示实现查找、插入和范围查询。");
    for (i = 1; i <= 12; i++) {
        root = bp_insert(root, i);
    }
    ds_check_true(bp_validate(root, 0, &leaf_depth, &leaves), "B+树结构合法");
    ds_check_true(leaves >= 2, "B+树有多个叶子结点");
    bp_collect(root, buf, &k);
    ds_check_ll(k, 12, "B+树叶子中共有 12 个关键字");
    for (i = 1; i < k; i++) {
        ds_check_true(buf[i - 1] < buf[i], "B+树叶子关键字有序");
    }
    for (i = 1; i <= 12; i++) {
        ds_check_true(bp_search(root, i), "B+树能查到已插入的关键字");
    }
    ds_check_true(!bp_search(root, 99), "B+树查不到 99");
    {
        int range[32];
        int n = 0;
        bp_range(root, 4, 9, range, &n);
        ds_ok("B+树范围查询 [4,9]：共 %d 个", n);
        ds_check_ll(n, 6, "范围查询返回 4..9 共 6 个关键字");
        ds_check_ll(range[0], 4, "范围查询首元素为 4");
        ds_check_ll(range[n - 1], 9, "范围查询末元素为 9");
    }
    bp_free(root);
}

/* ==================== 7.4 散列表 ==================== */

#define HASH_SIZE 11

typedef struct {
    int key;
    int state; /* 0 空，1 占用，2 已删除 */
} HashEntry;

static int hash_division(int key, int m)
{
    return (key % m + m) % m;
}

static bool hash_insert(HashEntry *table, int key)
{
    int h = hash_division(key, HASH_SIZE);
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        int pos = (h + i) % HASH_SIZE;
        if (table[pos].state != 1) {
            table[pos].key = key;
            table[pos].state = 1;
            return true;
        }
    }
    return false;
}

static bool hash_search(const HashEntry *table, int key, int *cmp)
{
    int h = hash_division(key, HASH_SIZE);
    int i;
    *cmp = 0;
    for (i = 0; i < HASH_SIZE; i++) {
        int pos = (h + i) % HASH_SIZE;
        (*cmp)++;
        if (table[pos].state == 0) {
            return false;
        }
        if (table[pos].state == 1 && table[pos].key == key) {
            return true;
        }
    }
    return false;
}

static bool hash_delete(HashEntry *table, int key)
{
    int h = hash_division(key, HASH_SIZE);
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        int pos = (h + i) % HASH_SIZE;
        if (table[pos].state == 0) {
            return false;
        }
        if (table[pos].state == 1 && table[pos].key == key) {
            table[pos].state = 2; /* 墓碑标记，不能直接置空 */
            return true;
        }
    }
    return false;
}

typedef struct HNode {
    int key;
    struct HNode *next;
} HNode;

static void chain_insert(HNode **table, int m, int key)
{
    int h = hash_division(key, m);
    HNode *p = (HNode *)malloc(sizeof(HNode));
    p->key = key;
    p->next = table[h];
    table[h] = p;
}

static bool chain_search(HNode **table, int m, int key, int *cmp)
{
    int h = hash_division(key, m);
    HNode *p;
    *cmp = 0;
    for (p = table[h]; p != NULL; p = p->next) {
        (*cmp)++;
        if (p->key == key) {
            return true;
        }
    }
    return false;
}

static void demo_hash(void)
{
    int keys[] = {22, 41, 53, 46, 30, 13, 1, 67};
    int n = 8;
    HashEntry table[HASH_SIZE];
    HNode *chain[HASH_SIZE];
    int i;
    int cmp;
    int total = 0;

    ds_section("7.4", "散列表的查找");
    ds_section("7.4.1", "散列表的基本概念");
    ds_note("散列函数把关键字映射为存储地址；不同关键字映到同一地址称冲突。");
    ds_note("装填因子 α = 表中记录数 / 表长，α 越大冲突越多。");

    ds_section("7.4.2", "散列函数的构造方法");
    ds_note("常用方法：直接定址、除留余数、数字分析、平方取中、折叠法。");
    ds_ok("除留余数：1234 %% 11 = %d", hash_division(1234, HASH_SIZE));
    ds_ok("直接定址：关键字 100 存在下标 100（线性函数）");
    ds_ok("折叠法：123456 分成 12+34+56 = %d", 12 + 34 + 56);
    ds_ok("平方取中：567^2 = 321489，取中间两位 14");
    ds_check_ll(hash_division(1234, 11), 2, "1234 除以 11 余 2");

    ds_section("7.4.3", "处理冲突的方法：开放定址法");
    ds_note("线性探测：冲突时依次探测下一个单元；删除时用墓碑标记保留探测链。");
    for (i = 0; i < HASH_SIZE; i++) {
        table[i].state = 0;
        table[i].key = 0;
    }
    for (i = 0; i < n; i++) {
        ds_check_true(hash_insert(table, keys[i]), "线性探测插入成功");
    }
    for (i = 0; i < n; i++) {
        cmp = 0;
        ds_check_true(hash_search(table, keys[i], &cmp), "线性探测能查到已插入关键字");
        total += cmp;
    }
    ds_ok("线性探测成功查找平均比较次数：%.2f", (double)total / n);
    ds_check_ll(total, 14, "成功查找总比较次数为 14");
    ds_check_true(hash_delete(table, 22), "删除 22 成功");
    ds_check_true(!hash_search(table, 22, &cmp), "删除后查不到 22");
    ds_check_true(hash_search(table, 30, &cmp), "删除 22 后仍能查到 30");
    ds_check_true(!hash_search(table, 99, &cmp), "查不到不存在的 99");

    ds_section("7.4.3", "处理冲突的方法：链地址法");
    ds_note("把散列到同一地址的记录串成链表；插入删除方便，装填因子可大于 1。");
    for (i = 0; i < HASH_SIZE; i++) {
        chain[i] = NULL;
    }
    for (i = 0; i < n; i++) {
        chain_insert(chain, HASH_SIZE, keys[i]);
    }
    total = 0;
    for (i = 0; i < n; i++) {
        cmp = 0;
        ds_check_true(chain_search(chain, HASH_SIZE, keys[i], &cmp),
                      "链地址法能查到已插入关键字");
        total += cmp;
    }
    ds_ok("链地址法成功查找平均比较次数：%.2f", (double)total / n);
    ds_check_true(chain_search(chain, HASH_SIZE, 67, &cmp), "链地址法查到 67");
    ds_check_true(!chain_search(chain, HASH_SIZE, 99, &cmp), "链地址法查不到 99");

    ds_section("7.4.4", "散列表的查找");
    ds_note("查找过程与插入一致：先算散列地址，再按同一冲突处理策略探测或查链。");
    ds_note("ASL 取决于散列函数、冲突处理方法和装填因子；α 控制在 0.75 左右较好。");
    ds_check_true(hash_search(table, 67, &cmp), "开放定址表查到 67");
    ds_check_true(chain_search(chain, HASH_SIZE, 67, &cmp), "链地址表查到 67");
    for (i = 0; i < HASH_SIZE; i++) {
        HNode *p = chain[i];
        while (p != NULL) {
            HNode *q = p->next;
            free(p);
            p = q;
        }
    }
}

int ds_ch07(void)
{
    demo_concepts();
    demo_seq_search();
    demo_binary_search();
    demo_block_search();
    demo_bst();
    demo_avl();
    demo_btree();
    demo_bplus_tree();
    demo_hash();
    ds_section("7.5", "小结");
    ds_note("线性表查找有顺序、折半、分块三种；折半要求有序，分块要求块间有序。");
    ds_note("树表查找用二叉排序树、平衡二叉树、B-树和 B+树，兼顾动态维护；");
    ds_note("散列查找用散列函数与冲突处理换取平均 O(1) 的查找速度。");
    return 0;
}
