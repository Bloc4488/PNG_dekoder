#include <inc/menu.h>

void Menu::Init_application()
{
	showApplicationInfo();
	int option;
	string name;
	do {
		option = getOption();
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
			cout << "Closing program" << endl; 
			break;
		default:
			cout << "There is no such option!" << endl;
			break;
		}
	} while (option != 6);
}

string Menu::getFileName()
{
	string Name;
	cout << "You can choose from list or write ypur own path" << endl;
	cout << "List: " << endl;
	cout << "1 - linux.png" << endl;
	cout << "2 - type_2.png" << endl;
	cout << "3 - type_3.png" << endl;
	cout << "4 - type_4.png" << endl;
	cout << "5 - dice.png" << endl;
	cout << "Please write FULL PATH to file(.png) or choose option: ";
	cin >> Name;
	if (Name == "1")
		Name = "png_files/linux.png";
	else if (Name == "2")
		Name = "png_files/type_2.png";
	else if (Name == "3")
		Name = "png_files/type_3.png";
	else if (Name == "4")
		Name = "png_files/type_4.png";
	else if (Name == "5")
		Name = "png_files/dice.png";
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
	cout << "* 6 - Exit                                  *" << endl;
	cout << "*********************************************" << endl;
}

int Menu::getOption()
{
	int menuOption = 0;
	do {
		cout << "Please choose option (1 - 6): ";
		cin >> menuOption;
	} while (menuOption > 6 && menuOption < 1);
	return menuOption;
}
