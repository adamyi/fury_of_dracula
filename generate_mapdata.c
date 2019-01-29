/**
 * COMP2521 Fury of Drucates
 *
 * Generate mapdata.c and mapdata.h based on data from places.c
 *
 * This would generate an adjacent list, an adjacent matrix,
 * shortest distance matrix, and shortest turns to move between
 * places in different turns (different for rail) by using Floyd
 * Algorithm.
 *
 * By moving these to compile time, we improve runtime performance.
 *
 * @author: Adam Yi <i@adamyi.com>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ac_log.h"

#include "places.h"

struct adj_connection {
  location_t v;
  transport_t type;
};

/// Is this the magic 'sentinel' edge?
static inline bool is_sentinel_edge(connection x) {
  return x.v == -1 && x.w == -1 && x.t == ANY;
}

static inline char get_connection_type_mask(transport_t t) {
  assert(t >= MIN_TRANSPORT && t <= MAX_TRANSPORT);
  switch (t) {
    case ROAD:
      return 1;  // 001
    case RAIL:
      return 2;  // 010
    case BOAT:
      return 4;  // 100
    default:
      __builtin_unreachable();
  }

  __builtin_unreachable();
}

static bool is_connected_by_rail(location_t from, location_t to,
                                 int max_rail_dist, int mcount, int *count,
                                 struct adj_connection adjc[][mcount]) {
  // printf("from %s max %d\n", location_get_abbrev(from), max_rail_dist);
  struct adj_connection *conns = adjc[from];
  if (max_rail_dist == 0) return false;
  for (int i = 0; i < count[from]; i++) {
    if (conns[i].type != RAIL) continue;
    if (conns[i].v == to) return true;
    if (is_connected_by_rail(conns[i].v, to, max_rail_dist - 1, mcount, count,
                             adjc))
      return true;
  }
  return false;
}

static inline void print_header_comments(FILE *fp) {
  fprintf(fp,
          "// this file is auto-generated\n"
          "// Do not modify\n"
          "//\n"
          "// COMP2521 Fury of Dracula\n"
          "// auto-generated map\n\n");
}

static inline void print_mapdata_h(FILE *fp, int mcount) {
  fprintf(fp,
          "#include <stdbool.h>\n"
          "#include <stdlib.h>\n"
          "#include \"places.h\"\n"
          "\n"
          "#ifndef FOD__MAPDATA_H_\n"
          "#define FOD__MAPDATA_H_\n"
          "\n"
          "#define MAX_CONNECTION_PER_PLACE %d"
          "\n"
          "extern char ADJMATRIX[][NUM_MAP_LOCATIONS];\n"
          "extern int SPDIST[][NUM_MAP_LOCATIONS];\n"
          "extern int SPROUND[][NUM_MAP_LOCATIONS][4];\n"
          "\n"
          "extern size_t ADJLIST_COUNT[];\n"
          "\n"
          "extern struct adj_connection {\n"
          "  location_t v;\n"
          "  transport_t type;\n"
          "} ADJLIST[][MAX_CONNECTION_PER_PLACE];\n"
          "\n"
          "#endif  // !defined (FOD__MAPDATA_H_)\n",
          mcount);
}

static inline void print_mapdata_c(FILE *fp, int mcount,
                                   int mat[][NUM_MAP_LOCATIONS], int *count,
                                   struct adj_connection adjc[][mcount],
                                   int dist[][NUM_MAP_LOCATIONS],
                                   int tdist[][NUM_MAP_LOCATIONS][4]) {
  fprintf(fp,
          "#include \"map.h\"\n"
          "#include \"places.h\"\n"
          "\n"
          "// clang-format off\n"
          "\n"
          "char ADJMATRIX[][NUM_MAP_LOCATIONS] = {");
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (i == 0)
      fprintf(fp, "\n");
    else
      fprintf(fp, ",\n");
    fprintf(fp, "  {");
    for (int j = 0; j < NUM_MAP_LOCATIONS; j++) {
      if (j == 0)
        fprintf(fp, "\n    ");
      else if (j & 15)
        fprintf(fp, ", ");
      else
        fprintf(fp, ",\n    ");
      fprintf(fp, "%d", mat[i][j]);
    }
    fprintf(fp, "\n  }");
  }
  fprintf(fp, "\n};\n\nsize_t ADJLIST_COUNT[] = {");
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (i == 0)
      fprintf(fp, "\n    ");
    else if (i & 15)
      fprintf(fp, ", ");
    else
      fprintf(fp, ",\n    ");
    fprintf(fp, "%d", count[i]);
  }
  fprintf(
      fp,
      "\n};\n\nstruct adj_connection ADJLIST[][MAX_CONNECTION_PER_PLACE] = {");
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (i == 0)
      fprintf(fp, "\n");
    else
      fprintf(fp, ",\n");
    fprintf(fp, "  {");
    for (int j = 0; j < count[i]; j++) {
      if (j == 0)
        fprintf(fp, "\n    ");
      else if (j & 7)
        fprintf(fp, ", ");
      else
        fprintf(fp, ",\n    ");
      fprintf(fp, "{%d, %d}", adjc[i][j].v, adjc[i][j].type);
    }
    fprintf(fp, "\n  }");
  }
  fprintf(fp, "\n};\n\nint SPDIST[][NUM_MAP_LOCATIONS] = {");
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (i == 0)
      fprintf(fp, "\n");
    else
      fprintf(fp, ",\n");
    fprintf(fp, "  {");
    for (int j = 0; j < NUM_MAP_LOCATIONS; j++) {
      if (j == 0)
        fprintf(fp, "\n    ");
      else if (j & 15)
        fprintf(fp, ", ");
      else
        fprintf(fp, ",\n    ");
      fprintf(fp, "%d", dist[i][j]);
    }
    fprintf(fp, "\n  }");
  }
  fprintf(fp, "\n};\n\nint SPROUND[][NUM_MAP_LOCATIONS][4] = {");
  for (size_t i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (i == 0)
      fprintf(fp, "\n");
    else
      fprintf(fp, ",\n");
    fprintf(fp, "  {");
    for (int j = 0; j < NUM_MAP_LOCATIONS; j++) {
      if (j == 0)
        fprintf(fp, "\n    ");
      else if (j & 3)
        fprintf(fp, ", ");
      else
        fprintf(fp, ",\n    ");
      fprintf(fp, "{%d, %d, %d, %d}", tdist[i][j][0], tdist[i][j][1],
              tdist[i][j][2], tdist[i][j][3]);
    }
    fprintf(fp, "\n  }");
  }
  fprintf(fp, "\n};\n// clang-format on\n");
}

int main() {
  ac_setLoggingTag("generate_mapdata");
  ac_log(AC_LOG_INFO, "Reading places.c");
  int count[NUM_MAP_LOCATIONS], mat[NUM_MAP_LOCATIONS][NUM_MAP_LOCATIONS];
  int dist[NUM_MAP_LOCATIONS][NUM_MAP_LOCATIONS];
  int tdist[NUM_MAP_LOCATIONS][NUM_MAP_LOCATIONS][4];
  int tmpc[NUM_MAP_LOCATIONS];
  int mcount = 0;
  memset(count, 0, sizeof(count));
  memset(mat, 0, sizeof(mat));
  memset(dist, -1, sizeof(dist));
  memset(tdist, -1, sizeof(tdist));
  for (size_t i = 0; !is_sentinel_edge(CONNECTIONS[i]); i++) {
    count[CONNECTIONS[i].v]++;
    count[CONNECTIONS[i].w]++;
    mat[CONNECTIONS[i].v][CONNECTIONS[i].w] |=
        get_connection_type_mask(CONNECTIONS[i].t);
    mat[CONNECTIONS[i].w][CONNECTIONS[i].v] |=
        get_connection_type_mask(CONNECTIONS[i].t);
    dist[CONNECTIONS[i].w][CONNECTIONS[i].v] = 1;
    dist[CONNECTIONS[i].v][CONNECTIONS[i].w] = 1;
    if (CONNECTIONS[i].t != RAIL) {
      tdist[CONNECTIONS[i].v][CONNECTIONS[i].w][0] = 1;
      tdist[CONNECTIONS[i].w][CONNECTIONS[i].v][0] = 1;
    }
    tdist[CONNECTIONS[i].v][CONNECTIONS[i].w][1] = 1;
    tdist[CONNECTIONS[i].w][CONNECTIONS[i].v][1] = 1;
    tdist[CONNECTIONS[i].v][CONNECTIONS[i].w][2] = 1;
    tdist[CONNECTIONS[i].w][CONNECTIONS[i].v][2] = 1;
    tdist[CONNECTIONS[i].v][CONNECTIONS[i].w][3] = 1;
    tdist[CONNECTIONS[i].w][CONNECTIONS[i].v][3] = 1;
  }
  for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (count[i] > mcount) mcount = count[i];
    dist[i][i] = 0;
    tdist[i][i][0] = tdist[i][i][1] = tdist[i][i][2] = tdist[i][i][3] = 0;
  }
  struct adj_connection adjc[NUM_MAP_LOCATIONS][mcount];
  memset(tmpc, 0, sizeof(tmpc));
  memset(adjc, 0, sizeof(adjc));
  for (size_t i = 0; !is_sentinel_edge(CONNECTIONS[i]); i++) {
    adjc[CONNECTIONS[i].v][tmpc[CONNECTIONS[i].v]++] = (struct adj_connection){
        .v = CONNECTIONS[i].w, .type = CONNECTIONS[i].t};
    adjc[CONNECTIONS[i].w][tmpc[CONNECTIONS[i].w]++] = (struct adj_connection){
        .v = CONNECTIONS[i].v, .type = CONNECTIONS[i].t};
  }
  for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
    for (int j = 0; j < NUM_MAP_LOCATIONS; j++) {
      if (tdist[i][j][3] == -1) {
        tdist[i][j][1] =
            is_connected_by_rail(i, j, 1, mcount, count, adjc) ? 1 : -1;
        tdist[i][j][2] =
            is_connected_by_rail(i, j, 2, mcount, count, adjc) ? 1 : -1;
        tdist[i][j][3] =
            is_connected_by_rail(i, j, 3, mcount, count, adjc) ? 1 : -1;
      }
    }
  }
  // Floyd Algorithm for shortest path distance
  for (int k = 0; k < NUM_MAP_LOCATIONS; k++) {
    for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
      for (int j = 0; j < NUM_MAP_LOCATIONS; j++) {
        if (dist[i][k] > -1 && dist[k][j] > -1 &&
            (dist[i][j] == -1 || dist[i][j] > dist[i][k] + dist[k][j]))
          dist[i][j] = dist[i][k] + dist[k][j];
        for (int t = 0; t < 4; t++) {
          int newt = (t + tdist[i][k][t]) % 4;
          if (tdist[i][k][t] > -1 && tdist[k][j][newt] > -1 &&
              (tdist[i][j][t] == -1 ||
               tdist[i][j][t] > tdist[i][k][t] + tdist[k][j][newt]))
            tdist[i][j][t] = tdist[i][k][t] + tdist[k][j][newt];
        }
      }
    }
  }
  ac_log(AC_LOG_INFO, "Writing mapdata.h");
  FILE *fp = fopen("mapdata.h", "w");
  print_header_comments(fp);
  print_mapdata_h(fp, mcount);
  fclose(fp);
  ac_log(AC_LOG_INFO, "Writing mapdata.c");
  fp = fopen("mapdata.c", "w");
  print_header_comments(fp);
  print_mapdata_c(fp, mcount, mat, count, adjc, dist, tdist);
  fclose(fp);
  return 0;
}
