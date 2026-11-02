/*
 * ch04_string_array.c —— 第 4 章 串、数组和广义表
 *
 * 覆盖目录：
 *   4.1 串的定义          4.2 案例引入（病毒检测）
 *   4.3 串的类型定义、存储结构及运算（含 BF、KMP 模式匹配）
 *   4.4 数组（类型定义、顺序存储、特殊矩阵压缩存储）
 *   4.5 广义表（定义、存储结构）  4.6 案例分析与实现  4.7 小结
 */
#include "common.h"

/* ==================== 4.3.1 串的抽象类型定义 ==================== */

typedef struct {
    char *ch;   /* 非空串按串长分配存储区，ch 指向串的基址 */
    int length;
} HString;

static bool StrAssign(HString *T, const char *s)
{
    int len = (int)strlen(s);
    T->ch = (char *)malloc((size_t)len + 1);
    if (T->ch == NULL) {
        return false;
    }
    memcpy(T->ch, s, (size_t)len);
    T->ch[len] = '\0';
    T->length = len;
    return true;
}

static void StrDestroy(HString *T)
{
    free(T->ch);
    T->ch = NULL;
    T->length = 0;
}

static bool StrCopy(HString *T, const HString *S)
{
    return StrAssign(T, S->ch);
}

static int StrCompare(const HString *S, const HString *T)
{
    int i;
    for (i = 0; i < S->length && i < T->length; i++) {
        if (S->ch[i] != T->ch[i]) {
            return (unsigned char)S->ch[i] - (unsigned char)T->ch[i];
        }
    }
    return S->length - T->length;
}

static bool Concat(HString *T, const HString *S1, const HString *S2)
{
    int len = S1->length + S2->length;
    T->ch = (char *)malloc((size_t)len + 1);
    if (T->ch == NULL) {
        return false;
    }
    memcpy(T->ch, S1->ch, (size_t)S1->length);
    memcpy(T->ch + S1->length, S2->ch, (size_t)S2->length);
    T->ch[len] = '\0';
    T->length = len;
    return true;
}

static bool SubString(HString *Sub, const HString *S, int pos, int len)
{
    if (pos < 1 || pos > S->length || len < 0 ||
        pos - 1 + len > S->length) {
        return false;
    }
    Sub->ch = (char *)malloc((size_t)len + 1);
    if (Sub->ch == NULL) {
        return false;
    }
    memcpy(Sub->ch, S->ch + pos - 1, (size_t)len);
    Sub->ch[len] = '\0';
    Sub->length = len;
    return true;
}

/* 简单匹配（BF）：返回 1 基位置，0 表示未找到 */
static int Index_BF(const HString *S, const HString *T)
{
    int i = 0;
    int j = 0;
    while (i < S->length && j < T->length) {
        if (S->ch[i] == T->ch[j]) {
            i++;
            j++;
        } else {
            i = i - j + 1; /* 主串指针回退 */
            j = 0;
        }
    }
    return j == T->length ? i - T->length + 1 : 0;
}

