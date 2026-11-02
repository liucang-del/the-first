/*
 * ch06_graph.c —— 第 6 章 图
 *
 * 覆盖目录：
 *   6.1 图的定义和基本术语    6.2 案例引入    6.3 图的类型定义
 *   6.4 图的存储结构（邻接矩阵、邻接表、十字链表、邻接多重表）
 *   6.5 图的遍历（DFS、BFS）
 *   6.6 图的应用（最小生成树、最短路径、拓扑排序、关键路径）
 *   6.7 案例分析与实现（校园导航）    6.8 小结
 */
#include "common.h"

#define MAXV 16
#define INF 0x3f3f3f3f

/* ==================== 6.4.1 邻接矩阵 ==================== */

typedef struct {
    char vexs[MAXV];
    int arcs[MAXV][MAXV];
    int vexnum;
    int arcnum;
} MGraph;

static void mg_init(MGraph *G, const char *names)
{
    int i, j;
    int n = (int)strlen(names);
    G->vexnum = n;
    G->arcnum = 0;
    for (i = 0; i < n; i++) {
        G->vexs[i] = names[i];
        for (j = 0; j < n; j++) {
            G->arcs[i][j] = (i == j) ? 0 : INF;
        }
    }
}

static void mg_add_undirected(MGraph *G, int u, int v, int w)
{
    G->arcs[u][v] = w;
    G->arcs[v][u] = w;
    G->arcnum++;
}

static void mg_add_directed(MGraph *G, int u, int v, int w)
{
    G->arcs[u][v] = w;
    G->arcnum++;
}

static void mg_print(const MGraph *G)
{
    int i, j;
    printf("  邻接矩阵（INF 表示无边）：\n");
    printf("      ");
    for (i = 0; i < G->vexnum; i++) {
        printf("%4c", G->vexs[i]);
    }
    printf("\n");
    for (i = 0; i < G->vexnum; i++) {
        printf("  %c  ", G->vexs[i]);
        for (j = 0; j < G->vexnum; j++) {
            if (G->arcs[i][j] == INF) {
                printf("%4s", "∞");
            } else {
                printf("%4d", G->arcs[i][j]);
            }
        }
        printf("\n");
    }
}

/* ==================== 6.4.2 邻接表 ==================== */

typedef struct ArcNode {
    int adjvex;
    int weight;
    struct ArcNode *nextarc;
} ArcNode;

typedef struct {
    char data;
    ArcNode *firstarc;
} VNode;

typedef struct {
    VNode vertices[MAXV];
    int vexnum;
    int arcnum;
} ALGraph;

static void al_add(ALGraph *G, int u, int v, int w)
{
    ArcNode *p = (ArcNode *)malloc(sizeof(ArcNode));
    ArcNode *tail;
    p->adjvex = v;
    p->weight = w;
    p->nextarc = NULL;
    if (G->vertices[u].firstarc == NULL) {
        G->vertices[u].firstarc = p;
    } else {
        tail = G->vertices[u].firstarc;
        while (tail->nextarc != NULL) {
            tail = tail->nextarc;
        }
        tail->nextarc = p; /* 尾插，保持邻接点按输入顺序排列 */
    }
}

static void al_build_undirected(ALGraph *G, const char *names,
                                const int edges[][3], int m)
{
    int i;
    int n = (int)strlen(names);
    G->vexnum = n;
    G->arcnum = 0;
    for (i = 0; i < n; i++) {
        G->vertices[i].data = names[i];
        G->vertices[i].firstarc = NULL;
    }
    for (i = 0; i < m; i++) {
        al_add(G, edges[i][0], edges[i][1], edges[i][2]);
        al_add(G, edges[i][1], edges[i][0], edges[i][2]);
        G->arcnum++;
    }
}

