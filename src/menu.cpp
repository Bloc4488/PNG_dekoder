#include <inc/menu.h>

void Menu::Init_application()
{
	showApplicationInfo();
	int option;
	string name;
	do {
		option = getOptionMain();
		switch (option)
		{
		case 1:
			name = getFileName();
			png = PNG(name);
			break;
		case 2:
			png.showInformationAllChunks();
			break;
		case 3:
			png.anonymizePNG();
			break;
		case 4:
			png.showFFT();
			break;
		case 5:
			png.showImage();
			break;
		case 6:
			cout << "Switching to RSA modul" << endl << endl;
			chooseRSA();
			showApplicationInfo();
			break;
		case 7:
			cout << "Closing program" << endl; 
			break;
		default:
			cout << "There is no such option!" << endl;
			break;
		}
	} while (option != 7);
}

string Menu::getFileName()
{
	string Name;
	cout << "You can choose from list or write ypur own path" << endl;
	cout << "List: " << endl;
	cout << "1 - dice.png" << endl;
	cout << "2 - linux.png" << endl;
	cout << "3 - type_2.png" << endl;
	cout << "4 - type_3.png" << endl;
	cout << "5 - type_4.png" << endl;
	cout << "Please write FULL PATH to file(.png) or choose option: ";
	cin >> Name;
	if (Name == "1")
		Name = "png_files/dice.png";
	else if (Name == "2")
		Name = "png_files/linux.png";
	else if (Name == "3")
		Name = "png_files/type_2.png";
	else if (Name == "4")
		Name = "png_files/type_3.png";
	else if (Name == "5")
		Name = "png_files/type_4.png";
	return Name;
}

void Menu::showApplicationInfo()
{
	cout << "*********************************************" << endl;
	cout << "* Welcome in application that gets metadata *" << endl;
	cout << "* from PNG file. Application gets chunks    *" << endl;
	cout << "* from file byte by byte. Also application  *" << endl;
	cout << "* can anonymize PNG file by deleting all    *" << endl;
	cout << "* ancillary chunks.                         *" << endl;
	cout << "* Options:                                  *" << endl;
	cout << "* 1 - Choose file (default: dice.png)       *" << endl;
	cout << "* 2 - Show all chunks                       *" << endl;
	cout << "* 3 - Anonymize file                        *" << endl;
	cout << "* 4 - Show FFT                              *" << endl;
	cout << "* 5 - Show image                            *" << endl;
	cout << "* 6 - RSA                                   *" << endl;
	cout << "* 7 - Exit                                  *" << endl;
	cout << "*********************************************" << endl;
}

int Menu::getOptionMain()
{
	int menuOption = 0;
	do {
		cout << "Please choose option (1 - 7): ";
		cin >> menuOption;
	} while (menuOption > 7 && menuOption < 1);
	return menuOption;
}

void Menu::chooseRSA()
{
	showRSAInfo();
	int option;
	string name;
	do {
		option = getOptionRSA();
		switch (option)
		{
		case 1:
			png.rsaProcess(ECB);
			break;
		case 2:
			png.rsaProcess(CBC);
			break;
		case 3:
			png.rsaProcess(Library);
			break;
		case 4:
			png.rsaSetKeylength();
			break;
		case 5:
			cout << "Closing RSA modul" << endl << endl;
			break;
		default:
			cout << "There is no such option!" << endl;
			break;
		}
	} while (option != 5);
}

int Menu::getOptionRSA()
{
	int menuOption = 0;
	do {
		cout << "Please choose option (1 - 5): ";
		cin >> menuOption;
	} while (menuOption > 5 && menuOption < 1);
	return menuOption;
}

void Menu::showRSAInfo()
{
	cout << "*********************************************" << endl;
	cout << "* Welcome to RSA decrypt and encrypt modul. *" << endl;
	cout << "* This modul can decrypt and encrypt        *" << endl;
	cout << "* data from PNG files. In PNG file data is  *" << endl;
	cout << "* located in IDAT chunks. Application       *" << endl;
	cout << "* provides 2 methods of RSA decryption and  *" << endl;
	cout << "* encryption: ECB and CBC. Also you have    *" << endl;
	cout << "* option to use RSA algoryth from OpenSSL   *" << endl;
	cout << "* library.                                  *" << endl;
	cout << "* Options:                                  *" << endl;
	cout << "* 1 - ECB                                   *" << endl;
	cout << "* 2 - CBC                                   *" << endl;
	cout << "* 3 - RSA from library                      *" << endl;
	cout << "* 4 - Set RSA key length                    *" << endl;
	cout << "* 5 - Exit to main menu                     *" << endl;
	cout << "*********************************************" << endl;
}
