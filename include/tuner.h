

typedef struct u16_node {
  unsigned short value;
  struct u16_node *next;
} u16_node_t;

void u16_node_print(u16_node_t *head);

void u16_node_push(u16_node_t **head, unsigned short value);

static double current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, (struct timezone *)NULL);
  return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

int is_fe_locked(int fd);

int parse_pat(char *data, unsigned size);

int tune(int fd, unsigned int frequency);