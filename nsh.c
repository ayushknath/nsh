#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define ALLOC_FAIL "allocation failure"
#define TOKEN_DELIM " \t\n"

#define BUF_SIZE 1024
char *read_line() {
  int c;
  int position = 0;
  int buf_size = BUF_SIZE;
  char *buffer = malloc(buf_size * sizeof(char));

  if (buffer == NULL) {
    fprintf(stderr, ALLOC_FAIL);
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    }

    buffer[position] = c;
    position++;

    if (position >= buf_size) {
      buf_size += BUF_SIZE;
      buffer = realloc(buffer, buf_size * sizeof(char));

      if (buffer == NULL) {
        fprintf(stderr, ALLOC_FAIL);
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define TOKENS_SIZE 64
char **parse_line(char *line) {
  int position = 0;
  int tokens_size = TOKENS_SIZE;
  char *token;
  char **tokens = malloc(tokens_size * sizeof(char *));

  if (tokens == NULL) {
    fprintf(stderr, ALLOC_FAIL);
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOKEN_DELIM);

  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= tokens_size) {
      tokens_size += TOKENS_SIZE;
      tokens = realloc(tokens, tokens_size * sizeof(char *));
      if (tokens == NULL) {
        fprintf(stderr, ALLOC_FAIL);
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, TOKEN_DELIM);
  }

  tokens[position] = NULL;
  return tokens;
}

int nsh_help(char **args);
int nsh_exit(char **args);

char *builtin_cmd[] = {"help", "exit"};
int (*builtin_cmd_fn[])(char **) = {&nsh_help, &nsh_exit};

int nsh_builtins() { return sizeof(builtin_cmd) / sizeof(char *); }

// Builtin command functions
int nsh_help(char **args) {
  printf("NSH shell - New shell\n");
  printf("Available builtins: \n");
  for (int i = 0; i < nsh_builtins(); i++) {
    printf("%s\n", builtin_cmd[i]);
  }

  return 1;
}

int nsh_exit(char **args) { return 0; }

int nsh_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) {
    // child process
    int exec_status = execvp(args[0], args);
    if (exec_status == -1) {
      perror("nsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("nsh");
  } else {
    // parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  // debug start
  // printf("wpid: %d\n", wpid);
  // debug end

  return 1;
}

int nsh_execute(char **args) {
  if (args[0] == NULL) {
    return 1;
  }

  // check for builtin commands
  for (int i = 0; i < nsh_builtins(); i++) {
    if (strcmp(args[0], builtin_cmd[i]) == 0) {
      // execute it
      return (*builtin_cmd_fn[i])(args);
    }
  }

  return nsh_launch(args);
}

void nsh_loop() {
  char *line;
  char **tokens;
  int status;

  do {
    printf(">> ");
    line = read_line();
    tokens = parse_line(line);
    status = nsh_execute(tokens);
  } while (status);
}

int main() {
  nsh_loop();

  return 0;
}
