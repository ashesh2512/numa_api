#include <stdio.h>
#include <unistd.h>
#include <vector>

#include <numa.h>

// Given PID, get the CPUs associated with it
void get_cpu_affinity_to_pid(pid_t pid, std::vector<int>& cpu_list) {
  // Allocate a CPU bitmask
  struct bitmask *mask = numa_allocate_cpumask();
  if (!mask) {
    fprintf(stderr, "numa_allocate_cpumask failed");
    return;
  }

  // Get the CPU affinity mask for the given PID
  int result = numa_sched_getaffinity(pid, mask);
  if (result < 0) {
    fprintf(stderr, "numa_sched_getaffinity failed");
    numa_free_cpumask(mask);
    return;
  }

  // Gather the CPU associated with PID, and print affinity
  printf("CPU affinity for PID %d: ", pid);
  int num_possible_cpus = numa_num_possible_cpus();
  for (int i = 0; i < num_possible_cpus; i++) {
    if (numa_bitmask_isbitset(mask, i)) {
      cpu_list.push_back(i);
      printf("%d ", i);
    }
  }
  printf("\n");

  // Free the allocated mask
  numa_free_cpumask(mask);
}

int main(int argc, char *argv[]) {

  std::vector<int> cpu_list;
  cpu_list.clear();

  pid_t pid = getpid();
  get_cpu_affinity_to_pid(pid, cpu_list);

  return EXIT_SUCCESS;
}
