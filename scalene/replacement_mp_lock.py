import multiprocessing
import sys
import threading

from scalene.scalene_profiler import Scalene
from typing import Any

def replacement_mp_lock(scalene: Scalene):
    orig_lock = multiprocessing.Lock

    class ReplacementLock(object):
        def __init__(self) -> None:
            self.__lock = orig_lock()
        def acquire(self, blocking: bool = True, timeout: float = -1) -> bool:
            tident = threading.get_ident()
            if blocking == 0:
                blocking = False
            start_time = scalene.get_wallclock_time()
            if blocking:
                if timeout < 0:
                    interval = sys.getswitchinterval()
                else:
                    interval = min(timeout, sys.getswitchinterval())
            else:
                interval = -1
            while True:
                scalene.set_thread_sleeping(tident)
                acquired_lock = self.__lock.acquire(blocking, interval)
                scalene.reset_thread_sleeping(tident)
                if acquired_lock:
                    return True
                if not blocking:
                    return False
                # If a timeout was specified, check to see if it's expired.
                if timeout != -1:
                    end_time = scalene.get_wallclock_time()
                    if end_time - start_time >= timeout:
                        return False

        def release(self) -> None:
            self.__lock.release()

        def __enter__(self) -> None:
            self.acquire()

        def __exit__(self, type: str, value: str, traceback: Any) -> None:
            self.release()
    multiprocessing.Lock = ReplacementLock
