SIGNAL(7)           Linux Programmer's Manual           SIGNAL(7)

NAME
signal - list of available signals

DESCRIPTION
Linux  supports  the  signals listed below. Several signal
numbers are  architecture  dependent.  First  the  signals
described in POSIX.1.

       Signal     Value     Action   Comment
       -------------------------------------------------------------------------
       SIGHUP        1        A      Hangup detected on controlling terminal
                                     or death of controlling process
       SIGINT        2        A      Interrupt from keyboard
       SIGQUIT       3        A      Quit from keyboard
       SIGILL        4        A      Illegal Instruction
       SIGABRT       6        C      Abort signal from abort(3)
       SIGFPE        8        C      Floating point exception
       SIGKILL       9       AEF     Kill signal
       SIGSEGV      11        C      Invalid memory reference
       SIGPIPE      13        A      Broken pipe: write to pipe with no readers
       SIGALRM      14        A      Timer signal from alarm(1)
       SIGTERM      15        A      Termination signal
       SIGUSR1   30,10,16     A      User-defined signal 1
       SIGUSR2   31,12,17     A      User-defined signal 2
       SIGCHLD   20,17,18     B      Child stopped or terminated
       SIGCONT   19,18,25            Continue if stopped
       SIGSTOP   17,19,23    DEF     Stop process
       SIGTSTP   18,20,24     D      Stop typed at tty
       SIGTTIN   21,21,26     D      tty input for background process
       SIGTTOU   22,22,27     D      tty output for background process

       Next various other signals.

       Signal       Value     Action   Comment
       ---------------------------------------------------------------------
       SIGTRAP        5         CG     Trace/breakpoint trap
       SIGIOT         6         CG     IOT trap. A synonym for SIGABRT
       SIGEMT       7,-,7       G
       SIGBUS      10,7,10      AG     Bus error
       SIGSYS      12,-,12      G      Bad argument to routine (SVID)
       SIGSTKFLT    -,16,-      AG     Stack fault on coprocessor
       SIGURG      16,23,21     BG     Urgent condition on socket (4.2 BSD)
       SIGIO       23,29,22     AG     I/O now possible (4.2 BSD)
       SIGPOLL                  AG     A synonym for SIGIO (System V)
       SIGCLD       -,-,18      G      A synonym for SIGCHLD
       SIGXCPU     24,24,30     AG     CPU time limit exceeded (4.2 BSD)
       SIGXFSZ     25,25,31     AG     File size limit exceeded (4.2 BSD)
       SIGVTALRM   26,26,28     AG     Virtual alarm clock (4.2 BSD)
       SIGPROF     27,27,29     AG     Profile alarm clock
       SIGPWR      29,30,19     AG     Power failure (System V)
       SIGINFO      29,-,-      G      A synonym for SIGPWR

Linux 1.3.88              April 14, 1996                        1

SIGNAL(7)           Linux Programmer's Manual           SIGNAL(7)

       SIGLOST      -,-,-       AG     File lock lost
       SIGWINCH    28,28,20     BG     Window resize signal (4.3 BSD, Sun)
       SIGUNUSED    -,31,-      AG     Unused signal
       (Here - denotes that a signal is absent; there where three
       values are given, the first one is usually valid for alpha
       and  sparc,  the middle one for i386 and ppc, the last one
       for mips. Signal 29 is SIGINFO / SIGPWR on  an  alpha  but
       SIGLOST on a sparc.)

       The  letters  in  the  "Action"  column have the following
       meanings:

       A      Default action is to terminate the process.

       B      Default action is to ignore the signal.

       C      Default action is to dump core.

       D      Default action is to stop the process.

       E      Signal cannot be caught.

       F      Signal cannot be ignored.

       G      Not a POSIX.1 conformant signal.

CONFORMING TO
POSIX.1

BUGS
SIGIO and SIGLOST have the same value.  The latter is com-
mented  out in the kernel source, but the build process of
some software still thinks that signal 29 is SIGLOST.

SEE ALSO
kill(1), kill(2), setitimer(2).

Linux 1.3.88              April 14, 1996                        2
