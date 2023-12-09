# C-signals
C implementation of a small simulation that handles userspace (not real time) signals. The idea is to manage two processes in a mono-core cpu environment silumation, each process should not be disturbe in its calculation, and no one should be left aside (avoid famine).