static void demo_string_adt(void)
{
    HString S, T, sub, cat;

    ds_section("4.1", "串的定义");
    ds_note("串是零个或多个字符组成的有限序列，是数据元素为字符的线性表。");
    ds_note("子串、主串、位置、空串、空格串是串的基本术语。");

    ds_section("4.2", "案例引入");
    ds_note("病毒感染检测：把环状病毒串的每个循环移位作为模式串，在人的 DNA");
    ds_note("中查找；这是模式匹配算法的直接应用。");

    ds_section("4.3", "串的类型定义、存储结构及其运算");
    ds_section("4.3.1", "串的抽象类型定义");
    ds_note("ADT String：StrAssign、StrCopy、StrCompare、StrLength、Concat、");
    ds_note("SubString、Index、StrInsert、StrDelete、Replace。");
    ds_check_true(StrAssign(&S, "DataStructure"), "StrAssign 赋值成功");
    ds_check_ll(S.length, 13, "串 DataStructure 长度为 13");
    {
        HString copy;
        ds_check_true(StrCopy(&copy, &S), "StrCopy 复制成功");
        ds_check_ll(StrCompare(&copy, &S), 0, "复制串与原串相等");
        StrDestroy(&copy);
    }
    ds_check_true(StrAssign(&T, "Data"), "第二个串赋值成功");
    ds_check_true(Concat(&cat, &T, &S), "Concat 连接成功");
    ds_ok("Data + DataStructure = %s", cat.ch);
    ds_check_ll(cat.length, 17, "连接后长度为 17");
    ds_check_true(SubString(&sub, &S, 5, 6), "SubString(S,5,6) 成功");
    ds_ok("S 的第 5 个字符起 6 个字符：%s", sub.ch);
    ds_check_str(sub.ch, "Struct", "子串为 Struct");
    ds_check_true(!SubString(&sub, &S, 9, 6), "越界取子串被拒绝");
    ds_check_true(StrCompare(&T, &S) < 0, "Data 小于 DataStructure");
    ds_check_true(StrCompare(&S, &S) == 0, "串与自身相等");

    ds_section("4.3.3", "串的模式匹配算法");
    ds_note("BF 算法失配时主串指针回退，最坏 O(m*n)。");
    {
        HString text, pat;
        StrAssign(&text, "ababcabcacbab");
        StrAssign(&pat, "abcac");
        ds_check_ll(Index_BF(&text, &pat), 6, "BF 在位置 6 找到 abcac");
        ds_check_ll(Index_BF(&text, &pat), 6, "BF 结果可重复");
        StrDestroy(&text);
        StrDestroy(&pat);
    }

    StrDestroy(&S);
    StrDestroy(&T);
    StrDestroy(&cat);
    StrDestroy(&sub);
}

/* ==================== 4.3.2 串的存储结构 ==================== */

/* 块链存储：每个结点存 4 个字符 */
#define CHUNK_SIZE 4
typedef struct Chunk {
    char ch[CHUNK_SIZE];
    int length;
    struct Chunk *next;
} Chunk;

static Chunk *build_block_string(const char *s)
{
    int len = (int)strlen(s);
    Chunk *head = NULL;
    Chunk *tail = NULL;
    int pos = 0;
    while (pos < len) {
        Chunk *c = (Chunk *)malloc(sizeof(Chunk));
        int k = 0;
        while (k < CHUNK_SIZE && pos + k < len) {
            c->ch[k] = s[pos + k];
            k++;
        }
        c->length = k;
        c->next = NULL;
        if (head == NULL) {
            head = tail = c;
        } else {
            tail->next = c;
            tail = c;
        }
        pos += k;
    }
    return head;
}

static void free_blocks(Chunk *head)
{
    while (head != NULL) {
        Chunk *next = head->next;
        free(head);
        head = next;
    }
}

static void print_blocks(Chunk *head)
{
    Chunk *p;
    for (p = head; p != NULL; p = p->next) {
        printf("[%.*s]", p->length, p->ch);
        if (p->next != NULL) {
            printf("->");
        }
    }
}

static void demo_string_storage(void)
{
    Chunk *head;

    ds_section("4.3.2", "串的存储结构");
    ds_note("定长顺序存储：用定长数组存串，超出长度会被截断。");
    ds_note("堆分配存储：按实际长度动态分配，HString 就采用这种结构。");
    ds_note("块链存储：每个结点存若干字符，用链表串起，适合超长串。");
    head = build_block_string("DataStructure");
    printf("  块链存储（每块 %d 字符）：", CHUNK_SIZE);
    print_blocks(head);
    printf("\n");
    ds_check_ll(head != NULL ? head->length : 0, 4, "首块存 4 个字符");
    ds_check_ll(head != NULL && head->next != NULL ? head->next->length : 0, 4,
                "第二块存 4 个字符");
    free_blocks(head);
}

/* ==================== 4.3.3 KMP ==================== */

