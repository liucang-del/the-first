/*
 * common.h —— 演示程序的公共输出与自检设施
 *
 * 每个章节目录条目都对应一个 ds_chXX() 演示，演示中调用 ds_check_* 记录断言；
 * 程序结束时汇总检查结果，任何断言失败都会让进程返回非零。
 */
#ifndef DS_COMMON_H
#define DS_COMMON_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int g_checks;   /* 已执行的检查数 */
extern int g_failures; /* 失败数 */

/* 章标题与节目录标题，括号内是书中的目录编号 */
void ds_chapter(int no, const char *title);
void ds_section(const char *no, const char *title);

/* 说明性输出：结论、要点、运行结果 */
void ds_note(const char *fmt, ...);
void ds_ok(const char *fmt, ...);

/* 自检：通过时打印 [PASS]，失败时打印 [FAIL] 并累加失败数 */
void ds_check_ll(long long actual, long long expected, const char *what);
void ds_check_true(bool cond, const char *what);
void ds_check_str(const char *actual, const char *expected, const char *what);

/* 八章演示入口，返回该章新增的失败数（便于单独运行时汇总） */
int ds_ch01(void);
int ds_ch02(void);
int ds_ch03(void);
int ds_ch04(void);
int ds_ch05(void);
int ds_ch06(void);
int ds_ch07(void);
int ds_ch08(void);

#endif
