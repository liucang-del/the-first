/*
 * ch02_list.c —— 第 2 章 线性表
 *
 * 覆盖目录：
 *   2.1 线性表的定义和特点      2.2 案例引入
 *   2.3 线性表的类型定义        2.4 顺序表示和实现（2.4.1、2.4.2）
 *   2.5 链式表示和实现（单链表、循环链表、双向链表）
 *   2.6 顺序表和链表的比较      2.7 线性表的应用（合并）
 *   2.8 案例分析与实现（一元多项式相加）  2.9 小结
 */
#include "common.h"

/* ==================== 2.4 顺序表 ==================== */

#define SQ_INIT_SIZE 8

typedef struct {
    int *elem;   /* 动态数组基址 */
    int length;  /* 当前表长 */
    int listsize;/* 当前分配的存储容量 */
} SqList;

static bool SqInit(SqList *L)
{
    L->elem = (int *)malloc(sizeof(int) * SQ_INIT_SIZE);
    if (L->elem == NULL) {
        return false;
    }
    L->length = 0;
    L->listsize = SQ_INIT_SIZE;
    return true;
}

static void SqDestroy(SqList *L)
{
    free(L->elem);
    L->elem = NULL;
    L->length = 0;
    L->listsize = 0;
}

static bool SqGet(const SqList *L, int i, int *e)
{
    if (i < 1 || i > L->length) {
        return false; /* 越界检查，体现健壮性 */
    }
    *e = L->elem[i - 1];
    return true;
}

static int SqLocate(const SqList *L, int e)
{
    int i;
    for (i = 0; i < L->length; i++) {
        if (L->elem[i] == e) {
            return i + 1; /* 返回 1 基位序，0 表示不存在 */
        }
    }
    return 0;
}

static bool SqInsert(SqList *L, int i, int e)
{
    int j;
    if (i < 1 || i > L->length + 1) {
        return false;
    }
    if (L->length >= L->listsize) { /* 空间不够时扩容 */
        int *newbase = (int *)realloc(L->elem,
                                      sizeof(int) * (size_t)L->listsize * 2);
        if (newbase == NULL) {
            return false;
        }
        L->elem = newbase;
        L->listsize *= 2;
    }
    for (j = L->length - 1; j >= i - 1; j--) {
        L->elem[j + 1] = L->elem[j]; /* 元素后移 */
    }
    L->elem[i - 1] = e;
    L->length++;
    return true;
}

static bool SqDelete(SqList *L, int i, int *e)
{
    int j;
    if (i < 1 || i > L->length) {
        return false;
    }
    *e = L->elem[i - 1];
    for (j = i; j < L->length; j++) {
        L->elem[j - 1] = L->elem[j]; /* 元素前移 */
    }
    L->length--;
    return true;
}

static void print_sq(const SqList *L, const char *name)
{
    int i;
    printf("  %s = [", name);
    for (i = 0; i < L->length; i++) {
        printf("%s%d", i == 0 ? "" : " ", L->elem[i]);
    }
    printf("]\n");
}

static void demo_seq_list(void)
{
    SqList L;
    int e = 0;
    int i;

    ds_section("2.1", "线性表的定义和特点");
    ds_note("线性表是 n 个数据元素的有限序列，元素之间是一对一的线性关系。");
    ds_note("除首尾外，每个元素有且仅有一个前驱和一个后继。");

    ds_section("2.2", "案例引入");
    ds_note("两个班级的学生名单合并、一元多项式相加，都可以用线性表表示。");

    ds_section("2.3", "线性表的类型定义");
    ds_note("ADT List { 数据对象、数据关系、基本操作 }：初始化、插入、删除、");
    ds_note("查找、取元素、求长度、遍历、销毁。以下用 C 的结构体与函数实现。");

    ds_section("2.4", "线性表的顺序表示和实现");
    ds_section("2.4.1", "线性表的顺序表示");
    ds_note("用一组地址连续的存储单元依次存放元素，逻辑相邻即物理相邻。");
    ds_note("第 i 个元素地址 = 基址 + (i-1)*sizeof(ElemType)，随机存取 O(1)。");

    ds_section("2.4.2", "顺序表中基本操作的实现");
    ds_check_true(SqInit(&L), "InitList 初始化成功");
    for (i = 1; i <= 10; i++) {
        if (!SqInsert(&L, i, i * 10)) {
            ds_check_true(false, "顺序表插入失败");
            break;
        }
    }
    ds_check_ll(L.length, 10, "插入 10 个元素后表长为 10");
    ds_check_ll(L.listsize >= 10, 1, "表满后自动扩容");
    print_sq(&L, "L");

    ds_check_true(SqGet(&L, 4, &e), "GetElem(L,4) 成功");
    ds_check_ll(e, 40, "第 4 个元素是 40");
    ds_check_ll(SqLocate(&L, 70), 7, "LocateElem(L,70) 返回位序 7");
    ds_check_ll(SqLocate(&L, 55), 0, "不存在的元素返回 0");

    ds_check_true(SqInsert(&L, 3, 25), "在第 3 位插入 25");
    ds_check_true(SqGet(&L, 3, &e) && e == 25, "插入后第 3 个元素是 25");
    ds_check_true(SqDelete(&L, 5, &e), "删除第 5 个元素");
    ds_check_ll(e, 40, "被删除的元素是 40");
    ds_check_ll(L.length, 10, "插入并删除后表长仍为 10");
    print_sq(&L, "L");
    ds_check_true(!SqInsert(&L, 0, 1), "在位序 0 插入被拒绝");
    ds_check_true(!SqDelete(&L, 11, &e), "删除越界位序被拒绝");
    SqDestroy(&L);
}

