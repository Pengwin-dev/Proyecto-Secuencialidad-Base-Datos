#include "conflicts.h"

int main(int argc, const char *argv[]) {
  // conjunto de transacciones
	char transaction_string[TRANSACTION_STRING_LENGTH];
  
	unsigned char oper_history[MAX_TRANSACTIONS][MAX_ATTRIBUTES];
  struct graph transaction_graph;
  struct ui_list running_transactions;
  char operation, attribute;
  unsigned int time, transaction, tx_idx, attr_idx, ts_ptr, i, j;
  unsigned int scheduling_counter = 1;

  for(i = 0; i < MAX_TRANSACTIONS; ++i) {
    for(j = 0; j < MAX_ATTRIBUTES; ++j) {
      oper_history[i][j] = 0;
    }
  }

  init_graph(&transaction_graph);
  init_list(&running_transactions);

  while(fscanf(stdin, "%u %u %c %c", &time, &transaction, &operation, &attribute) != EOF) {
    if(operation != 'C' && operation != 'W' && operation != 'R') {
      fprintf(stderr, "Invalid operation found: '%c'\n", operation);
    }

    tx_idx = find(&running_transactions, transaction);
    attr_idx = (unsigned char) attribute;

    if(tx_idx == -1) {
      tx_idx = insert(&running_transactions, transaction);
      ts_ptr = snprintf(transaction_string, sizeof transaction_string, "%u", running_transactions.elements[0]);
      for(i = 1; i < running_transactions.size; ++i) {
        ts_ptr += snprintf(transaction_string + ts_ptr, sizeof transaction_string, ",%u", running_transactions.elements[i]);
      }

      transaction_string[ts_ptr] = '\0';
      add_vertex(&transaction_graph, transaction);
    }

    if(operation == 'C') {
      delete(&running_transactions, transaction);

      for(i = 0; i < MAX_ATTRIBUTES; ++i) {
        oper_history[tx_idx][i] = 0;
      }

      if(empty(&running_transactions)) {
        fprintf(stdout, "%u %s %s\n", scheduling_counter, transaction_string, (check_seriability(&transaction_graph) == 0) ? ("NAO") : ("SIM"));
        transaction_string[0] = '\0';
        ++scheduling_counter;
        clear_graph(&transaction_graph);
      }
    } else {
      for(i = 0; i < running_transactions.size; ++i) {
        if(i != tx_idx) {
          if(oper_history[i][attr_idx] & OPER_WRITE || (oper_history[i][attr_idx] & OPER_READ && operation == 'W')) {
            add_edge(&transaction_graph, running_transactions.elements[i], transaction);
          }
        }
      }

      oper_history[tx_idx][attr_idx] |= (operation == 'W') ? OPER_WRITE : OPER_READ;
    }
  }

  clear_graph(&transaction_graph);
  return 0;
}
