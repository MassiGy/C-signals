# C-signals
C implementation of a small simulation that handles userspace (not real time) signals. The idea is to manage two processes in a hypothetical mono-core cpu environment, each process should not be disturbed in its calculations, and none of them should be left aside (avoid famine).
