#include <windows.h>
#include <commdlg.h>
static OPENFILENAME ofn ;
void PopFileInitialize (HWND hwnd)
{
    static TCHAR szFilter[] = TEXT ("Text Files (*.TXT)\0*.txt\0") \
        TEXT ("ASCII Files (*.ASC)\0*.asc\0") \
        TEXT ("All Files (*.*)\0*.*\0\0") ;

    ofn.lStructSize = sizeof (OPENFILENAME) ;
    ofn.hwndOwner = hwnd ;
    ofn.hInstance = NULL ;
    ofn.lpstrFilter = szFilter ;
    ofn.lpstrCustomFilter = NULL ;
    ofn.nMaxCustFilter = 0 ;
    ofn.nFilterIndex = 0 ;
    ofn.lpstrFile = NULL ; // Set in Open and Close functions
    ofn.nMaxFile = MAX_PATH ;
    ofn.lpstrFileTitle = NULL ; // Set in Open and Close functions
    ofn.nMaxFileTitle = MAX_PATH ;
    ofn.lpstrInitialDir = NULL ;
    ofn.lpstrTitle = NULL ;
    ofn.Flags = 0 ; // Set in Open and Close functions
    ofn.nFileOffset = 0 ;
    ofn.nFileExtension = 0 ;
    ofn.lpstrDefExt = TEXT ("txt") ;
    ofn.lCustData = 0L ;
    ofn.lpfnHook = NULL ;
    ofn.lpTemplateName = NULL ;
}

BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{//Cette fonction affiche une boite de dialogue ouvrir
    //standard
    //Retroune True si user a choisi un fichier, et FALSE sinon
    //Les noms de titre et chemin seront stockes dans pstrFileName
    //et pstrTitleName, grace aux membres de la structure ofn, qui pointeront
    //au meme endroit que ces pstr
    ofn.hwndOwner = hwnd ;
    ofn.lpstrFile = pstrFileName ;
    ofn.lpstrFileTitle = pstrTitleName ;
    //HIDEREADONLY cache latt:lecture seule
    //CREATEPROMPT affiche une boite de dialogue si le
    //fichier nexiste pas, LUI PERMETTANT ainsi de le creer
    //si il le souhaite
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
    //Ce qui suit realise l'ouverture proprement dite
    return GetOpenFileName (&ofn) ;
}

BOOL PopFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    /*
    Cette fonction affiche une boîte de dialogue
    "Enregistrer sous" standard permettant à l'utilisateur de
    spécifier un nom et un emplacement pour enregistrer un
     fichier,
     Cette fonction retourne OUI si User a choisi un nom de fichier
     et FALSE sinon*/
    ofn.hwndOwner = hwnd ;
    ofn.lpstrFile = pstrFileName ;
    ofn.lpstrFileTitle = pstrTitleName ;
    ofn.Flags = OFN_OVERWRITEPROMPT ;
    //OFN_OVERWRITEPROMPT affiche une boite de dialogue
    //si le fichier existe deja
    //OUVRIR La boite de dialogue
    return GetSaveFileName (&ofn) ;
}