/* next 数组采用 1 基下标，next[1] = 0 */
static void get_next(const char *pat, int len, int *next)
{
    int i = 1;
    int j = 0;
    next[1] = 0;
    while (i < len) {
        if (j == 0 || pat[i - 1] == pat[j - 1]) {
            i++;
            j++;
            next[i] = j;
        } else {
            j = next[j];
        }
    }
}

static void get_nextval(const char *pat, int len, int *nextval)
{
    int i = 1;
    int j = 0;
    nextval[1] = 0;
    while (i < len) {
        if (j == 0 || pat[i - 1] == pat[j - 1]) {
            i++;
            j++;
            if (pat[i - 1] != pat[j - 1]) {
                nextval[i] = j;
            } else {
                nextval[i] = nextval[j];
            }
        } else {
            j = nextval[j];
        }
    }
}

static int Index_KMP(const char *text, const char *pat)
{
    int n = (int)strlen(text);
    int m = (int)strlen(pat);
    int *next = (int *)malloc(sizeof(int) * (size_t)(m + 1));
    int i = 1;
    int j = 1;
    int pos = 0;

    if (m == 0) {
        free(next);
        return 1;
    }
    get_next(pat, m, next);
    while (i <= n && j <= m) {
        if (j == 0 || text[i - 1] == pat[j - 1]) {
            i++;
            j++;
        } else {
            j = next[j];
        }
    }
    if (j > m) {
        pos = i - m;
    }
    free(next);
    return pos;
}

static void demo_kmp(void)
{
    const char *pat = "abaabcac";
    int len = (int)strlen(pat);
    int *next = (int *)malloc(sizeof(int) * (size_t)(len + 1));
    int *nextval = (int *)malloc(sizeof(int) * (size_t)(len + 1));
    int i;

    ds_section("4.3.3", "串的模式匹配算法：KMP");
    ds_note("KMP 利用已匹配的前缀信息，失配时主串指针不回退，时间 O(m+n)。");
    ds_note("next[j] 表示失配后模式串应跳到哪个位置继续比较。");
    get_next(pat, len, next);
    get_nextval(pat, len, nextval);
    printf("  模式串 %s 的 next    =", pat);
    for (i = 1; i <= len; i++) {
        printf(" %d", next[i]);
    }
    printf("\n");
    printf("  模式串 %s 的 nextval =", pat);
    for (i = 1; i <= len; i++) {
        printf(" %d", nextval[i]);
    }
    printf("\n");
    ds_check_ll(next[1], 0, "next[1] = 0");
    ds_check_ll(next[4], 2, "next[4] = 2（前缀 a 与后缀 a 重合）");
    ds_check_ll(next[6], 3, "next[6] = 3（前缀 ab 与后缀 ab 重合）");
    ds_check_ll(nextval[3], 0, "nextval[3] = 0（a 与首字符相同，继续跳）");
    ds_check_ll(Index_KMP("ababcabcacbab", "abcac"), 6, "KMP 在位置 6 找到 abcac");
    ds_check_ll(Index_KMP("xxabaabcacyy", "abaabcac"), 3,
                "KMP 在位置 3 找到 abaabcac");
    ds_check_ll(Index_KMP("abc", "xyz"), 0, "KMP 未找到时返回 0");
    free(next);
    free(nextval);
}

/* ==================== 4.4 数组 ==================== */

/*
 * 二维数组按行优先顺序存储：Loc(i,j) = base + (i*cols + j) * size。
 * 这里用“元素序号”代替字节地址演示映射关系。
 */
static int row_major_offset(int i, int j, int cols)
{
    return i * cols + j;
}

static void demo_array(void)
{
    int cols = 4;
    int rows = 3;
    int i, j;

    ds_section("4.4", "数组");
    ds_section("4.4.1", "数组的类型定义");
    ds_note("数组是下标与值构成二元关系的线性表；二维数组可看成每个元素是");
    ds_note("一维数组的线性表。");

    ds_section("4.4.2", "数组的顺序存储");
    ds_note("二维数组按行优先存放：先存第 0 行，再存第 1 行……");
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            ds_check_ll(row_major_offset(i, j, cols), i * cols + j,
                        "行优先偏移公式正确");
        }
    }
    ds_ok("A[2][3]（3 行 4 列）的偏移序号 = %d", row_major_offset(2, 3, cols));
    ds_note("列优先存储则按列依次存放：Loc(i,j) = base + (j*rows + i)*size。");
}

