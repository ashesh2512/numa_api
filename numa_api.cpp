#include <stdio.h>
#include <unistd.h>
#include <vector>

#include <numa.h>

void isnuma_available()
{
  if (numa_available() == -1) {
    fprintf(stderr, "numa library functions are undefined");
  }
}

extern "C"
{
  // Given PID, get the CPUs associated with it
  void get_cpu_affinity_to_pid(pid_t pid, int *cpu_list, int *cpu_count)
  {
    std::vector<int> cpus;
    // Allocate a CPU bitmask
    struct bitmask *mask = numa_allocate_cpumask();
    if (!mask)
    {
      fprintf(stderr, "numa_allocate_cpumask failed");
      *cpu_count = 0;
      return;
    }

    // Get the CPU affinity mask for the given PID
    int result = numa_sched_getaffinity(pid, mask);
    if (result < 0)
    {
      fprintf(stderr, "numa_sched_getaffinity failed");
      numa_free_cpumask(mask);
      *cpu_count = 0;
      return;
    }

    // Gather the CPU associated with PID
    printf("CPU affinity for PID %d: ", pid);
    int num_possible_cpus = numa_num_possible_cpus();
    for (int i = 0; i < num_possible_cpus; i++)
    {
      if (numa_bitmask_isbitset(mask, i))
      {
        cpus.push_back(i);
        printf("%d ", i);
      }
    }
    printf("\n");

    // Copy the results back to the output array
    *cpu_count = cpus.size();
    for (int i = 0; i < cpus.size(); i++)
    {
      cpu_list[i] = cpus[i];
    }

    // Free the allocated mask
    numa_free_cpumask(mask);
  }
}

// Get GPU affinity based on GPU for Frontier nodes
int get_gpu_affinity_to_cpu_Frontier(int cpu) {
  if ((cpu >= 0 && cpu <= 7) || (cpu >= 64 && cpu <= 71)) {
    return 4;
  }
  else if ((cpu >= 8 && cpu <= 15) || (cpu >= 72 && cpu <= 79)) {
    return 5;
  }
  else if ((cpu >= 16 && cpu <= 23) || (cpu >= 80 && cpu <= 87)) {
    return 2;
  }
  else if ((cpu >= 24 && cpu <= 31) || (cpu >= 88 && cpu <= 95)) {
    return 3;
  }
  else if ((cpu >= 32 && cpu <= 39) || (cpu >= 96 && cpu <= 103)) {
    return 6;
  }
  else if ((cpu >= 40 && cpu <= 47) || (cpu >= 104 && cpu <= 111)) {
    return 7;
  }
  else if ((cpu >= 48 && cpu <= 55) || (cpu >= 112 && cpu <= 119)) {
    return 0;
  }
  else if ((cpu >= 56 && cpu <= 63) || (cpu >= 120 && cpu <= 127)) {
    return 1;
  }
  else {
    fprintf(stderr, "found cpu %d that shouldn't exist in Frontier architecture\n", cpu);
  }
}

int main(int argc, char *argv[]) {
  //ensure numa library is reachable
  isnuma_available();

  int* cpu_list = new int[128];
  int* cpu_count = new int[1];

  pid_t pid = getpid();
  get_cpu_affinity_to_pid(pid, cpu_list, cpu_count);

  return EXIT_SUCCESS;
}
