/********************************************************************************
 *                               libemu
 *
 *                    - x86 shellcode emulation -
 *
 *
 * Copyright (C) 2007  Paul Baecher & Markus Koetter
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * 
 *             contact nepenthesdev@users.sourceforge.net  
 *
 *******************************************************************************/


#include "emu/environment/linux/emu_env_linux.h"
#include "emu/environment/linux/env_linux_syscall_hooks.h"

#ifdef __cplusplus
extern "C" {
#endif

struct emu_env_linux_syscall_entry env_linux_syscalls[] = 
{
/* 0*/   { NULL                         , NULL}, 
/* 1*/   { "exit"                       , NULL}, 
/* 2*/   { "fork"                       , NULL}, 
/* 3*/   { "read"                       , NULL}, 
/* 4*/   { "write"                      , NULL}, 
/* 5*/   { "open"                       , NULL}, 
/* 6*/   { "close"                      , NULL}, 
/* 7*/   { "waitpid"                    , NULL}, 
/* 8*/   { "creat"                      , NULL}, 
/* 9*/   { "link"                       , NULL}, 
/* 10*/  { "unlink"                     , NULL}, 
/* 11*/  { "execve"                     , NULL}, 
/* 12*/  { "chdir"                      , NULL}, 
/* 13*/  { "time"                       , NULL}, 
/* 14*/  { "mknod"                      , NULL}, 
/* 15*/  { "chmod"                      , NULL}, 
/* 16*/  { "lchown"                     , NULL}, 
/* 17*/  { NULL                         , NULL}, 
/* 18*/  { "stat"                       , NULL}, 
/* 19*/  { "lseek"                      , NULL}, 
/* 20*/  { "getpid"                     , NULL}, 
/* 21*/  { "mount"                      , NULL}, 
/* 22*/  { "oldumount"                  , NULL}, 
/* 23*/  { "setuid"                     , NULL}, 
/* 24*/  { "getuid"                     , NULL}, 
/* 25*/  { "stime"                      , NULL}, 
/* 26*/  { "ptrace"                     , NULL}, 
/* 27*/  { "alarm"                      , NULL}, 
/* 28*/  { "fstat"                      , NULL}, 
/* 29*/  { "pause"                      , NULL}, 
/* 30*/  { "utime"                      , NULL}, 
/* 31*/  { NULL                         , NULL}, 
/* 32*/  { NULL                         , NULL}, 
/* 33*/  { "access"                     , NULL}, 
/* 34*/  { "nice"                       , NULL}, 
/* 35*/  { NULL                         , NULL}, 
/* 36*/  { "sync"                       , NULL}, 
/* 37*/  { "kill"                       , NULL}, 
/* 38*/  { "rename"                     , NULL}, 
/* 39*/  { "mkdir"                      , NULL}, 
/* 40*/  { "rmdir"                      , NULL}, 
/* 41*/  { "dup"                        , NULL}, 
/* 42*/  { "pipe"                       , NULL}, 
/* 43*/  { "times"                      , NULL}, 
/* 44*/  { NULL                         , NULL}, 
/* 45*/  { "brk"                        , NULL}, 
/* 46*/  { "setgid"                     , NULL}, 
/* 47*/  { "getgid"                     , NULL}, 
/* 48*/  { "signal"                     , NULL}, 
/* 49*/  { "geteuid"                    , NULL}, 
/* 50*/  { "getegid"                    , NULL}, 
/* 51*/  { "acct"                       , NULL}, 
/* 52*/  { "umount"                     , NULL}, 
/* 53*/  { NULL                         , NULL}, 
/* 54*/  { "ioctl"                      , NULL}, 
/* 55*/  { "fcntl"                      , NULL}, 
/* 56*/  { NULL                         , NULL}, 
/* 57*/  { "setpgid"                    , NULL}, 
/* 58*/  { NULL                         , NULL}, 
/* 59*/  { "olduname"                   , NULL}, 
/* 60*/  { "umask"                      , NULL}, 
/* 61*/  { "chroot"                     , NULL}, 
/* 62*/  { "ustat"                      , NULL}, 
/* 63*/  { "dup2"                       , NULL}, 
/* 64*/  { "getppid"                    , NULL}, 
/* 65*/  { "getpgrp"                    , NULL}, 
/* 66*/  { "setsid"                     , NULL}, 
/* 67*/  { "sigaction"                  , NULL}, 
/* 68*/  { "sgetmask"                   , NULL}, 
/* 69*/  { "ssetmask"                   , NULL}, 
/* 70*/  { "setreuid"                   , NULL}, 
/* 71*/  { "setregid"                   , NULL}, 
/* 72*/  { "sigsuspend"                 , NULL}, 
/* 73*/  { "sigpending"                 , NULL}, 
/* 74*/  { "sethostname"                , NULL}, 
/* 75*/  { "setrlimit"                  , NULL}, 
/* 76*/  { "getrlimit"                  , NULL}, 
/* 77*/  { "getrusage"                  , NULL}, 
/* 78*/  { "gettimeofday"               , NULL}, 
/* 79*/  { "settimeofday"               , NULL}, 
/* 80*/  { "getgroups"                  , NULL}, 
/* 81*/  { "setgroups"                  , NULL}, 
/* 82*/  { "old_select"                 , NULL}, 
/* 83*/  { "symlink"                    , NULL}, 
/* 84*/  { "lstat"                      , NULL}, 
/* 85*/  { "readlink"                   , NULL}, 
/* 86*/  { "uselib"                     , NULL}, 
/* 87*/  { "swapon"                     , NULL}, 
/* 88*/  { "reboot"                     , NULL}, 
/* 89*/  { "old_readdir"                , NULL}, 
/* 90*/  { "old_mmap"                   , NULL}, 
/* 91*/  { "munmap"                     , NULL}, 
/* 92*/  { "truncate"                   , NULL}, 
/* 93*/  { "ftruncate"                  , NULL}, 
/* 94*/  { "fchmod"                     , NULL}, 
/* 95*/  { "fchown"                     , NULL}, 
/* 96*/  { "getpriority"                , NULL}, 
/* 97*/  { "setpriority"                , NULL}, 
/* 98*/  { NULL                         , NULL}, 
/* 99*/  { "statfs"                     , NULL}, 
/* 100*/ { "fstatfs"                    , NULL}, 
/* 101*/ { "ioperm"                     , NULL}, 
/* 102*/ { NULL                         , env_linux_socketcall}, 
/* 103*/ { "syslog"                     , NULL}, 
/* 104*/ { "setitimer"                  , NULL}, 
/* 105*/ { "getitimer"                  , NULL}, 
/* 106*/ { "newstat"                    , NULL}, 
/* 107*/ { "newlstat"                   , NULL}, 
/* 108*/ { "newfstat"                   , NULL}, 
/* 109*/ { "uname"                      , NULL}, 
/* 110*/ { "iopl"                       , NULL}, 
/* 111*/ { "vhangup"                    , NULL}, 
/* 112*/ { "idle"                       , NULL}, 
/* 113*/ { "vm86old"                    , NULL}, 
/* 114*/ { "wait4"                      , NULL}, 
/* 115*/ { "swapoff"                    , NULL}, 
/* 116*/ { "sysinfo"                    , NULL}, 
/* 117*/ { "ipc"                        , NULL}, 
/* 118*/ { "fsync"                      , NULL}, 
/* 119*/ { "sigreturn"                  , NULL}, 
/* 120*/ { "clone"                      , NULL}, 
/* 121*/ { "setdomainname"              , NULL}, 
/* 122*/ { "newuname"                   , NULL}, 
/* 123*/ { "modify_ldt"                 , NULL}, 
/* 124*/ { "adjtimex"                   , NULL}, 
/* 125*/ { "mprotect"                   , NULL}, 
/* 126*/ { "sigprocmask"                , NULL}, 
/* 127*/ { "create_module"              , NULL}, 
/* 128*/ { "init_module"                , NULL}, 
/* 129*/ { "delete_module"              , NULL}, 
/* 130*/ { "get_kernel_syms"            , NULL}, 
/* 131*/ { "quotactl"                   , NULL}, 
/* 132*/ { "getpgid"                    , NULL}, 
/* 133*/ { "fchdir"                     , NULL}, 
/* 134*/ { "bdflush"                    , NULL}, 
/* 135*/ { "sysfs"                      , NULL}, 
/* 136*/ { "personality"                , NULL}, 
/* 137*/ { NULL                         , NULL}, 
/* 138*/ { "setfsuid"                   , NULL}, 
/* 139*/ { "setfsgid"                   , NULL}, 
/* 140*/ { "llseek"                     , NULL}, 
/* 141*/ { "getdents"                   , NULL}, 
/* 142*/ { "select"                     , NULL}, 
/* 143*/ { "flock"                      , NULL}, 
/* 144*/ { "msync"                      , NULL}, 
/* 145*/ { "readv"                      , NULL}, 
/* 146*/ { "writev"                     , NULL}, 
/* 147*/ { "getsid"                     , NULL}, 
/* 148*/ { "fdatasync"                  , NULL}, 
/* 149*/ { "sysctl"                     , NULL}, 
/* 150*/ { "mlock"                      , NULL}, 
/* 151*/ { "munlock"                    , NULL}, 
/* 152*/ { "mlockall"                   , NULL}, 
/* 153*/ { "munlockall"                 , NULL}, 
/* 154*/ { "sched_setparam"             , NULL}, 
/* 155*/ { "sched_getparam"             , NULL}, 
/* 156*/ { "sched_setscheduler"         , NULL}, 
/* 157*/ { "sched_getscheduler"         , NULL}, 
/* 158*/ { "sched_yield"                , NULL}, 
/* 159*/ { "sched_get_priority_max"     , NULL}, 
/* 160*/ { "sched_get_priority_min"     , NULL}, 
/* 161*/ { "sched_rr_get_interval"      , NULL}, 
/* 162*/ { "nanosleep"                  , NULL}, 
/* 163*/ { "mremap"                     , NULL}, 
/* 164*/ { "setresuid"                  , NULL}, 
/* 165*/ { "getresuid"                  , NULL}, 
/* 166*/ { "vm86"                       , NULL}, 
/* 167*/ { "query_module"               , NULL}, 
/* 168*/ { "poll"                       , NULL}, 
/* 169*/ { "nfsservctl"                 , NULL}, 
/* 170*/ { "setresgid"                  , NULL}, 
/* 171*/ { "getresgid"                  , NULL}, 
/* 172*/ { "prctl"                      , NULL}, 
/* 173*/ { "rt_sigreturn"               , NULL}, 
/* 174*/ { "rt_sigaction"               , NULL}, 
/* 175*/ { "rt_sigprocmask"             , NULL}, 
/* 176*/ { "rt_sigpending"              , NULL}, 
/* 177*/ { "rt_sigtimedwait"            , NULL}, 
/* 178*/ { "rt_sigqueueinfo"            , NULL}, 
/* 179*/ { "rt_sigsuspend"              , NULL}, 
/* 180*/ { "pread"                      , NULL}, 
/* 181*/ { "pwrite"                     , NULL}, 
/* 182*/ { "chown"                      , NULL}, 
/* 183*/ { "getcwd"                     , NULL}, 
/* 184*/ { "capget"                     , NULL}, 
/* 185*/ { "capset"                     , NULL}, 
/* 186*/ { "sigaltstack"                , NULL}, 
/* 187*/ { "sendfile"                   , NULL}, 
/* 188*/ { NULL                         , NULL}, 
/* 189*/ { NULL                         , NULL}, 
/* 190*/ { "vfork"                      , NULL}, 
};


struct emu_env_linux_syscall syscall_hooks[] = 
{
	{ "accept"              		, env_linux_hook_socketcall},
	{ "access"              		, NULL},
	{ "acct"                		, NULL},
	{ "adjtimex"            		, NULL},
	{ "alarm"               		, NULL},
	{ "bdflush"             		, NULL},
	{ "bind"                		, env_linux_hook_socketcall},
	{ "brk"                 		, NULL},
	{ "capget"              		, NULL},
	{ "capset"              		, NULL},
	{ "chdir"               		, NULL},
	{ "chmod"               		, NULL},
	{ "chown"               		, NULL},
	{ "chroot"              		, NULL},
	{ "clone"               		, NULL},
	{ "close"               		, NULL},
	{ "connect"             		, env_linux_hook_socketcall},
	{ "creat"               		, NULL},
	{ "create_module"       		, NULL},
	{ "delete_module"       		, NULL},
	{ "dup"                 		, NULL},
	{ "dup2"                		, env_linux_hook_dup2},
	{ "execve"              		, env_linux_hook_execve},
	{ "exit"                		, env_linux_hook_exit},
	{ "fchdir"              		, NULL},
	{ "fchmod"              		, NULL},
	{ "fchown"              		, NULL},
	{ "fcntl"               		, NULL},
	{ "fdatasync"           		, NULL},
	{ "flock"               		, NULL},
	{ "fork"                		, env_linux_hook_fork},
	{ "fstat"               		, NULL},
	{ "fstatfs"             		, NULL},
	{ "fsync"               		, NULL},
	{ "ftruncate"           		, NULL},
	{ "getcwd"              		, NULL},
	{ "getdents"            		, NULL},
	{ "getegid"             		, NULL},
	{ "geteuid"             		, NULL},
	{ "getgid"              		, NULL},
	{ "getgroups"           		, NULL},
	{ "getitimer"           		, NULL},
	{ "get_kernel_syms"     		, NULL},
	{ "getpeername"         		, env_linux_hook_socketcall},
	{ "getpgid"             		, NULL},
	{ "getpgrp"             		, NULL},
	{ "getpid"              		, NULL},
	{ "getppid"             		, NULL},
	{ "getpriority"         		, NULL},
	{ "getresgid"           		, NULL},
	{ "getresuid"           		, NULL},
	{ "getrlimit"           		, NULL},
	{ "getrusage"           		, NULL},
	{ "getsid"              		, NULL},
	{ "getsockname"         		, env_linux_hook_socketcall},
	{ "getsockopt"          		, env_linux_hook_socketcall},
	{ "gettimeofday"        		, NULL},
	{ "getuid"              		, NULL},
	{ "idle"                		, NULL},
	{ "init_module"         		, NULL},
	{ "ioctl"               		, NULL},
	{ "ioperm"              		, NULL},
	{ "iopl"                		, NULL},
	{ "ipc"                 		, NULL},
	{ "kill"                		, NULL},
	{ "lchown"              		, NULL},
	{ "link"                		, NULL},
	{ "listen"              		, env_linux_hook_socketcall},
	{ "llseek"              		, NULL},
	{ "lseek"               		, NULL},
	{ "lstat"               		, NULL},
	{ "mkdir"               		, NULL},
	{ "mknod"               		, NULL},
	{ "mlock"               		, NULL},
	{ "mlockall"            		, NULL},
	{ "modify_ldt"          		, NULL},
	{ "mount"               		, NULL},
	{ "mprotect"            		, NULL},
	{ "mremap"              		, NULL},
	{ "msync"               		, NULL},
	{ "munlock"             		, NULL},
	{ "munlockall"          		, NULL},
	{ "munmap"              		, NULL},
	{ "nanosleep"           		, NULL},
	{ "newfstat"            		, NULL},
	{ "newlstat"            		, NULL},
	{ "newstat"             		, NULL},
	{ "newuname"            		, NULL},
	{ "nfsservctl"          		, NULL},
	{ "nice"                		, NULL},
	{ "old_mmap"            		, NULL},
	{ "old_readdir"         		, NULL},
	{ "old_select"          		, NULL},
	{ "oldumount"           		, NULL},
	{ "olduname"            		, NULL},
	{ "open"                		, NULL},
	{ "pause"               		, NULL},
	{ "personality"         		, NULL},
	{ "pipe"                		, NULL},
	{ "poll"                		, NULL},
	{ "prctl"               		, NULL},
	{ "pread"               		, NULL},
	{ "ptrace"              		, NULL},
	{ "pwrite"              		, NULL},
	{ "query_module"        		, NULL},
	{ "quotactl"            		, NULL},
	{ "read"                		, NULL},
	{ "readlink"            		, NULL},
	{ "readv"               		, NULL},
	{ "reboot"              		, NULL},
	{ "recv"                		, env_linux_hook_socketcall},
	{ "recvfrom"            		, env_linux_hook_socketcall},
	{ "recvmsg"             		, env_linux_hook_socketcall},
	{ "rename"              		, NULL},
	{ "rmdir"               		, NULL},
	{ "rt_sigaction"                , NULL},
	{ "rt_sigpending"               , NULL},
	{ "rt_sigprocmask"              , NULL},
	{ "rt_sigqueueinfo"             , NULL},
	{ "rt_sigreturn"                , NULL},
	{ "rt_sigsuspend"               , NULL},
	{ "rt_sigtimedwait"             , NULL},
	{ "sched_getparam"              , NULL},
	{ "sched_get_priority_max"      , NULL},
	{ "sched_get_priority_min"      , NULL},
	{ "sched_getscheduler"          , NULL},
	{ "sched_rr_get_interval"       , NULL},
	{ "sched_setparam"              , NULL},
	{ "sched_setscheduler"          , NULL},
	{ "sched_yield"                 , NULL},
	{ "select"              		, NULL},
	{ "send"                		, env_linux_hook_socketcall},
	{ "sendfile"            		, NULL},
	{ "sendmsg"             		, env_linux_hook_socketcall},
	{ "sendto"              		, env_linux_hook_socketcall},
	{ "setdomainname"       		, NULL},
	{ "setfsgid"            		, NULL},
	{ "setfsuid"            		, NULL},
	{ "setgid"              		, NULL},
	{ "setgroups"           		, NULL},
	{ "sethostname"         		, NULL},
	{ "setitimer"           		, NULL},
	{ "setpgid"             		, NULL},
	{ "setpriority"         		, NULL},
	{ "setregid"            		, NULL},
	{ "setresgid"           		, NULL},
	{ "setresuid"           		, NULL},
	{ "setreuid"            		, NULL},
	{ "setrlimit"           		, NULL},
	{ "setsid"              		, NULL},
	{ "setsockopt"          		, NULL},
	{ "settimeofday"        		, NULL},
	{ "setuid"              		, NULL},
	{ "sgetmask"            		, NULL},
	{ "shutdown"            		, env_linux_hook_socketcall},
	{ "sigaction"           		, NULL},
	{ "sigaltstack"         		, NULL},
	{ "signal"              		, NULL},
	{ "sigpending"          		, NULL},
	{ "sigprocmask"         		, NULL},
	{ "sigreturn"           		, NULL},
	{ "sigsuspend"          		, NULL},
	{ "socket"              		, env_linux_hook_socketcall},
	{ "socketpair"          		, env_linux_hook_socketcall},
	{ "ssetmask"            		, NULL},
	{ "stat"                		, NULL},
	{ "statfs"              		, NULL},
	{ "stime"               		, NULL},
	{ "swapoff"             		, NULL},
	{ "swapon"              		, NULL},
	{ "symlink"             		, NULL},
	{ "sync"                		, NULL},
	{ "sysctl"              		, NULL},
	{ "sysfs"               		, NULL},
	{ "sysinfo"             		, NULL},
	{ "syslog"              		, NULL},
	{ "time"                		, NULL},
	{ "times"               		, NULL},
	{ "truncate"            		, NULL},
	{ "umask"               		, NULL},
	{ "umount"              		, NULL},
	{ "uname"               		, NULL},
	{ "unlink"              		, NULL},
	{ "uselib"              		, NULL},
	{ "ustat"               		, NULL},
	{ "utime"               		, NULL},
	{ "vfork"               		, NULL},
	{ "vhangup"             		, NULL},
	{ "vm86"                		, NULL},
	{ "vm86old"             		, NULL},
	{ "wait4"               		, NULL},
	{ "waitpid"             		, NULL},
	{ "write"               		, NULL},
	{ "writev"              		, NULL},
};

#ifdef __cplusplus
}
#endif