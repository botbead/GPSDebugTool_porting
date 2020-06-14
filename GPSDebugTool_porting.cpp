#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

USEFORM("main_form.cpp", Form1);

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		Application->Initialize();
		Application->Title = "GPSDebugTool_porting";
		CreateMutexA(NULL, False, "GPSDebugTool_porting");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			MessageBoxA(Application->Handle, "The Tool is running!", "Warning",
				MB_OK | MB_ICONWARNING);
			return 0;
		}
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...) {
		try {
			throw Exception("");
		}
		catch (Exception &exception) {
			Application->ShowException(&exception);
		}
	}
	return 0;
}
