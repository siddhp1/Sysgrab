#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "number.h"
#include "string.h"

#define MAX_LEN 256

char *get_architecture(void) {
  char *temp = NULL;
  char *architecture = NULL;

  temp = get_string_from_command("uname -m", NULL);
  if (temp == NULL) {
    perror("Failed to get architecture");
    return NULL;
  }

  architecture = clean_string(temp, NULL, "\n");
  if (architecture == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);
  return architecture;
}

char *get_computer_helper(const char *file_path, const char *prefix,
                          const char *suffix, const char *data_point) {
  char *temp = NULL;
  char *computer_data_point = NULL;

  temp = get_string_from_file(file_path, NULL);
  if (temp == NULL) {
    fprintf(stderr, "Failed to get %s\n", data_point);
    return NULL;
  }

  computer_data_point = clean_string(temp, prefix, suffix);
  if (computer_data_point == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);
  return computer_data_point;
}

char *get_computer(void) {
  char *temp = NULL;
  char *name = NULL;
  char *version = NULL;
  char *computer = NULL;

  name = get_computer_helper("/sys/devices/virtual/dmi/id/product_name", "",
                             "\n", "computer name");
  if (name == NULL) {
    perror("Failed to get computer name");
    return NULL;
  }

  computer = strdup(name);
  if (computer == NULL) {
    perror("Failed to create string");
    return NULL;
  }

  version = get_computer_helper("/sys/devices/virtual/dmi/id/product_version",
                                "", "\n", "computer version");
  if (version) {
    int length = strlen(name) + strlen(version);
    if (length <= 0) {
      perror("Length is zero");
      return NULL;
    }

    temp = realloc(computer, (length + 2) * sizeof(char));
    if (temp == NULL) {
      perror("Failed to allocate memory");
      return NULL;
    }
    computer = temp;

    strcat(computer, name);
    strcat(computer, "");
    strcat(computer, version);
  } else {
    perror("Failed to get computer verison");
    free(name);
    return computer;
  }

  free(name);
  free(version);
  return computer;
}

char *get_cpu_helper(const char *look_up, const char *prefix,
                     const char *suffix, const char *data_point) {
  char *temp = NULL;
  char *cpu_data_point = NULL;

  temp = get_string_from_command("lscpu", look_up);
  if (temp == NULL) {
    fprintf(stderr, "Failed to get %s\n", data_point);
    return NULL;
  }

  cpu_data_point = clean_string(temp, prefix, suffix);
  if (cpu_data_point == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);
  return cpu_data_point;
}

char *get_cpu(void) {
  char *temp = NULL;
  char *cpu = NULL;
  char *cpu_name = NULL;
  char *cpu_threads = NULL;
  char *cpu_frequency = NULL;

  cpu_name = get_cpu_helper("Model name:", "Model name:", "\n", "cpu name");
  if (cpu_name == NULL) {
    perror("Failed to get cpu name");
    return NULL;
  }

  cpu = strdup(cpu_name);
  if (cpu == NULL) {
    perror("Failed to create string");
    return NULL;
  }

  cpu_threads = get_cpu_helper("CPU(s):", "CPU(s): ", "\n", "cpu threads");
  if (cpu_threads) {
    free(cpu);
    cpu = NULL;

    int length = strlen(cpu_name) + strlen(cpu_threads);
    if (length <= 0) {
      perror("Length is zero");
      return NULL;
    }

    temp = realloc(cpu, (length + 4) * sizeof(char));
    if (temp == NULL) {
      perror("Failed to allocate memory");
      return NULL;
    }
    cpu = temp;

    snprintf(cpu, length + 4, "%s (%s)", cpu_name, cpu_threads);
  } else {
    perror("Failed to get cpu threads");
    free(cpu_name);
    return cpu;
  }

  cpu_frequency =
      get_cpu_helper("CPU max MHz", "CPU max MHz:", "\n", "cpu frequency");
  if (cpu_frequency) {
    free(cpu);
    cpu = NULL;

    double cpu_frequency_mhz = get_double_from_string(cpu_frequency);
    if (cpu_frequency_mhz == NAN) {
      perror("Failed to get double");
      return NULL;
    }

    double cpu_frequency_ghz = cpu_frequency_mhz / 1000;

    cpu_frequency = get_string_from_double(cpu_frequency_ghz, ".2f");
    if (cpu_frequency == NULL) {
      perror("Failed to get string");
      return NULL;
    }

    int length = strlen(cpu_name) + strlen(cpu_threads) + strlen(cpu_frequency);
    if (length <= 0) {
      perror("Length is zero");
      return NULL;
    }

    temp = realloc(cpu, (length + 10) * sizeof(char));
    if (temp == NULL) {
      perror("Failed to allocate memory");
      return NULL;
    }
    cpu = temp;

    snprintf(cpu, length + 10, "%s (%s) @ %sGHz", cpu_name, cpu_threads,
             cpu_frequency);
  } else {
    perror("Failed to get cpu frequency");
    free(cpu_name);
    free(cpu_threads);
    return cpu;
  }

  free(cpu_name);
  free(cpu_threads);
  free(cpu_frequency);
  return cpu;
}

