#include <wx/wxprec.h>

#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "client/core.h"


Core * core = new Core();

enum { TEXT_username = wxID_HIGHEST + 1, TEXT_password };

std::pair<int, int> WINDOW_SIZE(400, 400);

class InfosFrame : public wxFrame {

  public:


};
class RankingFrame : public wxFrame {
  public:
  RankingFrame();
};
class OnlineFrame : public wxFrame {};
class GameFrame : public wxFrame {};

class InitialFrame : public wxFrame {
 public:
  InitialFrame(const wxString&);

  void Ranking(wxCommandEvent& event);
  void Online(wxCommandEvent& event);
  void Perfil(wxCommandEvent& event);


private:
  std::string username;
  std::string password;
};

class Interface : public wxApp {
 private:
  static void detach(Interface*);

  void backupCommunication();

 public:
  virtual bool OnInit();

 private:
  InitialFrame* init;
  InfosFrame* infos;
  RankingFrame* ranking;
  OnlineFrame* online;
  GameFrame* game;
};
