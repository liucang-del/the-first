/*
 * ch03_stack_queue.c —— 第 3 章 栈和队列
 *
 * 覆盖目录：
 *   3.1 栈和队列的定义和特点        3.2 案例引入（数制转换）
 *   3.3 栈的表示和操作（顺序栈、链栈）
 *   3.4 栈与递归（递归问题、递归工作栈、效率、递归转非递归）
 *   3.5 队列的表示和操作（循环队列、链队列）
 *   3.6 案例分析与实现（舞伴问题）  3.7 小结
 */
#include "common.h"

/* ==================== 3.3.2 顺序栈 ==================== */

#define STACK_INIT_SIZE 8

typedef struct {
    int *base;
    int *top;
    int stacksize;
} SqStack;

static bool StackInit(SqStack *S)
{
    S->base = (int *)malloc(sizeof(int) * STACK_INIT_SIZE);
    if (S->base == NULL) {
        return false;
    }
    S->top = S->base;
    S->stacksize = STACK_INIT_SIZE;
    return true;
}

static void StackDestroy(SqStack *S)
{
    free(S->base);
    S->base = S->top = NULL;
    S->stacksize = 0;
}

static bool StackPush(SqStack *S, int e)
{
    if (S->top - S->base >= S->stacksize) { /* 栈满则扩容 */
        int offset = (int)(S->top - S->base);
        int *newbase = (int *)realloc(S->base,
                                      sizeof(int) * (size_t)S->stacksize * 2);
        if (newbase == NULL) {
            return false;
        }
        S->base = newbase;
        S->top = newbase + offset;
        S->stacksize *= 2;
    }
    *S->top++ = e;
    return true;
}

static bool StackPop(SqStack *S, int *e)
{
    if (S->top == S->base) {
        return false; /* 空栈 */
    }
    *e = *--S->top;
    return true;
}

static bool StackTop(const SqStack *S, int *e)
{
    if (S->top == S->base) {
        return false;
    }
    *e = S->top[-1];
    return true;
}

static void demo_seq_stack(void)
{
    SqStack S;
    int e = 0;
    int i;

    ds_section("3.1", "栈和队列的定义和特点");
    ds_note("栈和队列都是操作受限的线性表，区别在允许操作的一端。");
    ds_section("3.1.1", "栈的定义和特点");
    ds_note("栈是后进先出（LIFO）的线性表，只在栈顶插入和删除。");
    ds_section("3.1.2", "队列的定义和特点");
    ds_note("队列是先进先出（FIFO）的线性表，在队尾插入、在队头删除。");

    ds_section("3.3", "栈的表示和操作的实现");
    ds_section("3.3.1", "栈的类型定义");
    ds_note("ADT Stack：InitStack、DestroyStack、ClearStack、StackEmpty、");
    ds_note("StackLength、GetTop、Push、Pop、StackTraverse。");

    ds_section("3.3.2", "顺序栈的表示和实现");
    ds_note("用一组连续单元存放栈元素，base 指向栈底，top 指向栈顶下一位置。");
    ds_check_true(StackInit(&S), "InitStack 初始化成功");
    for (i = 1; i <= 5; i++) {
        StackPush(&S, i * 11);
    }
    ds_check_ll(S.top - S.base, 5, "压入 5 个元素后栈长为 5");
    ds_check_true(StackTop(&S, &e), "GetTop 成功");
    ds_check_ll(e, 55, "栈顶元素为 55");
    ds_check_true(StackPop(&S, &e), "Pop 成功");
    ds_check_ll(e, 55, "先弹出 55（后进先出）");
    ds_check_true(StackPop(&S, &e), "再次 Pop 成功");
    ds_check_ll(e, 44, "再弹出 44");
    ds_check_ll(S.top - S.base, 3, "两次出栈后栈长为 3");
    ds_check_true(StackTop(&S, &e) && e == 33, "出栈后新栈顶为 33");
    while (StackPop(&S, &e)) {
        /* 清空栈 */
    }
    ds_check_true(!StackPop(&S, &e), "空栈出栈被拒绝");
    StackDestroy(&S);
}

/* ==================== 3.3.3 链栈 ==================== */

