
#define SYS_FREEMEM_ID          0              /* length of process "name"     */
#define SYS_CHPRIO_ID      	1               /* id of the null process; it   */
#define SYS_GETPID_ID         	2              /* used when invalid pid needed */
#define SYS_GETPRIO_ID		3
#define SYS_GETTIME_ID		4
#define SYS_KILL_ID		5
#define SYS_RECEIVE_ID		6
#define SYS_RECVCLR_ID		7
#define SYS_RECVTIM_ID		8
#define SYS_RESUME_ID		9
#define SYS_SCOUNT_ID		10
#define SYS_SDELETE_ID		11
#define SYS_SEND_ID		12
#define SYS_SETDEV_ID		13
#define SYS_SETNOK_ID		14
#define SYS_SCREATE_ID		15
#define SYS_SIGNAL_ID		16
#define SYS_SIGNALN_ID		17
#define SYS_SLEEP_ID		18
#define SYS_SLEEP10_ID		19
#define SYS_SLEEP100_ID		20
#define SYS_SLEEP1000_ID	21
#define SYS_SRESET_ID		22
#define SYS_STACKTRACE_ID	23
#define SYS_SUSPEND_ID		24
#define SYS_UNSLEEP_ID		25
#define SYS_WAIT_ID		26

#define MAX_SYS_CALL_TRACE	27

extern const char *syscallname[MAX_SYS_CALL_TRACE];
 

extern int syscalltrace;
void printsegaddress(void);
void printsyscallsummary(void);
void printprocstks(int priority);
void syscallsummary_start(void);
void syscallsummary_stop(void);
