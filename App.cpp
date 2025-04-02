#include "wx/wx.h"
#include "MainForm.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MainForm* mainForm = new MainForm("Yapay Sinir A��");
    mainForm->Show(true);
    return true;
}