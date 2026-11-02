/*
 * ch05_tree.c —— 第 5 章 树和二叉树
 *
 * 覆盖目录：
 *   5.1 树和二叉树的定义        5.2 案例引入
 *   5.3 树和二叉树的抽象数据类型定义
 *   5.4 二叉树的性质和存储结构  5.5 遍历二叉树和线索二叉树
 *   5.6 树和森林（存储、转换、遍历）
 *   5.7 哈夫曼树及其应用        5.8 案例分析与实现（表达式树）  5.9 小结
 */
#include "common.h"

/* ==================== 5.1 / 5.3 二叉树的定义 ==================== */

typedef char TElemType;

typedef struct BiTNode {
    TElemType data;
    struct BiTNode *lchild;
    struct BiTNode *rchild;
} BiTNode, *BiTree;

static BiTree bi_new(char c)
{
    BiTree t = (BiTree)malloc(sizeof(BiTNode));
    t->data = c;
    t->lchild = NULL;
    t->rchild = NULL;
    return t;
}

/* 按先序序列建二叉树，'#' 表示空子树 */
static BiTree CreateBiTree(const char **p)
{
    BiTree t;
    if (**p == '#' || **p == '\0') {
        if (**p == '#') {
            (*p)++;
        }
        return NULL;
    }
    t = bi_new(**p);
    (*p)++;
    t->lchild = CreateBiTree(p);
    t->rchild = CreateBiTree(p);
    return t;
}

static void bi_free(BiTree t)
{
    if (t == NULL) {
        return;
    }
    bi_free(t->lchild);
    bi_free(t->rchild);
    free(t);
}

static void preorder(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    buf[(*k)++] = t->data;
    preorder(t->lchild, buf, k);
    preorder(t->rchild, buf, k);
}

static void inorder(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    inorder(t->lchild, buf, k);
    buf[(*k)++] = t->data;
    inorder(t->rchild, buf, k);
}

static void postorder(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    postorder(t->lchild, buf, k);
    postorder(t->rchild, buf, k);
    buf[(*k)++] = t->data;
}

static int bi_nodes(BiTree t)
{
    if (t == NULL) {
        return 0;
    }
    return 1 + bi_nodes(t->lchild) + bi_nodes(t->rchild);
}

static int bi_leaves(BiTree t)
{
    if (t == NULL) {
        return 0;
    }
    if (t->lchild == NULL && t->rchild == NULL) {
        return 1;
    }
    return bi_leaves(t->lchild) + bi_leaves(t->rchild);
}

static int bi_two_children(BiTree t)
{
    int n = 0;
    if (t == NULL) {
        return 0;
    }
    if (t->lchild != NULL && t->rchild != NULL) {
        n = 1;
    }
    return n + bi_two_children(t->lchild) + bi_two_children(t->rchild);
}

static int bi_depth(BiTree t)
{
    int dl, dr;
    if (t == NULL) {
        return 0;
    }
    dl = bi_depth(t->lchild);
    dr = bi_depth(t->rchild);
    return 1 + (dl > dr ? dl : dr);
}

static void level_order(BiTree t, char *buf, int *k)
{
    BiTree q[32];
    int front = 0;
    int rear = 0;
    if (t != NULL) {
        q[rear++] = t;
    }
    while (front < rear) {
        BiTree p = q[front++];
        buf[(*k)++] = p->data;
        if (p->lchild != NULL) {
            q[rear++] = p->lchild;
        }
        if (p->rchild != NULL) {
            q[rear++] = p->rchild;
        }
    }
}

static void inorder_nonrecursive(BiTree t, char *buf, int *k)
{
    BiTree stack[32];
    int top = 0;
    BiTree p = t;
    while (p != NULL || top > 0) {
        while (p != NULL) {
            stack[top++] = p;
            p = p->lchild;
        }
        p = stack[--top];
        buf[(*k)++] = p->data;
        p = p->rchild;
    }
}