/* ==================== 2.5.1 / 2.5.2 单链表 ==================== */

typedef struct LNode {
    int data;
    struct LNode *next;
} LNode, *LinkList;

/* 尾插法建表，带表头结点；返回表头指针，失败返回 NULL */
static LinkList LL_CreateTail(const int *a, int n)
{
    LinkList L = (LinkList)malloc(sizeof(LNode));
    LNode *r;
    int i;
    if (L == NULL) {
        return NULL;
    }
    L->next = NULL;
    r = L;
    for (i = 0; i < n; i++) {
        LNode *p = (LNode *)malloc(sizeof(LNode));
        if (p == NULL) {
            return NULL; /* 示例程序不在此处回收，实际应逐个释放 */
        }
        p->data = a[i];
        p->next = NULL;
        r->next = p;
        r = p;
    }
    return L;
}

static void LL_Destroy(LinkList L)
{
    LNode *p = L;
    while (p != NULL) {
        LNode *q = p->next;
        free(p);
        p = q;
    }
}

static int LL_Length(LinkList L)
{
    int n = 0;
    LNode *p = L->next;
    while (p != NULL) {
        n++;
        p = p->next;
    }
    return n;
}

static LNode *LL_Locate(LinkList L, int e)
{
    LNode *p = L->next;
    while (p != NULL && p->data != e) {
        p = p->next;
    }
    return p;
}

static bool LL_Insert(LinkList L, int i, int e)
{
    int j = 0;
    LNode *p = L;
    LNode *s;
    if (i < 1) {
        return false;
    }
    while (p != NULL && j < i - 1) {
        p = p->next;
        j++;
    }
    if (p == NULL) {
        return false;
    }
    s = (LNode *)malloc(sizeof(LNode));
    if (s == NULL) {
        return false;
    }
    s->data = e;
    s->next = p->next;
    p->next = s;
    return true;
}

static bool LL_Delete(LinkList L, int i, int *e)
{
    int j = 0;
    LNode *p = L;
    LNode *q;
    if (i < 1) {
        return false;
    }
    while (p->next != NULL && j < i - 1) {
        p = p->next;
        j++;
    }
    if (p->next == NULL) {
        return false;
    }
    q = p->next;
    *e = q->data;
    p->next = q->next;
    free(q);
    return true;
}

static void print_ll(LinkList L, const char *name)
{
    LNode *p = L->next;
    printf("  %s = (", name);
    while (p != NULL) {
        printf("%d%s", p->data, p->next == NULL ? "" : " -> ");
        p = p->next;
    }
    printf(")\n");
}

