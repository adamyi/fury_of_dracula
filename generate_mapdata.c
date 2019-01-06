/**
 * COMP2521 Fury of Drucates
 *
 * Generate map.c and map.h based on data from places.c
 *
 * @author: Adam Yi <i@adamyi.com>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "places.h"

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

struct adj_connection {
  location_t v;
  transport_t type;
};

static inline void print_mapdata_c(FILE *fp, int mcount,
                               int mat[][NUM_MAP_LOCATIONS], int *count,
                               struct adj_connection adjc[][mcount]) {
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
  fprintf(fp, "\n};\n// clang-format on\n");
}

int main() {
  int count[NUM_MAP_LOCATIONS], mat[NUM_MAP_LOCATIONS][NUM_MAP_LOCATIONS];
  int tmpc[NUM_MAP_LOCATIONS];
  int mcount = 0, mcounti = 0;
  memset(count, 0, sizeof(count));
  memset(mat, 0, sizeof(mat));
  for (size_t i = 0; !is_sentinel_edge(CONNECTIONS[i]); i++) {
    count[CONNECTIONS[i].v]++;
    count[CONNECTIONS[i].w]++;
    mat[CONNECTIONS[i].v][CONNECTIONS[i].w] |=
        get_connection_type_mask(CONNECTIONS[i].t);
    mat[CONNECTIONS[i].w][CONNECTIONS[i].v] |=
        get_connection_type_mask(CONNECTIONS[i].t);
  }
  for (int i = 0; i < NUM_MAP_LOCATIONS; i++) {
    if (count[i] > mcount) {
      mcount = count[i];
      mcounti = i;
    }
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
  FILE *fp = fopen("mapdata.h", "w");
  print_header_comments(fp);
  print_mapdata_h(fp, mcount);
  fclose(fp);
  fp = fopen("mapdata.c", "w");
  print_header_comments(fp);
  print_mapdata_c(fp, mcount, mat, count, adjc);
  fclose(fp);
  return 0;
}
