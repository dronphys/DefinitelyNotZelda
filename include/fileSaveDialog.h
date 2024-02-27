#pragma once
bool SaveFileDialog(char* filePath, DWORD maxPath)
{
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	// Display the Save As dialog box 
	if (GetSaveFileName(&ofn) == TRUE) {
		strncpy_s(filePath, maxPath, ofn.lpstrFile, _TRUNCATE);
		return true;
	}
	return false;
}


bool OpenFileDialog(char* filePath, DWORD maxPath) {
	OPENFILENAME ofn;       // Structure to store dialog settings
	CHAR szFile[260] = { 0 }; // Buffer to store the file path

	// Initialize OPENFILENAME structure
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // If you have a window, use its handle here
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Text Files\0*.TXT\0All Files\0*.*\0"; // Filter for text files
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; // Starting directory (NULL = current directory)
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box
	if (GetOpenFileName(&ofn) == TRUE) {
		strncpy_s(filePath, maxPath, ofn.lpstrFile, _TRUNCATE);
		return true;
	}
	return false; 
}