#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_CURL_LEN 8192

#define TARGET_OFFSET_MS 0

#define PHASE1_CYCLES 20
#define PHASE1_DELAY_MS 4
#define PHASE1_FIRST_COURSE_REPEATS 1
#define PHASE1_SUBSEQUENT_COURSE_REPEATS 3

#define INTER_PHASE_PAUSE_SEC 2

#define PHASE2_CYCLES 5
#define PHASE2_DELAY_MS 1001
#define PHASE2_FIRST_COURSE_REPEATS 1
#define PHASE2_SUBSEQUENT_COURSE_REPEATS 1

typedef struct {
    char curl_cmd[MAX_CURL_LEN];
    int id;
} CourseRequest;

void precise_sleep_ms(long ms) {
    if (ms <= 0) return;
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_multiline_curl(char* dest, size_t max_len, int id) {
    printf("Paste cURL command for course %d (press Enter on a blank line when done):\n", id);
    dest[0] = '\0';
    char line[2048];

    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
            if (strlen(dest) > 0) {
                break;
            }
            continue;
        }
        strncat(dest, line, max_len - strlen(dest) - 1);
    }
}

void wait_until_target_time(int hour, int min, int sec, long offset_ms) {
    struct timespec now_ts;
    clock_gettime(CLOCK_REALTIME, &now_ts);

    struct tm target_tm;
    localtime_r(&now_ts.tv_sec, &target_tm);

    target_tm.tm_hour = hour;
    target_tm.tm_min  = min;
    target_tm.tm_sec  = sec;

    time_t target_sec = mktime(&target_tm);
    if (target_sec <= now_ts.tv_sec) {
        target_sec += 86400;
    }

    struct timespec target_ts;
    target_ts.tv_sec = target_sec;
    target_ts.tv_nsec = offset_ms * 1000000L;

    printf("\n[INFO] Synchronizing clock... Waiting for target time %02d:%02d:%02d (+%ld ms offset)\n", 
           hour, min, sec, offset_ms);

    while (1) {
        clock_gettime(CLOCK_REALTIME, &now_ts);
        
        long sec_diff = target_ts.tv_sec - now_ts.tv_sec;
        long nsec_diff = target_ts.tv_nsec - now_ts.tv_nsec;

        double remaining_ms = (sec_diff * 1000.0) + (nsec_diff / 1000000.0);

        if (remaining_ms <= 0) {
            break;
        }

        if (remaining_ms > 1000) {
            precise_sleep_ms(500);
        } else if (remaining_ms > 10) {
            precise_sleep_ms(2);
        } else {
            usleep(100);
        }
    }
}

int main() {
    int num_courses = 0;

    printf("=== Professional Course Registration Script (Sequential Network Engine) ===\n\n");
    
    printf("Enter the number of courses to register: ");
    if (scanf("%d", &num_courses) != 1 || num_courses <= 0) {
        fprintf(stderr, "[ERR] Invalid number of courses.\n");
        return 1;
    }
    clear_input_buffer();

    CourseRequest* courses = (CourseRequest*)malloc(num_courses * sizeof(CourseRequest));
    if (!courses) {
        fprintf(stderr, "[ERR] Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < num_courses; i++) {
        courses[i].id = i + 1;
        read_multiline_curl(courses[i].curl_cmd, sizeof(courses[i].curl_cmd), i + 1);
    }

    int target_hour, target_min, target_sec;
    printf("\nEnter target time (HH MM SS):\n> ");
    if (scanf("%d %d %d", &target_hour, &target_min, &target_sec) != 3) {
        fprintf(stderr, "[ERR] Invalid time format.\n");
        free(courses);
        return 1;
    }

    wait_until_target_time(target_hour, target_min, target_sec, TARGET_OFFSET_MS);

    printf("\n[LAUNCH] Target time reached! Triggering execution engine...\n");

    printf("\n--- Starting Phase 1 (%d Cycles, Delay: %d ms) ---\n", PHASE1_CYCLES, PHASE1_DELAY_MS);
    for (int cycle = 1; cycle <= PHASE1_CYCLES; cycle++) {
        printf("\n[Phase 1] --- Cycle %d / %d ---\n", cycle, PHASE1_CYCLES);
        for (int i = 0; i < num_courses; i++) {
            int repeats = (i == 0) ? PHASE1_FIRST_COURSE_REPEATS : PHASE1_SUBSEQUENT_COURSE_REPEATS;
            
            for (int r = 1; r <= repeats; r++) {
                if (repeats > 1) {
                    printf("[+] [Phase 1 | Cycle %d] Sending request for Course %d (Burst %d/%d)...\n", 
                           cycle, courses[i].id, r, repeats);
                } else {
                    printf("[+] [Phase 1 | Cycle %d] Sending request for Course %d...\n", cycle, courses[i].id);
                }
                
                int res = system(courses[i].curl_cmd);
                (void)res;
                
                precise_sleep_ms(PHASE1_DELAY_MS);
            }
        }
    }

    printf("\n[PAUSE] Phase 1 complete. Pausing for %d seconds before Phase 2...\n", INTER_PHASE_PAUSE_SEC);
    sleep(INTER_PHASE_PAUSE_SEC);

    printf("\n--- Starting Phase 2 (%d Cycles, Delay: %d ms) ---\n", PHASE2_CYCLES, PHASE2_DELAY_MS);
    for (int cycle = 1; cycle <= PHASE2_CYCLES; cycle++) {
        printf("\n[Phase 2] --- Cycle %d / %d ---\n", cycle, PHASE2_CYCLES);
        for (int i = 0; i < num_courses; i++) {
            int repeats = (i == 0) ? PHASE2_FIRST_COURSE_REPEATS : PHASE2_SUBSEQUENT_COURSE_REPEATS;
            
            for (int r = 1; r <= repeats; r++) {
                if (repeats > 1) {
                    printf("[+] [Phase 2 | Cycle %d] Sending request for Course %d (Burst %d/%d)...\n", 
                           cycle, courses[i].id, r, repeats);
                } else {
                    printf("[+] [Phase 2 | Cycle %d] Sending request for Course %d...\n", cycle, courses[i].id);
                }

                int res = system(courses[i].curl_cmd);
                (void)res;
                
                precise_sleep_ms(PHASE2_DELAY_MS);
            }
        }
    }

    printf("\n[DONE] All registration cycles completed successfully.\n");

    free(courses);
    return 0;
}
