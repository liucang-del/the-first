/*
 * ch08_sort.c —— 第 8 章 排序
 *
 * 覆盖目录：
 *   8.1 基本概念和排序方法概述
 *   8.2 插入排序（直接插入、折半插入、希尔）
 *   8.3 交换排序（冒泡、快速）
 *   8.4 选择排序（简单选择、树形选择、堆）
 *   8.5 归并排序
 *   8.6 基数排序（多关键字、链式基数）
 *   8.7 外部排序（基本方法、多路平衡归并、置换-选择、最佳归并树）
 *   8.8 小结
 */
#include "common.h"

typedef struct {
    long long cmp;
    long long moves;
} Stats;

static void stats_reset(Stats *st)
{
    st->cmp = 0;
    st->moves = 0;
}

static void swap_int(int *a, int *b, Stats *st)
{
    int t = *a;
    *a = *b;
    *b = t;
    st->moves += 3;
}

static bool is_sorted(const int *a, int n)
{
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) {
            return false;
        }
    }
    return true;
}

static void print_array(const int *a, int n)
{
    int i;
    printf("  [");
    for (i = 0; i < n; i++) {
        printf("%d%s", a[i], i + 1 == n ? "" : " ");
    }
    printf("]\n");
}

static void copy_array(const int *src, int *dst, int n)
{
    memcpy(dst, src, sizeof(int) * (size_t)n);
}

/* ==================== 8.2.1 直接插入排序 ==================== */

static void insertion_sort(int *a, int n, Stats *st)
{
    int i;
    for (i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        st->moves++;
        while (j >= 0 && (st->cmp++, a[j] > key)) {
            a[j + 1] = a[j];
            st->moves++;
            j--;
        }
        a[j + 1] = key;
        st->moves++;
    }
}

/* ==================== 8.2.2 折半插入排序 ==================== */

static void binary_insertion_sort(int *a, int n, Stats *st)
{
    int i;
    for (i = 1; i < n; i++) {
        int key = a[i];
        int low = 0;
        int high = i - 1;
        int j;
        while (low <= high) {
            int mid = (low + high) / 2;
            st->cmp++;
            if (a[mid] > key) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }
        for (j = i - 1; j >= low; j--) {
            a[j + 1] = a[j];
            st->moves++;
        }
        a[low] = key;
        st->moves++;
    }
}

/* ==================== 8.2.3 希尔排序 ==================== */

static void shell_sort(int *a, int n, Stats *st)
{
    int gap;
    for (gap = n / 2; gap > 0; gap /= 2) {
        int i;
        for (i = gap; i < n; i++) {
            int key = a[i];
            int j = i - gap;
            st->moves++;
            while (j >= 0 && (st->cmp++, a[j] > key)) {
                a[j + gap] = a[j];
                st->moves++;
                j -= gap;
            }
            a[j + gap] = key;
            st->moves++;
        }
    }
}

/* ==================== 8.3.1 冒泡排序 ==================== */

static void bubble_sort(int *a, int n, Stats *st)
{
    int i;
    for (i = 0; i < n - 1; i++) {
        bool changed = false;
        int j;
        for (j = 0; j < n - 1 - i; j++) {
            st->cmp++;
            if (a[j] > a[j + 1]) {
                swap_int(&a[j], &a[j + 1], st);
                changed = true;
            }
        }
        if (!changed) {
            break; /* 本趟无交换，已经有序 */
        }
    }
}

/* ==================== 8.3.2 快速排序 ==================== */

static int partition(int *a, int low, int high, Stats *st)
{
    int pivot = a[high];
    int i = low - 1;
    int j;
    for (j = low; j < high; j++) {
        st->cmp++;
        if (a[j] <= pivot) {
            i++;
            swap_int(&a[i], &a[j], st);
        }
    }
    swap_int(&a[i + 1], &a[high], st);
    return i + 1;
}

