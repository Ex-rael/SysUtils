#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define RED(string) "\x1b[31m" string "\x1b[0m"

#define CPU_GOVERNOR_PATH      "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor"
#define CPU_AVAILABLE_GOV_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors"

#define debug

#define CPU_GOVERNORS_PATH_LENGTH 65
#define CPU_GOVERNOR_LENGTH 32
#define CPU_GOVERNOR_PATH_LENGTH 54

typedef enum {
    PERFORMANCE = 0,
    POWERSAVE = 1,
    UNKNOWN = 2,
} Governor;

// retora um aray de strings
// const char** get_available_governors(char *ar_buffer, size_t n_buffer_size) {
//     char ar_governor_path[CPU_GOVERNORS_PATH_LENGTH];
//     snprintf(ar_governor_path, CPU_GOVERNORS_PATH_LENGTH, CPU_AVAILABLE_GOV_PATH);
//     char * strResult = NULL;

//     // file descriptor eh mais performatico que fopen
//     int file_descriptor = open(ar_governor_path, O_RDONLY);

//     // se o arquivo existe
//     if (file_descriptor > 0){
//         ssize_t n_count = 0;
//         ssize_t n_read_bytes = read(file_descriptor, ar_buffer, n_buffer_size - 1);
//         close(file_descriptor);

//         if (n_read_bytes > 0) {
//             ar_buffer[n_read_bytes] = '\0';
//             char *p_newline = ar_buffer;
//             while (*p_newline) {
//                 if ((*p_newline == '\n') && (n_read_bytes == n_count)) {
//                     *p_newline = '\0';
//                     break;
//                 }
//                 p_newline++;
//                 n_count++;
//             }
//         } else {
//             n_result= -1;
//         }
//     } else {
//         n_result = -1;
//     }
// }

int read_governor(int n_cpus, char *ar_buffer, size_t n_buffer_size) {

    int n_result;
    char ar_governor_path[CPU_GOVERNOR_PATH_LENGTH];

    for (int n_cpu = 0; n_cpu < n_cpus; n_cpu++) {
        snprintf(ar_governor_path, CPU_GOVERNOR_PATH_LENGTH, CPU_GOVERNOR_PATH, n_cpu);

        // file descriptor eh mais performatico que fopen
        int file_descriptor = open(ar_governor_path, O_RDONLY);

        // se o arquivo existe
        if (file_descriptor > 0){
            ssize_t n_read_bytes = read(file_descriptor, ar_buffer, n_buffer_size - 1);
            close(file_descriptor);

            if (n_read_bytes > 0) {
                ar_buffer[n_read_bytes] = '\0';
                char *p_newline = ar_buffer;
                while (*p_newline) {
                    if (*p_newline == '\n') {
                        *p_newline = '\0';
                        break;
                    }
                    p_newline++;
                }
                n_result = 0;
            } else {
                n_result= -1;
            }
        } else {
            n_result = -1;
        }

        printf("CPU%d: %s\n", n_cpu, ar_buffer);
    }
    return n_result;
}

const char* governor_to_string(Governor governor) {
    switch (governor) {
        case PERFORMANCE:
            return "performance";
        case POWERSAVE:
            return "powersave";
        default:
            return "";
    }
}

int set_governor(int n_cpus, Governor governor) {
    const char* str_governor = governor_to_string(governor);
    const int n_governor_size = strlen(str_governor);

    int n_result = -1;
    char ar_governor_path[CPU_GOVERNOR_PATH_LENGTH];

    if (n_governor_size > 0) {
        for (int n_cpu = 0; n_cpu < n_cpus; n_cpu++) {
            printf("set_governor: new governor: %s; \n", str_governor);

            // escreve o path no buffer
            snprintf(ar_governor_path, CPU_GOVERNOR_PATH_LENGTH, CPU_GOVERNOR_PATH, n_cpu);

            // file descriptor eh mais performatico que fopen
            int file_descriptor = open(ar_governor_path, O_WRONLY);

            if (file_descriptor > 0) {
                ssize_t n_written_bytes = write(file_descriptor, str_governor, n_governor_size);
                close(file_descriptor);

                if (n_written_bytes == n_governor_size) {
                    n_result = 0;
                } else {
                    n_result = -1;
                }
            } else {
                printf(RED("set_governor: %s \n"), strerror(errno));
                n_result = -1;
            }
        }
    }
    return n_result;
}

int main(int argc, char *argv[]){
    int n_num_cpus = sysconf(_SC_NPROCESSORS_ONLN); // obtem o numero de CPUs logicas disponiveis (syscall)
    int n_opt = 0;
    char ar_buffer[CPU_GOVERNOR_LENGTH];            // Buffer para leitura e escrita em disc
    Governor governor = UNKNOWN;

    if (argc == 1){
        // chamado sem argumentos -> mostra o governor atual
        read_governor(n_num_cpus, ar_buffer, CPU_GOVERNOR_LENGTH);
    } else{
        while ((n_opt = getopt(argc, argv, "hs:")) != -1){
            switch (n_opt){
                case 'h':
                    printf("CPUs logicas disponiveis: %d \n", n_num_cpus);
                    //printf("Governors disponiveis: %s\n", get_available_governors(ar_buffer, CPU_GOVERNOR_LENGTH));
                    break;
                case 's':
                    governor = atoi(optarg);

                    printf("governor: %s \n", governor_to_string(governor));

                    set_governor(n_num_cpus, governor);
                    //read_governor(n_num_cpus, ar_buffer, CPU_GOVERNOR_LENGTH);
            }
        }
    }
    //TStringList lstGovernors = (TStringList) malloc(sizeof(StringList));
}