static void demo_tree_basics(void)
{
    const char *s = "ABD##E##C##";
    const char *p = s;
    BiTree root = CreateBiTree(&p);
    char pre[32], in[32], post[32], level[32];
    int k = 0;

    ds_section("5.1", "树和二叉树的定义");
    ds_note("树和二叉树都是层次结构，二叉树是每个结点至多两个孩子的特例。");
    ds_section("5.1.1", "树的定义");
    ds_note("树是 n 个结点的有限集，有且仅有一个根，其余结点分成若干子树。");
    ds_section("5.1.2", "树的基本术语");
    ds_note("术语：结点的度、树的度、叶子、层次、深度、路径、森林。");
    ds_section("5.1.3", "二叉树的定义");
    ds_note("二叉树每个结点至多两棵子树，且左右子树有顺序，不能互换。");

    ds_section("5.2", "案例引入");
    ds_note("文件目录、表达式、家谱都是树形结构；用二叉树可以统一表示它们。");
    ds_note("以下示例二叉树：A(B(D,E), C)，先序序列 ABD##E##C##。");

    ds_section("5.3", "树和二叉树的抽象数据类型定义");
    ds_note("ADT BiTree：InitBiTree、DestroyBiTree、CreateBiTree、BiTreeEmpty、");
    ds_note("BiTreeDepth、Root、Value、Assign、Parent、LeftChild、RightChild、");
    ds_note("InsertChild、DeleteChild、Traverse。");

    ds_section("5.4", "二叉树的性质和存储结构");
    ds_section("5.4.1", "二叉树的性质");
    ds_note("性质：第 i 层至多 2^(i-1) 个结点；深度为 k 的二叉树至多 2^k-1 个结点；");
    ds_note("对任何二叉树，叶子数 n0 = 度为 2 的结点数 n2 + 1。");
    ds_check_ll(bi_nodes(root), 5, "二叉树共有 5 个结点");
    ds_check_ll(bi_leaves(root), 3, "二叉树有 3 个叶子");
    ds_check_ll(bi_two_children(root), 2, "二叉树有 2 个度为 2 的结点");
    ds_check_ll(bi_leaves(root), bi_two_children(root) + 1, "n0 = n2 + 1");
    ds_check_ll(bi_depth(root), 3, "二叉树深度为 3");

    ds_section("5.4.2", "二叉树的存储结构");
    ds_note("顺序存储：按完全二叉树编号，空结点用特殊值占位，适合完全二叉树。");
    {
        char seq[16];
        int i;
        memset(seq, 0, sizeof(seq));
        seq[1] = 'A';
        seq[2] = 'B';
        seq[3] = 'C';
        seq[4] = 'D';
        seq[5] = 'E';
        printf("  顺序存储下标 1..7：");
        for (i = 1; i <= 7; i++) {
            printf("%c%s", seq[i] == 0 ? '_' : seq[i], i == 7 ? "" : " ");
        }
        printf("\n");
        ds_check_ll(seq[1], 'A', "根结点 A 在下标 1");
        ds_check_ll(seq[2], 'B', "A 的左孩子 B 在下标 2");
        ds_check_ll(seq[3], 'C', "A 的右孩子 C 在下标 3");
        ds_note("链式存储：每个结点含 data、lchild、rchild 三个域。");
    }

    ds_section("5.5", "遍历二叉树和线索二叉树");
    ds_section("5.5.1", "遍历二叉树");
    ds_note("先序：根—左—右；中序：左—根—右；后序：左—右—根；层序：逐层。");
    k = 0;
    preorder(root, pre, &k);
    pre[k] = '\0';
    k = 0;
    inorder(root, in, &k);
    in[k] = '\0';
    k = 0;
    postorder(root, post, &k);
    post[k] = '\0';
    k = 0;
    level_order(root, level, &k);
    level[k] = '\0';
    ds_ok("先序遍历：%s", pre);
    ds_ok("中序遍历：%s", in);
    ds_ok("后序遍历：%s", post);
    ds_ok("层序遍历：%s", level);
    ds_check_str(pre, "ABDEC", "先序序列为 ABDEC");
    ds_check_str(in, "DBEAC", "中序序列为 DBEAC");
    ds_check_str(post, "DEBCA", "后序序列为 DEBCA");
    ds_check_str(level, "ABCDE", "层序序列为 ABCDE");

    k = 0;
    inorder_nonrecursive(root, in, &k);
    in[k] = '\0';
    ds_ok("非递归中序（显式栈）：%s", in);
    ds_check_str(in, "DBEAC", "非递归中序与递归中序一致");
    bi_free(root);
}

/* ==================== 5.5.2 线索二叉树 ==================== */

typedef struct ThrNode {
    char data;
    struct ThrNode *lchild;
    struct ThrNode *rchild;
    int ltag; /* 0 表示孩子，1 表示前驱线索 */
    int rtag; /* 0 表示孩子，1 表示后继线索 */
} ThrNode, *ThrBiTree;