static void quick_sort_rec(int *a, int low, int high, Stats *st)
{
    if (low < high) {
        int p = partition(a, low, high, st);
        quick_sort_rec(a, low, p - 1, st);
        quick_sort_rec(a, p + 1, high, st);
    }
}

static void quick_sort(int *a, int n, Stats *st)
{
    quick_sort_rec(a, 0, n - 1, st);
}

/* ==================== 8.4.1 简单选择排序 ==================== */

static void selection_sort(int *a, int n, Stats *st)
{
    int i, j;
    for (i = 0; i < n - 1; i++) {
        int min = i;
        for (j = i + 1; j < n; j++) {
            st->cmp++;
            if (a[j] < a[min]) {
                min = j;
            }
        }
        if (min != i) {
            swap_int(&a[i], &a[min], st);
        }
    }
}

/* ==================== 8.4.2 树形选择排序 ==================== */

#define TREE_LEAVES 8
#define TREE_INF 0x3f3f3f3f

static void tree_selection_sort(const int *a, int n, int *out, Stats *st)
{
    int tree[2 * TREE_LEAVES];
    int i;
    int k;

    for (i = 0; i < TREE_LEAVES; i++) {
        tree[TREE_LEAVES + i] = i < n ? a[i] : TREE_INF;
    }
    for (i = TREE_LEAVES - 1; i >= 1; i--) {
        st->cmp++;
        tree[i] = tree[2 * i] <= tree[2 * i + 1] ? tree[2 * i]
                                                : tree[2 * i + 1];
    }
    for (k = 0; k < n; k++) {
        int idx = 1;
        out[k] = tree[1];
        while (idx < TREE_LEAVES) {
            st->cmp++;
            idx = tree[2 * idx] <= tree[2 * idx + 1] ? 2 * idx : 2 * idx + 1;
        }
        tree[idx] = TREE_INF;
        for (idx /= 2; idx >= 1; idx /= 2) {
            st->cmp++;
            tree[idx] = tree[2 * idx] <= tree[2 * idx + 1] ? tree[2 * idx]
                                                           : tree[2 * idx + 1];
        }
    }
}

/* ==================== 8.4.3 堆排序 ==================== */

static void heap_sift_down(int *a, int start, int n, Stats *st)
{
    int root = start;
    while (2 * root + 1 < n) {
        int child = 2 * root + 1;
        int sw = root;
        st->cmp++;
        if (a[sw] < a[child]) {
            sw = child;
        }
        if (child + 1 < n) {
            st->cmp++;
            if (a[sw] < a[child + 1]) {
                sw = child + 1;
            }
        }
        if (sw == root) {
            return;
        }
        swap_int(&a[root], &a[sw], st);
        root = sw;
    }
}

static void heap_sort(int *a, int n, Stats *st)
{
    int i;
    for (i = n / 2 - 1; i >= 0; i--) {
        heap_sift_down(a, i, n, st);
    }
    for (i = n - 1; i > 0; i--) {
        swap_int(&a[0], &a[i], st);
        heap_sift_down(a, 0, i, st);
    }
}

/* ==================== 8.5 归并排序 ==================== */

static void merge_sort_rec(int *a, int *tmp, int low, int high, Stats *st)
{
    int mid;
    int i, j, k;
    if (low >= high) {
        return;
    }
    mid = (low + high) / 2;
    merge_sort_rec(a, tmp, low, mid, st);
    merge_sort_rec(a, tmp, mid + 1, high, st);
    i = low;
    j = mid + 1;
    k = low;
    while (i <= mid && j <= high) {
        st->cmp++;
        if (a[i] <= a[j]) {
            tmp[k++] = a[i++];
        } else {
            tmp[k++] = a[j++];
        }
        st->moves++;
    }
    while (i <= mid) {
        tmp[k++] = a[i++];
        st->moves++;
    }
    while (j <= high) {
        tmp[k++] = a[j++];
        st->moves++;
    }
    for (i = low; i <= high; i++) {
        a[i] = tmp[i];
        st->moves++;
    }
}

