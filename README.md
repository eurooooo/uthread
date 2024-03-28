## Project Description
The objective of this project is to design and implement a **user-level thread library** called uthread. This library will mimic the interface of the well-established pthread library but operate entirely within **user space**. The implementation will encompass three fundamental scheduling algorithms: **Round-Robin**, **Static Priority** and **Dynamic Priority** Scheduler with aging mechanism.

## API Specifications for ULT Library
- **uthread_create**: Spawns a new ULT, commencing execution at the specified start_routine function with the provided argument.

- **uthread_yield**: Voluntarily relinquishes the control of the current ULT, enabling another ULT to execute.

- **uthread_self**: Retrieves the thread identifier of the currently executing ULT.

- **uthread_join**: Blocks the calling ULT until the specified ULT, identified by tid, concludes execution. Compared to pthread_join, we do not expect a return value.

- **uthread_exit**: Terminates the current ULT. Compared to pthread_exit, we do not expect a return value from the start_routine.

- **uthread_init**: Initializes the uthread library.

- **uthread_suspend**: Temporarily suspends the execution of the specified ULT.

- **uthread_resume**: Resumes the execution of a previously suspended ULT.

- **uthread_priority**: Retrieves the priority of the thread.

## Makefile instructions to run the test cases
All the tests can be run with the default test commands: ex. 'make rr_t1'