/* 对称矩阵：只存下三角，共 n(n+1)/2 个元素 */
#define SYM_N 5
static int sym_get(const int *a, int n, int i, int j)
{
    int k;
    (void)n;
    if (i < j) {
        int t = i;
        i = j;
        j = t;
    }
    k = i * (i - 1) / 2 + (j - 1);
    return a[k];
}

static void demo_special_matrix(void)
{
    int n = SYM_N;
    int size = n * (n + 1) / 2;
    int *a = (int *)malloc(sizeof(int) * (size_t)size);
    int i, j;

    ds_section("4.4.3", "特殊矩阵的压缩存储");
    ds_note("对称矩阵只需存下三角（或上三角），空间从 n^2 降到 n(n+1)/2。");
    ds_note("下三角元素 a[i][j]（i>=j，1 基）存于一维数组下标");
    ds_note("k = i(i-1)/2 + j - 1；上三角元素利用对称性交换 i、j。");
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= i; j++) {
            a[i * (i - 1) / 2 + (j - 1)] = i * 10 + j;
        }
    }
    ds_ok("紧缩后占用 %d 个单元（原矩阵 %d 个）", size, n * n);
    ds_check_ll(size, 15, "5 阶对称矩阵压缩后存 15 个元素");
    ds_check_ll(sym_get(a, n, 3, 2), 32, "a[3][2] = 32");
    ds_check_ll(sym_get(a, n, 2, 3), 32, "a[2][3] 利用对称性也等于 32");
    ds_check_ll(sym_get(a, n, 5, 1), 51, "a[5][1] = 51");
    ds_check_ll(sym_get(a, n, 1, 1), 11, "a[1][1] = 11");
    free(a);
}

/* 稀疏矩阵三元组顺序表 */
typedef struct {
    int i, j;
    int e;
} Triple;

typedef struct {
    Triple data[100];
    int mu, nu, tu; /* 行数、列数、非零元个数 */
} TSMatrix;

static void sparse_print(const TSMatrix *M)
{
    int k;
    printf("  三元组表（行,列,值）：");
    for (k = 0; k < M->tu; k++) {
        printf("(%d,%d,%d)%s", M->data[k].i, M->data[k].j, M->data[k].e,
               k + 1 == M->tu ? "" : " ");
    }
    printf("\n");
}

static bool sparse_fast_transpose(const TSMatrix *M, TSMatrix *T)
{
    int *num = (int *)calloc((size_t)M->nu, sizeof(int));
    int *cpot = (int *)malloc(sizeof(int) * (size_t)M->nu);
    int col, t, p;

    if (num == NULL || cpot == NULL) {
        free(num);
        free(cpot);
        return false;
    }
    T->mu = M->nu;
    T->nu = M->mu;
    T->tu = M->tu;
    if (M->tu == 0) {
        free(num);
        free(cpot);
        return true;
    }
    for (col = 0; col < M->nu; col++) {
        num[col] = 0;
    }
    for (t = 0; t < M->tu; t++) {
        num[M->data[t].j]++;
    }
    cpot[0] = 0;
    for (col = 1; col < M->nu; col++) {
        cpot[col] = cpot[col - 1] + num[col - 1];
    }
    for (p = 0; p < M->tu; p++) {
        int q = cpot[M->data[p].j]++;
        T->data[q].i = M->data[p].j;
        T->data[q].j = M->data[p].i;
        T->data[q].e = M->data[p].e;
    }
    free(num);
    free(cpot);
    return true;
}

