#pragma hdrstop
#include "pcomm.h"
#include "pcommex.h"

#pragma package(smart_init)

int UserStopbitsToPCOMM(int iStopbits) {
	switch (iStopbits) {
	case 1:
		return STOP_1;
	case 2:
		return STOP_2;
	default:
		return STOP_1;
	}
}

int UserDatabitsToPCOMM(int iDatabits) {
	switch (iDatabits) {
	case 5:
		return BIT_5;
	case 6:
		return BIT_6;
	case 7:
		return BIT_7;
	case 8:
		return BIT_8;
	default:
		return BIT_8;
	}
}

int UserBaudToPCOMM(int iBaud) {
	switch (iBaud) {
	case 50:
		return B50;
	case 75:
		return B75;
	case 110:
		return B110;
	case 134:
		return B134;
	case 150:
		return B150;
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 1800:
		return B1800;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 7200:
		return B7200;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 921600:
		return B921600;
	default:
		return B9600;
	}
}

int UserParityToPCOMM(int iParity) {
	switch (iParity) {
	case 0:
		return P_NONE;
	case 2:
		return P_EVEN;
	case 1:
		return P_ODD;
	default:
		return P_NONE;
	}
}