BOOL PopFileRead (HWND hwndEdit, PTSTR pstrFileName)
{
    /*Cette fonction lit un fichier, détermine son encodage
    (Unicode ou ASCII), convertit le contenu en un format
    compatible avec le contrôle Edit (Unicode ou ANSI selon
    la compilation) et affiche le contenu dans le contrôle Edit.*/
    BYTE bySwap ;//Pour echanger les octets lors de la conversion dencodage
    DWORD dwBytesRead ;//pour stcoker le nombre doctets lus
    HANDLE hFile ;//handle di fichier ouvert
    int i, iFileLength, iUniTest ;//pour bouclesm taille dufichier et test dencodage
    PBYTE pBuffer, pText, pConv ;// Pointeurs vers des buffers pour le
    //contenu du fichier, le texte à afficher et le texte converti.

    // Open the file.en lecture seule
     if (INVALID_HANDLE_VALUE ==
     (hFile = CreateFile (pstrFileName, GENERIC_READ, FILE_SHARE_READ,
     NULL, OPEN_EXISTING, 0, NULL)))
     return FALSE ;

     // Get file size in bytes and allocate memory for read.
     // Add an extra two bytes for zero termination.

     //obtenir la taille du fichier en octet
     iFileLength = GetFileSize (hFile, NULL) ;
     pBuffer = malloc (iFileLength + 2) ;
     // Read file and put terminating zeros at end.
     //lit le contenu du fichier dans le buffer
     ReadFile (hFile, pBuffer, iFileLength, &dwBytesRead, NULL) ;
     //ferme le fichier ouvert
     CloseHandle (hFile) ;
     //ajoute le caractere null a la fin du buffer
     pBuffer[iFileLength] = '\0' ;
    pBuffer[iFileLength + 1] = '\0' ;
     // Test to see if the text is Unicode
      //Définit les flags pour tester l'encodage Unicode.
     //il fau noter que un texte est unicode si on remarque au
     //debut la une suite de deux octets 0x**** 0x****
     iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;

     //tester si le fichier est encoder en unicode
     if (IsTextUnicode (pBuffer, iFileLength, &iUniTest))
     {
         //si oui, alors on decale pText des deux premiers
         //octets marquantla signature unicode
        pText = pBuffer + 2 ;
        //on retranche la taille en octet du texte de deux octets,
        //il sagit de ceux qui represente les marqueurs unicode
        iFileLength -= 2 ;

        if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
        {//Si lencodage est unicode Big Endian
            //alors il faut inverser lordre des octets
            //une suite ABCDEF devinedra BADCFE

            for (i = 0 ; i < iFileLength / 2 ; i++)
            {
                bySwap = ((BYTE *) pText) [2 * i] ;
                ((BYTE *) pText) [2 * i] = ((BYTE *) pText) [2 * i + 1] ;

                ((BYTE *) pText) [2 * i + 1] = bySwap ;
            }
        }

        // Allocate memory for possibly converted string
        pConv = malloc (iFileLength + 2) ;
        // If the edit control is not Unicode, convert Unicode text to
        // non-Unicode (i.e., in general, wide character).
        #ifndef UNICODE
        //si edit est non unicode, alors convertir en ascii
        WideCharToMultiByte (CP_ACP, 0, (PWSTR) pText, -1,(LPSTR) pConv,
        iFileLength + 2, NULL, NULL) ;
        // If the edit control is Unicode, just copy the string
        #else
            lstrcpy ((PTSTR) pConv, (PTSTR) pText) ;
        #endif
    }
    else // the file is not Unicode
    {
        pText = pBuffer ;
        // Allocate memory for possibly converted string.
        pConv = malloc (2 * iFileLength + 2) ;
        // If the edit control is Unicode, convert ASCII text.
        #ifdef UNICODE
            MultiByteToWideChar (CP_ACP, 0, pText, -1, (PTSTR) pConv,
            iFileLength + 1) ;
            // If not, just copy buffer
        #else
            lstrcpy ((PTSTR) pConv, (PTSTR) pText) ;
        #endif
    }
    SetWindowText (hwndEdit, (PTSTR) pConv) ;
    free (pBuffer) ;
    free (pConv) ;
    return TRUE ;
}

BOOL PopFileWrite (HWND hwndEdit, PTSTR pstrFileName)
{
    //Cette fonction écrit le contenu d'un contrôle Edit dans un
    //fichier. Elle gère également l'écriture de la marque d'ordre
    // d'octet (BOM) pour les fichiers Unicode.

     DWORD dwBytesWritten ;//variable pour stocker le nombre doctets ecrits
     HANDLE hFile ;//handle du fichier ou ecrire
     int iLength ;//variable pour stocker la longueur du texte dans hwndedit
     PTSTR pstrBuffer ;//pointeur vers un buffer pour stocker le contenu du control edit
     WORD wByteOrderMark = 0xFEFF ;//valeur de la marque doctet BOM
     //pour les fichiers unicode

     // Open the file, creating it if necessary,si il nexiste pas creer quand meme
     if (INVALID_HANDLE_VALUE ==
     (hFile = CreateFile (pstrFileName, GENERIC_WRITE, 0,
     NULL, CREATE_ALWAYS, 0, NULL)))
        return FALSE ;

     // Get the number of characters in the edit control and allocate
     // memory for them.+1 pour le \0
     iLength = GetWindowTextLength (hwndEdit) ;
     pstrBuffer = (PTSTR) malloc ((iLength + 1) * sizeof (TCHAR)) ;

    if (!pstrBuffer)
    {
        CloseHandle (hFile) ;
        return FALSE ;
    }
    // If the edit control will return Unicode text, write the
    // byte order mark to the file.
    #ifdef UNICODE
    //COMMENCE le fichier par le marqueur BOM unicode pour specifier
    //quil sagit dun fichier unicode
        WriteFile (hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL) ;
    #endif
    // Get the edit buffer and write that out to the file.
    GetWindowText (hwndEdit, pstrBuffer, iLength + 1) ;
    WriteFile (hFile, pstrBuffer, iLength * sizeof (TCHAR),
    &dwBytesWritten, NULL) ;
    if ((iLength * sizeof (TCHAR)) != (int) dwBytesWritten)
    {
        //Si le nombre de mots senses etre copier na pas ete copier
        //alors, il ya erreur
     CloseHandle (hFile) ;
     free (pstrBuffer) ;
     return FALSE ;
    }
     CloseHandle (hFile) ;
     free (pstrBuffer) ;
     return TRUE ;
}