static void demo_sparse_matrix(void)
{
    TSMatrix M, T;
    int i, j;
    int vals[4][5] = {
        {0, 3, 0, 5, 0},
        {0, 0, -1, 0, 0},
        {4, 0, 0, 0, 0},
        {0, 0, 0, 2, 0},
    };

    ds_section("4.4.3", "特殊矩阵的压缩存储：稀疏矩阵");
    ds_note("稀疏矩阵非零元很少，只存三元组 (行, 列, 值)，节省空间。");
    M.mu = 4;
    M.nu = 5;
    M.tu = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 5; j++) {
            if (vals[i][j] != 0) {
                M.data[M.tu].i = i;
                M.data[M.tu].j = j;
                M.data[M.tu].e = vals[i][j];
                M.tu++;
            }
        }
    }
    ds_ok("4x5 矩阵共 %d 个非零元", M.tu);
    sparse_print(&M);
    ds_check_ll(M.tu, 5, "稀疏矩阵有 5 个非零元");
    ds_check_true(sparse_fast_transpose(&M, &T), "快速转置成功");
    sparse_print(&T);
    ds_check_ll(T.mu, 5, "转置后行数为 5");
    ds_check_ll(T.nu, 4, "转置后列数为 4");
    ds_check_ll(T.data[0].i, 0, "转置后第一个三元组在第 0 行");
    ds_check_ll(T.data[1].i, 1, "转置后第二个三元组在第 1 行");
    ds_check_ll(T.data[0].e, 4, "转置后 (0,2) 位置值为 4");
    ds_check_ll(T.data[3].e, 5, "转置后 (3,0) 位置值为 5");
}

/* ==================== 4.5 广义表 ==================== */

typedef enum { ATOM, LIST } ElemTag;

typedef struct GLNode {
    ElemTag tag;
    union {
        char atom;
        struct {
            struct GLNode *hp; /* 表头 */
            struct GLNode *tp; /* 表尾 */
        } ptr;
    } un;
} GLNode, *GList;

static GList gl_parse_list(const char **p);

static GList gl_parse_element(const char **p)
{
    GList node;
    if (**p == '(') {
        return gl_parse_list(p);
    }
    node = (GList)malloc(sizeof(GLNode));
    node->tag = ATOM;
    node->un.atom = **p;
    (*p)++;
    return node;
}

/* 解析括号内从当前位置到 ')' 的元素序列（表头—表尾链） */
static GList gl_parse_body(const char **p)
{
    GList node;
    if (**p == ')') {
        (*p)++;
        return NULL; /* 空表 */
    }
    node = (GList)malloc(sizeof(GLNode));
    node->tag = LIST;
    node->un.ptr.hp = gl_parse_element(p);
    if (**p == ',') {
        (*p)++;
        node->un.ptr.tp = gl_parse_body(p);
    } else {
        node->un.ptr.tp = NULL;
        if (**p == ')') {
            (*p)++;
        }
    }
    return node;
}

static GList gl_parse_list(const char **p)
{
    if (**p != '(') {
        return NULL;
    }
    (*p)++;
    return gl_parse_body(p);
}

static void gl_print(const GList L)
{
    if (L == NULL) {
        printf("()");
        return;
    }
    printf("(");
    {
        const GLNode *p = L;
        bool first = true;
        while (p != NULL) {
            if (!first) {
                printf(",");
            }
            first = false;
            if (p->un.ptr.hp == NULL) {
                printf("()");
            } else if (p->un.ptr.hp->tag == ATOM) {
                printf("%c", p->un.ptr.hp->un.atom);
            } else {
                gl_print(p->un.ptr.hp);
            }
            p = p->un.ptr.tp;
        }
    }
    printf(")");
}

static int gl_length(const GList L)
{
    int n = 0;
    const GLNode *p = L;
    while (p != NULL) {
        n++;
        p = p->un.ptr.tp;
    }
    return n;
}

static int gl_depth(const GList L)
{
    int max = 0;
    const GLNode *p;
    if (L == NULL) {
        return 1; /* 空表深度为 1 */
    }
    for (p = L; p != NULL; p = p->un.ptr.tp) {
        int d;
        GList h = p->un.ptr.hp;
        if (h == NULL) {
            d = 1;
        } else if (h->tag == ATOM) {
            d = 0;
        } else {
            d = gl_depth(h);
        }
        if (d > max) {
            max = d;
        }
    }
    return max + 1;
}