static void demo_linked_list(void)
{
    int a[] = {10, 20, 30, 40};
    LinkList L;
    int e = 0;

    ds_section("2.5", "线性表的链式表示和实现");
    ds_section("2.5.1", "单链表的定义和表示");
    ds_note("每个结点由数据域和指针域组成，指针指向后继；表头结点不存数据。");
    ds_note("单链表只能顺序存取，按位序查找需从头遍历，时间 O(n)。");

    ds_section("2.5.2", "单链表基本操作的实现");
    L = LL_CreateTail(a, 4);
    ds_check_true(L != NULL, "尾插法建立单链表");
    if (L == NULL) {
        return;
    }
    print_ll(L, "L");
    ds_check_ll(LL_Length(L), 4, "单链表长度为 4");
    ds_check_true(LL_Locate(L, 30) != NULL, "按值查找 30 成功");
    ds_check_true(LL_Locate(L, 99) == NULL, "查找不存在的 99 返回 NULL");

    ds_check_true(LL_Insert(L, 2, 15), "在第 2 位插入 15");
    print_ll(L, "L");
    ds_check_true(LL_Delete(L, 3, &e), "删除第 3 个结点");
    ds_check_ll(e, 20, "被删除的结点数据为 20");
    print_ll(L, "L");
    ds_check_ll(LL_Length(L), 4, "插入并删除后长度仍为 4");
    ds_check_true(!LL_Delete(L, 9, &e), "删除越界位序被拒绝");
    LL_Destroy(L);
}

/* ==================== 2.5.3 循环链表 ==================== */

static void josephus(int n, int m, int *order)
{
    LNode *head = NULL;
    LNode *tail = NULL;
    LNode *p, *prev;
    int i, k = 0;

    for (i = 1; i <= n; i++) {
        LNode *node = (LNode *)malloc(sizeof(LNode));
        node->data = i;
        node->next = NULL;
        if (head == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    tail->next = head; /* 首尾相连成环 */
    prev = tail;
    p = head;
    while (p->next != p) {
        for (i = 0; i < m - 1; i++) {
            prev = p;
            p = p->next;
        }
        order[k++] = p->data;
        prev->next = p->next;
        free(p);
        p = prev->next;
    }
    order[k++] = p->data;
    free(p);
}

static void demo_circular_list(void)
{
    int order[6];
    int expected[] = {3, 6, 4, 2, 5, 1};
    int i;

    ds_section("2.5.3", "循环链表");
    ds_note("尾结点指针指回表头，形成环；从任一结点出发都能遍历整个链表。");
    ds_note("约瑟夫环：n=6 人围成环，从 1 开始每数到 3 出列。");
    josephus(6, 3, order);
    ds_ok("出列顺序：");
    for (i = 0; i < 6; i++) {
        printf(" %d", order[i]);
    }
    printf("\n");
    for (i = 0; i < 6; i++) {
        ds_check_ll(order[i], expected[i], "约瑟夫出列顺序正确");
    }
}

/* ==================== 2.5.4 双向链表 ==================== */

typedef struct DuLNode {
    int data;
    struct DuLNode *prior;
    struct DuLNode *next;
} DuLNode, *DuLinkList;

static DuLinkList Du_Create(const int *a, int n)
{
    DuLinkList L = (DuLinkList)malloc(sizeof(DuLNode));
    DuLNode *r;
    int i;
    if (L == NULL) {
        return NULL;
    }
    L->prior = NULL;
    L->next = NULL;
    r = L;
    for (i = 0; i < n; i++) {
        DuLNode *p = (DuLNode *)malloc(sizeof(DuLNode));
        p->data = a[i];
        p->prior = r;
        p->next = NULL;
        r->next = p;
        r = p;
    }
    return L;
}

static void Du_Destroy(DuLinkList L)
{
    DuLNode *p = L;
    while (p != NULL) {
        DuLNode *q = p->next;
        free(p);
        p = q;
    }
}

static bool Du_InsertBefore(DuLinkList L, int before, int e)
{
    DuLNode *p = L->next;
    DuLNode *s;
    while (p != NULL && p->data != before) {
        p = p->next;
    }
    if (p == NULL) {
        return false;
    }
    s = (DuLNode *)malloc(sizeof(DuLNode));
    if (s == NULL) {
        return false;
    }
    s->data = e;
    s->prior = p->prior;
    s->next = p;
    p->prior->next = s;
    p->prior = s;
    return true;
}

static void Du_Print(const DuLinkList L, const char *name)
{
    DuLNode *p = L->next;
    printf("  %s 正向：(", name);
    while (p != NULL) {
        printf("%d%s", p->data, p->next == NULL ? "" : " <-> ");
        p = p->next;
    }
    printf(")\n");
}

static void demo_double_list(void)
{
    int a[] = {1, 2, 3, 4};
    DuLinkList L = Du_Create(a, 4);
    DuLNode *p;

    ds_section("2.5.4", "双向链表");
    ds_note("每个结点有两个指针域，分别指向前驱和后继，可双向遍历。");
    ds_note("插入、删除时需同时修改两个方向的指针，注意指针修改顺序。");
    ds_check_true(L != NULL, "建立双向链表");
    if (L == NULL) {
        return;
    }
    Du_Print(L, "L");
    ds_check_true(Du_InsertBefore(L, 3, 25), "在结点 3 之前插入 25");
    Du_Print(L, "L");
    p = L->next;
    while (p->next != NULL) {
        p = p->next;
    }
    printf("  从表尾反向遍历：");
    while (p != L) {
        printf("%d%s", p->data, p->prior == L ? "" : " <-> ");
        p = p->prior;
    }
    printf("\n");
    ds_check_true(p == L, "反向遍历回到表头结点");
    ds_check_ll(L->next->data, 1, "插入后首元素仍为 1");
    Du_Destroy(L);
}

/* ==================== 2.7 线性表的应用 ==================== */

static int merge_union(const int *A, int na, const int *B, int nb, int *C)
{
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (A[i] < B[j]) {
            C[k++] = A[i++];
        } else if (A[i] > B[j]) {
            C[k++] = B[j++];
        } else {
            C[k++] = A[i++];
            j++; /* 相同元素只保留一个 */
        }
    }
    while (i < na) {
        C[k++] = A[i++];
    }
    while (j < nb) {
        C[k++] = B[j++];
    }
    return k;
}

static int merge_ordered(const int *A, int na, const int *B, int nb, int *C)
{
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (A[i] <= B[j]) {
            C[k++] = A[i++];
        } else {
            C[k++] = B[j++];
        }
    }
    while (i < na) {
        C[k++] = A[i++];
    }
    while (j < nb) {
        C[k++] = B[j++];
    }
    return k;
}