static ThrBiTree thr_new(char c)
{
    ThrBiTree t = (ThrBiTree)malloc(sizeof(ThrNode));
    t->data = c;
    t->lchild = NULL;
    t->rchild = NULL;
    t->ltag = 0;
    t->rtag = 0;
    return t;
}

static ThrBiTree ThrCreate(const char **p)
{
    ThrBiTree t;
    if (**p == '#' || **p == '\0') {
        if (**p == '#') {
            (*p)++;
        }
        return NULL;
    }
    t = thr_new(**p);
    (*p)++;
    t->lchild = ThrCreate(p);
    t->rchild = ThrCreate(p);
    return t;
}

static void InThreading(ThrBiTree p, ThrBiTree *pre)
{
    if (p == NULL) {
        return;
    }
    InThreading(p->lchild, pre);
    if (p->lchild == NULL) {
        p->ltag = 1;
        p->lchild = *pre;
    }
    if (*pre != NULL && (*pre)->rchild == NULL) {
        (*pre)->rtag = 1;
        (*pre)->rchild = p;
    }
    *pre = p;
    InThreading(p->rchild, pre);
}

static ThrBiTree InFirst(ThrBiTree p)
{
    while (p != NULL && p->ltag == 0) {
        p = p->lchild;
    }
    return p;
}

static ThrBiTree InNext(ThrBiTree p)
{
    if (p->rtag == 1) {
        return p->rchild;
    }
    return InFirst(p->rchild);
}

static void ThrFree(ThrBiTree t)
{
    if (t == NULL) {
        return;
    }
    if (t->ltag == 0) {
        ThrFree(t->lchild);
    }
    if (t->rtag == 0) {
        ThrFree(t->rchild);
    }
    free(t);
}

static void demo_threaded_tree(void)
{
    const char *s = "ABD##E##C##";
    const char *p = s;
    ThrBiTree root = ThrCreate(&p);
    ThrBiTree pre = NULL;
    ThrBiTree q;
    char buf[32];
    int k = 0;

    ds_section("5.5.2", "线索二叉树");
    ds_note("利用空指针域存放前驱/后继线索，使遍历不需要栈、也不需要递归。");
    ds_note("ltag=1 时 lchild 指前驱，rtag=1 时 rchild 指后继。");
    InThreading(root, &pre);
    for (q = InFirst(root); q != NULL; q = InNext(q)) {
        buf[k++] = q->data;
    }
    buf[k] = '\0';
    ds_ok("中序线索化后遍历：%s", buf);
    ds_check_str(buf, "DBEAC", "线索遍历结果与中序一致");
    ds_check_ll(InFirst(root)->data, 'D', "中序第一个结点是 D");
    ds_check_ll(root->ltag, 0, "根结点 A 有左孩子，ltag=0");
    ds_check_true(InNext(InFirst(root)) != NULL, "D 的后继线索存在");
    ThrFree(root);
}

/* ==================== 5.6 树和森林 ==================== */

typedef struct {
    char data;
    int parent;
} PTNode;

typedef struct {
    PTNode nodes[16];
    int n;
} PTree;

typedef struct CSNode {
    char data;
    struct CSNode *firstchild;
    struct CSNode *nextsibling;
} CSNode, *CSTree;

static CSNode *cs_new(char c)
{
    CSNode *p = (CSNode *)malloc(sizeof(CSNode));
    p->data = c;
    p->firstchild = NULL;
    p->nextsibling = NULL;
    return p;
}

static void cs_free(CSTree t)
{
    if (t == NULL) {
        return;
    }
    cs_free(t->firstchild);
    cs_free(t->nextsibling);
    free(t);
}

static void tree_preorder(CSTree t, char *buf, int *k)
{
    CSNode *c;
    if (t == NULL) {
        return;
    }
    buf[(*k)++] = t->data;
    for (c = t->firstchild; c != NULL; c = c->nextsibling) {
        tree_preorder(c, buf, k);
    }
}

static void tree_postorder(CSTree t, char *buf, int *k)
{
    CSNode *c;
    if (t == NULL) {
        return;
    }
    for (c = t->firstchild; c != NULL; c = c->nextsibling) {
        tree_postorder(c, buf, k);
    }
    buf[(*k)++] = t->data;
}

