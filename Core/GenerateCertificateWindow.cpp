#include "stdafx.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Secret_Input.H>
#undef CREATE
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Input_Choice.H>

#include <memory>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <sstream>

#include "LogWindow.h"
#include "ConnectionInfoWindow.h"
#include "GenerateCertificateWindow.h"
#include "Server_Config.h"
#include "crypto.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Server {
			namespace UI {
				class GenerateCertificateWindowImpl : Fl_Window {
				public:

					Fl_Input* _FullPathToSaveFolder = nullptr;
					Fl_Input* _Filename = nullptr;
					Fl_Input* _CommonName = nullptr;
					Fl_Input* _CompanyName = nullptr;
					Fl_Input* _Country = nullptr;
					Fl_Value_Input* _DaysValid = nullptr;
					Fl_Input_Choice* _bitstrength = nullptr;

					Fl_Secret_Input* _PasswordToPrivateKey = nullptr;

					Fl_Button* StartStopBtn = nullptr;


					GenerateCertificateWindowImpl() : Fl_Window(400, 400, 800, 250, "Generate Self-Signed Certificate"){
					
						auto startleft = 200;
						auto workingy = 0;

						_FullPathToSaveFolder = new Fl_Input(startleft, workingy, w() - startleft, 20, "Path to Certificate: ");
						_FullPathToSaveFolder->tooltip("This is the full path to the certificate file");
						_FullPathToSaveFolder->align(FL_ALIGN_LEFT);
						_FullPathToSaveFolder->readonly(1);
						_FullPathToSaveFolder->callback(_FullPathToSaveFolderCB, this);
						workingy += 24;

						_Filename = new Fl_Input(startleft, workingy, w() - startleft, 20, "Filename Prefix: ");
						_Filename->tooltip("This will be the prefix for the filename");
						_Filename->align(FL_ALIGN_LEFT);
						workingy += 24;

						_PasswordToPrivateKey = new Fl_Secret_Input(startleft, workingy, w() - startleft, 20, "Private Key Password: ");
						_PasswordToPrivateKey->tooltip("This is the password needed to open the Private Keyfile");
						_PasswordToPrivateKey->align(FL_ALIGN_LEFT);
						workingy += 24;

						_CommonName = new Fl_Input(startleft, workingy, w() - startleft, 20, "Common Name: ");
						_CommonName->tooltip("This should be the domain name of the host: i.e.  google.com    or      cnn.com      or localhost");
						_CommonName->align(FL_ALIGN_LEFT);
						_CommonName->value("localhost");
						workingy += 24;

						_CompanyName = new Fl_Input(startleft, workingy, w() - startleft, 20, "Company Name: ");
						_CompanyName->tooltip("This should be the name of your organization: Microsoft, or Bank of America.");
						_CompanyName->align(FL_ALIGN_LEFT);
						_CompanyName->value("Company Name");
						workingy += 24;

						_Country = new Fl_Input(startleft, workingy, w() - startleft, 20, "Country: ");
						_Country->tooltip("This should be the Country your Organization");
						_Country->align(FL_ALIGN_LEFT);
						_Country->value("USA");
						workingy += 24;

						_DaysValid = new Fl_Value_Input(startleft, workingy, w() - startleft, 20, "Days Valid: ");
						_DaysValid->tooltip("Days this certificate should be valid.");
						_DaysValid->align(FL_ALIGN_LEFT);
						_DaysValid->range(1, 365 * 5);
						_DaysValid->value(365.0);
						workingy += 24;

						_bitstrength = new Fl_Input_Choice(startleft, workingy, w() - startleft, 20, "Encryption Strength: ");
						_bitstrength->tooltip("Certificate Encryption Strength");
						_bitstrength->align(FL_ALIGN_LEFT);
						_bitstrength->add("128 bit");
						_bitstrength->add("192 bit");
						_bitstrength->add("256 bit");
						_bitstrength->value(0);
						workingy += 24;

						StartStopBtn = new Fl_Button(0, h() - 30, w(), 30, "Generate");
						StartStopBtn->callback(startgeneration, this);
						this->end();

					}
					virtual ~GenerateCertificateWindowImpl() {

					}
					static void startgeneration(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (GenerateCertificateWindowImpl*)data;
						std::string saveloc = p->_FullPathToSaveFolder->value();
						if (saveloc.empty()) {
							fl_alert("The save location cannot be empty!");
							return;
						}
						if (saveloc.back() != '\\' || saveloc.back() != '/') saveloc += '/';
						std::string certfile = p->_Filename->value();
						if (certfile.empty()) {
							fl_alert("The filename cannot be empty!");
							return;
						}
						Crypto::CertInfo info;
						info.password = p->_PasswordToPrivateKey->value();
						if (info.password.empty()) {
							fl_alert("The password cannot be empty!");
							return;
						}
						if (info.password.size() < 8) {
							fl_alert("The password must be at least 8 character long!");
							return;
						}
						info.savelocation = saveloc;
						info.filename = p->_Filename->value();
						info.commonname = p->_CommonName->value();
						info.companyname = p->_CompanyName->value();
						info.country = p->_Country->value();
						if (p->_DaysValid->value() == 0) {
							info.bits = 3072;
						} else if (p->_DaysValid->value() ==1) {
							info.bits = 7680;
						}
						else if (p->_DaysValid->value() == 2) {
							info.bits = 15360;
						}

						auto saved = Crypto::CreateCertificate(info);
						if (!saved.Certificate.empty() && !saved.Private_Key.empty()) {
							fl_alert("Successfully generated a certificate!");
							p->hide();
						}
					}
					static void _FullPathToSaveFolderCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (GenerateCertificateWindowImpl*)data;
						Fl_Native_File_Chooser chooser;
						chooser.title("Select the file location to save the Cert and Key");
						chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_DIRECTORY);

						auto action = chooser.show();
						if (action == -1 || action == 1) return;//cancel was hit

						p->_FullPathToSaveFolder->value(chooser.filename());
					}

					void Show() {
						this->show();
					}
					void Hide() {
						this->hide();
					}
				};
			}
		}
	}
}


SL::Remote_Access_Library::Server::UI::GenerateCertificateWindow::GenerateCertificateWindow()
{
	_GenerateCertificateWindowImpl = new GenerateCertificateWindowImpl();

}
SL::Remote_Access_Library::Server::UI::GenerateCertificateWindow::~GenerateCertificateWindow()
{
	delete _GenerateCertificateWindowImpl;
}

void SL::Remote_Access_Library::Server::UI::GenerateCertificateWindow::Show()
{
	_GenerateCertificateWindowImpl->Show();
}


void SL::Remote_Access_Library::Server::UI::GenerateCertificateWindow::Hide()
{
	_GenerateCertificateWindowImpl->Hide();
}