static void demo_merge(void)
{
    int A[] = {1, 3, 5, 7};
    int B[] = {2, 3, 6, 7};
    int C[8];
    int n;
    int i;

    ds_section("2.7", "线性表的应用");
    ds_section("2.7.1", "线性表的合并");
    ds_note("把两个集合合并成一个集合：相同元素只保留一份，结果仍有序。");
    n = merge_union(A, 4, B, 4, C);
    ds_ok("A∪B = [%d %d %d %d %d %d]", C[0], C[1], C[2], C[3], C[4], C[5]);
    ds_check_ll(n, 6, "并集元素个数为 6");
    for (i = 1; i < n; i++) {
        ds_check_true(C[i - 1] < C[i], "并集严格递增且无重复");
    }

    ds_section("2.7.2", "有序表的合并");
    ds_note("两个有序表归并为一个有序表，时间 O(m+n)，空间 O(m+n)。");
    {
        int a[] = {1, 4, 9};
        int b[] = {2, 3, 10, 11};
        int c[7];
        int m = merge_ordered(a, 3, b, 4, c);
        ds_ok("归并结果：%d %d %d %d %d %d %d", c[0], c[1], c[2], c[3], c[4],
              c[5], c[6]);
        ds_check_ll(m, 7, "有序表归并后长度为 7");
        for (i = 1; i < m; i++) {
            ds_check_true(c[i - 1] <= c[i], "归并结果有序");
        }
        ds_check_ll(c[0], 1, "归并结果首元素为 1");
        ds_check_ll(c[6], 11, "归并结果末元素为 11");
    }
}

/* ==================== 2.8 一元多项式相加 ==================== */

typedef struct PNode {
    double coef;
    int exp;
    struct PNode *next;
} PNode, *Polynomial;

static Polynomial poly_create(const double *coef, const int *exp, int n)
{
    Polynomial head = (Polynomial)malloc(sizeof(PNode));
    PNode *r;
    int i;
    if (head == NULL) {
        return NULL;
    }
    head->next = NULL;
    r = head;
    for (i = 0; i < n; i++) {
        PNode *p = (PNode *)malloc(sizeof(PNode));
        p->coef = coef[i];
        p->exp = exp[i];
        p->next = NULL;
        r->next = p;
        r = p;
    }
    return head;
}

static void poly_free(Polynomial P)
{
    while (P != NULL) {
        Polynomial q = P->next;
        free(P);
        P = q;
    }
}

