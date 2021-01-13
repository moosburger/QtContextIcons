// IconInjector.cpp : Implementation of CIconInjector

#include "stdafx.h"
#include "IconInjector.h"
#include <algorithm>
#include <QtCore>
#include <QtGui>
#include <qimage.h>

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT HBITMAP qt_pixmapToWinHBITMAP(const QPixmap &p, int hbitmapFormat = 0);
QT_END_NAMESPACE

#define BUFSIZE 4096

// CIconInjector
	STDMETHODIMP CIconInjector::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID) 
    {
      return S_OK;
	}

	STDMETHODIMP CIconInjector::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirst, UINT uidLast, UINT flags) 
    {
        using namespace std;
		if (flags & CMF_DEFAULTONLY) return S_OK; // Don't do anything if it's just a double-click
		int itemsCount = GetMenuItemCount(hmenu);

        // KontextMenü hintergrund
        COLORREF Color;
        int Red, Green, Blue;
/*        HWND l_pExplorerhwnd = ::GetForegroundWindow();
        Color = GetBkColor(GetDC(l_pExplorerhwnd));
        Red = GetRValue(Color);
        Green = GetGValue(Color);
        Blue = GetBValue(Color);

        Color = GetSysColor(COLOR_HIGHLIGHT);
        Red = GetRValue(Color);
        Green = GetGValue(Color);
        Blue = GetBValue(Color);

        Color = GetSysColor(COLOR_MENU);
        Red = GetRValue(Color);
        Green = GetGValue(Color);
        Blue = GetBValue(Color);

        Color = GetSysColor(COLOR_BTNFACE);
        Red = GetRValue(Color);
        Green = GetGValue(Color);
        Blue = GetBValue(Color);*/

        Color = GetSysColor(COLOR_MENUBAR);
        Red = GetRValue(Color);
        Green = GetGValue(Color);
        Blue = GetBValue(Color);

        WCHAR   DllPath[MAX_PATH] = { 0 };
        GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, _countof(DllPath));
        QString dllPfad = QString::fromStdWString(DllPath);
        QFileInfo qFileInfo(dllPfad);
        dllPfad = qFileInfo.absolutePath();

        QCoreApplication::addLibraryPath(dllPfad);
        qDebug() << QLibraryInfo::location(QLibraryInfo::PluginsPath);

        QString User = QString("%1\\AppData\\Local\\ContextIcon").arg(QString(qgetenv("UserProfile")));
        QDir UserPath(User);
        UserPath.mkpath(User);
        QFile file(QString("%1\\ContextIcons.ini").arg(User));
        QFile dbgfile(QString("%1\\Debug.ini").arg(User));
        QStringList cntxtProg;

        bool bErr = file.open(QIODevice::ReadWrite | QIODevice::Text);
        dbgfile.open(QIODevice::Append | QIODevice::Text);

        if (bErr)
        {
            QTextStream in(&file);
            while (!in.atEnd())
                cntxtProg.append(in.readLine());
        }
        int argc = 0; 
        char *argv[1];
        QGuiApplication* GuiDummy = new QGuiApplication(argc, argv);

		for (int i = 0; i < itemsCount; i++) 
        { // Iterate over the menu items
			MENUITEMINFO mii;
			ZeroMemory(&mii, sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_FTYPE | MIIM_STRING;
			mii.dwTypeData = NULL;
			BOOL ok = GetMenuItemInfo(hmenu, i, TRUE, &mii); // Get the string length
			if (mii.fType != MFT_STRING) 
                continue;

            UINT size = (mii.cch + 1) * 2 ; // Allocate space
			LPWSTR menuTitle = (LPWSTR)malloc(size);
			mii.cch = size;
			mii.fMask = MIIM_TYPE;
			mii.dwTypeData = menuTitle;
			ok = GetMenuItemInfo(hmenu, i, TRUE, &mii); // Get the actual string data
            if (mii.fType != MFT_STRING) // 4 wäre <Fehler beim Lesen der Zeichen der Zeichenfolge.>
                continue;

            mii.fMask = MIIM_BITMAP;
            ok = GetMenuItemInfo(hmenu, i, TRUE, &mii); // Get the actual string data

			bool chIcon = true;    

            QString qtTitle = QString::fromStdWString(menuTitle);
            QString dbgTitle = qtTitle;

            QRegExp rx("&", Qt::CaseInsensitive);
            if (rx.indexIn(qtTitle) != -1)
                qtTitle.replace(rx, "");

            rx.setPattern("\\.*");
            if (rx.indexIn(qtTitle) != -1)
                qtTitle.replace(rx, "");
            
            rx.setPattern("\\{|\\}");
            if (rx.indexIn(qtTitle) != -1)
                continue;

            qtTitle = qtTitle.replace("\t", " ");
            qtTitle = qtTitle.replace("\n", " ");
            qtTitle = qtTitle.trimmed();

            QString frm = QString("%1=./ContextIcon/question.png\n").arg(qtTitle);
            if (cntxtProg.isEmpty())
            {
                cntxtProg.append(frm);
            }

            QStringList lstHlp;
            bool bFound = false;
            foreach(QString Prog, cntxtProg)
            {
               if ((Prog.startsWith('#')) || (Prog.isEmpty()))
                    continue;

                lstHlp = Prog.split("=");
                rx.setPattern("(.*)? ?(" + lstHlp[0] + " ?)((.*) (.*))?");      // Kopieren
                if (rx.indexIn(qtTitle) != -1)                                  // Kopieren rückgängig machenStrg+Z 
                {
                    // hier auftrennen nach erste gruppe zweite gruppe gefunden "Kopieren rückgängig" wird rückgängig gesucht
                    QStringList lstFnd = rx.capturedTexts();// 
                    /*QStringList lstTitle = qtTitle.split(" ");
                    if (lstTitle.size() > 1)
                        continue;*/
                    //if (!lstFnd[1].isEmpty())
                    //    continue;
                        
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                cntxtProg.append(frm);
                if (bErr)
                {
                    QTextStream out(&file);
                    out << frm;

                    QTextStream dbgout(&dbgfile);
                    dbgout << QString(" %1 - %2\n").arg(dbgTitle.size()).arg(dbgTitle);
                }
                bFound = true;
            }
            rx.setPattern("^\\.*"); 
            if (rx.indexIn(lstHlp[1]) != -1)
                lstHlp[1].replace(rx, "");

            if (!lstHlp[1].startsWith("/"))
                lstHlp[1] += "/";
            
            qFileInfo.setFile(dllPfad + lstHlp[1]);
            QString IcoPath = qFileInfo.absoluteFilePath();

            // Image Laden
            QImage CntxtIco(IcoPath);
            CntxtIco = CntxtIco.scaled(16, 16);

            // Da BMP keine Transparency unterstützen zweites Image erzeugen und weiß ausmalen
            QImage BckGrnd(CntxtIco);  

            BckGrnd.fill(QColor(Red, Green, Blue).rgb());
            QPainter painter(&BckGrnd);
            // Das eigentliche Icon drüber malen
            painter.drawImage(0, 0, CntxtIco);

            // In BMP konvertieren
            QImage bmp = BckGrnd.convertToFormat(QImage::Format_RGB888);
            
            QPixmap pixmap;
            bool bConv = pixmap.convertFromImage(bmp);
            HBITMAP bmpHdl = qt_pixmapToWinHBITMAP(pixmap);

            if (bFound)
                mii.hbmpItem = bmpHdl;
			else 
				chIcon = false;

			if (chIcon) 
                SetMenuItemInfo(hmenu, i, TRUE, &mii);

			free(menuTitle);
		}

        file.close();
        dbgfile.close();
        delete GuiDummy;

		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0); // Same as S_OK (= 0) but is The Right Thing To Do [TM]
	}
	STDMETHODIMP CIconInjector::InvokeCommand(LPCMINVOKECOMMANDINFO info) {
		return S_OK;
	}
	STDMETHODIMP CIconInjector::GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT) {
		return S_OK;
    }