char *get_hostname(void) {
  char temp[MAX_LEN];

  char *hostname = NULL;

  if (gethostname(temp, MAX_LEN) == 0) {
    hostname = strdup(temp);
    if (hostname == NULL) {
      perror("Failed to create string");
      return NULL;
    }
  } else {
    perror("Failed to get hostname");
  }

  return hostname;
}

char *get_kernel(void) {
  char *temp = NULL;
  char *kernel = NULL;

  temp = get_string_from_command("uname -r", NULL);
  if (temp == NULL) {
    perror("Failed to get kernel");
    return NULL;
  }

  kernel = clean_string(temp, NULL, "\n");
  if (kernel == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);
  return kernel;
}

long get_memory_helper(const char *look_up, const char *prefix,
                       const char *suffix, const char *data_point) {
  char *temp_raw_string = NULL;
  char *temp_clean_string = NULL;

  temp_raw_string = get_string_from_file("/proc/meminfo", look_up);
  if (temp_raw_string == NULL) {
    fprintf(stderr, "Failed to get %s\n", data_point);
    return -1;
  }

  temp_clean_string = clean_string(temp_raw_string, prefix, suffix);
  if (temp_clean_string == NULL) {
    perror("Failed to clean string");
    return -1;
  }

  long memory = get_long_from_string(temp_clean_string);
  if (memory == -1) {
    perror("Failed to get long");
    return -1;
  }

  free(temp_raw_string);
  free(temp_clean_string);

  return memory;
}