static void al_print(const ALGraph *G)
{
    int i;
    printf("  邻接表：\n");
    for (i = 0; i < G->vexnum; i++) {
        const ArcNode *p;
        printf("  %c ->", G->vertices[i].data);
        for (p = G->vertices[i].firstarc; p != NULL; p = p->nextarc) {
            printf(" %c(%d)", G->vertices[p->adjvex].data, p->weight);
        }
        printf("\n");
    }
}

static void al_free(ALGraph *G)
{
    int i;
    for (i = 0; i < G->vexnum; i++) {
        ArcNode *p = G->vertices[i].firstarc;
        while (p != NULL) {
            ArcNode *q = p->nextarc;
            free(p);
            p = q;
        }
        G->vertices[i].firstarc = NULL;
    }
}

/* ==================== 6.3 图的类型定义 ==================== */

static void demo_graph_types(void)
{
    ds_section("6.1", "图的定义和基本术语");
    ds_note("图由顶点集和边集组成，用来描述对象之间的多对多关系。");
    ds_section("6.1.1", "图的定义");
    ds_note("图 G=(V,E)：V 是顶点集，E 是边集。边有方向为有向图，否则为无向图。");
    ds_section("6.1.2", "图的基本术语");
    ds_note("术语：度、入度、出度、路径、回路、连通图、连通分量、生成树、网。");

    ds_section("6.2", "案例引入");
    ds_note("交通网、通信网、工程进度图都可以抽象为图：顶点表示对象，");
    ds_note("边表示对象之间的联系，边上的权表示距离、费用或工期。");

    ds_section("6.3", "图的类型定义");
    ds_note("ADT Graph：CreateGraph、DestroyGraph、LocateVex、GetVex、PutVex、");
    ds_note("FirstAdjVex、NextAdjVex、InsertVex、DeleteVex、InsertArc、");
    ds_note("DeleteArc、DFSTraverse、BFSTraverse。");
}

/* ==================== 6.5.1 深度优先搜索 ==================== */

static void dfs_matrix(const MGraph *G, int v, bool *visited, char *buf, int *k)
{
    int i;
    visited[v] = true;
    buf[(*k)++] = G->vexs[v];
    for (i = 0; i < G->vexnum; i++) {
        if (G->arcs[v][i] != 0 && G->arcs[v][i] != INF && !visited[i]) {
            dfs_matrix(G, i, visited, buf, k);
        }
    }
}

static void dfs_matrix_nonrecursive(const MGraph *G, int start, char *buf,
                                    int *k)
{
    int stack[MAXV];
    int top = 0;
    bool visited[MAXV] = {false};
    int i;

    stack[top++] = start;
    while (top > 0) {
        int v = stack[--top];
        if (visited[v]) {
            continue;
        }
        visited[v] = true;
        buf[(*k)++] = G->vexs[v];
        for (i = G->vexnum - 1; i >= 0; i--) { /* 逆序压栈，弹出时按序号访问 */
            if (G->arcs[v][i] != 0 && G->arcs[v][i] != INF && !visited[i]) {
                stack[top++] = i;
            }
        }
    }
}

static void dfs_list(const ALGraph *G, int v, bool *visited, char *buf, int *k)
{
    const ArcNode *p;
    visited[v] = true;
    buf[(*k)++] = G->vertices[v].data;
    for (p = G->vertices[v].firstarc; p != NULL; p = p->nextarc) {
        if (!visited[p->adjvex]) {
            dfs_list(G, p->adjvex, visited, buf, k);
        }
    }
}

/* ==================== 6.5.2 广度优先搜索 ==================== */

static void bfs_matrix(const MGraph *G, int start, char *buf, int *k)
{
    int q[MAXV];
    int front = 0, rear = 0;
    bool visited[MAXV] = {false};
    int i;

    visited[start] = true;
    q[rear++] = start;
    while (front < rear) {
        int v = q[front++];
        buf[(*k)++] = G->vexs[v];
        for (i = 0; i < G->vexnum; i++) {
            if (G->arcs[v][i] != 0 && G->arcs[v][i] != INF && !visited[i]) {
                visited[i] = true;
                q[rear++] = i;
            }
        }
    }
}