static void merge_sort(int *a, int n, Stats *st)
{
    int *tmp = (int *)malloc(sizeof(int) * (size_t)n);
    merge_sort_rec(a, tmp, 0, n - 1, st);
    free(tmp);
}

/* ==================== 8.6 基数排序 ==================== */

static void radix_sort(int *a, int n, Stats *st)
{
    int exp;
    int *tmp = (int *)malloc(sizeof(int) * (size_t)n);
    for (exp = 1; exp <= 100; exp *= 10) {
        int count[10] = {0};
        int i;
        for (i = 0; i < n; i++) {
            count[(a[i] / exp) % 10]++;
            st->cmp++;
        }
        for (i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }
        for (i = n - 1; i >= 0; i--) {
            int digit = (a[i] / exp) % 10;
            tmp[--count[digit]] = a[i];
            st->moves++;
        }
        for (i = 0; i < n; i++) {
            a[i] = tmp[i];
            st->moves++;
        }
    }
    free(tmp);
}

/* 多关键字排序：扑克牌先按花色、再按点数 */
typedef struct {
    int suit;
    int rank;
} Card;

static int card_cmp(const Card *x, const Card *y)
{
    if (x->suit != y->suit) {
        return x->suit - y->suit;
    }
    return x->rank - y->rank;
}

static void multi_key_sort(Card *c, int n)
{
    int i;
    for (i = 1; i < n; i++) { /* 稳定插入排序，保证同一花色内点数有序 */
        Card key = c[i];
        int j = i - 1;
        while (j >= 0 && card_cmp(&c[j], &key) > 0) {
            c[j + 1] = c[j];
            j--;
        }
        c[j + 1] = key;
    }
}

/* ==================== 8.7 外部排序 ==================== */

/* 把输入分成若干块，块内排序，得到初始归并段 */
static int make_runs(const int *a, int n, int block, int runs[][16],
                     int lens[], int max_runs)
{
    int count = 0;
    int pos = 0;
    while (pos < n && count < max_runs) {
        int len = n - pos < block ? n - pos : block;
        int i;
        for (i = 0; i < len; i++) {
            runs[count][i] = a[pos + i];
        }
        /* 对当前块做直接插入排序 */
        for (i = 1; i < len; i++) {
            int key = runs[count][i];
            int j = i - 1;
            while (j >= 0 && runs[count][j] > key) {
                runs[count][j + 1] = runs[count][j];
                j--;
            }
            runs[count][j + 1] = key;
        }
        lens[count] = len;
        count++;
        pos += len;
    }
    return count;
}

/* 胜者树实现 k 路平衡归并；每个内部结点保存当前最小值 */
#define MERGE_K 4

static int winner_tree_merge(int runs[][16], const int *lens, int k, int *out,
                             Stats *st)
{
    int pos[MERGE_K] = {0};
    int tree[2 * MERGE_K];
    int total = 0;
    int i;

    for (i = 0; i < MERGE_K; i++) {
        tree[MERGE_K + i] = (i < k && pos[i] < lens[i]) ? runs[i][pos[i]]
                                                        : TREE_INF;
    }
    for (i = MERGE_K - 1; i >= 1; i--) {
        st->cmp++;
        tree[i] = tree[2 * i] <= tree[2 * i + 1] ? tree[2 * i]
                                                 : tree[2 * i + 1];
    }
    while (tree[1] != TREE_INF) {
        int idx = 1;
        out[total++] = tree[1];
        while (idx < MERGE_K) {
            st->cmp++;
            idx = tree[2 * idx] <= tree[2 * idx + 1] ? 2 * idx : 2 * idx + 1;
        }
        i = idx - MERGE_K;
        pos[i]++;
        tree[idx] = (i < k && pos[i] < lens[i]) ? runs[i][pos[i]] : TREE_INF;
        for (idx /= 2; idx >= 1; idx /= 2) {
            st->cmp++;
            tree[idx] = tree[2 * idx] <= tree[2 * idx + 1] ? tree[2 * idx]
                                                           : tree[2 * idx + 1];
        }
    }
    return total;
}

