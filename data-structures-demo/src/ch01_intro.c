/*
 * ch01_intro.c —— 第 1 章 绪论
 *
 * 覆盖目录：
 *   1.1 数据结构的研究内容
 *   1.2.1 数据、数据元素、数据项和数据对象
 *   1.2.2 数据结构
 *   1.2.3 数据类型和抽象数据类型
 *   1.3 抽象数据类型的表示与实现
 *   1.4.1 算法的定义及特性
 *   1.4.2 评价算法优劣的基本标准
 *   1.4.3 算法的时间复杂度
 *   1.4.4 算法的空间复杂度
 *   1.5 小结
 */
#include "common.h"

/* ---------- 1.2.1 数据、数据元素、数据项、数据对象 ---------- */

typedef struct {
    int id;         /* 数据项：学号 */
    char name[16];  /* 数据项：姓名 */
    double score;   /* 数据项：成绩 */
} Student;          /* 数据元素：一名学生 */

static void demo_data_terms(void)
{
    Student class3[] = {
        {1001, "张三", 92.5},
        {1002, "李四", 78.0},
        {1003, "王五", 88.5},
    };
    int n = (int)(sizeof(class3) / sizeof(class3[0]));
    int i;

    ds_section("1.2", "数据结构的基本概念和术语");
    ds_section("1.2.1", "数据、数据元素、数据项和数据对象");
    ds_note("数据项是不能再分的最小单位，如学号、姓名、成绩。");
    ds_note("数据元素由若干数据项组成，如一条学生记录。");
    ds_note("性质相同的数据元素组成数据对象，如 3 名学生的成绩表。");
    for (i = 0; i < n; i++) {
        ds_ok("元素 %d：学号=%d，姓名=%s，成绩=%.1f", i + 1, class3[i].id,
              class3[i].name, class3[i].score);
    }
    ds_check_ll(n, 3, "数据对象中有 3 个数据元素");
}

/* ---------- 1.2.2 数据结构：逻辑结构的四种形态 ---------- */

static void demo_structures(void)
{
    int linear[] = {1, 2, 3, 4};     /* 线性结构：一对一 */
    int parent[] = {-1, 0, 0, 1};    /* 树形结构：一对多（父结点下标） */
    int graph[][2] = {{0, 1}, {1, 2}, {0, 2}}; /* 图结构：多对多 */
    int set_[] = {2, 3, 5, 7};       /* 集合结构：只有同属关系 */

    ds_section("1.2.2", "数据结构");
    ds_note("数据结构 = 逻辑结构 + 存储结构 + 数据的运算。");
    ds_note("逻辑结构分为集合、线性、树形、图状四种。");
    ds_ok("线性结构（一对一）：%d %d %d %d", linear[0], linear[1], linear[2],
          linear[3]);
    ds_ok("树形结构（一对多）：parent[1]=%d，parent[2]=%d，parent[3]=%d",
          parent[1], parent[2], parent[3]);
    ds_ok("图结构（多对多）：边 (%d,%d) (%d,%d) (%d,%d)", graph[0][0],
          graph[0][1], graph[1][0], graph[1][1], graph[2][0], graph[2][1]);
    ds_ok("集合结构（同属一个集合）：%d %d %d %d", set_[0], set_[1], set_[2],
          set_[3]);
    ds_check_ll(parent[3], 1, "树中结点 3 的父结点是 1");
}

/* ---------- 1.2.3 / 1.3 数据类型与抽象数据类型 ---------- */

typedef struct {
    double real;
    double imag;
} Complex; /* 抽象数据类型 Complex 的表示 */

typedef struct {
    double e[3]; /* 三元组：用一维数组表示，逻辑上是一个整体 */
} Triplet;

static bool TripletInit(Triplet *t, double a, double b, double c)
{
    if (t == NULL) {
        return false;
    }
    t->e[0] = a;
    t->e[1] = b;
    t->e[2] = c;
    return true;
}

static bool TripletGet(const Triplet *t, int i, double *out)
{
    if (t == NULL || out == NULL || i < 1 || i > 3) {
        return false;
    }
    *out = t->e[i - 1];
    return true;
}

static bool TripletPut(Triplet *t, int i, double value)
{
    if (t == NULL || i < 1 || i > 3) {
        return false;
    }
    t->e[i - 1] = value;
    return true;
}

static bool TripletMax(const Triplet *t, double *out)
{
    int i;
    if (t == NULL || out == NULL) {
        return false;
    }
    *out = t->e[0];
    for (i = 1; i < 3; i++) {
        if (t->e[i] > *out) {
            *out = t->e[i];
        }
    }
    return true;
}

static Complex complex_add(Complex a, Complex b)
{
    Complex c = {a.real + b.real, a.imag + b.imag};
    return c;
}