static void demo_traverse(const MGraph *G, const ALGraph *AL)
{
    char buf[32];
    bool visited[MAXV];
    int k;

    ds_section("6.5", "图的遍历");
    ds_section("6.5.1", "深度优先搜索");
    ds_note("DFS 沿一条路走到底，再回退；用递归或栈实现，时间 O(n+e)。");
    memset(visited, 0, sizeof(visited));
    k = 0;
    dfs_matrix(G, 0, visited, buf, &k);
    buf[k] = '\0';
    ds_ok("邻接矩阵递归 DFS：%s", buf);
    ds_check_str(buf, "ABCDFE", "DFS 访问序列为 ABCDFE");

    k = 0;
    dfs_matrix_nonrecursive(G, 0, buf, &k);
    buf[k] = '\0';
    ds_ok("邻接矩阵非递归 DFS：%s", buf);
    ds_check_str(buf, "ABCDFE", "非递归 DFS 与递归结果一致");

    memset(visited, 0, sizeof(visited));
    k = 0;
    dfs_list(AL, 0, visited, buf, &k);
    buf[k] = '\0';
    ds_ok("邻接表 DFS：%s", buf);
    ds_check_str(buf, "ABCDFE", "邻接表 DFS 结果一致");

    ds_section("6.5.2", "广度优先搜索");
    ds_note("BFS 逐层访问，用队列实现；时间也是 O(n+e)，可求无权图最短路。");
    k = 0;
    bfs_matrix(G, 0, buf, &k);
    buf[k] = '\0';
    ds_ok("BFS 访问序列：%s", buf);
    ds_check_str(buf, "ABCDEF", "BFS 访问序列为 ABCDEF");
}

/* ==================== 6.4.3 十字链表 ==================== */

typedef struct ArcBox {
    int tailvex;
    int headvex;
    int weight;
    struct ArcBox *hlink; /* 指向弧头相同的下一条弧 */
    struct ArcBox *tlink; /* 指向弧尾相同的下一条弧 */
} ArcBox;

typedef struct {
    char data;
    ArcBox *firstin;
    ArcBox *firstout;
} VexNode;

typedef struct {
    VexNode xlist[MAXV];
    int vexnum;
    int arcnum;
} OLGraph;

static void ol_add(OLGraph *G, int tail, int head, int w)
{
    ArcBox *p = (ArcBox *)malloc(sizeof(ArcBox));
    p->tailvex = tail;
    p->headvex = head;
    p->weight = w;
    p->tlink = G->xlist[tail].firstout;
    G->xlist[tail].firstout = p;
    p->hlink = G->xlist[head].firstin;
    G->xlist[head].firstin = p;
    G->arcnum++;
}

static void demo_cross_list(void)
{
    OLGraph G;
    int edges[][3] = {{0, 1, 1}, {0, 2, 1}, {1, 2, 1}, {2, 0, 1}, {2, 3, 1}};
    int i;

    ds_section("6.4.3", "十字链表");
    ds_note("十字链表用于有向图：每条弧同时挂在弧尾的出链和弧头的入链上，");
    ds_note("既能快速找某顶点的所有出边，也能快速找所有入边。");
    G.vexnum = 4;
    G.arcnum = 0;
    for (i = 0; i < 4; i++) {
        G.xlist[i].data = (char)('A' + i);
        G.xlist[i].firstin = NULL;
        G.xlist[i].firstout = NULL;
    }
    for (i = 0; i < 5; i++) {
        ol_add(&G, edges[i][0], edges[i][1], edges[i][2]);
    }
    printf("  顶点  出边          入边\n");
    for (i = 0; i < G.vexnum; i++) {
        ArcBox *p;
        printf("  %c     ", G.xlist[i].data);
        for (p = G.xlist[i].firstout; p != NULL; p = p->tlink) {
            printf("%c ", G.xlist[p->headvex].data);
        }
        printf("        ");
        for (p = G.xlist[i].firstin; p != NULL; p = p->hlink) {
            printf("%c ", G.xlist[p->tailvex].data);
        }
        printf("\n");
    }
    ds_check_ll(G.arcnum, 5, "十字链表共有 5 条弧");
    ds_check_ll(G.xlist[2].firstin->headvex, 2, "顶点 C 的入边以 C 为弧头");
    ds_check_true(G.xlist[0].firstout != NULL, "顶点 A 有出边");
    for (i = 0; i < G.vexnum; i++) {
        ArcBox *p = G.xlist[i].firstout;
        while (p != NULL) {
            ArcBox *q = p->tlink;
            free(p);
            p = q;
        }
    }
}