/* 置换-选择排序：堆容量为 w，当前段用堆维护，装不下的记录冻结到下一段 */

static void heap_item_sift_values(int *h, int n, int i)
{
    while (2 * i + 1 < n) {
        int child = 2 * i + 1;
        int sw = i;
        if (h[sw] > h[child]) {
            sw = child;
        }
        if (child + 1 < n && h[sw] > h[child + 1]) {
            sw = child + 1;
        }
        if (sw == i) {
            return;
        }
        {
            int t = h[i];
            h[i] = h[sw];
            h[sw] = t;
        }
        i = sw;
    }
}

static int replacement_selection(const int *in, int n, int w, int runs[][16],
                                 int lens[], int max_runs)
{
    int heap[16];
    int frozen[16];
    int hn = 0;
    int fn = 0;
    int pos = 0;
    int current = 0;
    int last = -1;
    int i;

    while (pos < n && hn < w) {
        heap[hn++] = in[pos++];
    }
    for (i = hn / 2 - 1; i >= 0; i--) {
        heap_item_sift_values(heap, hn, i);
    }

    while (current < max_runs) {
        while (hn > 0) {
            int top = heap[0];
            if (lens[current] < 16) {
                runs[current][lens[current]++] = top;
            }
            last = top;
            if (pos < n) {
                int x = in[pos++];
                if (x >= last) {
                    heap[0] = x; /* 能接在当前段后面，留在堆中 */
                } else {
                    frozen[fn++] = x; /* 比 last 小，冻结到下一段 */
                    hn--;
                    heap[0] = heap[hn];
                }
            } else {
                hn--;
                heap[0] = heap[hn];
            }
            if (hn > 0) {
                heap_item_sift_values(heap, hn, 0);
            }
        }
        if (fn == 0) {
            break;
        }
        current++;
        if (current >= max_runs) {
            break;
        }
        for (i = 0; i < fn; i++) {
            heap[i] = frozen[i];
        }
        hn = fn;
        fn = 0;
        last = -1;
        for (i = hn / 2 - 1; i >= 0; i--) {
            heap_item_sift_values(heap, hn, i);
        }
    }
    return current + 1;
}

/* 最佳归并树：每次合并长度最小的两段，总读写代价最小 */
static long long optimal_merge_tree(int *w, int n)
{
    long long total = 0;
    while (n > 1) {
        int i, j;
        int min1 = 0;
        int min2 = 1;
        if (w[min1] > w[min2]) {
            int t = min1;
            min1 = min2;
            min2 = t;
        }
        for (i = 2; i < n; i++) {
            if (w[i] < w[min1]) {
                min2 = min1;
                min1 = i;
            } else if (w[i] < w[min2]) {
                min2 = i;
            }
        }
        total += w[min1] + w[min2];
        w[min1] = w[min1] + w[min2];
        for (j = min2; j < n - 1; j++) {
            w[j] = w[j + 1];
        }
        n--;
    }
    return total;
}

/* ==================== 演示入口 ==================== */

static int base[] = {49, 38, 65, 97, 76, 13, 27, 49};
#define BASE_N ((int)(sizeof(base) / sizeof(base[0])))

static void demo_basic_concepts(void)
{
    ds_section("8.1", "基本概念和排序方法概述");
    ds_section("8.1.1", "排序的基本概念");
    ds_note("排序是把记录按关键字递增（或递减）排列；若关键字相同记录的相对");
    ds_note("次序不变，则该排序方法是稳定的。");
    ds_section("8.1.2", "内部排序方法的分类");
    ds_note("插入类：直接插入、折半插入、希尔；交换类：冒泡、快速；");
    ds_note("选择类：简单选择、树形选择、堆；归并类：归并；分配类：基数。");
    ds_section("8.1.3", "待排序记录的存储方式");
    ds_note("记录可顺序存放在数组中，也可用链表存放；本演示用数组。");
    ds_section("8.1.4", "排序算法效率的评价指标");
    ds_note("主要看比较次数、移动次数和辅助空间；还要看稳定性。");
}

