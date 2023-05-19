# cpu_scheduling_simulator

A preemptive SJF scheduler with quantum prediction based on a fake os with a fake allocation of processes.

The commit message "aggiorno" are due to an alias used for general and not fundamental commits {e.g. the update of the README.md file, the change of the name of some variables in the entire code or the adding of simple functions (akin to "is_empty" one)}
[alias: commit -> git commit -m "aggiorno"]

In order to correctly use this program to run it you should write:
./sched_sim <decay_coefficient> <cores_number> <scheduling_policy> <quantum> <process_files>

The <scheduling_policy> should be RR or SJF (default SJF for other values)
The <quantum> is for the preemption