char *get_memory(void) {
  char *current_memory_usage = NULL;
  char *total_memory = NULL;
  char *memory = NULL;

  long total_memory_kb =
      get_memory_helper("MemTotal", "MemTotal:", " kB\n", "total memory");
  if (total_memory_kb == -1) {
    perror("Failed to get total memory");
    return NULL;
  }

  long shared_memory_kb =
      get_memory_helper("Shmem:", "Shmem:", " kB\n", "shared memory");
  if (shared_memory_kb == -1) {
    perror("Failed to get shared memory");
    return NULL;
  }

  long free_memory_kb =
      get_memory_helper("MemFree", "MemFree:", " kB\n", "free memory");
  if (free_memory_kb == -1) {
    perror("Failed to get free memory");
    return NULL;
  }

  long buffer_memory_kb =
      get_memory_helper("Buffers", "Buffers:", " kB\n", "buffer memory");
  if (buffer_memory_kb == -1) {
    perror("Failed to get buffer memory");
    return NULL;
  }

  long sreclaimable_memory_kb = get_memory_helper(
      "SReclaimable", "SReclaimable:", " kB\n", "sreclaimable memory");
  if (sreclaimable_memory_kb == -1) {
    perror("Failed to get sreclaimable memory");
    return NULL;
  }

  long cached_memory_kb =
      get_memory_helper("Cached", "Cached:", " kB\n", "cached memory");
  if (cached_memory_kb == -1) {
    perror("Failed to get cached memory");
    return NULL;
  }

  double current_memory_usage_mb =
      (total_memory_kb + shared_memory_kb - free_memory_kb - buffer_memory_kb -
       cached_memory_kb - sreclaimable_memory_kb) /
      1024.0;
  double total_memory_mb = total_memory_kb / 1024.0;

  current_memory_usage = get_string_from_double(current_memory_usage_mb, ".0f");
  if (current_memory_usage == NULL) {
    perror("Failed to get string");
    return NULL;
  }

  total_memory = get_string_from_double(total_memory_mb, ".0f");
  if (total_memory == NULL) {
    perror("Failed to get string");
    return NULL;
  }

  int length = strlen(current_memory_usage) + strlen(total_memory);
  if (length <= 0) {
    perror("Length is zero");
    return NULL;
  }

  memory = malloc(length + 10);
  if (memory == NULL) {
    perror("Failed to allocate memory");
    return NULL;
  }

  snprintf(memory, length + 10, "%sMiB / %sMiB", current_memory_usage,
           total_memory);

  free(current_memory_usage);
  free(total_memory);

  return memory;
}

char *get_os(void) {
  char *temp = NULL;
  char *os = NULL;

  temp = get_string_from_file("/etc/os-release", "PRETTY_NAME");
  if (temp == NULL) {
    perror("Failed to get os");
    return NULL;
  }

  os = clean_string(temp, "PRETTY_NAME=\"", "\"\n");
  if (os == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);
  return os;
}

char *get_shell(void) {
  char *path = NULL;
  char *last = NULL;
  char *shell = NULL;

  path = getenv("SHELL");
  if (path == NULL) {
    perror("Failed to get shell");
    return NULL;
  }

  // Get the token following the last /
  last = strrchr(path, '/');
  if (last != NULL) {
    last++;
  }

  shell = strdup(last);
  if (shell == NULL) {
    perror("Failed to create string");
    return NULL;
  }

  return shell;
}

char *get_uptime(void) {
  char *temp = NULL;
  char *uptime = NULL;

  temp = get_string_from_file("/proc/uptime", NULL);
  if (temp == NULL) {
    perror("Failed to get uptime");
    return NULL;
  }

  uptime = clean_string(temp, "", "\n");
  if (uptime == NULL) {
    perror("Failed to clean string");
    return NULL;
  }

  free(temp);

  long uptime_seconds = get_long_from_string(uptime);
  if (uptime_seconds == -1) {
    perror("Failed to get long");
    return NULL;
  }

  free(uptime);
  uptime = NULL;

  long hours = uptime_seconds / 3600;
  int minutes = (uptime_seconds - (3600 * hours) / 60);
  int seconds = uptime_seconds - (3600 * hours) - (60 * minutes);

  int length = snprintf(NULL, 0, "%ld:%.2d:%.2d", hours, minutes, seconds);
  if (length <= 0) {
    perror("Length is zero");
    return NULL;
  }

  uptime = malloc((length + 1) * sizeof(char));
  if (uptime == NULL) {
    perror("Failled to allocate memory");
    return NULL;
  }

  snprintf(uptime, length + 1, "%ld:%.2d:%.2d", hours, minutes, seconds);

  return uptime;
}

char *get_username(void) {
  char *username = NULL;

  struct passwd *pw = getpwuid(geteuid());

  if (pw) {
    username = strdup(pw->pw_name);
    if (username == NULL) {
      perror("Failed to create string");
      return NULL;
    }
  } else {
    perror("Failed to get username");
  }

  return username;
}

// TESTING
int main(void) {
  // For testing, remove later
  char *temp = get_memory();

  puts(temp);

  free(temp);

  return 0;
}
