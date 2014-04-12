//
// Prototypes for IPC operations.
// Included for use in dumb systems without proper prototypes
// - these are the linux versions.
//

#ifndef ipc_protos_incl
#define ipc_protos_incl

#ifdef ultrix
extern "C" {
key_t ftok(char*, char);
}
#endif

#if !defined(linux) && !defined(ultrix) && !defined(__CC)

struct shmid_ds;
struct msgbuf;
struct msqid_ds;
struct sembuf;

#include <sys/types.h>

extern "C" {

extern key_t ftok (char *__pathname, char __proc_id);
extern int shmctl (int __shmid, int __cmd, struct shmid_ds *__buf);
extern int shmget (key_t __key, int __size, int __flag);
extern char *shmat (int __shmid, char *__shmaddr, int __shmflg);
extern int shmdt (char *__shmaddr);
extern int msgget (key_t __key, int __msgflg);
extern int msgsnd (int __msqid, struct msgbuf *__msgp, int __msgsz, 
		       int __msgflg);
extern int msgrcv (int __msqid, struct msgbuf *__msgp, int __msgsz,
		       long __msgtyp, int __msgflg);
extern int msgctl (int __msqid, int __cmd, struct msqid_ds *__buf);
extern int semget (key_t __key, int __nsems, int __semflg);
extern int semop (int __semid, struct sembuf *__sops, unsigned __nsops);
extern int semctl (int __semid, int __semnum, int __cmd, 
		       union semun __arg);
}
#endif
#endif
