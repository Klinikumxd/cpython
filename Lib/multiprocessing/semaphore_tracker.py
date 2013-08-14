#
# On Unix we run a server process which keeps track of unlinked
# semaphores. The server ignores SIGINT and SIGTERM and reads from a
# pipe.  Every other process of the program has a copy of the writable
# end of the pipe, so we get EOF when all other processes have exited.
# Then the server process unlinks any remaining semaphore names.
#
# This is important because the system only supports a limited number
# of named semaphores, and they will not be automatically removed till
# the next reboot.  Without this semaphore tracker process, "killall
# python" would probably leave unlinked semaphores.
#

import errno
import os
import signal
import sys
import threading
import warnings
import _multiprocessing

from . import spawn
from . import util
from . import current_process

__all__ = ['ensure_running', 'register', 'unregister']


_lock = threading.Lock()


def ensure_running():
    '''Make sure that semaphore tracker process is running.

    This can be run from any process.  Usually a child process will use
    the semaphore created by its parent.'''
    with _lock:
        config = current_process()._config
        if config.get('semaphore_tracker_fd') is not None:
            return
        fds_to_pass = []
        try:
            fds_to_pass.append(sys.stderr.fileno())
        except Exception:
            pass
        cmd = 'from multiprocessing.semaphore_tracker import main; main(%d)'
        r, semaphore_tracker_fd = util.pipe()
        try:
            fds_to_pass.append(r)
            # process will out live us, so no need to wait on pid
            exe = spawn.get_executable()
            args = [exe] + util._args_from_interpreter_flags()
            args += ['-c', cmd % r]
            util.spawnv_passfds(exe, args, fds_to_pass)
        except:
            os.close(semaphore_tracker_fd)
            raise
        else:
            config['semaphore_tracker_fd'] = semaphore_tracker_fd
        finally:
            os.close(r)


def register(name):
    '''Register name of semaphore with semaphore tracker.'''
    _send('REGISTER', name)


def unregister(name):
    '''Unregister name of semaphore with semaphore tracker.'''
    _send('UNREGISTER', name)


def _send(cmd, name):
    msg = '{0}:{1}\n'.format(cmd, name).encode('ascii')
    if len(name) > 512:
        # posix guarantees that writes to a pipe of less than PIPE_BUF
        # bytes are atomic, and that PIPE_BUF >= 512
        raise ValueError('name too long')
    fd = current_process()._config['semaphore_tracker_fd']
    nbytes = os.write(fd, msg)
    assert nbytes == len(msg)


def main(fd):
    '''Run semaphore tracker.'''
    # protect the process from ^C and "killall python" etc
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    signal.signal(signal.SIGTERM, signal.SIG_IGN)

    for f in (sys.stdin, sys.stdout):
        try:
            f.close()
        except Exception:
            pass

    cache = set()
    try:
        # keep track of registered/unregistered semaphores
        with open(fd, 'rb') as f:
            for line in f:
                try:
                    cmd, name = line.strip().split(b':')
                    if cmd == b'REGISTER':
                        cache.add(name)
                    elif cmd == b'UNREGISTER':
                        cache.remove(name)
                    else:
                        raise RuntimeError('unrecognized command %r' % cmd)
                except Exception:
                    try:
                        sys.excepthook(*sys.exc_info())
                    except:
                        pass
    finally:
        # all processes have terminated; cleanup any remaining semaphores
        if cache:
            try:
                warnings.warn('semaphore_tracker: There appear to be %d '
                              'leaked semaphores to clean up at shutdown' %
                              len(cache))
            except Exception:
                pass
        for name in cache:
            # For some reason the process which created and registered this
            # semaphore has failed to unregister it. Presumably it has died.
            # We therefore unlink it.
            try:
                name = name.decode('ascii')
                try:
                    _multiprocessing.sem_unlink(name)
                except Exception as e:
                    warnings.warn('semaphore_tracker: %r: %s' % (name, e))
            finally:
                pass