/* ==================== 6.4.4 邻接多重表 ==================== */

typedef struct EBox {
    int ivex;
    int jvex;
    struct EBox *ilink;
    struct EBox *jlink;
    int mark;
} EBox;

typedef struct {
    char data;
    EBox *firstedge;
} VexBox;

typedef struct {
    VexBox adjmulist[MAXV];
    int vexnum;
    int edgenum;
} AMLGraph;

static void aml_add(AMLGraph *G, int u, int v, EBox *storage, int *used)
{
    EBox *p = &storage[(*used)++];
    p->ivex = u;
    p->jvex = v;
    p->mark = 0;
    p->ilink = G->adjmulist[u].firstedge;
    G->adjmulist[u].firstedge = p;
    p->jlink = G->adjmulist[v].firstedge;
    G->adjmulist[v].firstedge = p;
    G->edgenum++;
}

static void demo_adjacency_multilist(void)
{
    AMLGraph G;
    EBox storage[4];
    int edges[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    int i;

    ds_section("6.4.4", "邻接多重表");
    ds_note("邻接多重表用于无向图：一条边只存一个边结点，两个顶点共享它，");
    ds_note("便于删除边和判断边是否访问过。");
    G.vexnum = 4;
    G.edgenum = 0;
    for (i = 0; i < 4; i++) {
        G.adjmulist[i].data = (char)('A' + i);
        G.adjmulist[i].firstedge = NULL;
    }
    {
        int used = 0;
        for (i = 0; i < 4; i++) {
            aml_add(&G, edges[i][0], edges[i][1], storage, &used);
        }
    }
    printf("  顶点  相邻顶点\n");
    for (i = 0; i < G.vexnum; i++) {
        EBox *p;
        int degree = 0;
        printf("  %c     ", G.adjmulist[i].data);
        for (p = G.adjmulist[i].firstedge; p != NULL;
             p = (p->ivex == i) ? p->ilink : p->jlink) {
            int other = (p->ivex == i) ? p->jvex : p->ivex;
            printf("%c ", G.adjmulist[other].data);
            degree++;
        }
        printf("\n");
        ds_check_ll(degree, 2, "该顶点在 4 环中的度为 2");
    }
    ds_check_ll(G.edgenum, 4, "邻接多重表共有 4 条边");
    /* 边结点用局部数组分配，函数返回时自动回收，不会出现悬挂指针 */
}

/* ==================== 6.6.1 最小生成树 ==================== */

static int prim(const MGraph *G, int start)
{
    int dist[MAXV];
    int parent[MAXV];
    bool visited[MAXV] = {false};
    int total = 0;
    int i, k;

    for (i = 0; i < G->vexnum; i++) {
        dist[i] = G->arcs[start][i];
        parent[i] = start;
    }
    visited[start] = true;
    printf("  Prim 选边：");
    for (k = 1; k < G->vexnum; k++) {
        int min = INF;
        int u = -1;
        for (i = 0; i < G->vexnum; i++) {
            if (!visited[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1) {
            break;
        }
        visited[u] = true;
        total += dist[u];
        printf("%c-%c(%d) ", G->vexs[parent[u]], G->vexs[u], dist[u]);
        for (i = 0; i < G->vexnum; i++) {
            if (!visited[i] && G->arcs[u][i] < dist[i]) {
                dist[i] = G->arcs[u][i];
                parent[i] = u;
            }
        }
    }
    printf("\n");
    return total;
}

typedef struct {
    int u, v, w;
} Edge;

static int cmp_edge(const void *a, const void *b)
{
    const Edge *ea = (const Edge *)a;
    const Edge *eb = (const Edge *)b;
    return ea->w - eb->w;
}

static int uf_find(int *parent, int x)
{
    while (parent[x] != x) {
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

static int kruskal(const MGraph *G)
{
    Edge edges[MAXV * MAXV];
    int parent[MAXV];
    int m = 0;
    int total = 0;
    int used = 0;
    int i;

    for (i = 0; i < G->vexnum; i++) {
        int j;
        for (j = i + 1; j < G->vexnum; j++) {
            if (G->arcs[i][j] != INF) {
                edges[m].u = i;
                edges[m].v = j;
                edges[m].w = G->arcs[i][j];
                m++;
            }
        }
    }
    qsort(edges, (size_t)m, sizeof(Edge), cmp_edge);
    for (i = 0; i < G->vexnum; i++) {
        parent[i] = i;
    }
    printf("  Kruskal 选边：");
    for (i = 0; i < m && used < G->vexnum - 1; i++) {
        int ru = uf_find(parent, edges[i].u);
        int rv = uf_find(parent, edges[i].v);
        if (ru != rv) {
            parent[ru] = rv;
            total += edges[i].w;
            used++;
            printf("%c-%c(%d) ", G->vexs[edges[i].u], G->vexs[edges[i].v],
                   edges[i].w);
        }
    }
    printf("\n");
    return total;
}

static void demo_mst(const MGraph *G)
{
    int total;

    ds_section("6.6", "图的应用");
    ds_section("6.6.1", "最小生成树");
    ds_note("生成树含全部 n 个顶点、n-1 条边，且连通无回路；权值之和最小");
    ds_note("的生成树称最小生成树。Prim 适合稠密图，Kruskal 适合稀疏图。");
    total = prim(G, 0);
    ds_ok("Prim 最小生成树总权值：%d", total);
    ds_check_ll(total, 15, "Prim 求得的 MST 权值为 15");
    total = kruskal(G);
    ds_ok("Kruskal 最小生成树总权值：%d", total);
    ds_check_ll(total, 15, "Kruskal 求得的 MST 权值为 15");
}

/* ==================== 6.6.2 最短路径 ==================== */

static void dijkstra(const MGraph *G, int src, int dist[], int path[])
{
    bool done[MAXV] = {false};
    int i, k;

    for (i = 0; i < G->vexnum; i++) {
        dist[i] = G->arcs[src][i];
        path[i] = (G->arcs[src][i] < INF) ? src : -1;
    }
    dist[src] = 0;
    path[src] = -1; /* 源点没有前驱，作为递归输出路径的终止条件 */
    done[src] = true;
    for (k = 1; k < G->vexnum; k++) {
        int min = INF;
        int u = -1;
        for (i = 0; i < G->vexnum; i++) {
            if (!done[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1) {
            break;
        }
        done[u] = true;
        for (i = 0; i < G->vexnum; i++) {
            if (!done[i] && G->arcs[u][i] != INF &&
                dist[u] + G->arcs[u][i] < dist[i]) {
                dist[i] = dist[u] + G->arcs[u][i];
                path[i] = u;
            }
        }
    }
}

static void print_path(const MGraph *G, const int *path, int v)
{
    if (path[v] == -1) {
        printf("%c", G->vexs[v]);
        return;
    }
    print_path(G, path, path[v]);
    printf("->%c", G->vexs[v]);
}

static void floyd(const MGraph *G, int dist[][MAXV])
{
    int i, j, k;
    for (i = 0; i < G->vexnum; i++) {
        for (j = 0; j < G->vexnum; j++) {
            dist[i][j] = G->arcs[i][j];
        }
    }
    for (k = 0; k < G->vexnum; k++) {
        for (i = 0; i < G->vexnum; i++) {
            for (j = 0; j < G->vexnum; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

static void demo_shortest_path(const MGraph *G)
{
    int dist[MAXV];
    int path[MAXV];
    int all[MAXV][MAXV];
    int i;

    ds_section("6.6.2", "最短路径");
    ds_note("Dijkstra 求单源最短路径，每轮确定一个最近的顶点，O(n^2)。");
    ds_note("Floyd 求每对顶点间的最短路径，三重循环 O(n^3)，允许负权但不允许");
    ds_note("负权回路。");
    dijkstra(G, 0, dist, path);
    printf("  从 A 出发的最短距离：");
    for (i = 0; i < G->vexnum; i++) {
        printf("%c=%d%s", G->vexs[i], dist[i], i + 1 == G->vexnum ? "" : " ");
    }
    printf("\n");
    ds_check_ll(dist[1], 6, "A 到 B 的最短距离为 6");
    ds_check_ll(dist[2], 1, "A 到 C 的最短距离为 1");
    ds_check_ll(dist[3], 5, "A 到 D 的最短距离为 5");
    ds_check_ll(dist[4], 7, "A 到 E 的最短距离为 7");
    ds_check_ll(dist[5], 5, "A 到 F 的最短距离为 5");

    printf("  A 到 E 的路径：");
    print_path(G, path, 4);
    printf("\n");
    {
        int v = 4;
        ds_check_ll(path[v], 2, "E 的前驱是 C");
        ds_check_ll(path[2], 0, "C 的前驱是 A");
    }

    floyd(G, all);
    ds_check_ll(all[0][4], 7, "Floyd：A 到 E 为 7");
    ds_check_ll(all[1][5], 9, "Floyd：B 到 F 为 9");
    ds_check_ll(all[0][3], 5, "Floyd：A 到 D 为 5");
    ds_check_true(all[2][0] == all[0][2], "Floyd 结果在无向图上对称");
}

/* ==================== 6.6.3 拓扑排序 ==================== */

static bool topological_sort(const MGraph *G, int *order, int *count)
{
    int indegree[MAXV] = {0};
    int queue[MAXV];
    int front = 0, rear = 0;
    int i, j;
    int n = 0;

    for (i = 0; i < G->vexnum; i++) {
        for (j = 0; j < G->vexnum; j++) {
            if (G->arcs[j][i] != 0 && G->arcs[j][i] != INF) {
                indegree[i]++;
            }
        }
    }
    for (i = 0; i < G->vexnum; i++) {
        if (indegree[i] == 0) {
            queue[rear++] = i;
        }
    }
    while (front < rear) {
        int v = queue[front++];
        order[n++] = v;
        for (j = 0; j < G->vexnum; j++) {
            if (G->arcs[v][j] != 0 && G->arcs[v][j] != INF) {
                if (--indegree[j] == 0) {
                    queue[rear++] = j;
                }
            }
        }
    }
    *count = n;
    return n == G->vexnum;
}

static void demo_topological_sort(void)
{
    MGraph G;
    int order[MAXV];
    int count = 0;
    int i, j;
    bool ok;

    ds_section("6.6.3", "拓扑排序");
    ds_note("AOV 网中顶点表示活动、弧表示先后关系；不断输出入度为 0 的顶点。");
    ds_note("若输出顶点数少于 n，说明网中存在有向回路。");
    mg_init(&G, "ABCDEF");
    mg_add_directed(&G, 0, 1, 1);
    mg_add_directed(&G, 0, 2, 1);
    mg_add_directed(&G, 1, 3, 1);
    mg_add_directed(&G, 2, 3, 1);
    mg_add_directed(&G, 3, 4, 1);
    mg_add_directed(&G, 3, 5, 1);
    ok = topological_sort(&G, order, &count);
    printf("  拓扑序列：");
    for (i = 0; i < count; i++) {
        printf("%c ", G.vexs[order[i]]);
    }
    printf("\n");
    ds_check_true(ok, "该 AOV 网可以完成拓扑排序");
    ds_check_ll(count, 6, "拓扑序列包含全部 6 个顶点");
    for (i = 0; i < G.vexnum; i++) {
        for (j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j] != 0 && G.arcs[i][j] != INF) {
                int pi = -1, pj = -1, k;
                for (k = 0; k < count; k++) {
                    if (order[k] == i) {
                        pi = k;
                    }
                    if (order[k] == j) {
                        pj = k;
                    }
                }
                ds_check_true(pi < pj, "拓扑序满足每条弧的先后关系");
            }
        }
    }

    {
        MGraph C;
        int corder[MAXV];
        int ccount = 0;
        mg_init(&C, "AB");
        mg_add_directed(&C, 0, 1, 1);
        mg_add_directed(&C, 1, 0, 1);
        ds_check_true(!topological_sort(&C, corder, &ccount), "有向回路无法完成拓扑排序");
        ds_check_ll(ccount, 0, "有回路时输出 0 个顶点");
    }
}

/* ==================== 6.6.4 关键路径 ==================== */

static void demo_critical_path(void)
{
    MGraph G;
    int order[MAXV];
    int count = 0;
    int ve[MAXV] = {0};
    int vl[MAXV];
    int i, j;
    int max_time = 0;
    int critical = 0;

    ds_section("6.6.4", "关键路径");
    ds_note("AOE 网中弧表示活动、权表示持续时间；从源点到汇点的最长路径为");
    ds_note("关键路径，其上的活动为关键活动。e(i)=ve(u)，l(i)=vl(v)-w(u,v)，");
    ds_note("e(i)=l(i) 的活动即关键活动。");
    mg_init(&G, "ABCDEF");
    mg_add_directed(&G, 0, 1, 3);
    mg_add_directed(&G, 0, 2, 2);
    mg_add_directed(&G, 1, 3, 2);
    mg_add_directed(&G, 2, 3, 4);
    mg_add_directed(&G, 1, 4, 3);
    mg_add_directed(&G, 3, 5, 3);
    mg_add_directed(&G, 4, 5, 2);

    topological_sort(&G, order, &count);
    ds_check_ll(count, 6, "AOE 网拓扑排序成功");
    for (i = 0; i < count; i++) {
        int u = order[i];
        for (j = 0; j < G.vexnum; j++) {
            if (G.arcs[u][j] != 0 && G.arcs[u][j] != INF) {
                if (ve[u] + G.arcs[u][j] > ve[j]) {
                    ve[j] = ve[u] + G.arcs[u][j];
                }
            }
        }
    }
    for (i = 0; i < G.vexnum; i++) {
        if (ve[i] > max_time) {
            max_time = ve[i];
        }
    }
    for (i = 0; i < G.vexnum; i++) {
        vl[i] = max_time;
    }
    for (i = count - 1; i >= 0; i--) {
        int u = order[i];
        for (j = 0; j < G.vexnum; j++) {
            if (G.arcs[u][j] != 0 && G.arcs[u][j] != INF) {
                if (vl[j] - G.arcs[u][j] < vl[u]) {
                    vl[u] = vl[j] - G.arcs[u][j];
                }
            }
        }
    }
    printf("  事件最早发生时间 ve：");
    for (i = 0; i < G.vexnum; i++) {
        printf("%c=%d ", G.vexs[i], ve[i]);
    }
    printf("\n");
    printf("  事件最迟发生时间 vl：");
    for (i = 0; i < G.vexnum; i++) {
        printf("%c=%d ", G.vexs[i], vl[i]);
    }
    printf("\n");
    printf("  关键活动：");
    for (i = 0; i < G.vexnum; i++) {
        for (j = 0; j < G.vexnum; j++) {
            if (G.arcs[i][j] != 0 && G.arcs[i][j] != INF &&
                ve[i] == vl[j] - G.arcs[i][j]) {
                printf("%c->%c(%d) ", G.vexs[i], G.vexs[j], G.arcs[i][j]);
                critical++;
            }
        }
    }
    printf("\n");
    ds_check_ll(max_time, 9, "关键路径长度为 9");
    ds_check_ll(critical, 3, "共有 3 项关键活动");
    ds_check_ll(ve[3], 6, "事件 D 的最早发生时间为 6");
    ds_check_ll(vl[1], 4, "事件 B 的最迟发生时间为 4");
}

/* ==================== 6.7 校园导航 ==================== */

static void demo_campus_navigation(const MGraph *G)
{
    int dist[MAXV];
    int path[MAXV];

    ds_section("6.7", "案例分析与实现：校园导航");
    ds_note("把校园地点抽象为顶点、道路抽象为带权边，用 Dijkstra 求两点间");
    ds_note("最短路。这里仍用 A～F 表示 6 个地点，A 为入口、E 为目标。");
    dijkstra(G, 0, dist, path);
    printf("  推荐路线：");
    print_path(G, path, 4);
    printf("，全程 %d\n", dist[4]);
    ds_check_ll(dist[4], 7, "校园导航给出长度 7 的路线");
    ds_check_ll(path[4], 2, "路线在 E 之前经过 C");
}

int ds_ch06(void)
{
    MGraph G;
    ALGraph AL;
    const int edges[][3] = {
        {0, 1, 6}, {0, 2, 1}, {0, 3, 5}, {1, 2, 5}, {1, 4, 3},
        {2, 3, 5}, {2, 4, 6}, {2, 5, 4}, {3, 5, 2}, {4, 5, 6},
    };

    demo_graph_types();

    ds_section("6.4", "图的存储结构");
    ds_section("6.4.1", "邻接矩阵");
    ds_note("用二维数组 arcs[i][j] 存边，稠密图省事，空间 O(n^2)。");
    mg_init(&G, "ABCDEF");
    {
        int i;
        for (i = 0; i < 10; i++) {
            mg_add_undirected(&G, edges[i][0], edges[i][1], edges[i][2]);
        }
    }
    mg_print(&G);
    ds_check_ll(G.vexnum, 6, "图有 6 个顶点");
    ds_check_ll(G.arcnum, 10, "图有 10 条边");
    ds_check_ll(G.arcs[0][2], 1, "A-C 边权为 1");
    ds_check_ll(G.arcs[1][0], 6, "无向图邻接矩阵对称");

    ds_section("6.4.2", "邻接表");
    ds_note("每个顶点挂一条边链表，空间 O(n+e)，适合稀疏图。");
    al_build_undirected(&AL, "ABCDEF", edges, 10);
    al_print(&AL);
    ds_check_ll(AL.vertices[0].firstarc->adjvex, 1, "A 的第一个邻接点是 B");

    demo_cross_list();
    demo_adjacency_multilist();
    demo_traverse(&G, &AL);
    al_free(&AL);
    demo_mst(&G);
    demo_shortest_path(&G);
    demo_topological_sort();
    demo_critical_path();
    demo_campus_navigation(&G);

    ds_section("6.8", "小结");
    ds_note("图的存储有邻接矩阵、邻接表、十字链表、邻接多重表，按稠密/稀疏、");
    ds_note("有向/无向选择。DFS 与 BFS 是图运算的基础；Prim/Kruskal 求最小");
    ds_note("生成树，Dijkstra/Floyd 求最短路径，拓扑排序判回路，AOE 网求工期。");
    return 0;
}
