// ---------------------------------------------------------------------------

#ifndef main_formH
#define main_formH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ImgList.hpp>
#include <Data.DB.hpp>
#include <FireDAC.Comp.Client.hpp>
#include <FireDAC.Phys.hpp>
#include <FireDAC.Phys.Intf.hpp>
#include <FireDAC.Phys.SQLite.hpp>
#include <FireDAC.Phys.SQLiteDef.hpp>
#include <FireDAC.Stan.Async.hpp>
#include <FireDAC.Stan.Def.hpp>
#include <FireDAC.Stan.Error.hpp>
#include <FireDAC.Stan.ExprFuncs.hpp>
#include <FireDAC.Stan.Intf.hpp>
#include <FireDAC.Stan.Option.hpp>
#include <FireDAC.Stan.Pool.hpp>
#include <FireDAC.UI.Intf.hpp>
#include <FireDAC.DApt.Intf.hpp>
#include <FireDAC.DatS.hpp>
#include <FireDAC.Stan.Param.hpp>
#include <FireDAC.Comp.DataSet.hpp>
#include <FireDAC.DApt.hpp>
#include <Vcl.DBGrids.hpp>
#include <Vcl.Grids.hpp>
#include <FireDAC.Comp.UI.hpp>
#include <FireDAC.VCLUI.Wait.hpp>
#include "GPS_Protocol.h"
#include "SimpleThreads.h"
#include <System.ImageList.hpp>

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components

	TGroupBox *GroupBox1;
	TComboBox *ComboBox1;
	TGroupBox *GroupBox2;
	TComboBox *ComboBox2;
	TStaticText *StaticText1;
	TStaticText *StaticText2;
	TComboBox *ComboBox3;
	TStaticText *StaticText3;
	TComboBox *ComboBox4;
	TStaticText *StaticText4;
	TComboBox *ComboBox5;
	TStaticText *StaticText5;
	TComboBox *ComboBox6;
	TButton *Button1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TGroupBox *GroupBox3;
	TMemo *Memo1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button5;
	TTrayIcon *TrayIcon1;
	TPopupMenu *PopupMenu1;
	TMenuItem *Show1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TImageList *ImageList1;
	TTimer *Timer1;
	TFDConnection *FDConnection1;
	TFDPhysSQLiteDriverLink *FDPhysSQLiteDriverLink1;
	TFDCommand *FDCommand1;
	TFDQuery *FDQuery1;
	TDataSource *DataSource1;
	TFDGUIxWaitCursor *FDGUIxWaitCursor1;
	TButton *Button6;
	TButton *Button100;
	TTimer *Timer2;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall TrayIcon1DblClick(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Show1Click(TObject *Sender);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button100Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);

private: // User declarations

	int exit_msg_source;

	void __fastcall create_sdb();
	void __fastcall init_user_intf();
	void __fastcall save_user_intf();

public: // User declarations

	thread_common tc;
	thread_common m_pSerialRecvThread;

	__fastcall TForm1(TComponent* Owner);

	int m_serialPort;
	int m_serialBaud;
	int m_serialDatabits;
	int m_serialStopbits;
	int m_serialParity;
	int m_protocalType;
	bool m_PortIsOpen;
	bool m_runFlag;
	bool m_StopDisp;
	CGPS_Protocol protocol;

	bool __fastcall GetCommConfig();
	void __fastcall ReadStartTime();
	bool __fastcall ReOpenComm();
	void __fastcall SerialRecvClose();
	void __fastcall CloseComm();
	bool __fastcall OpenComm();
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
