#include "../include/chsc.h"

void chsc_detach(chsc_args* argsp)
{
    MERR_ptrace(PTRACE_GETREGSET, argsp -> pid, NT_PRSTATUS, &(argsp -> iov));
    argsp -> inbak = MERR_ptrace(PTRACE_PEEKTEXT, argsp -> pid, (argsp -> regs).REG_PC, 0);
    MERR_ptrace(PTRACE_POKETEXT, argsp -> pid, (argsp -> regs).REG_PC, LOOP_CODE);
    MERR_ptrace(PTRACE_DETACH, argsp -> pid, 0, 0);
}