typedef struct StackNode {
    int data;
    struct StackNode *next;
} StackNode, *LinkStack;

static void LinkStackPush(LinkStack *S, int e)
{
    StackNode *p = (StackNode *)malloc(sizeof(StackNode));
    p->data = e;
    p->next = *S;
    *S = p;
}

static bool LinkStackPop(LinkStack *S, int *e)
{
    StackNode *p;
    if (*S == NULL) {
        return false;
    }
    p = *S;
    *e = p->data;
    *S = p->next;
    free(p);
    return true;
}

static void demo_linked_stack(void)
{
    LinkStack S = NULL;
    int e = 0;

    ds_section("3.3.3", "链栈的表示和实现");
    ds_note("链栈用链表实现，栈顶在表头，插入删除都在表头进行，不会栈满。");
    LinkStackPush(&S, 1);
    LinkStackPush(&S, 2);
    LinkStackPush(&S, 3);
    ds_check_true(LinkStackPop(&S, &e), "链栈弹出成功");
    ds_check_ll(e, 3, "链栈先弹出 3");
    ds_check_true(LinkStackPop(&S, &e), "链栈再次弹出成功");
    ds_check_ll(e, 2, "链栈再弹出 2");
    LinkStackPop(&S, &e);
    ds_check_true(!LinkStackPop(&S, &e), "链栈为空时弹出被拒绝");
}

/* ==================== 3.2 案例：数制转换 ==================== */

static void conversion(unsigned int n, int base, char *out)
{
    SqStack S;
    const char *digits = "0123456789ABCDEF";
    int e;
    int k = 0;

    StackInit(&S);
    while (n > 0) {
        StackPush(&S, (int)(n % (unsigned)base));
        n /= (unsigned)base;
    }
    while (StackPop(&S, &e)) {
        out[k++] = digits[e];
    }
    out[k] = '\0';
    StackDestroy(&S);
}

static bool bracket_match(const char *s)
{
    SqStack S;
    int i;
    bool ok = true;

    StackInit(&S);
    for (i = 0; s[i] != '\0' && ok; i++) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') {
            StackPush(&S, c);
        } else if (c == ')' || c == ']' || c == '}') {
            int top = 0;
            if (!StackPop(&S, &top)) {
                ok = false;
            } else if ((c == ')' && top != '(') || (c == ']' && top != '[') ||
                       (c == '}' && top != '{')) {
                ok = false;
            }
        }
    }
    if (S.top != S.base) {
        ok = false;
    }
    StackDestroy(&S);
    return ok;
}

static void demo_conversion(void)
{
    char buf[64];

    ds_section("3.2", "案例引入：数制转换");
    ds_note("十进制转其他进制时，先得低位、后得高位，正好用栈逆序输出。");
    conversion(1348, 8, buf);
    ds_ok("十进制 1348 转八进制：%s", buf);
    ds_check_str(buf, "2504", "1348 的八进制表示是 2504");
    conversion(255, 16, buf);
    ds_ok("十进制 255 转十六进制：%s", buf);
    ds_check_str(buf, "FF", "255 的十六进制表示是 FF");
    ds_note("括号匹配是栈的另一个典型应用：左括号入栈，右括号与栈顶配对。");
    ds_check_true(bracket_match("([{}])"), "括号串 ([{}]) 匹配");
    ds_check_true(bracket_match("{[()()]}"), "括号串 {[()()]} 匹配");
    ds_check_true(!bracket_match("([)]"), "括号串 ([)] 不匹配");
    ds_check_true(!bracket_match("(()"), "括号串 (() 不匹配");
}

/* ==================== 3.4.1 递归算法 ==================== */

