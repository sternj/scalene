import sys
import time
import os
import multiprocessing
import threading

def get_wallclock_time() -> float:
    """Wall-clock time."""
    return time.perf_counter()



def join(self, timeout: float = -1):
    from multiprocessing.process import _children
    # print(multiprocessing.process.active_children())
    self._check_closed()
    assert self._parent_pid == os.getpid(), 'can only join a child process'
    assert self._popen is not None, 'can only join a started process'
    tident = threading.get_ident()
    if timeout < 0:
        interval = sys.getswitchinterval()
    else:
        interval = min(timeout, sys.getswitchinterval())
    start_time = get_wallclock_time()
    while True:
        # Scalene.set_thread_sleeping(tident)
        res = self._popen.wait(timeout)
        if res is not None:
            _children.discard(self)
            return
        print(multiprocessing.process.active_children())
        # Scalene.reset_thread_sleeping(tident)
        if timeout != -1:
            end_time = get_wallclock_time()
            if end_time - start_time >= timeout:
                _children.discard(self)
                return


multiprocessing.Process.join = join

with open('multiprocessing_test.py', 'rb') as f:
    code = compile(f.read(), 'multiprocessing_test.py', 'exec')
    import __main__
    __main__.__dict__['file'] = 'multiprocessing_test.py'
    exec(code, __main__.__dict__)