/* 森林是若干棵树的根结点用 nextsibling 串成的链 */
static void forest_preorder(CSTree t, char *buf, int *k)
{
    for (; t != NULL; t = t->nextsibling) {
        tree_preorder(t, buf, k);
    }
}

static void forest_postorder(CSTree t, char *buf, int *k)
{
    for (; t != NULL; t = t->nextsibling) {
        tree_postorder(t, buf, k);
    }
}

/* 孩子—兄弟表示转二叉树：左指针指第一个孩子，右指针指下一个兄弟 */
static BiTree cs_to_binary(CSTree t)
{
    BiTree b;
    if (t == NULL) {
        return NULL;
    }
    b = bi_new(t->data);
    b->lchild = cs_to_binary(t->firstchild);
    b->rchild = cs_to_binary(t->nextsibling);
    return b;
}

static void demo_tree_forest(void)
{
    PTree pt;
    CSNode *A, *B, *C, *D, *E, *F, *G;
    CSNode *FA, *FB, *FC, *FD, *FE;
    char buf[32];
    int k;
    BiTree b;
    int i;

    ds_section("5.6", "树和森林");
    ds_section("5.6.1", "树的存储结构");
    ds_note("双亲表示法：每个结点记下双亲下标，找双亲 O(1)，找孩子需扫描。");
    pt.n = 7;
    pt.nodes[0] = (PTNode){'A', -1};
    pt.nodes[1] = (PTNode){'B', 0};
    pt.nodes[2] = (PTNode){'C', 0};
    pt.nodes[3] = (PTNode){'D', 0};
    pt.nodes[4] = (PTNode){'E', 1};
    pt.nodes[5] = (PTNode){'F', 1};
    pt.nodes[6] = (PTNode){'G', 2};
    printf("  双亲表示法（结点,双亲下标）：");
    for (i = 0; i < pt.n; i++) {
        printf("(%c,%d)%s", pt.nodes[i].data, pt.nodes[i].parent,
               i + 1 == pt.n ? "" : " ");
    }
    printf("\n");
    ds_check_ll(pt.nodes[4].parent, 1, "E 的双亲是 B");
    ds_check_ll(pt.nodes[6].parent, 2, "G 的双亲是 C");

    ds_note("孩子—兄弟表示法：firstchild 指第一个孩子，nextsibling 指下一个兄弟。");
    A = cs_new('A');
    B = cs_new('B');
    C = cs_new('C');
    D = cs_new('D');
    E = cs_new('E');
    F = cs_new('F');
    G = cs_new('G');
    A->firstchild = B;
    B->nextsibling = C;
    C->nextsibling = D;
    B->firstchild = E;
    E->nextsibling = F;
    C->firstchild = G;

    k = 0;
    tree_preorder(A, buf, &k);
    buf[k] = '\0';
    ds_ok("树 A(B(E,F),C(G),D) 的先根遍历：%s", buf);
    ds_check_str(buf, "ABEFCGD", "树的先根遍历为 ABEFCGD");
    k = 0;
    tree_postorder(A, buf, &k);
    buf[k] = '\0';
    ds_ok("树的后根遍历：%s", buf);
    ds_check_str(buf, "EFBGCDA", "树的后根遍历为 EFBGCDA");

    ds_section("5.6.2", "森林与二叉树的转换");
    ds_note("树转二叉树：左孩子为原树的第一个孩子，右孩子为原树的下一个兄弟。");
    b = cs_to_binary(A);
    k = 0;
    preorder(b, buf, &k);
    buf[k] = '\0';
    ds_ok("转换后二叉树先序：%s", buf);
    ds_check_str(buf, "ABEFCGD", "二叉树先序等于树的先根遍历");
    k = 0;
    inorder(b, buf, &k);
    buf[k] = '\0';
    ds_ok("转换后二叉树中序：%s", buf);
    ds_check_str(buf, "EFBGCDA", "二叉树中序等于树的后根遍历");
    bi_free(b);
    cs_free(A);

    ds_section("5.6.3", "树和森林的遍历");
    ds_note("森林的先序遍历等于各棵树先根遍历；后序遍历等于各棵树后根遍历。");
    FA = cs_new('A');
    FB = cs_new('B');
    FC = cs_new('C');
    FD = cs_new('D');
    FE = cs_new('E');
    FA->firstchild = FB;
    FB->nextsibling = FC;
    FD->firstchild = FE;
    FA->nextsibling = FD;
    k = 0;
    forest_preorder(FA, buf, &k);
    buf[k] = '\0';
    ds_ok("森林 {A(B,C), D(E)} 的先序：%s", buf);
    ds_check_str(buf, "ABCDE", "森林先序遍历为 ABCDE");
    k = 0;
    forest_postorder(FA, buf, &k);
    buf[k] = '\0';
    ds_ok("该森林的后序：%s", buf);
    ds_check_str(buf, "BCAED", "森林后序遍历为 BCAED");
    cs_free(FA);
}

