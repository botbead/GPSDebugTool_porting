// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Registry.hpp"
#include <System.IOUtils.hpp>
#include <System.DateUtils.hpp>
#include "main_form.h"
#include "pcommex.h"
#include "PCOMM.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

#define KEY_AUTO_RUN "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define BUF_SIZE 128

void AppendOnOSStartup(String ExecCaption, String ExecName, String Params);
bool ExistsOnOSStartup(String ExecCaption);
void DeleteOnOSStartup(String ExecCaption);

int nProcessQd, s_qd_start, s_qd_end;

unsigned __stdcall thread_func(void *param) {
	bool leave_or_not;
	while (leave_thread_common_or_not(&leave_or_not, &(Form1->tc)),
		!leave_or_not) {
		UnicodeString thread_info("Hello,i'm a new thread——");
		thread_info += Format("%d", ARRAYOFCONST(((Form1->tc).handle)));
		OutputDebugString(thread_info.w_str());
	}
	return 0;
};

unsigned __stdcall SerialRecvThread(void *param) {
	bool leave_or_not = false;
	unsigned char recvBuffer[BUF_SIZE];
	TDateTime dt;
	UnicodeString strText, strTime, strCloseMsg, dbg_str;
	OutputDebugStringA("serial receiving thread start\r\n");

	while (leave_thread_common_or_not(&leave_or_not,
		&(Form1->m_pSerialRecvThread)), !leave_or_not) {
		lock(&(Form1->m_pSerialRecvThread.running_flag));
		ZeroMemory(recvBuffer, BUF_SIZE);

		sio_flush(Form1->m_serialPort, 2);

		while (Form1->m_runFlag) {
			nProcessQd = 1;
			switch (nProcessQd) {
			case 1:
				break;
			}
			if (Form1->protocol.m_StopProcessRecv == true) {

				Form1->CloseComm();
				Form1->m_PortIsOpen = !Form1->m_PortIsOpen;
				dt = Now();

				strText = "The last receiving time:";

				strTime =
					Format("GPS Data after receiving, the serial port is closed and waiting for the arrival of the next data! \r\n%s: %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
					ARRAYOFCONST((strText, YearOf(dt), MonthOf(dt), DayOf(dt),
					HourOf(dt), MinuteOf(dt), SecondOf(dt))));

				Form1->Memo1->Lines->Add(strTime);

				Form1->m_runFlag = false;

				Sleep(200);
				continue;
			}

			Form1->protocol.SetProtocalType(Form1->m_protocalType);

			int inBufLen = sio_iqueue(Form1->m_serialPort);

			if (inBufLen <= 0) {
				Sleep(200);
				continue;
			}

			if (inBufLen > BUF_SIZE) {
				sio_flush(Form1->m_serialPort, 2);
				Sleep(200);
				continue;
			}

			ZeroMemory(recvBuffer, BUF_SIZE);

			int count = sio_read(Form1->m_serialPort, (char*)recvBuffer,
				inBufLen);
			if (count <= 0) {
				Sleep(200);
				continue;
			}

			dbg_str = Format("recv = %d", ARRAYOFCONST((count)));
			OutputDebugString(dbg_str.w_str());

			Form1->protocol.processRecv(recvBuffer, count);

			Sleep(200);
			unlock(&(Form1->m_pSerialRecvThread.running_flag));
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
	exit_msg_source = 0;
	m_PortIsOpen = false;
	m_runFlag = false;
	m_StopDisp = false;
	init_thread_common(&m_pSerialRecvThread);
	start_thread_common(&m_pSerialRecvThread, SerialRecvThread, NULL);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	if (Button1->Caption == "Open Serial Port") {
		if (OpenComm() == false) {
			Timer1->Enabled = false;
			Memo1->Lines->Add
				("Open Serial port failed! Please check the configuration of the serial port!"
				);
			return;
		}
		else {
			m_PortIsOpen = !m_PortIsOpen;
			Button1->Caption = "Close Serial Port";
			ComboBox1->Enabled = false;
			ComboBox2->Enabled = false;
			ComboBox3->Enabled = false;
			ComboBox4->Enabled = false;
			ComboBox5->Enabled = false;
			ComboBox6->Enabled = false;
		}

	}
	else if (Button1->Caption == "Close Serial Port") {
		Button1->Caption = "Open Serial Port";
		ComboBox1->Enabled = true;
		ComboBox2->Enabled = true;
		ComboBox3->Enabled = true;
		ComboBox4->Enabled = true;
		ComboBox5->Enabled = true;
		ComboBox6->Enabled = true;

		Timer1->Enabled = false;
		Timer2->Enabled = false;
		CloseComm();
		m_PortIsOpen = false;
		protocol.Send_Message
			("The serial port is closed. Reopen the serial port for the next data."
			);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::TrayIcon1DblClick(TObject *Sender) {
	if (Form1->Visible == false) {
		Form1->Visible = true;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender) {
	Form1->Visible = false;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender) {
	exit_msg_source = 1;
	end_thread_common(&m_pSerialRecvThread);
	Close();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Show1Click(TObject *Sender) {
	Form1->Visible = true;
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender) {
	exit_msg_source = 1;
	end_thread_common(&m_pSerialRecvThread);
	Close();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (exit_msg_source)
		CanClose = true;
	else {
		CanClose = false;
		Form1->Visible = false;
	}
}
// ---------------------------------------------------------------------------

void AppendOnOSStartup(String ExecCaption, String ExecName, String Params) {
	TRegistry* aRegistry = new TRegistry();
	aRegistry->RootKey = HKEY_CURRENT_USER;
	if (aRegistry->OpenKey(KEY_AUTO_RUN, false)) {
		if (Params.operator != ("")) {
			aRegistry->WriteString(ExecCaption, ExecName =
				"\"" + ExecName + "\" " + Params);
		}
		else {
			aRegistry->WriteString(ExecCaption, ExecName);
		}
	}
	aRegistry->CloseKey();
	delete aRegistry;
}

// ---------------------------------------------------------------------------
bool ExistsOnOSStartup(String ExecCaption) {
	bool Result = false;

	TRegistry* aRegistry = new TRegistry();
	aRegistry->RootKey = HKEY_CURRENT_USER;
	if (aRegistry->OpenKeyReadOnly(KEY_AUTO_RUN)) {
		TStrings* ss = new TStringList();
		aRegistry->GetValueNames(ss);
		Result = ss->IndexOf(ExecCaption) > -1;
		delete ss;
	}
	aRegistry->CloseKey();
	delete aRegistry;

	return Result;
}

// ---------------------------------------------------------------------------
void DeleteOnOSStartup(String ExecCaption) {
	TRegistry* aRegistry = new TRegistry();
	aRegistry->RootKey = HKEY_CURRENT_USER;
	if (aRegistry->OpenKey(KEY_AUTO_RUN, false)) {
		aRegistry->DeleteValue(ExecCaption);
	}
	aRegistry->CloseKey();
	delete aRegistry;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::CheckBox2Click(TObject *Sender) {
	if (CheckBox2->Checked == false) {
		if (ExistsOnOSStartup("GPSDeubgTool")) {
			DeleteOnOSStartup("GPSDeubgTool");
		}
	}
	else {
		AppendOnOSStartup("GPSDeubgTool", Application->ExeName, "");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender) {
	if (Button3->Caption == "Stop Display") {
		Button3->Caption = "Start Display";
		m_StopDisp = true;
	}
	else if (Button3->Caption == "Start Display") {
		Button3->Caption = "Stop Display";
		m_StopDisp = false;
	}
	protocol.m_StopDisp = m_StopDisp;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::create_sdb() {
	UnicodeString tp = TPath::GetTempPath();
	UnicodeString sdb_path("Database=");
	UnicodeString sqlite_cmd("CREATE TABLE IF NOT EXISTS usr_intf_state (\
protocol_index INTEGER NOT NULL DEFAULT 2,\
sp_index INTEGER NOT NULL DEFAULT 0,\
br_index INTEGER NOT NULL DEFAULT 12,\
pb_index INTEGER NOT NULL DEFAULT 0,\
db_index INTEGER NOT NULL DEFAULT 2,\
sb_index INTEGER NOT NULL DEFAULT 0,\
rowid_2th INTEGER PRIMARY KEY AUTOINCREMENT\
);");
	FDConnection1->Params->Add("DriverID=SQLite");
	sdb_path += ExtractFilePath(Application->ExeName);
	sdb_path += "intf_state.sdb";
	FDConnection1->Params->Add(sdb_path);
	FDConnection1->Params->Add("SQLiteAdvanced=temp_store=Memory");
	FDConnection1->Params->Add("LockingMode=Exclusive");

	FDQuery1->Connection = FDConnection1;
	DataSource1->DataSet = FDQuery1;
	FDCommand1->Connection = FDConnection1;

	FDConnection1->Connected = true;
	FDCommand1->CommandText->Add(sqlite_cmd);
	FDCommand1->Active = true;
}

void __fastcall TForm1::FormCreate(TObject *Sender) {
	create_sdb();
	init_user_intf();
	Button100->Visible = false;
	Button6->Visible = false;
	Timer1->Interval = 1000;
	Timer1->Enabled = false;
	Timer2->Interval = 1000;
	Timer2->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::init_user_intf() {
	int rc;
	FDQuery1->Open("SELECT * FROM usr_intf_state;");
	rc = FDQuery1->RecordCount;
	if (rc) {
		FDQuery1->First();
		ComboBox1->ItemIndex = FDQuery1->FieldByName("protocol_index")
			->AsInteger;
		ComboBox2->ItemIndex = FDQuery1->FieldByName("sp_index")->AsInteger;
		ComboBox3->ItemIndex = FDQuery1->FieldByName("br_index")->AsInteger;
		ComboBox4->ItemIndex = FDQuery1->FieldByName("pb_index")->AsInteger;
		ComboBox5->ItemIndex = FDQuery1->FieldByName("db_index")->AsInteger;
		ComboBox6->ItemIndex = FDQuery1->FieldByName("sb_index")->AsInteger;
	}
	else {
		ComboBox1->ItemIndex = 2;
		ComboBox2->ItemIndex = 0;
		ComboBox3->ItemIndex = 12;
		ComboBox4->ItemIndex = 0;
		ComboBox5->ItemIndex = 2;
		ComboBox6->ItemIndex = 0;
	}
}

void __fastcall TForm1::save_user_intf() {
	UnicodeString sqlcmd("");
	sqlcmd.operator = ("DELETE FROM usr_intf_state;");
	FDQuery1->SQL->Clear();
	FDQuery1->SQL->Add(sqlcmd);
	FDQuery1->ExecSQL();
	sqlcmd.operator =
		("INSERT INTO usr_intf_state (protocol_index, sp_index, br_index, pb_index, db_index, sb_index) VALUES ("
		);
	sqlcmd.operator += (IntToStr((int)ComboBox1->ItemIndex));
	sqlcmd += ",";
	sqlcmd.operator += (IntToStr((int)ComboBox2->ItemIndex));
	sqlcmd += ",";
	sqlcmd.operator += (IntToStr((int)ComboBox3->ItemIndex));
	sqlcmd += ",";
	sqlcmd.operator += (IntToStr((int)ComboBox4->ItemIndex));
	sqlcmd += ",";
	sqlcmd.operator += (IntToStr((int)ComboBox5->ItemIndex));
	sqlcmd += ",";
	sqlcmd.operator += (IntToStr((int)ComboBox6->ItemIndex));
	sqlcmd += ");";
	FDQuery1->SQL->Clear();
	FDQuery1->SQL->Add(sqlcmd);
	FDQuery1->ExecSQL();
}

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
	save_user_intf();
	FDConnection1->Connected = false;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
	Memo1->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Button100Click(TObject *Sender) {
	init_thread_common(&tc);
	start_thread_common(&tc, thread_func, NULL);
	active_thread_common(&tc);
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender) {
	end_thread_common(&tc);
}

// ---------------------------------------------------------------------------
bool __fastcall TForm1::GetCommConfig() {
	m_serialBaud = UserBaudToPCOMM(ComboBox3->Text.ToInt());

	m_serialPort = ComboBox2->ItemIndex + 1;
	m_serialDatabits = UserDatabitsToPCOMM(ComboBox5->ItemIndex + 5);
	m_serialStopbits = UserStopbitsToPCOMM(ComboBox6->ItemIndex + 1);
	m_serialParity = UserParityToPCOMM(ComboBox4->ItemIndex);

	m_protocalType = ComboBox1->ItemIndex;

	return true;
}

void __fastcall TForm1::ReadStartTime() {
	TDateTime dt = Now();
	s_qd_start = SecondOf(dt);
	UnicodeString s("Open Serial port at: ");
	s += Format("%.2d:%.2d", ARRAYOFCONST((MinuteOf(dt), SecondOf(dt))));
	protocol.Send_Message(s);
	Timer2->Interval = 1000;
	Timer2->Enabled = true;
}

bool __fastcall TForm1::ReOpenComm() {

	if (!m_PortIsOpen) {
		if (GetCommConfig() != true)
			return false;

		int iOpenErrorCode = sio_open(m_serialPort);
		int iCtrlErrorCode = sio_ioctl(m_serialPort, m_serialBaud,
			m_serialDatabits | m_serialStopbits | m_serialParity);

		if ((iOpenErrorCode == SIO_OK) && (iCtrlErrorCode == SIO_OK)) {
			nProcessQd = 1;

			m_PortIsOpen = !m_PortIsOpen;
			Button1->Caption = "Close Serial Port";
			ComboBox1->Enabled = false;
			ComboBox2->Enabled = false;
			ComboBox3->Enabled = false;
			ComboBox4->Enabled = false;
			ComboBox5->Enabled = false;
			ComboBox6->Enabled = false;

		}
		else {
			return false;
		}
	}
	else {

	}
	return true;
}

void __fastcall TForm1::SerialRecvClose() {
	TDateTime dt = Now();
	s_qd_end = SecondOf(dt);
	int i;
	if (s_qd_end >= s_qd_start)
		i = s_qd_end - s_qd_start;
	else
		i = 60 + s_qd_end - s_qd_start;
	UnicodeString s("No data received within 5 seconds.Close Serial port at: ");
	if (i == 5) {
		s += Format("%.2d:%.2d", ARRAYOFCONST((MinuteOf(dt), SecondOf(dt))));
		s += " and wait for the arrival of the next data.";
		protocol.Send_Message(s);

		Timer2->Enabled = false;
		CloseComm();
		m_PortIsOpen = false;
		protocol.m_StopProcessRecv = false;
	}
}

void __fastcall TForm1::CloseComm() {
	bool thread_is_running = false;
	if (m_runFlag) {
		m_runFlag = false;

		while (1) {
			if (tc_logic_running(&thread_is_running, &m_pSerialRecvThread),
				!thread_is_running) {
				break;
			}
			else {
				Sleep(200);
			}
		}
	}

	if (m_PortIsOpen) {
		sio_close(m_serialPort);
	}
}

bool __fastcall TForm1::OpenComm() {
	if (GetCommConfig() != true)
		return false;

	int iOpenErrorCode = sio_open(m_serialPort);
	int iCtrlErrorCode = sio_ioctl(m_serialPort, m_serialBaud,
		m_serialDatabits | m_serialStopbits | m_serialParity);

	if ((iOpenErrorCode == SIO_OK) && (iCtrlErrorCode == SIO_OK)) {

		m_runFlag = true;
		active_thread_common(&m_pSerialRecvThread);
		nProcessQd = 1;
		ReadStartTime();
		protocol.m_StopProcessRecv = false;
		Timer1->Enabled = true;
	}
	else {
		return false;
	}

	return true;
}

void __fastcall TForm1::Timer1Timer(TObject *Sender) {
	m_PortIsOpen = false;
	ReOpenComm();
	Sleep(200);
	ReadStartTime();
	protocol.m_StopProcessRecv = false;
	m_runFlag = true;
	active_thread_common(&m_pSerialRecvThread);
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender) {
	if (!protocol.m_StopProcessRecv || m_runFlag) {
		SerialRecvClose();
	}
}
// ---------------------------------------------------------------------------