/* 两个按指数降序排列的多项式相加，结果按指数降序 */
static Polynomial poly_add(Polynomial A, Polynomial B)
{
    Polynomial C = (Polynomial)malloc(sizeof(PNode));
    PNode *pa = A->next;
    PNode *pb = B->next;
    PNode *pc = C;
    C->next = NULL;
    while (pa != NULL && pb != NULL) {
        PNode *node = (PNode *)malloc(sizeof(PNode));
        if (pa->exp > pb->exp) {
            node->coef = pa->coef;
            node->exp = pa->exp;
            pa = pa->next;
        } else if (pa->exp < pb->exp) {
            node->coef = pb->coef;
            node->exp = pb->exp;
            pb = pb->next;
        } else {
            node->coef = pa->coef + pb->coef;
            node->exp = pa->exp;
            pa = pa->next;
            pb = pb->next;
        }
        node->next = NULL;
        pc->next = node;
        pc = node;
    }
    {
        PNode *rest = pa != NULL ? pa : pb;
        while (rest != NULL) {
            PNode *node = (PNode *)malloc(sizeof(PNode));
            node->coef = rest->coef;
            node->exp = rest->exp;
            node->next = NULL;
            pc->next = node;
            pc = node;
            rest = rest->next;
        }
    }
    return C;
}

static void poly_print(Polynomial P)
{
    PNode *p = P->next;
    bool first = true;
    if (p == NULL) {
        printf("0");
        return;
    }
    while (p != NULL) {
        if (p->coef != 0.0) {
            double c = p->coef;
            if (c < 0) {
                printf(first ? "-" : " - ");
                c = -c;
            } else if (!first) {
                printf(" + ");
            }
            if (p->exp == 0) {
                printf("%g", c);
            } else if (p->exp == 1) {
                printf("%gx", c);
            } else {
                printf("%gx^%d", c, p->exp);
            }
            first = false;
        }
        p = p->next;
    }
    if (first) {
        printf("0");
    }
}

static double poly_eval(Polynomial P, double x)
{
    double v = 0.0;
    PNode *p;
    for (p = P->next; p != NULL; p = p->next) {
        double power = 1.0;
        int i;
        for (i = 0; i < p->exp; i++) {
            power *= x;
        }
        v += p->coef * power;
    }
    return v;
}

static void demo_polynomial(void)
{
    double c1[] = {3, 5, 1};
    int e1[] = {2, 1, 0};
    double c2[] = {2, -5, 4};
    int e2[] = {3, 1, 0};
    Polynomial A = poly_create(c1, e1, 3);
    Polynomial B = poly_create(c2, e2, 3);
    Polynomial C = poly_add(A, B);

    ds_section("2.8", "案例分析与实现：一元多项式相加");
    ds_note("用线性表表示一元多项式，每一项存系数和指数；两个多项式相加就是");
    ds_note("按指数归并同类项。");
    printf("  A(x) = ");
    poly_print(A);
    printf("\n");
    printf("  B(x) = ");
    poly_print(B);
    printf("\n");
    printf("  C(x) = A(x) + B(x) = ");
    poly_print(C);
    printf("\n");
    ds_check_ll(C->next->exp, 3, "和多项式最高次为 3");
    ds_check_ll((long long)C->next->coef, 2, "三次项系数为 2");
    ds_check_ll((long long)C->next->next->coef, 3, "二次项系数为 3");
    ds_check_ll((long long)C->next->next->next->coef, 0, "x 项系数为 0");
    ds_check_ll((long long)C->next->next->next->next->coef, 5, "常数项为 5");
    ds_check_ll((long long)poly_eval(C, 2.0), 33, "C(2) = 33");
    poly_free(A);
    poly_free(B);
    poly_free(C);
}

/* ==================== 2.6 比较 ==================== */

static void demo_compare(void)
{
    ds_section("2.6", "顺序表和链表的比较");
    ds_note("两种存储结构没有绝对优劣，按空间、时间和使用场景权衡。");
    ds_section("2.6.1", "空间性能的比较");
    ds_note("顺序表需预分配、可能浪费；链表按需分配，但每个结点有指针开销。");
    ds_section("2.6.2", "时间性能的比较");
    ds_note("顺序表随机存取 O(1)，插入删除平均移动 n/2 个元素；");
    ds_note("链表按位序存取 O(n)，已知位置时插入删除只需改指针 O(1)。");
    ds_note("选型：表长稳定、查询多、插删少用顺序表；频繁插删、长度变化大用链表。");
}

int ds_ch02(void)
{
    demo_seq_list();
    demo_linked_list();
    demo_circular_list();
    demo_double_list();
    demo_compare();
    demo_merge();
    demo_polynomial();
    ds_section("2.9", "小结");
    ds_note("线性表有两种基本存储结构：顺序存储与链式存储。");
    ds_note("顺序表支持随机存取，链表插入删除灵活；循环链表与双向链表是链式");
    ds_note("存储的变体。合并与多项式相加展示了线性表的典型应用。");
    return 0;
}