static void gl_free(GList L)
{
    GLNode *p = L;
    while (p != NULL) {
        GList tp = p->un.ptr.tp;
        if (p->tag == LIST) {
            GList hp = p->un.ptr.hp;
            if (hp != NULL) {
                if (hp->tag == LIST) {
                    gl_free(hp); /* 子表递归释放 */
                } else {
                    free(hp); /* 原子结点没有 tp 域，只能直接释放 */
                }
            }
        }
        free(p);
        p = tp;
    }
}

static void demo_generalized_list(void)
{
    const char *s = "(a,(b,c),d)";
    const char *p = s;
    GList L = gl_parse_list(&p);

    ds_section("4.5", "广义表");
    ds_section("4.5.1", "广义表的定义");
    ds_note("广义表是线性表的推广，元素可以是原子，也可以是子表。");
    ds_note("表头是第一个元素，表尾是其余元素组成的表；空表长度为 0、深度为 1。");

    ds_section("4.5.2", "广义表的存储结构");
    ds_note("结点分原子结点和表结点；表结点用 hp 指表头、tp 指表尾。");
    printf("  L = ");
    gl_print(L);
    printf("\n");
    ds_check_ll(gl_length(L), 3, "广义表 (a,(b,c),d) 长度为 3");
    ds_check_ll(gl_depth(L), 2, "广义表 (a,(b,c),d) 深度为 2");

    {
        const char *s2 = "((a),(b,(c)))";
        const char *p2 = s2;
        GList L2 = gl_parse_list(&p2);
        printf("  L2 = ");
        gl_print(L2);
        printf("\n");
        ds_check_ll(gl_length(L2), 2, "L2 长度为 2");
        ds_check_ll(gl_depth(L2), 3, "L2 深度为 3");
        gl_free(L2);
    }
    gl_free(L);
}

/* ==================== 4.6 病毒感染检测 ==================== */

static bool virus_match(const char *virus, const char *human)
{
    int n = (int)strlen(virus);
    int r;
    char *rot = (char *)malloc((size_t)n + 1);
    bool found = false;

    if (n == 0) {
        free(rot);
        return true;
    }
    for (r = 0; r < n && !found; r++) {
        int k;
        for (k = 0; k < n; k++) {
            rot[k] = virus[(r + k) % n];
        }
        rot[n] = '\0';
        if (Index_KMP(human, rot) != 0) {
            found = true;
        }
    }
    free(rot);
    return found;
}

static void demo_virus_detection(void)
{
    ds_section("4.6", "案例分析与实现：病毒感染检测");
    ds_note("病毒 DNA 是环状串，人的 DNA 是线状串；把病毒串的每个循环移位");
    ds_note("都作为模式串，用 KMP 在人的 DNA 中查找。");
    ds_check_true(virus_match("baa", "aabbcc"), "环状病毒 baa 能在 aabbcc 中找到");
    ds_check_true(virus_match("aba", "babab"), "环状病毒 aba 能在 babab 中找到");
    ds_check_true(!virus_match("abc", "abab"), "病毒 abc 不能在 abab 中找到");
    ds_note("依次尝试 n 个循环移位，每次 KMP 为 O(m+n)，总计 O(n(m+n))。");
}

int ds_ch04(void)
{
    demo_string_adt();
    demo_string_storage();
    demo_kmp();
    demo_array();
    demo_special_matrix();
    demo_sparse_matrix();
    demo_generalized_list();
    demo_virus_detection();
    ds_section("4.7", "小结");
    ds_note("串的存储有定长顺序、堆分配、块链三种；BF 与 KMP 是两种模式匹配");
    ds_note("算法。数组按行（列）优先映射到一维空间，特殊矩阵可压缩存储。");
    ds_note("广义表用表头—表尾结构表示，可递归求长度与深度。");
    return 0;
}
