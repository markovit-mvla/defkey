#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <bpf/libbpf.h>

int main(int argc, char **argv)
{
	struct bpf_object *obj;
	int prog_fd[4];

	// load eBPF
	obj = bpf_object__open_file("defkey.o", NULL);
	if (libbpf_err(obj)) {
		fprintf(stderr, "error loading eBPF obj");
		return -1;
	}

	// load and attach prog
	prog_fd[0] = bpf_program__fd(bpf_object__find_program_by_title(obj, "kprobe/sys_read"));
	if (prog_fd[0] < 0) {
		fprintf(stderr, "error attaching kprobe to read\n");
		return -1;	
	}
	bpf_attach_kprobe(prog_fd[0], "sys_read");	

	prog_fd[1] = bpf_program__fd(bpf_object__find_program_by_title(obj, "kprobe/sys_write"));
	if (prog_fd[1] < 0) {
		fprintf(stderr, "error attaching kprobe to write\n");
		return -1;
	}
	bpf_attach_kprobe(prog_fd[1], "sys_write");

	prog_fd[2] = bpf_program__fd(bpf_object__find_program_by_title(obj, "kprobe/sys_ioctl"));
	if (prog_fd[2] < 0) {
		fprintf(stderr, "error attaching kprobe to ioctl\n");
		return -1;
	}
	bpf_attach_kprobe(prog_fd[2], "sys_ioctl");

	prog_fd[3] = bpf_program__fd(bpf_object__find_program_by_title(obj, "kprobe/sys_socket"));
	if (prog_fd[3] < 0) {
		fprintf(stderr, "error attaching kprobe to socket\n");
		return -1;
	}
	bpf_attach_kprobe(prog_fd[3], "sys_socket");

	while(1) { sleep(1); }

	return 0;
}
