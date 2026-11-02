/* common.c —— 公共输出与自检设施的实现 */
#include "common.h"

int g_checks = 0;
int g_failures = 0;

void ds_chapter(int no, const char *title)
{
    printf("\n");
    printf("===============================================================\n");
    printf("第 %d 章  %s\n", no, title);
    printf("===============================================================\n");
}

void ds_section(const char *no, const char *title)
{
    printf("\n[%s] %s\n", no, title);
}

void ds_note(const char *fmt, ...)
{
    va_list ap;
    printf("  ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void ds_ok(const char *fmt, ...)
{
    va_list ap;
    printf("  结果：");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

static void check_result(bool pass, const char *what)
{
    g_checks++;
    if (pass) {
        printf("  [PASS] %s\n", what);
    } else {
        g_failures++;
        printf("  [FAIL] %s\n", what);
    }
}

void ds_check_ll(long long actual, long long expected, const char *what)
{
    bool pass = actual == expected;
    g_checks++;
    if (pass) {
        printf("  [PASS] %s（实际 %lld）\n", what, actual);
    } else {
        g_failures++;
        printf("  [FAIL] %s（期望 %lld，实际 %lld）\n", what, expected, actual);
    }
}

void ds_check_true(bool cond, const char *what)
{
    check_result(cond, what);
}

void ds_check_str(const char *actual, const char *expected, const char *what)
{
    bool pass = actual != NULL && expected != NULL && strcmp(actual, expected) == 0;
    g_checks++;
    if (pass) {
        printf("  [PASS] %s（实际 \"%s\"）\n", what, actual);
    } else {
        g_failures++;
        printf("  [FAIL] %s（期望 \"%s\"，实际 \"%s\"）\n", what,
               expected ? expected : "(null)", actual ? actual : "(null)");
    }
}
