/*
 * main.c —— 《数据结构（C语言版 第2版）》目录知识点演示程序
 *
 * 用法：
 *   ./ds-demo          依次运行第 1～8 章全部演示
 *   ./ds-demo 3        只运行第 3 章
 *   ./ds-demo --list   列出各章标题
 *
 * 每个演示都会打印该书对应目录条目的要点，并用断言核对运行结果；
 * 所有断言通过时进程返回 0，否则返回 1。
 */
#include "common.h"

typedef int (*ChapterFn)(void);

typedef struct {
    const char *title;
    ChapterFn fn;
} Chapter;

static const Chapter kChapters[] = {
    {"绪论", ds_ch01},
    {"线性表", ds_ch02},
    {"栈和队列", ds_ch03},
    {"串、数组和广义表", ds_ch04},
    {"树和二叉树", ds_ch05},
    {"图", ds_ch06},
    {"查找", ds_ch07},
    {"排序", ds_ch08},
};

#define CHAPTER_COUNT ((int)(sizeof(kChapters) / sizeof(kChapters[0])))

static void print_usage(const char *prog)
{
    printf("用法：%s [章号 1-%d | --list]\n", prog, CHAPTER_COUNT);
    printf("  不带参数：运行全部章节\n");
}

static void print_list(void)
{
    int i;
    printf("《数据结构（C语言版 第2版）》目录知识点演示，共 %d 章：\n",
           CHAPTER_COUNT);
    for (i = 0; i < CHAPTER_COUNT; i++) {
        printf("  %d. %s\n", i + 1, kChapters[i].title);
    }
}

static int run_chapter(int index)
{
    int before = g_failures;
    ds_chapter(index + 1, kChapters[index].title);
    kChapters[index].fn();
    return g_failures - before;
}

int main(int argc, char **argv)
{
    int i;
    int failures;

    printf("《数据结构（C语言版 第2版）》知识点演示程序\n");
    printf("严蔚敏 李冬梅 吴伟民 编著 · 人民邮电出版社 · 2022 年\n");

    if (argc == 2 && (strcmp(argv[1], "--list") == 0 ||
                      strcmp(argv[1], "list") == 0)) {
        print_list();
        return 0;
    }

    if (argc > 2) {
        print_usage(argv[0]);
        return 2;
    }

    if (argc == 2) {
        char *end = NULL;
        long n = strtol(argv[1], &end, 10);
        if (end == argv[1] || *end != '\0' || n < 1 || n > CHAPTER_COUNT) {
            printf("无效章号：%s\n", argv[1]);
            print_usage(argv[0]);
            return 2;
        }
        printf("本次只运行第 %ld 章。\n", n);
        run_chapter((int)n - 1);
    } else {
        for (i = 0; i < CHAPTER_COUNT; i++) {
            run_chapter(i);
        }
    }

    failures = g_failures;
    printf("\n===============================================================\n");
    printf("检查汇总：共 %d 项，通过 %d 项，失败 %d 项\n",
           g_checks, g_checks - failures, failures);
    printf("===============================================================\n");
    if (failures == 0) {
        printf("全部知识点演示通过。\n");
    } else {
        printf("有 %d 项检查未通过，请查看上面的 [FAIL] 行。\n", failures);
    }
    return failures == 0 ? 0 : 1;
}
