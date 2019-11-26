#include "client/interface.h"

void Interface::detach(Interface *interface) {
  interface->backupCommunication();
}
void Interface::backupCommunication() { std::cout << "asdasd" << std::endl; }

bool Interface::OnInit() {
  // username = new wxTextCtrl(this, TEXT_username, "username", wxDefaultPosition,
  //                           wxDefaultSize, wxTE_MULTILINE | wxTE_RICH,
  //                           wxDefaultValidator, wxTextCtrlNameStr);

  // password = new wxTextCtrl(this, TEXT_password, "password", wxDefaultPosition,
  //                           wxDefaultSize, wxTE_MULTILINE | wxTE_RICH,
  //                           wxDefaultValidator, wxTextCtrlNameStr);

  // createPlayer();

  

  this->init = new InitialFrame(wxT("Messages"));
  this->init->Show(true);

  std::thread(Interface::detach, this).detach();
  return true;
}

InitialFrame::InitialFrame(const wxString &title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition,
              wxSize(WINDOW_SIZE.first, WINDOW_SIZE.second)) {
  wxPanel *panel = new wxPanel(this, wxID_ANY);


  wxString user(wxT("Username: somente letras"));
  wxTextEntryDialog username(panel, user);
  username.SetTextValidator(wxFILTER_ALPHA);
  if (username.ShowModal() == wxID_OK) {
    this->username = username.GetValue();
  }

  wxString pass(wxT("Senha: somente letras"));
  wxTextEntryDialog password(panel, pass);
  password.SetTextValidator(wxFILTER_ALPHA);
  if (password.ShowModal() == wxID_OK) {
    this->password = password.GetValue();
  }

  core->createPlayer(this->username, this->password);

  core->connectToServer(std::string("127.0.0.1"), 9999);

  wxBoxSizer *hbox = new wxBoxSizer(wxVERTICAL);
  wxGridSizer *gs = new wxGridSizer(3, 3, 5);

  int ID_RANKING = 1;
  int ID_ONLINE = 2;
  int ID_PERFIL = 3;


  wxButton *btn1 = new wxButton(panel, ID_RANKING, wxT("Ranking"));
  wxButton *btn2 = new wxButton(panel, ID_ONLINE, wxT("Online"));
  wxButton *btn3 = new wxButton(panel, ID_PERFIL, wxT("Perfil"));

  Connect(ID_RANKING, wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(InitialFrame::Ranking));
  Connect(ID_ONLINE, wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(InitialFrame::Online));
  Connect(ID_PERFIL, wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(InitialFrame::Perfil));


  gs->Add(btn1, 1);
  gs->Add(btn2, 1);
  gs->Add(btn3, 1);

  hbox->Add(gs, 0, wxALL, 15);
  panel->SetSizer(hbox);

  Center();
}


void InitialFrame::Ranking(wxCommandEvent &event) {
  // wxPanel *panel = new wxPanel(this, wxID_ANY);



  auto result = core->ranking();

  for (auto it = result.begin() ; it != result.end(); ++it) {
    std::cout << (*it).username() << std::endl;
  }


  // wxBoxSizer *hbox = new wxBoxSizer(wxVERTICAL);
  // wxGridSizer *gs = new wxGridSizer(3, 3, 3);

  // wxButton *btn1 = new wxButton(panel, ID_RANKING, wxT("Ranking"));
  // wxButton *btn2 = new wxButton(panel, ID_ONLINE, wxT("Online"));
  // wxButton *btn3 = new wxButton(panel, ID_PERFIL, wxT("Perfil"));

  // Connect(ID_RANKING, wxEVT_COMMAND_BUTTON_CLICKED,
  //         wxCommandEventHandler(InitialFrame::Ranking));
  // Connect(ID_ONLINE, wxEVT_COMMAND_BUTTON_CLICKED,
  //         wxCommandEventHandler(InitialFrame::Online));
  // Connect(ID_PERFIL, wxEVT_COMMAND_BUTTON_CLICKED,
  //         wxCommandEventHandler(InitialFrame::Perfil));

  // gs->Add(btn1, 1);
  // gs->Add(btn2, 1);
  // gs->Add(btn3, 1);

  // hbox->Add(gs, 0, wxALL, 15);
  // panel->SetSizer(hbox);

  // Center();
}
void InitialFrame::Online(wxCommandEvent &event) {}
void InitialFrame::Perfil(wxCommandEvent &event) {}

IMPLEMENT_APP(Interface)