static void demo_sort_one(const char *name, void (*fn)(int *, int, Stats *),
                          const int *src, int n, Stats *st)
{
    int buf[16];
    copy_array(src, buf, n);
    stats_reset(st);
    fn(buf, n, st);
    ds_ok("%s：比较 %lld 次，移动 %lld 次", name, st->cmp, st->moves);
    print_array(buf, n);
    ds_check_true(is_sorted(buf, n), name);
}

static void demo_insert_sorts(void)
{
    Stats st;

    ds_section("8.2", "插入排序");
    ds_section("8.2.1", "直接插入排序");
    ds_note("把待插记录插入前面已排好序的子序列；基本有序时接近 O(n)。");
    demo_sort_one("直接插入排序", insertion_sort, base, BASE_N, &st);

    ds_section("8.2.2", "折半插入排序");
    ds_note("用折半查找确定插入位置，减少比较次数，但移动次数不变。");
    demo_sort_one("折半插入排序", binary_insertion_sort, base, BASE_N, &st);

    ds_section("8.2.3", "希尔排序");
    ds_note("按递减增量分组做插入排序，最后一趟增量必须为 1；时间约 O(n^1.3)。");
    demo_sort_one("希尔排序", shell_sort, base, BASE_N, &st);
}

static void demo_exchange_sorts(void)
{
    Stats st;

    ds_section("8.3", "交换排序");
    ds_section("8.3.1", "冒泡排序");
    ds_note("相邻元素两两比较，大的后移；某趟无交换即可提前结束。");
    demo_sort_one("冒泡排序", bubble_sort, base, BASE_N, &st);

    ds_section("8.3.2", "快速排序");
    ds_note("选枢轴把表分成两段，再递归排序；平均 O(n log n)，最坏 O(n^2)。");
    demo_sort_one("快速排序", quick_sort, base, BASE_N, &st);
    {
        int rev[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
        int buf[9];
        copy_array(rev, buf, 9);
        stats_reset(&st);
        quick_sort(buf, 9, &st);
        ds_check_true(is_sorted(buf, 9), "逆序输入快速排序结果正确");
        ds_ok("逆序输入时快速排序比较 %lld 次", st.cmp);
    }
}

static void demo_selection_sorts(void)
{
    Stats st;

    ds_section("8.4", "选择排序");
    ds_section("8.4.1", "简单选择排序");
    ds_note("每趟从待排区选最小元素放到已排区末尾；比较次数与初始序列无关。");
    demo_sort_one("简单选择排序", selection_sort, base, BASE_N, &st);

    ds_section("8.4.2", "树形选择排序");
    ds_note("用完全二叉树的内部结点记录两个孩子中的较小者，每选出一个最小");
    ds_note("元素只需沿路径修改，比较次数 O(n log n)。");
    {
        int out[8];
        stats_reset(&st);
        tree_selection_sort(base, 8, out, &st);
        ds_ok("树形选择排序：比较 %lld 次", st.cmp);
        ds_check_true(is_sorted(out, 8), "树形选择排序结果有序");
        ds_check_ll(out[0], 13, "树形选择排序最小值为 13");
        ds_check_ll(out[7], 97, "树形选择排序最大值为 97");
    }

    ds_section("8.4.3", "堆排序");
    ds_note("先建大根堆，再反复把堆顶与堆尾交换并调整；最坏也是 O(n log n)。");
    demo_sort_one("堆排序", heap_sort, base, BASE_N, &st);
}

static void demo_merge_sort(void)
{
    Stats st;

    ds_section("8.5", "归并排序");
    ds_note("把两个有序子表合并成一个有序表；时间 O(n log n)，需要 O(n) 辅助空间。");
    demo_sort_one("归并排序", merge_sort, base, BASE_N, &st);
}

static void demo_radix_sort(void)
{
    int a[] = {329, 457, 657, 839, 436, 720, 355};
    int n = (int)(sizeof(a) / sizeof(a[0]));
    Stats st;
    Card cards[] = {{2, 3}, {1, 5}, {2, 1}, {1, 3}, {2, 5}, {1, 1}};
    int m = (int)(sizeof(cards) / sizeof(cards[0]));
    int i;

    ds_section("8.6", "基数排序");
    ds_section("8.6.1", "多关键字的排序");
    ds_note("先按次要关键字排序，再按主要关键字稳定排序，结果按主要、次要");
    ds_note("关键字有序。下面是扑克牌先按花色、再按点数排序。");
    multi_key_sort(cards, m);
    printf("  排序后的牌：");
    for (i = 0; i < m; i++) {
        printf("(%d,%d)%s", cards[i].suit, cards[i].rank,
               i + 1 == m ? "" : " ");
    }
    printf("\n");
    for (i = 1; i < m; i++) {
        ds_check_true(card_cmp(&cards[i - 1], &cards[i]) <= 0,
                      "多关键字排序结果有序");
    }
    ds_check_ll(cards[0].rank, 1, "最小牌为花色 1、点数 1");

    ds_section("8.6.2", "链式基数排序");
    ds_note("按个位、十位、百位依次分配和收集，每趟稳定，共 d 趟；");
    ds_note("时间 O(d(n+r))，空间 O(n+r)，r 为基数。");
    stats_reset(&st);
    radix_sort(a, n, &st);
    ds_ok("基数排序：分配/收集 %lld 次", st.moves);
    print_array(a, n);
    ds_check_true(is_sorted(a, n), "基数排序结果有序");
    ds_check_ll(a[0], 329, "基数排序最小值为 329");
    ds_check_ll(a[6], 839, "基数排序最大值为 839");
}

static void demo_external_sort(void)
{
    int input[] = {42, 17, 93, 8, 55, 21, 76, 34, 60, 3, 88, 47,
                   12, 69, 25, 90, 51, 36};
    int n = (int)(sizeof(input) / sizeof(input[0]));
    int runs[8][16];
    int lens[8] = {0};
    int rc;
    int i;
    Stats st;

    ds_section("8.7", "外部排序");
    ds_section("8.7.1", "外部排序的基本方法");
    ds_note("数据量太大不能一次装入内存：先分段读入、内部排序生成初始归并段，");
    ds_note("再对这些归并段做多路归并，直到得到整个有序文件。");
    ds_note("这里用内存数组模拟外存块，重点演示归并段、胜者树和归并次序。");
    rc = make_runs(input, n, 3, runs, lens, 8);
    printf("  初始归并段（块长 3）：");
    for (i = 0; i < rc; i++) {
        int j;
        printf("[");
        for (j = 0; j < lens[i]; j++) {
            printf("%d%s", runs[i][j], j + 1 == lens[i] ? "" : " ");
        }
        printf("]%s", i + 1 == rc ? "" : " ");
    }
    printf("\n");
    ds_check_ll(rc, 6, "18 个记录、块长 3 得到 6 个初始归并段");
    for (i = 0; i < rc; i++) {
        ds_check_true(is_sorted(runs[i], lens[i]), "每个初始归并段内部有序");
    }

    ds_section("8.7.2", "多路平衡归并的实现");
    ds_note("用胜者树（败者树的对称形式）组织 k 路归并，每次取最小只需");
    ds_note("沿树调整 log2(k) 次，不必每步扫描 k 个段头。");
    {
        int r3[][16] = {{1, 5, 9}, {2, 7, 10}, {3, 4, 11}};
        int l3[3] = {3, 3, 3};
        int out[32];
        int total;
        stats_reset(&st);
        total = winner_tree_merge(r3, l3, 3, out, &st);
        ds_check_ll(total, 9, "3 路归并输出 9 个记录");
        ds_check_true(is_sorted(out, 9), "3 路归并结果有序");
        ds_check_ll(out[0], 1, "归并结果首元素为 1");
        ds_check_ll(out[8], 11, "归并结果末元素为 11");
    }

    ds_section("8.7.3", "置换-选择排序");
    ds_note("用容量为 w 的堆，输出当前段最小值；若新读入记录小于刚输出的值，");
    ds_note("则它属于下一段。这样一段的平均长度可接近 2w。");
    {
        int rr[8][16];
        int rl[8] = {0};
        int count;
        int total = 0;
        for (i = 0; i < 8; i++) {
            int j;
            for (j = 0; j < 16; j++) {
                rr[i][j] = 0;
            }
        }
        count = replacement_selection(input, n, 3, rr, rl, 8);
        printf("  置换-选择得到的归并段：");
        for (i = 0; i < count; i++) {
            int j;
            printf("[");
            total += rl[i];
            for (j = 0; j < rl[i]; j++) {
                printf("%d%s", rr[i][j], j + 1 == rl[i] ? "" : " ");
            }
            printf("]%s", i + 1 == count ? "" : " ");
        }
        printf("\n");
        ds_check_ll(total, n, "置换-选择覆盖全部 18 个记录");
        for (i = 0; i < count; i++) {
            ds_check_true(is_sorted(rr[i], rl[i]), "每个置换-选择段内部有序");
        }
    }

    ds_section("8.7.4", "最佳归并树");
    ds_note("把初始归并段长度看成叶权，按哈夫曼算法每次合并最小的两段，");
    ds_note("得到读写次数最少的最佳归并树。");
    {
        int w[] = {2, 3, 4, 5, 6};
        long long cost = optimal_merge_tree(w, 5);
        ds_ok("归并段长度 {2,3,4,5,6} 的最佳归并代价：%lld", cost);
        ds_check_ll(cost, 45, "最佳归并树的合并代价为 45");
    }
}

static void demo_stability(void)
{
    typedef struct {
        int key;
        int id;
    } Item;
    Item a[] = {{2, 1}, {1, 2}, {2, 3}, {1, 4}};
    int n = 4;
    int i;

    ds_section("8.1.4", "排序的稳定性");
    ds_note("关键字相同的记录若排序后相对次序不变，则方法是稳定的。");
    for (i = 1; i < n; i++) { /* 按 key 做直接插入排序，相等时不交换 */
        Item key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j].key > key.key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
    printf("  排序后（key,id）：");
    for (i = 0; i < n; i++) {
        printf("(%d,%d)%s", a[i].key, a[i].id, i + 1 == n ? "" : " ");
    }
    printf("\n");
    ds_check_ll(a[0].id, 2, "key=1 的记录保持原相对次序");
    ds_check_ll(a[1].id, 4, "key=1 的第二条记录仍在其后");
    ds_check_ll(a[2].id, 1, "key=2 的记录保持原相对次序");
    ds_check_ll(a[3].id, 3, "key=2 的第二条记录仍在其后");
}

int ds_ch08(void)
{
    demo_basic_concepts();
    demo_stability();
    demo_insert_sorts();
    demo_exchange_sorts();
    demo_selection_sorts();
    demo_merge_sort();
    demo_radix_sort();
    demo_external_sort();
    ds_section("8.8", "小结");
    ds_note("插入、交换、选择、归并、基数五类内部排序各有适用场景；");
    ds_note("快速排序平均最快但最坏 O(n^2)，堆排序和归并排序最坏 O(n log n)，");
    ds_note("归并排序稳定但费空间，基数排序适合关键字位数少的整数。");
    ds_note("外部排序的核心是减少外存读写：多路归并、置换-选择和最佳归并树。");
    return 0;
}