/* ==================== 5.7 哈夫曼树 ==================== */

typedef struct {
    int weight;
    int parent;
    int lchild;
    int rchild;
} HTNode;

/* 在 1..n 中选出 parent 为 0 且权值最小的两个结点 */
static void huff_select(const HTNode *ht, int n, int *s1, int *s2)
{
    int i;
    int min1 = -1;
    int min2 = -1;
    for (i = 1; i <= n; i++) {
        if (ht[i].parent != 0) {
            continue;
        }
        if (min1 == -1 || ht[i].weight < ht[min1].weight) {
            min2 = min1;
            min1 = i;
        } else if (min2 == -1 || ht[i].weight < ht[min2].weight) {
            min2 = i;
        }
    }
    *s1 = min1;
    *s2 = min2;
}

static void huffman_build(int *w, int n, HTNode **out)
{
    int m = 2 * n - 1;
    HTNode *ht = (HTNode *)calloc((size_t)m + 1, sizeof(HTNode));
    int i;
    for (i = 1; i <= n; i++) {
        ht[i].weight = w[i - 1];
    }
    for (i = n + 1; i <= m; i++) {
        int s1, s2;
        huff_select(ht, i - 1, &s1, &s2);
        ht[s1].parent = i;
        ht[s2].parent = i;
        ht[i].lchild = s1;
        ht[i].rchild = s2;
        ht[i].weight = ht[s1].weight + ht[s2].weight;
    }
    *out = ht;
}

/* 叶结点 i 的编码写入 code（从根到叶，左 0 右 1） */
static void huffman_code(const HTNode *ht, int i, char *code)
{
    char tmp[32];
    int start = 31;
    int c = i;
    tmp[start] = '\0';
    while (ht[c].parent != 0) {
        int par = ht[c].parent;
        tmp[--start] = (ht[par].lchild == c) ? '0' : '1';
        c = par;
    }
    strcpy(code, &tmp[start]);
}

static int huffman_wpl(const HTNode *ht, int n)
{
    int i;
    int wpl = 0;
    for (i = 1; i <= n; i++) {
        int depth = 0;
        int c = i;
        while (ht[c].parent != 0) {
            depth++;
            c = ht[c].parent;
        }
        wpl += depth * ht[i].weight;
    }
    return wpl;
}

static void demo_huffman(void)
{
    int w[] = {5, 29, 7, 8, 14, 23, 3, 11};
    int n = 8;
    HTNode *ht = NULL;
    char codes[9][32];
    char encoded[512];
    int i;
    int k = 0;

    ds_section("5.7", "哈夫曼树及其应用");
    ds_section("5.7.1", "哈夫曼树的基本概念");
    ds_note("带权路径长度 WPL 最小的二叉树叫哈夫曼树，权值大的叶子离根更近。");

    ds_section("5.7.2", "哈夫曼树的构造算法");
    ds_note("每次从森林中选两棵根权最小的树合并，新根权为两者之和，共合并 n-1 次。");
    huffman_build(w, n, &ht);
    ds_check_ll(huffman_wpl(ht, n), 271, "示例权值的哈夫曼树 WPL = 271");
    ds_ok("8 个权值 {5,29,7,8,14,23,3,11} 构造完成，共 %d 个结点", 2 * n - 1);

    ds_section("5.7.3", "哈夫曼编码");
    ds_note("从根到叶，左分支记 0、右分支记 1，得到前缀编码，任一编码不是");
    ds_note("另一编码的前缀，因此解码无歧义。");
    for (i = 1; i <= n; i++) {
        huffman_code(ht, i, codes[i]);
        ds_ok("权值 %2d（字符 %c）的编码：%s", w[i - 1], 'a' + i - 1,
              codes[i]);
        ds_check_true(strlen(codes[i]) > 0, "编码非空");
    }
    for (i = 0; i < n; i++) {
        strcpy(encoded + k, codes[i + 1]);
        k += (int)strlen(codes[i + 1]);
    }
    encoded[k] = '\0';
    ds_ok("把 abcdefgh 编码为 %d 位二进制串", (int)strlen(encoded));

    {
        int root = 2 * n - 1;
        int node = root;
        char decoded[16];
        int dk = 0;
        for (i = 0; i < k; i++) {
            node = encoded[i] == '0' ? ht[node].lchild : ht[node].rchild;
            if (ht[node].lchild == 0 && ht[node].rchild == 0) {
                decoded[dk++] = (char)('a' + node - 1);
                node = root;
            }
        }
        decoded[dk] = '\0';
        ds_ok("解码结果：%s", decoded);
        ds_check_str(decoded, "abcdefgh", "哈夫曼编码解码正确");
    }
    free(ht);
}

