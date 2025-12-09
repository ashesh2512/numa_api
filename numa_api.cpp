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

  // Given PID, get the cores associated with it
  void get_core_affinity_to_pid(pid_t pid, int* core_list, int* core_count)
  {
    std::vector<int> cores;
    // Allocate a CPU bitmask
    struct bitmask* cpu_mask = numa_allocate_cpumask();
    numa_bitmask_clearall(cpu_mask);
    if (!cpu_mask)
    {
      fprintf(stderr, "numa_allocate_cpumask failed\n");
      *core_count = 0;
      return;
    }

    // Get the CPU affinity mask for the given PID
    int result = numa_sched_getaffinity(pid, cpu_mask);
    if (result < 0)
    {
      fprintf(stderr, "numa_sched_getaffinity failed\n");
      numa_free_cpumask(cpu_mask);
      *core_count = 0;
      return;
    }

    // Gather the core associated with PID
    printf("core affinity for PID %d: ", pid);
    int num_possible_cores = numa_num_possible_cpus();
    for (int i = 0; i < num_possible_cores; i++)
    {
      if (numa_bitmask_isbitset(cpu_mask, i))
      {
        cores.push_back(i);
        printf("%d ", i);
      }
    }
    printf("\n");

    // Copy the results back to the output array
    *core_count = cores.size();
    for (int i = 0; i < cores.size(); i++)
    {
      core_list[i] = cores[i];
    }

    // Free the allocated mask
    numa_free_cpumask(cpu_mask);
  }

  // Get GPU affinity based on core for Frontier nodes
  int get_gpu_affinity_to_core_Frontier(int core)
  {
    if ((core >= 0 && core <= 7) || (core >= 64 && core <= 71))
    {
      return 4;
    }
    else if ((core >= 8 && core <= 15) || (core >= 72 && core <= 79))
    {
      return 5;
    }
    else if ((core >= 16 && core <= 23) || (core >= 80 && core <= 87))
    {
      return 2;
    }
    else if ((core >= 24 && core <= 31) || (core >= 88 && core <= 95))
    {
      return 3;
    }
    else if ((core >= 32 && core <= 39) || (core >= 96 && core <= 103))
    {
      return 6;
    }
    else if ((core >= 40 && core <= 47) || (core >= 104 && core <= 111))
    {
      return 7;
    }
    else if ((core >= 48 && core <= 55) || (core >= 112 && core <= 119))
    {
      return 0;
    }
    else if ((core >= 56 && core <= 63) || (core >= 120 && core <= 127))
    {
      return 1;
    }
    else
    {
      fprintf(stderr, "found core %d that shouldn't exist in Frontier architecture\n", core);
      return -1;
    }
  }

  // Given PID, get the GPUs associated with it
  void get_gpu_affinity_to_pid_Frontier(pid_t pid, int* gpu_list, int* gpu_count) {
    
    int* core_list = new int[128];
    int* core_count = new int[1];
    get_core_affinity_to_pid(pid, core_list, core_count);

    std::set<int> unique_gpus; // will automatically store only unique values

    for (int i = 0; i < *core_count; ++i) {
      int core = core_list[i];
      int gpu = get_gpu_affinity_to_core_Frontier(core);
      unique_gpus.insert(gpu);
    }

    // Sanity check for uniqueness
    if (unique_gpus.size() > 1) {
      printf("Warning in PID %d: Not all cores are associated with the same GPU!\n", pid);
    } 

    // Populate the gpu_list with unique GPUs
    printf("Suggested GPU for PID %d: ", pid);
    *gpu_count = unique_gpus.size();
    int index = 0;
    for (const auto& gpu : unique_gpus) {
      gpu_list[index++] = gpu;
      printf("%d ", gpu); // Print the associated GPU
    }
    printf("\n");

    // Clean up dynamically allocated memory
    delete[] core_list;
    delete[] core_count;
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
