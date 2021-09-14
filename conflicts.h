/*
 * conflito - Database Seriability Conflict Test
 *
 * Copyright (C) 2016  Rafael Ravedutti Lucio Machado
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>

/* Transaction string length */
#define TRANSACTION_STRING_LENGTH   256

/* Max number of running transactions */
#define MAX_TRANSACTIONS            4096

/* Max number of attributes = 2^sizeof(char) */
#define MAX_ATTRIBUTES              256

/* Operations */
#define OPER_WRITE                  1
#define OPER_READ                   2

struct ui_list {
  unsigned int elements[MAX_TRANSACTIONS];
  unsigned int size;
};

struct graph {
  struct vertex *graph_vertices;
  unsigned int graph_nvertices;
};

struct vertex {
  unsigned int vertex_id;
  unsigned int vertex_number;
  struct edge *vertex_edges;
  struct vertex *vertex_next;
};

struct edge {
  struct vertex *edge_dest;
  struct edge *edge_next;
};

/* Seriability test functions */
int check_seriability(struct graph *graph);
int _check_seriability(struct vertex *vertex, char *visited);

/* Graph functions */
void init_graph(struct graph *graph);
void add_vertex(struct graph *graph, unsigned int vertex_id);
void add_edge(struct graph *graph, unsigned int source, unsigned int dest);
struct vertex *find_vertex_by_id(struct graph *graph, unsigned int id);
void clear_graph(struct graph *graph);

/* List functions */
void init_list(struct ui_list *list);
int insert(struct ui_list *list, unsigned int element);
void delete(struct ui_list *list, unsigned int element);
int find(struct ui_list *list, unsigned int element);
int empty(struct ui_list *list);

int check_seriability(struct graph *graph) {
  struct vertex *vertex;
  char *visited;
  unsigned int i;

  visited = (char *) malloc(sizeof(char) * graph->graph_nvertices);

  if(visited != NULL) {
    for(i = 0; i < graph->graph_nvertices; ++i) {
      visited[i] = 0;
    }

    for(vertex = graph->graph_vertices; vertex != NULL; vertex = vertex->vertex_next) {
      if(visited[vertex->vertex_number] == 0) {
        visited[vertex->vertex_number] = 1;
        if(_check_seriability(vertex, visited) == 0) {
          free(visited);
          return 0;
        }
      }
    }
  }

  free(visited);
  return 1;
}

int _check_seriability(struct vertex *vertex, char *visited) {
  struct edge *edge;

  for(edge = vertex->vertex_edges; edge != NULL; edge = edge->edge_next) {
    if(visited[edge->edge_dest->vertex_number] == 1) {
      return 0;
    }

    visited[edge->edge_dest->vertex_number] = 1;

    if(_check_seriability(edge->edge_dest, visited) == 0) {
      return 0;
    }
  }

  return 1;
}

void init_graph(struct graph *graph) {
  graph->graph_vertices = NULL;
  graph->graph_nvertices = 0;
}

void add_vertex(struct graph *graph, unsigned int vertex_id) {
  struct vertex *vertex;

  if(graph != NULL) {
    vertex = (struct vertex *) malloc(sizeof(struct vertex));

    if(vertex != NULL) {
      vertex->vertex_id = vertex_id;
      vertex->vertex_number = graph->graph_nvertices;
      vertex->vertex_edges = NULL;
      vertex->vertex_next = graph->graph_vertices;
      graph->graph_vertices = vertex;
      graph->graph_nvertices++;
    }
  }
}

void add_edge(struct graph *graph, unsigned int source, unsigned int dest) {
  struct vertex *vertex;
  struct edge *edge;

  if(graph != NULL) {
    for(vertex = graph->graph_vertices; vertex != NULL; vertex = vertex->vertex_next) {
      if(vertex->vertex_id == source) {
        edge = (struct edge *) malloc(sizeof(struct edge));

        if(edge != NULL) {
          edge->edge_dest = find_vertex_by_id(graph, dest);
          edge->edge_next = vertex->vertex_edges;
          vertex->vertex_edges = edge;
        }
      }
    }
  }
}

struct vertex *find_vertex_by_id(struct graph *graph, unsigned int id) {
  struct vertex *vertex;

  for(vertex = graph->graph_vertices; vertex != NULL; vertex = vertex->vertex_next) {
    if(vertex->vertex_id == id) {
      return vertex;
    }
  }

  return NULL;
}

void clear_graph(struct graph *graph) {
  struct vertex *vertex;
  struct edge *edge;

  if(graph != NULL) {
    while(graph->graph_vertices != NULL) {
      vertex = graph->graph_vertices;
      graph->graph_vertices = graph->graph_vertices->vertex_next;

      while(vertex->vertex_edges != NULL) {
        edge = vertex->vertex_edges;
        vertex->vertex_edges = vertex->vertex_edges->edge_next;
        free(edge);
      }

      free(vertex);
    }

    graph->graph_nvertices = 0;
  }
}

void init_list(struct ui_list *list) {
  list->size = 0;
}

int insert(struct ui_list *list, unsigned int element) {
  list->elements[list->size++] = element;
  return list->size - 1;
}

void delete(struct ui_list *list, unsigned int element) {
  unsigned int i, j;

  for(i = 0; i < list->size; ++i) {
    if(list->elements[i] == element) {
      for(j = i; j < list->size - 1; ++j) {
        list->elements[j] = list->elements[j + 1];
      }

      --list->size;
    }
  }
}

int find(struct ui_list *list, unsigned int element) {
  unsigned int i;

  for(i = 0; i < list->size; ++i) {
    if(list->elements[i] == element) {
      return i;
    }
  }

  return -1;
}

int empty(struct ui_list *list) {
  return (list->size == 0);
}


