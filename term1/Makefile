CC = gcc

all: ssu_backup cmd_add cmd_remove cmd_recover cmd_vi cmd_ls cmd_vim cmd_help cmd_exit

ssu_backup: ssu_backup.c
	$(CC)  -o ssu_backup ssu_backup.c

cmd_add: add.c
	$(CC)  -o cmd_add add.c -lcrypto

cmd_remove: remove.c
	$(CC)  -o cmd_remove remove.c -lcrypto

cmd_recover: recover.c
	$(CC)  -o cmd_recover recover.c -lcrypto

cmd_vi: vi.c
	$(CC)  -o cmd_vi vi.c

cmd_ls: ls.c
	$(CC)  -o cmd_ls ls.c

cmd_vim: vim.c
	$(CC)  -o cmd_vim vim.c

cmd_help: help.c
	$(CC)  -o cmd_help help.c

cmd_exit: exit.c
	$(CC)  -o cmd_exit exit.c

clean :
	rm -f *.o ssu_backup cmd_add cmd_remove cmd_recover cmd_vi cmd_ls cmd_vim cmd_help cmd_exit

