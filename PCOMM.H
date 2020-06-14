#ifndef PCOMMH
#define PCOMMH

#define B50			0x00
#define B75			0x01
#define B110		0x02
#define B134		0x03
#define B150		0x04
#define B300		0x05
#define B600		0x06
#define B1200		0x07
#define B1800		0x08
#define B2400		0x09
#define B4800		0x0A
#define B7200		0x0B
#define B9600		0x0C
#define B19200		0x0D
#define B38400		0x0E
#define B57600		0x0F
#define B115200 	0x10
#define B230400 	0x11
#define B460800 	0x12
#define B921600 	0x13

#define BIT_5		0x00
#define BIT_6		0x01
#define BIT_7		0x02
#define BIT_8		0x03

#define STOP_1		0x00
#define STOP_2		0x04

#define P_EVEN		0x18
#define P_ODD		0x08
#define P_SPC		0x38
#define P_MRK		0x28
#define P_NONE		0x00

#define C_DTR		0x01
#define C_RTS		0x02

#define S_CTS		0x01
#define S_DSR		0x02
#define S_RI		0x04
#define S_CD		0x08

#define SIO_OK		0
#define SIO_BADPORT	-1
#define SIO_OUTCONTROL	-2
#define SIO_NODATA	-4
#define SIO_OPENFAIL	-5
#define SIO_RTS_BY_HW	-6
#define SIO_BADPARM	-7
#define SIO_WIN32FAIL	-8

#define SIO_BOARDNOTSUPPORT -9
#define SIO_FAIL	-10
#define SIO_ABORTWRITE	-11
#define SIO_WRITETIMEOUT    -12

#define SIOFT_OK	0
#define SIOFT_BADPORT	-1
#define SIOFT_TIMEOUT	-2
#define SIOFT_ABORT	-3
#define SIOFT_FUNC	-4
#define SIOFT_FOPEN	-5
#define SIOFT_CANABORT	-6
#define SIOFT_PROTOCOL	-7
#define SIOFT_SKIP	-8
#define SIOFT_LACKRBUF	-9
#define SIOFT_WIN32FAIL -10

#define SIOFT_BOARDNOTSUPPORT -11

#ifdef __cplusplus
extern "C" {
#endif

	int __stdcall sio_ioctl(int port, int baud, int mode);
	int __stdcall sio_getch(int port);
	int __stdcall sio_read(int port, char *buf, int len);
	int __stdcall sio_putch(int port, int term);
	int __stdcall sio_write(int port, char *buf, int len);
	int __stdcall sio_flush(int port, int func);
	long __stdcall sio_iqueue(int port);
	long __stdcall sio_oqueue(int port);
	int __stdcall sio_lstatus(int port);
	int __stdcall sio_lctrl(int port, int mode);
	int __stdcall sio_cnt_irq(int port, void(__stdcall *func)(int), int count);
	int __stdcall sio_modem_irq(int port, void(__stdcall *func)(int));
	int __stdcall sio_break_irq(int port, void(__stdcall *func)(int));
	int __stdcall sio_Tx_empty_irq(int port, void(__stdcall *func)(int));
	int __stdcall sio_break(int port, int time);
	int __stdcall sio_flowctrl(int port, int mode);
	int __stdcall sio_Tx_hold(int port);
	int __stdcall sio_close(int port);
	int __stdcall sio_open(int port);
	long __stdcall sio_getbaud(int port);
	int __stdcall sio_getmode(int port);
	int __stdcall sio_getflow(int port);
	int __stdcall sio_DTR(int port, int mode);
	int __stdcall sio_RTS(int port, int mode);
	int __stdcall sio_baud(int port, long speed);
	int __stdcall sio_data_status(int port);
	int __stdcall sio_term_irq(int port, void(__stdcall *func)(int), char code);
	int __stdcall sio_linput(int port, char *buf, int lne, int term);
	int __stdcall sio_putb_x(int port, char *buf, int len, int tick);
	int __stdcall sio_putb_x_ex(int port, char *buf, int len, int tms);
	int __stdcall sio_view(int port, char *buf, int len);
	int __stdcall sio_TxLowWater(int port, int size);
	int __stdcall sio_AbortWrite(int port);
	int __stdcall sio_SetWriteTimeouts(int port, unsigned long TotalTimeouts);
	int __stdcall sio_GetWriteTimeouts(int port, unsigned long *TotalTimeouts);
	int __stdcall sio_SetReadTimeouts(int port, unsigned long TotalTimeouts,
		unsigned long IntervalTimeouts);
	int __stdcall sio_GetReadTimeouts(int port, unsigned long *TotalTimeouts,
		unsigned long *IntervalTimeouts);
	int __stdcall sio_AbortRead(int port);

	int __stdcall sio_FtASCIITx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtASCIIRx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key, int sec);
	int __stdcall sio_FtXmodemCheckSumTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtXmodemCheckSumRx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtXmodemCRCTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtXmodemCRCRx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtXmodem1KCRCTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtXmodem1KCRCRx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtYmodemTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtYmodemRx(int port, char **ffname, int fno,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtZmodemTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtZmodemRx(int port, char **ffname, int fno,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtKermitTx(int port, char *fname,
		int(__stdcall *func)(long, int, char *, long), int key);
	int __stdcall sio_FtKermitRx(int port, char **ffname, int fno,
		int(__stdcall *func)(long, int, char *, long), int key);

#ifdef __cplusplus
}
#endif

#define sio_putb(x, y, z) sio_write(x, y, z)

#endif
