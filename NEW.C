#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completed;
} Process;

void inputProcesses(Process p[], int *n) {
    printf("Enter number of processes: ");
    scanf("%d", n);
    for (int i = 0; i < *n; i++) {
        p[i].pid = i + 1;
        printf("Enter Arrival Time, Burst Time and Priority for P%d: ", p[i].pid);
        scanf("%d %d %d", &p[i].arrival_time, &p[i].burst_time, &p[i].priority);
        p[i].remaining_time = p[i].burst_time;
        p[i].completed = 0;
    }
}

void resetProcesses(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].waiting_time = 0;
        p[i].turnaround_time = 0;
        p[i].remaining_time = p[i].burst_time;
        p[i].completed = 0;
    }
}

void fcfs(Process p[], int n, float *avg_wt, float *avg_tat) {
    resetProcesses(p, n);
    Process temp;
    for (int i = 0; i < n-1; i++)
        for (int j = i+1; j < n; j++)
            if (p[i].arrival_time > p[j].arrival_time) {
                temp = p[i]; p[i] = p[j]; p[j] = temp;
            }

    int time = 0;
    *avg_wt = *avg_tat = 0;
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrival_time)
            time = p[i].arrival_time;
        p[i].waiting_time = time - p[i].arrival_time;
        time += p[i].burst_time;
        p[i].turnaround_time = p[i].waiting_time + p[i].burst_time;
        *avg_wt += p[i].waiting_time;
        *avg_tat += p[i].turnaround_time;
    }
    *avg_wt /= n;
    *avg_tat /= n;
}

void sjf(Process p[], int n, float *avg_wt, float *avg_tat) {
    resetProcesses(p, n);
    int completed = 0, time = 0;
    *avg_wt = *avg_tat = 0;

    while (completed < n) {
        int idx = -1, min_bt = 9999;
        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival_time <= time && p[i].burst_time < min_bt) {
                min_bt = p[i].burst_time;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        p[idx].waiting_time = time - p[idx].arrival_time;
        time += p[idx].burst_time;
        p[idx].turnaround_time = p[idx].waiting_time + p[idx].burst_time;
        p[idx].completed = 1;
        completed++;
        *avg_wt += p[idx].waiting_time;
        *avg_tat += p[idx].turnaround_time;
    }
    *avg_wt /= n;
    *avg_tat /= n;
}

void roundRobin(Process p[], int n, int tq, float *avg_wt, float *avg_tat) {
    resetProcesses(p, n);
    int time = 0, completed = 0;
    *avg_wt = *avg_tat = 0;
    int queue[MAX], front = 0, rear = 0;
    int visited[MAX] = {0};

    queue[rear++] = 0;
    visited[0] = 1;

    while (completed < n) {
        int idx = queue[front++];
        if (p[idx].remaining_time > tq) {
            time += tq;
            p[idx].remaining_time -= tq;
        } else {
            time += p[idx].remaining_time;
            p[idx].waiting_time = time - p[idx].arrival_time - p[idx].burst_time;
            p[idx].turnaround_time = p[idx].waiting_time + p[idx].burst_time;
            p[idx].remaining_time = 0;
            p[idx].completed = 1;
            completed++;
            *avg_wt += p[idx].waiting_time;
            *avg_tat += p[idx].turnaround_time;
        }

        for (int i = 0; i < n; i++) {
            if (!visited[i] && p[i].arrival_time <= time && p[i].remaining_time > 0) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }
        if (p[idx].remaining_time > 0)
            queue[rear++] = idx;

        if (front == rear && completed < n) {
            for (int i = 0; i < n; i++) {
                if (!p[i].completed) {
                    time = p[i].arrival_time;
                    queue[rear++] = i;
                    visited[i] = 1;
                    break;
                }
            }
        }
    }

    *avg_wt /= n;
    *avg_tat /= n;
}

void priority(Process p[], int n, float *avg_wt, float *avg_tat) {
    resetProcesses(p, n);
    int time = 0, completed = 0;
    *avg_wt = *avg_tat = 0;

    while (completed < n) {
        int idx = -1, highest = 9999;
        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival_time <= time && p[i].priority < highest) {
                highest = p[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].waiting_time = time - p[idx].arrival_time;
        time += p[idx].burst_time;
        p[idx].turnaround_time = p[idx].waiting_time + p[idx].burst_time;
        p[idx].completed = 1;
        completed++;
        *avg_wt += p[idx].waiting_time;
        *avg_tat += p[idx].turnaround_time;
    }

    *avg_wt /= n;
    *avg_tat /= n;
}

int main() {
    Process p[MAX];
    int n;
    float avg_wt_fcfs, avg_tat_fcfs;
    float avg_wt_sjf, avg_tat_sjf;
    float avg_wt_rr, avg_tat_rr;
    float avg_wt_pri, avg_tat_pri;

    inputProcesses(p, &n);

    fcfs(p, n, &avg_wt_fcfs, &avg_tat_fcfs);
    sjf(p, n, &avg_wt_sjf, &avg_tat_sjf);
    roundRobin(p, n, 2, &avg_wt_rr, &avg_tat_rr);
    priority(p, n, &avg_wt_pri, &avg_tat_pri);

    printf("\n--- Average Waiting and Turnaround Times ---\n");
    printf("Algorithm\tWaiting Time\tTurnaround Time\n");
    printf("FCFS     \t%.2f\t\t%.2f\n", avg_wt_fcfs, avg_tat_fcfs);
    printf("SJF      \t%.2f\t\t%.2f\n", avg_wt_sjf, avg_tat_sjf);
    printf("RoundRobin\t%.2f\t\t%.2f\n", avg_wt_rr, avg_tat_rr);
    printf("Priority \t%.2f\t\t%.2f\n", avg_wt_pri, avg_tat_pri);

    return 0;
}