static Complex complex_mul(Complex a, Complex b)
{
    Complex c = {a.real * b.real - a.imag * b.imag,
                 a.real * b.imag + a.imag * b.real};
    return c;
}

static void demo_adt(void)
{
    Complex a = {3.0, 4.0};
    Complex b = {1.0, 2.0};
    Complex sum = complex_add(a, b);
    Complex prod = complex_mul(a, b);
    Triplet t;
    double v = 0.0;
    double maxv = 0.0;

    ds_section("1.2.3", "数据类型和抽象数据类型");
    ds_note("数据类型 = 一组值的集合 + 定义在这组值上的一组操作。");
    ds_note("抽象数据类型（ADT）只关心逻辑特性，不关心具体存储和实现。");
    ds_ok("ADT Complex：(%g+%gi) + (%g+%gi) = (%g+%gi)", a.real, a.imag,
          b.real, b.imag, sum.real, sum.imag);
    ds_ok("ADT Complex：(%g+%gi) * (%g+%gi) = (%g+%gi)", a.real, a.imag,
          b.real, b.imag, prod.real, prod.imag);
    ds_check_ll((long long)sum.real, 4, "复数加法实部为 4");
    ds_check_ll((long long)sum.imag, 6, "复数加法虚部为 6");
    ds_check_ll((long long)prod.real, -5, "复数乘法实部为 -5");
    ds_check_ll((long long)prod.imag, 10, "复数乘法虚部为 10");

    ds_section("1.3", "抽象数据类型的表示与实现");
    ds_note("用 C 的 struct 表示 ADT 的值，用一组函数实现 ADT 的操作。");
    ds_check_true(TripletInit(&t, 8.0, 3.0, 5.0), "InitTriplet 成功");
    ds_check_true(TripletGet(&t, 2, &v), "Get 第 2 个分量成功");
    ds_check_ll((long long)v, 3, "第 2 个分量的值为 3");
    ds_check_true(TripletPut(&t, 3, 9.0), "Put 第 3 个分量成功");
    ds_check_true(TripletMax(&t, &maxv), "Max 求最大值成功");
    ds_check_ll((long long)maxv, 9, "三元组最大值为 9");
    ds_check_true(!TripletGet(&t, 4, &v), "越界访问被拒绝（健壮性）");
}

/* ---------- 1.4.1 算法的定义及特性 ---------- */

static int max_of(const int *a, int n, bool *ok)
{
    int i;
    int m;
    if (a == NULL || n <= 0) {
        if (ok != NULL) {
            *ok = false;
        }
        return 0;
    }
    m = a[0];
    for (i = 1; i < n; i++) {
        if (a[i] > m) {
            m = a[i];
        }
    }
    if (ok != NULL) {
        *ok = true;
    }
    return m;
}

static void demo_algorithm(void)
{
    int a[] = {7, 2, 9, 4, 9, 1};
    bool ok = false;
    int m = max_of(a, 6, &ok);

    ds_section("1.4", "算法和算法分析");
    ds_section("1.4.1", "算法的定义及特性");
    ds_note("算法是对特定问题求解步骤的描述，具有有穷性、确定性、可行性、");
    ds_note("输入和输出五个特性。");
    ds_check_true(ok, "max_of 正常结束（有穷性、可行性）");
    ds_check_ll(m, 9, "同一输入恒得同一输出（确定性）");
    ds_note("函数 max_of 有 1 个输入、1 个输出；空输入时返回失败标志。");
}

/* ---------- 1.4.2 评价算法优劣的基本标准 ---------- */

static bool safe_average(const int *a, int n, double *avg)
{
    int i;
    long long sum = 0;
    if (a == NULL || n <= 0 || avg == NULL) {
        return false; /* 健壮性：非法输入不崩溃 */
    }
    for (i = 0; i < n; i++) {
        sum += a[i];
    }
    *avg = (double)sum / n;
    return true;
}

static void demo_evaluation(void)
{
    int a[] = {80, 90, 100};
    double avg = 0.0;

    ds_section("1.4.2", "评价算法优劣的基本标准");
    ds_note("基本标准：正确性、可读性、健壮性、高效率与低存储量。");
    ds_check_true(safe_average(a, 3, &avg), "合法输入返回成功（正确性）");
    ds_check_true(avg == 90.0, "平均分为 90（正确性）");
    ds_check_true(!safe_average(NULL, 3, &avg), "空指针被拒绝（健壮性）");
    ds_check_true(!safe_average(a, 0, &avg), "空数组被拒绝（健壮性）");
    ds_note("函数只用一个累加变量，空间开销与 n 无关（低存储量）。");
}

/* ---------- 1.4.3 算法的时间复杂度 ---------- */

