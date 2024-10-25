#include <set>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include <numa.h>

extern "C"
{
  void isnuma_available()
  {
    if (numa_available() == -1)
    {
      fprintf(stderr, "numa library functions are undefined\n");
    }
  }

  // Given PID, get the CPUs associated with it
  void get_cpu_affinity_to_pid(pid_t pid, int* cpu_list, int* cpu_count)
  {
    std::vector<int> cpus;
    // Allocate a CPU bitmask
    struct bitmask* cpu_mask = numa_allocate_cpumask();
    numa_bitmask_clearall(cpu_mask);
    if (!cpu_mask)
    {
      fprintf(stderr, "numa_allocate_cpumask failed\n");
      *cpu_count = 0;
      return;
    }

    // Get the CPU affinity mask for the given PID
    int result = numa_sched_getaffinity(pid, cpu_mask);
    if (result < 0)
    {
      fprintf(stderr, "numa_sched_getaffinity failed\n");
      numa_free_cpumask(cpu_mask);
      *cpu_count = 0;
      return;
    }

    // Gather the CPU associated with PID
    printf("CPU affinity for PID %d: ", pid);
    int num_possible_cpus = numa_num_possible_cpus();
    for (int i = 0; i < num_possible_cpus; i++)
    {
      if (numa_bitmask_isbitset(cpu_mask, i))
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
    numa_free_cpumask(cpu_mask);
  }

  // Get GPU affinity based on CPU for Frontier nodes
  int get_gpu_affinity_to_cpu_Frontier(int cpu)
  {
    if ((cpu >= 0 && cpu <= 7) || (cpu >= 64 && cpu <= 71))
    {
      return 4;
    }
    else if ((cpu >= 8 && cpu <= 15) || (cpu >= 72 && cpu <= 79))
    {
      return 5;
    }
    else if ((cpu >= 16 && cpu <= 23) || (cpu >= 80 && cpu <= 87))
    {
      return 2;
    }
    else if ((cpu >= 24 && cpu <= 31) || (cpu >= 88 && cpu <= 95))
    {
      return 3;
    }
    else if ((cpu >= 32 && cpu <= 39) || (cpu >= 96 && cpu <= 103))
    {
      return 6;
    }
    else if ((cpu >= 40 && cpu <= 47) || (cpu >= 104 && cpu <= 111))
    {
      return 7;
    }
    else if ((cpu >= 48 && cpu <= 55) || (cpu >= 112 && cpu <= 119))
    {
      return 0;
    }
    else if ((cpu >= 56 && cpu <= 63) || (cpu >= 120 && cpu <= 127))
    {
      return 1;
    }
    else
    {
      fprintf(stderr, "found cpu %d that shouldn't exist in Frontier architecture\n", cpu);
    }
  }

  // Given PID, get the GPUs associated with it
  void get_gpu_affinity_to_pid_Frontier(pid_t pid, int* gpu_list, int* gpu_count) {
    
    int* cpu_list = new int[128];
    int* cpu_count = new int[1];
    get_cpu_affinity_to_pid(pid, cpu_list, cpu_count);

    std::set<int> unique_gpus; // will automatically store only unique values

    for (int i = 0; i < *cpu_count; ++i) {
      int cpu = cpu_list[i];
      int gpu = get_gpu_affinity_to_cpu_Frontier(cpu);
      unique_gpus.insert(gpu);
    }

    // Check for uniqueness
    if (unique_gpus.size() > 1) {
      printf("Warning in PID %d: Not all CPUs are associated with the same GPU!\n", pid);
    } 

    // Populate the gpu_list with unique GPUs
    printf("GPU affinity for PID %d: ", pid);
    *gpu_count = unique_gpus.size();
    int index = 0;
    for (const auto& gpu : unique_gpus) {
      gpu_list[index++] = gpu;
      printf("%d ", gpu); // Print the associated GPU
    }
    printf("\n");

    // Clean up dynamically allocated memory
    delete[] cpu_list;
    delete[] cpu_count;
  }
}

int main(int argc, char* argv[])
{
  // ensure numa library is reachable
  isnuma_available();

  int* gpu_list = new int[8];
  int* gpu_count = new int[1];

  pid_t pid = getpid();
  get_gpu_affinity_to_pid_Frontier(pid, gpu_list, gpu_count);

  delete[] gpu_list;
  delete[] gpu_count;

  return EXIT_SUCCESS;
}
