#include <linux/sched.h>
#include <linux/fs.h>
#include <uapi/linux/ptrace.h>

#define BLOCKED_SYSCALL = -1;
#define WHITELIST_LEN = 10;

const u32 whitelist[WHITELIST_LEN] = {
	// Need to populate this - determine current PIDs
};

keylogger_event {
	u32 pid;
	char comm[TASK_COMM_LEN];
	u32 syscall;
}

BPF_HASH(susp_procs, struct keylogger_event, u32);

SEC("kprobe/sys_read")
int trace_read(struct pt_regs *ctx, int fd)
{
	return trace(ctx, fd, __NR_read);
}

SEC("kprobe/sys_write")
int trace_write(struct pt_regs *ctx, int fd)
{
	return trace(ctx, fd, __NR_write);
}

SEC("kprobe/sys_ioctl") 
int trace_ioctl(struct pt_regs *ctx, int fd)
{
	return trace(ctx, fd, __NR_ioctl);
}

SEC("kprobe/sys_socket")
int trace_socket(struct pt_regs *ctx, int fd)
{
	return trace(ctx, fd, __NR_socket);
}

int trace(struct pt_regs *ctx, int fd, u32 syscall)
{
	u32 pid = bpf_get_current_pid_tgid() >> 32;
	char comm[TASK_COMM_LEN];
	bpf_get_current_comm(&comm, sizeof(comm));

	if (fd < 10) {
		u32 blacklist = (check_whitelist(&pid)) ? 1 : 0;
		struct keylogger_event event = {
			pid,
			NULL,
			syscall,
		};
		__builtin_memcpy(event.comm, comm, TASK_COMM_LEN);

		susp_procs.update(&pid, &event, &blacklist);
		
		return (check_whitelist(&pid)) ? BLOCKED_SYSCALL : 0;
	}

	return 0;
}

int check_whitelist(u32 *pid) {
	for (int i = 0; i < WHITELIST_LEN; i++) {
		if (whitelist[i] == proc) {
			return 1;
		}
	}
	return 0;
}

char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = LINUX_VERSION_CODE;
