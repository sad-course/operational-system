#include <csignal>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

const char *RED_BG    = "\x1b[41m";
const char *GREEN_BG  = "\x1b[42m";
const char *YELLOW_BG = "\x1b[43m";
const char *RESET     = "\x1b[0m";

// state variables: 0=RED,1=GREEN,2=YELLOW
volatile sig_atomic_t parent_color = 0;
volatile sig_atomic_t child_color = 0;
volatile sig_atomic_t remaining_time = 0;

pid_t pid = -1;

void handlerPrintColor(int);
void handlerAlarmSignaler(int);
void handlerSigint(int);
void handlerSigtstp(int);
void handlerChildReset(int);

void handlerPrintColor(int) {
    const char *out = NULL;
    switch (child_color) {
        case 0: out = "RED "; break;
        case 1: out = "GREEN "; break;
        default: out = "YELLOW "; break;
    }


    char buf[140];
    size_t len = 0;
    if (child_color == 0) {
        len = snprintf(buf, sizeof(buf), "%s%s   %s\n", out, RED_BG, RESET);
    } else if (child_color == 1) {
        len = snprintf(buf, sizeof(buf), "%s%s   %s\n", out, GREEN_BG, RESET);
    } else {
        len = snprintf(buf, sizeof(buf), "%s%s   %s\n", out, YELLOW_BG, RESET);
    }
    if (len > 0) write(STDOUT_FILENO, buf, (size_t)len);

    child_color = (child_color + 1) % 3;
}

void handlerChildReset(int) {
    child_color = 0;
}

void handlerAlarmSignaler(int) {
    if (pid > 0) kill(pid, SIGUSR1);
    if (parent_color == 0) {
        remaining_time = alarm(3);
        parent_color = 1;
        return;
    }
    if (parent_color == 1) {
        remaining_time = alarm(4);
        parent_color = 2;
        return;
    }

    remaining_time = alarm(3);
    parent_color = 0;
}

void handlerSigint(int) {
    parent_color = 0;
    if (pid > 0) {
        kill(pid, SIGUSR2);
        kill(pid, SIGUSR1);
    }
    alarm(1);
}

void handlerSigtstp(int) {
    if (pid > 0) {
        kill(pid, SIGKILL);
    }
    _exit(0);
}

int main() {
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        signal(SIGTSTP, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR1, handlerPrintColor);
        signal(SIGUSR2, handlerChildReset);

        while (1) pause();
    } else {
        printf("PID PAI %d -> filho %d\n", (int)getpid(), (int)pid);
        signal(SIGALRM, handlerAlarmSignaler);
        signal(SIGINT, handlerSigint);
        signal(SIGTSTP, handlerSigtstp);

        parent_color = 0;
        child_color = 0;
        alarm(1);
        while (1) pause();
    }
    return 0;
}