static long long factorial(int n)
{
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

static int hanoi_moves = 0;

static void hanoi(int n, char from, char via, char to)
{
    if (n == 1) {
        hanoi_moves++;
        return;
    }
    hanoi(n - 1, from, to, via);
    hanoi_moves++;
    hanoi(n - 1, via, from, to);
}

static void demo_recursion(void)
{
    ds_section("3.4", "栈与递归");
    ds_section("3.4.1", "采用递归算法解决的问题");
    ds_note("递归 = 递推 + 回归；问题可分解为规模更小、形式相同的子问题。");
    ds_check_ll((long long)factorial(5), 120, "5! = 120");
    ds_check_ll((long long)factorial(10), 3628800, "10! = 3628800");

    hanoi_moves = 0;
    hanoi(3, 'A', 'B', 'C');
    ds_ok("3 层汉诺塔移动次数：%d", hanoi_moves);
    ds_check_ll(hanoi_moves, 7, "n 层汉诺塔需 2^n-1 次移动");
    hanoi_moves = 0;
    hanoi(10, 'A', 'B', 'C');
    ds_check_ll(hanoi_moves, 1023, "10 层汉诺塔需 1023 次移动");
}

/* ==================== 3.4.2 递归工作栈 ==================== */

/*
 * 阶乘的递归过程本质上是把 n, n-1, ..., 1 依次压栈，再逐个弹出相乘。
 * 这里用显式栈模拟系统栈的行为。
 */
static long long factorial_with_stack(int n)
{
    int st[64];
    int top = 0;
    long long result = 1;

    while (n > 1) {
        st[top++] = n; /* 保存本层的 n，对应递归调用前的现场 */
        n--;
    }
    while (top > 0) {
        result *= st[--top]; /* 回归：弹出本层并相乘 */
    }
    return result;
}

static void demo_recursion_stack(void)
{
    ds_section("3.4.2", "递归过程与递归工作栈");
    ds_note("每次递归调用都要保存返回地址、实参和局部变量，系统用工作栈管理。");
    ds_note("递归深度决定栈空间；层数过深可能栈溢出。");
    ds_check_ll(factorial_with_stack(5), 120, "显式栈模拟 5! = 120");
    ds_check_ll(factorial_with_stack(8), 40320, "显式栈模拟 8! = 40320");
    ds_ok("factorial(5) 的显式栈过程：压入 5,4,3,2，再弹出相乘");
}

/* ==================== 3.4.3 递归效率分析 ==================== */

static long long fib_recursive(int n, long long *calls)
{
    (*calls)++;
    if (n <= 1) {
        return n;
    }
    return fib_recursive(n - 1, calls) + fib_recursive(n - 2, calls);
}

static long long fib_iterative(int n, long long *loops)
{
    long long a = 0;
    long long b = 1;
    int i;
    if (n <= 1) {
        return n;
    }
    for (i = 2; i <= n; i++) {
        long long t = a + b;
        a = b;
        b = t;
        (*loops)++;
    }
    return b;
}

static void demo_recursion_efficiency(void)
{
    long long calls = 0;
    long long loops = 0;
    long long fr = fib_recursive(20, &calls);
    long long fi = fib_iterative(20, &loops);

    ds_section("3.4.3", "递归算法的效率分析");
    ds_note("斐波那契递归存在大量重复计算，时间复杂度 O(2^n)；迭代为 O(n)。");
    ds_check_ll(fr, 6765, "递归 fib(20) = 6765");
    ds_check_ll(fi, 6765, "迭代 fib(20) = 6765");
    ds_check_ll(calls, 21891, "递归 fib(20) 共调用 21891 次");
    ds_check_ll(loops, 19, "迭代 fib(20) 只循环 19 次");
    ds_note("递归写法简洁，但重复子问题会带来指数级开销；可改用迭代或记忆化。");
}

/* ==================== 3.4.4 递归转非递归：显式栈汉诺塔 ==================== */

typedef struct {
    int n;
    char from;
    char via;
    char to;
    int stage;
} HanoiFrame;

static int hanoi_with_stack(int n, char from, char via, char to)
{
    HanoiFrame st[64];
    int top = 0;
    int moves = 0;

    st[top].n = n;
    st[top].from = from;
    st[top].via = via;
    st[top].to = to;
    st[top].stage = 0;
    top++;

    while (top > 0) {
        HanoiFrame *f = &st[top - 1];
        if (f->n == 0) {
            top--;
        } else if (f->n == 1) {
            moves++;
            top--;
        } else if (f->stage == 0) {
            f->stage = 1;
            st[top].n = f->n - 1;
            st[top].from = f->from;
            st[top].via = f->to;
            st[top].to = f->via;
            st[top].stage = 0;
            top++;
        } else if (f->stage == 1) {
            moves++;
            f->stage = 2;
            st[top].n = f->n - 1;
            st[top].from = f->via;
            st[top].via = f->from;
            st[top].to = f->to;
            st[top].stage = 0;
            top++;
        } else {
            top--;
        }
    }
    return moves;
}

static void demo_recursion_to_iteration(void)
{
    ds_section("3.4.4", "利用栈将递归转换为非递归的方法");
    ds_note("递归转非递归的关键：用显式栈保存“当前层参数 + 返回后继续的位置”。");
    ds_check_ll(hanoi_with_stack(3, 'A', 'B', 'C'), 7, "显式栈汉诺塔 3 层走 7 步");
    ds_check_ll(hanoi_with_stack(10, 'A', 'B', 'C'), 1023,
                "显式栈汉诺塔 10 层走 1023 步");
    ds_note("栈帧中 stage 记录本层执行到哪一步，与系统递归的返回地址作用相同。");
}

/* ==================== 3.5.2 循环队列 ==================== */

#define QUEUE_MAX 6

typedef struct {
    int data[QUEUE_MAX];
    int front; /* 队头下标 */
    int count; /* 当前元素个数，用来区分队空与队满 */
} SqQueue;

static void QueueInit(SqQueue *Q)
{
    Q->front = 0;
    Q->count = 0;
}

static bool QueueEmpty(const SqQueue *Q)
{
    return Q->count == 0;
}

static bool QueueFull(const SqQueue *Q)
{
    return Q->count == QUEUE_MAX;
}

static bool EnQueue(SqQueue *Q, int e)
{
    int rear;
    if (QueueFull(Q)) {
        return false;
    }
    rear = (Q->front + Q->count) % QUEUE_MAX;
    Q->data[rear] = e;
    Q->count++;
    return true;
}

static bool DeQueue(SqQueue *Q, int *e)
{
    if (QueueEmpty(Q)) {
        return false;
    }
    *e = Q->data[Q->front];
    Q->front = (Q->front + 1) % QUEUE_MAX;
    Q->count--;
    return true;
}

static bool GetHead(const SqQueue *Q, int *e)
{
    if (QueueEmpty(Q)) {
        return false;
    }
    *e = Q->data[Q->front];
    return true;
}

static void demo_circular_queue(void)
{
    SqQueue Q;
    int e = 0;
    int i;

    ds_section("3.5", "队列的表示和操作的实现");
    ds_section("3.5.1", "队列的类型定义");
    ds_note("ADT Queue：InitQueue、DestroyQueue、QueueEmpty、QueueLength、");
    ds_note("GetHead、EnQueue、DeQueue、QueueTraverse。");

    ds_section("3.5.2", "循环队列——队列的顺序表示和实现");
    ds_note("把顺序队列的数组看成首尾相接的环，用取模运算让 rear 回到 0。");
    ds_note("用 count 区分队空（count=0）与队满（count=容量）。");
    QueueInit(&Q);
    for (i = 1; i <= 5; i++) {
        ds_check_true(EnQueue(&Q, i * 10), "元素入队");
    }
    ds_check_true(!QueueFull(&Q), "容量 6、放入 5 个后尚未满");
    ds_check_true(EnQueue(&Q, 60), "第 6 个元素入队后队满");
    ds_check_true(!EnQueue(&Q, 70), "队满时入队被拒绝");
    ds_check_true(GetHead(&Q, &e), "取队头元素成功");
    ds_check_ll(e, 10, "队头元素是 10");
    ds_check_true(DeQueue(&Q, &e), "出队成功");
    ds_check_ll(e, 10, "先出队的是 10（先进先出）");
    ds_check_true(EnQueue(&Q, 70), "出队后空位可继续入队");
    ds_check_ll(Q.count, 6, "循环队列重新变满");
    ds_check_true(DeQueue(&Q, &e), "出队");
    ds_check_ll(e, 20, "第二次出队的是 20");
    ds_check_true(DeQueue(&Q, &e), "出队");
    ds_check_ll(e, 30, "第三次出队的是 30");
    ds_check_true(!QueueEmpty(&Q), "队列非空");
}

/* ==================== 3.5.3 链队列 ==================== */

typedef struct QNode {
    int data;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
} LinkQueue;

static void LQ_Init(LinkQueue *Q)
{
    Q->front = Q->rear = NULL;
}

static void LQ_EnQueue(LinkQueue *Q, int e)
{
    QNode *p = (QNode *)malloc(sizeof(QNode));
    p->data = e;
    p->next = NULL;
    if (Q->rear == NULL) {
        Q->front = Q->rear = p;
    } else {
        Q->rear->next = p;
        Q->rear = p;
    }
}

static bool LQ_DeQueue(LinkQueue *Q, int *e)
{
    QNode *p;
    if (Q->front == NULL) {
        return false;
    }
    p = Q->front;
    *e = p->data;
    Q->front = p->next;
    if (Q->front == NULL) {
        Q->rear = NULL;
    }
    free(p);
    return true;
}

static void demo_linked_queue(void)
{
    LinkQueue Q;
    int e = 0;

    ds_section("3.5.3", "链队列——队列的链式表示和实现");
    ds_note("链队列用链表实现，front 指向队头，rear 指向队尾，不会队满。");
    LQ_Init(&Q);
    LQ_EnQueue(&Q, 1);
    LQ_EnQueue(&Q, 2);
    LQ_EnQueue(&Q, 3);
    ds_check_true(LQ_DeQueue(&Q, &e), "链队列出队成功");
    ds_check_ll(e, 1, "链队列先出队 1");
    ds_check_true(LQ_DeQueue(&Q, &e), "链队列再次出队成功");
    ds_check_ll(e, 2, "链队列再出队 2");
    LQ_DeQueue(&Q, &e);
    ds_check_true(!LQ_DeQueue(&Q, &e), "链队列为空时出队被拒绝");
}

/* ==================== 3.6 舞伴问题 ==================== */

typedef struct {
    const char *names[16];
    int front;
    int rear;
} NameQueue;

static void NQ_Init(NameQueue *Q)
{
    Q->front = Q->rear = 0;
}

static void NQ_Push(NameQueue *Q, const char *name)
{
    Q->names[Q->rear++] = name;
}

static bool NQ_Pop(NameQueue *Q, const char **name)
{
    if (Q->front == Q->rear) {
        return false;
    }
    *name = Q->names[Q->front++];
    return true;
}

static void demo_dance_partners(void)
{
    NameQueue males;
    NameQueue females;
    const char *m = NULL;
    const char *f = NULL;
    int pairs = 0;

    ds_section("3.6", "案例分析与实现：舞伴问题");
    ds_note("男女各自排队，队头依次出队配对；某一队空时，另一队剩下的人等待。");
    NQ_Init(&males);
    NQ_Init(&females);
    NQ_Push(&males, "M1");
    NQ_Push(&males, "M2");
    NQ_Push(&males, "M3");
    NQ_Push(&males, "M4");
    NQ_Push(&females, "F1");
    NQ_Push(&females, "F2");
    NQ_Push(&females, "F3");

    printf("  配对结果：");
    while (NQ_Pop(&males, &m) && NQ_Pop(&females, &f)) {
        printf("%s-%s%s", m, f, pairs == 2 ? "" : "  ");
        pairs++;
        m = NULL;
        f = NULL;
    }
    printf("\n");
    ds_check_ll(pairs, 3, "共配成 3 对");
    if (m != NULL) {
        ds_ok("等待的男宾：%s", m);
        ds_check_str(m, "M4", "剩下等待的是 M4");
    } else {
        ds_check_true(false, "应剩下 M4 等待");
    }
}

int ds_ch03(void)
{
    demo_seq_stack();
    demo_linked_stack();
    demo_conversion();
    demo_recursion();
    demo_recursion_stack();
    demo_recursion_efficiency();
    demo_recursion_to_iteration();
    demo_circular_queue();
    demo_linked_queue();
    demo_dance_partners();
    ds_section("3.7", "小结");
    ds_note("栈和队列都是操作受限的线性表：栈在表尾操作，队列在表头删除、");
    ds_note("表尾插入。顺序栈与链栈、循环队列与链队列各有适用场景；");
    ds_note("递归本质上是系统栈的使用，可用显式栈改写为非递归算法。");
    return 0;
}