static long long count_max_ops(const int *a, int n)
{
    long long ops = 0;
    int i;
    int m;
    if (n <= 0) {
        return 0;
    }
    m = a[0];
    ops++;
    for (i = 1; i < n; i++) {
        ops++; /* 循环体执行一次记一次基本操作 */
        if (a[i] > m) {
            m = a[i];
        }
    }
    return ops;
}

static long long count_sum_ops(int n)
{
    long long ops = 0;
    int i;
    for (i = 1; i <= n; i++) {
        ops++; /* 每次累加记一次基本操作 */
    }
    return ops;
}

static long long count_pairs_ops(int n)
{
    long long ops = 0;
    int i, j;
    for (i = 1; i <= n; i++) {
        for (j = i + 1; j <= n; j++) {
            ops++;
        }
    }
    return ops;
}

static int bsearch_count(const int *a, int n, int key, int *cmp)
{
    int low = 0;
    int high = n - 1;
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
    return -1;
}

static void demo_time_complexity(void)
{
    int sizes[] = {10, 100, 1000};
    int i;
    int a[1000];
    int cmp = 0;
    int pos;

    ds_section("1.4.3", "算法的时间复杂度");
    ds_note("大 O 记号只保留最高阶项、忽略常数因子。");
    ds_note("常见阶：O(1) < O(log n) < O(n) < O(n log n) < O(n^2) < O(2^n)。");
    for (i = 0; i < 3; i++) {
        int n = sizes[i];
        ds_ok("n=%4d：sum 操作数=%lld（O(n)），数对操作数=%lld（O(n^2)）", n,
              count_sum_ops(n), count_pairs_ops(n));
    }
    ds_check_ll(count_sum_ops(100), 100, "O(n) 求和循环执行 n 次");
    ds_check_ll(count_pairs_ops(100), 100 * 99 / 2, "O(n^2) 数对执行 n(n-1)/2 次");

    for (i = 0; i < 1000; i++) {
        a[i] = i + 1;
    }
    pos = bsearch_count(a, 1000, 777, &cmp);
    ds_ok("在 1..1000 中折半查找 777：下标=%d，比较次数=%d", pos, cmp);
    ds_check_ll(pos, 776, "折半查找返回正确下标 776");
    ds_check_true(cmp <= 10, "1000 个元素的折半查找比较次数不超过 10");

    ds_ok("n=1000：max 操作数=%lld（O(n)）", count_max_ops(a, 1000));
    ds_check_ll(count_max_ops(a, 1000), 1000, "O(n) 求最大值恰好 n 次基本操作");
}

/* ---------- 1.4.4 算法的空间复杂度 ---------- */

static void reverse_in_place(int *a, int n)
{
    int i;
    for (i = 0; i < n / 2; i++) {
        int t = a[i];
        a[i] = a[n - 1 - i];
        a[n - 1 - i] = t;
    }
}

static void reverse_with_copy(const int *a, int n, int *out)
{
    int i;
    for (i = 0; i < n; i++) {
        out[i] = a[n - 1 - i];
    }
}

static void demo_space_complexity(void)
{
    int a[] = {1, 2, 3, 4, 5};
    int original[] = {1, 2, 3, 4, 5};
    int b[5];
    int i;

    ds_section("1.4.4", "算法的空间复杂度");
    ds_note("空间复杂度只统计辅助空间，不计输入本身占用的空间。");
    reverse_in_place(a, 5);
    ds_ok("原地逆置（O(1) 辅助空间）：%d %d %d %d %d", a[0], a[1], a[2], a[3],
          a[4]);
    ds_check_ll(a[0], 5, "原地逆置后首元素为 5");

    reverse_with_copy(original, 5, b);
    ds_ok("借助数组逆置（O(n) 辅助空间）：%d %d %d %d %d", b[0], b[1], b[2],
          b[3], b[4]);
    for (i = 0; i < 5; i++) {
        ds_check_ll(b[i], 5 - i, "辅助数组逆置结果正确");
    }
    ds_note("原地算法只增加 i、t 等常数个变量，空间复杂度为 O(1)；");
    ds_note("辅助数组法需要 n 个额外单元，空间复杂度为 O(n)。");
}

int ds_ch01(void)
{
    ds_section("1.1", "数据结构的研究内容");
    ds_note("数据结构研究数据的逻辑结构、存储结构以及定义在其上的运算，");
    ds_note("并分析这些运算的时间与空间效率。");
    demo_data_terms();
    demo_structures();
    demo_adt();
    demo_algorithm();
    demo_evaluation();
    demo_time_complexity();
    demo_space_complexity();
    ds_section("1.5", "小结");
    ds_note("本章建立三个基本认识：数据结构的三要素、ADT 的表示与实现、");
    ds_note("用时间与空间复杂度评价算法。后续各章都按“逻辑结构—存储结构—");
    ds_note("运算实现—复杂度分析”这条线索展开。");
    return 0;
}