/* ==================== 5.8 表达式树 ==================== */

static bool is_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

static int eval_expr(BiTree t)
{
    if (t == NULL) {
        return 0;
    }
    if (!is_operator(t->data)) {
        return t->data - '0';
    }
    {
        int a = eval_expr(t->lchild);
        int b = eval_expr(t->rchild);
        switch (t->data) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return b == 0 ? 0 : a / b;
        default:
            return 0;
        }
    }
}

static void expr_prefix(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    buf[(*k)++] = t->data;
    expr_prefix(t->lchild, buf, k);
    expr_prefix(t->rchild, buf, k);
}

static void expr_infix(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    if (!is_operator(t->data)) {
        buf[(*k)++] = t->data;
        return;
    }
    buf[(*k)++] = '(';
    expr_infix(t->lchild, buf, k);
    buf[(*k)++] = t->data;
    expr_infix(t->rchild, buf, k);
    buf[(*k)++] = ')';
}

static void expr_postfix(BiTree t, char *buf, int *k)
{
    if (t == NULL) {
        return;
    }
    expr_postfix(t->lchild, buf, k);
    expr_postfix(t->rchild, buf, k);
    buf[(*k)++] = t->data;
}

static BiTree build_expr_tree(const char *postfix)
{
    BiTree stack[32];
    int top = 0;
    const char *p = postfix;
    while (*p != '\0') {
        while (*p == ' ') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        if (is_operator(*p)) {
            BiTree t = bi_new(*p);
            if (top < 2) {
                bi_free(t);
                return NULL;
            }
            t->rchild = stack[--top];
            t->lchild = stack[--top];
            stack[top++] = t;
        } else {
            stack[top++] = bi_new(*p);
        }
        p++;
    }
    return top == 1 ? stack[0] : NULL;
}

static void demo_expression_tree(void)
{
    BiTree root = build_expr_tree("3 4 + 2 *");
    char buf[64];
    int k;

    ds_section("5.8", "案例分析与实现：用二叉树表示表达式");
    ds_note("操作数为叶子，运算符为内部结点；后序遍历求值，先序得前缀式，");
    ds_note("中序加括号得中缀式。");
    ds_check_true(root != NULL, "由后缀式 3 4 + 2 * 建树成功");
    if (root == NULL) {
        return;
    }
    k = 0;
    expr_prefix(root, buf, &k);
    buf[k] = '\0';
    ds_ok("前缀表达式：%s", buf);
    k = 0;
    expr_infix(root, buf, &k);
    buf[k] = '\0';
    ds_ok("中缀表达式：%s", buf);
    k = 0;
    expr_postfix(root, buf, &k);
    buf[k] = '\0';
    ds_ok("后缀表达式：%s", buf);
    ds_check_str(buf, "34+2*", "后缀表达式为 34+2*");
    ds_check_ll(eval_expr(root), 14, "表达式求值 (3+4)*2 = 14");
    bi_free(root);
}

int ds_ch05(void)
{
    demo_tree_basics();
    demo_threaded_tree();
    demo_tree_forest();
    demo_huffman();
    demo_expression_tree();
    ds_section("5.9", "小结");
    ds_note("二叉树的链式存储与四种遍历是本章基础；线索二叉树把空指针变成");
    ds_note("线索，遍历无需栈。树、森林与二叉树可以相互转换；哈夫曼树给出");
    ds_note("最优前缀编码。表达式树把树结构与表达式求值联系起来。");
    return 0;
}
