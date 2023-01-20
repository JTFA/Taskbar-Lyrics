﻿#include "TaskbarLyrics.hpp"


int WINAPI wWinMain(
    _In_        HINSTANCE   hInstance,
    _In_opt_    HINSTANCE   hPrevInstance,
    _In_        LPWSTR      lpCmdLine,
    _In_        int         nCmdShow
) {
    //AllocConsole();
    //freopen("conout$", "w", stdout);

    任务栏歌词类 任务栏歌词类(hInstance, lpCmdLine, nCmdShow);
    任务栏歌词类.注册窗口();
    任务栏歌词类.创建窗口();
    任务栏歌词类.网络线程();
    任务栏歌词类.显示窗口();
    任务栏歌词类.窗口消息();

    return 0;
}


任务栏歌词类::任务栏歌词类(
    HINSTANCE   hInstance,
    LPWSTR      lpCmdLine,
    int         nCmdShow
) {
    this->互斥锁 = CreateMutex(NULL, TRUE, this->窗口类名.c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND 任务栏_句柄 = FindWindow(L"Shell_TrayWnd", NULL);
        HWND 旧句柄 = FindWindowEx(任务栏_句柄, NULL, this->窗口类名.c_str(), NULL);
        if (!旧句柄) 旧句柄 = FindWindow(this->窗口类名.c_str(), NULL);
        PostMessage(旧句柄, WM_CLOSE, NULL, NULL);
    }

    this->_this = this;

    this->hInstance = hInstance;
    this->lpCmdLine = lpCmdLine;
    this->nCmdShow = nCmdShow;

    int argCount;
    LPWSTR* szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

    if (szArgList[1])
    {
        this->宽字符转换流 << szArgList[1];
        this->宽字符转换流 >> this->网络服务器_端口;
    }

    LocalFree(szArgList);
    GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput, NULL);

    HWND 网易云句柄 = FindWindow(L"OrpheusBrowserHost", NULL);
    if (网易云句柄)
    {
        DWORD pid;
        GetWindowThreadProcessId(网易云句柄, &pid);
        HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        RegisterWaitForSingleObject(
            &this->waitHandle,
            process,
            this->网易云进程结束,
            NULL,
            INFINITE,
            WT_EXECUTEONLYONCE
        );
    }

    this->任务栏_句柄 = FindWindow(L"Shell_TrayWnd", NULL);
    this->开始按钮_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"Start", NULL);
    this->活动区域_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"ReBarWindow32", NULL);
    this->通知区域_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"TrayNotifyWnd", NULL);

    GetWindowRect(this->任务栏_句柄, &this->任务栏_矩形);
    GetWindowRect(this->开始按钮_句柄, &this->开始按钮_矩形);
    GetWindowRect(this->活动区域_句柄, &this->活动区域_矩形);
    GetWindowRect(this->通知区域_句柄, &this->通知区域_矩形);

    this->剩余宽度检测();
}


任务栏歌词类::~任务栏歌词类()
{
    GdiplusShutdown(this->gdiplusToken);
    UnregisterWaitEx(this->网易云进程结束, INVALID_HANDLE_VALUE);
    ReleaseMutex(this->互斥锁);

    this->网络服务器_线程->detach();
    delete this->网络服务器_线程;
    this->网络服务器_线程 = nullptr;

    this->剩余宽度检测_线程->detach();
    delete this->剩余宽度检测_线程;
    this->剩余宽度检测_线程 = nullptr;

    delete this->_this;
    this->_this = nullptr;
}


void 任务栏歌词类::注册窗口()
{
    this->wcex.cbSize = sizeof(WNDCLASSEX);
    this->wcex.style = CS_HREDRAW | CS_VREDRAW;
    this->wcex.lpfnWndProc = this->窗口过程;
    this->wcex.cbClsExtra = 0;
    this->wcex.cbWndExtra = 0;
    this->wcex.hInstance = this->hInstance;
    this->wcex.lpszClassName = this->窗口类名.c_str();
    RegisterClassEx(&this->wcex);
}


void 任务栏歌词类::创建窗口()
{
    this->hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        this->窗口类名.c_str(),
        this->窗口名字.c_str(),
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        NULL,
        NULL,
        this->hInstance,
        NULL
    );

    SetParent(this->hwnd, this->任务栏_句柄);
}


void 任务栏歌词类::网络线程()
{
    auto lyrics = [&] (const httplib::Request& req, httplib::Response& res) {
        auto basic = req.get_param_value("basic");
        auto extra = req.get_param_value("extra");

        this->基本歌词 = this->字符转换.from_bytes(basic);
        this->扩展歌词 = this->字符转换.from_bytes(extra);

        this->更新窗口();
        res.status = 200;
    };

    auto font = [&] (const httplib::Request& req, httplib::Response& res) {
        auto font_family = req.get_param_value("font_family");

        HFONT font = CreateFont(
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            this->字符转换.from_bytes(font_family).c_str()
        );
        if (font) this->字体名称 = this->字符转换.from_bytes(font_family).c_str();
        DeleteObject(font);

        this->更新窗口();
        res.status = 200;
    };

    auto color = [&] (const httplib::Request& req, httplib::Response& res) {
        auto light_basic = req.get_param_value("light_basic");
        auto light_extra = req.get_param_value("light_extra");
        auto dark_basic = req.get_param_value("dark_basic");
        auto dark_extra = req.get_param_value("dark_extra");

        auto set_color = [&] (const std::string& color_str, Color& color_obj) {
            if (color_str.size() == 6) {
                int color_hex = std::stoul(color_str, nullptr, 16);
                int r = (color_hex & 0xFF0000) >> 16;
                int g = (color_hex & 0x00FF00) >> 8;
                int b = (color_hex & 0x0000FF);
                color_obj = Color(r, g, b);
            }
        };

        set_color(light_basic, this->字体颜色_浅色_基本歌词);
        set_color(light_extra, this->字体颜色_浅色_扩展歌词);
        set_color(dark_basic, this->字体颜色_深色_基本歌词);
        set_color(dark_extra, this->字体颜色_深色_扩展歌词);

        this->更新窗口();
        res.status = 200;
    };

    auto position = [&] (const httplib::Request& req, httplib::Response& res) {
        auto position = req.get_param_value("position");
        auto force_use = req.get_param_value("force_use");

        if (position == std::string("left"))
        {
            this->居中对齐 = true;
        }

        if (position == std::string("right"))
        {
            this->居中对齐 = false;
        }

        if (force_use == std::string("true"))
        {
            this->强制使用设置位置选项 = true;
        }
        else
        {
            this->强制使用设置位置选项 = false;
        }

        this->更新窗口();
        res.status = 200;
    };

    auto align = [&] (const httplib::Request& req, httplib::Response& res) {
        auto basic = req.get_param_value("basic");
        auto extra = req.get_param_value("extra");

        if (basic == std::string("left"))
        {
            this->对齐方式_基本歌词 = StringAlignmentNear;
        }

        if (extra == std::string("left"))
        {
            this->对齐方式_扩展歌词 = StringAlignmentNear;
        }

        if (basic == std::string("center"))
        {
            this->对齐方式_基本歌词 = StringAlignmentCenter;
        }

        if (extra == std::string("center"))
        {
            this->对齐方式_扩展歌词 = StringAlignmentCenter;
        }

        if (basic == std::string("right"))
        {
            this->对齐方式_基本歌词 = StringAlignmentFar;
        }

        if (extra == std::string("right"))
        {
            this->对齐方式_扩展歌词 = StringAlignmentFar;
        }

        this->更新窗口();
        res.status = 200;
    };

    auto screen = [&] (const httplib::Request& req, httplib::Response& res) {
        auto parent_taskbar = req.get_param_value("parent_taskbar");

        this->任务栏_句柄 = FindWindow(this->字符转换.from_bytes(parent_taskbar).c_str(), NULL);
        this->开始按钮_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"Start", NULL);

        GetWindowRect(this->任务栏_句柄, &this->任务栏_矩形);
        GetWindowRect(this->开始按钮_句柄, &this->开始按钮_矩形);

        SetParent(this->hwnd, this->任务栏_句柄);
        this->更新窗口();
        res.status = 200;
    };

    auto 线程函数 = [&] () {
        this->网络服务器.Get("/taskbar/lyrics", lyrics);
        this->网络服务器.Get("/taskbar/font", font);
        this->网络服务器.Get("/taskbar/color", color);
        this->网络服务器.Get("/taskbar/position", position);
        this->网络服务器.Get("/taskbar/align", align);
        this->网络服务器.Get("/taskbar/screen", screen);
        this->网络服务器.listen("127.0.0.1", this->网络服务器_端口);
    };

    this->网络服务器_线程 = new std::thread(线程函数);
}


void 任务栏歌词类::显示窗口()
{
    this->更新窗口();
    ShowWindow(this->hwnd, this->nCmdShow);
    UpdateWindow(this->hwnd);
}


void 任务栏歌词类::窗口消息()
{
    while (GetMessage(&this->msg, NULL, 0, 0)) {
        TranslateMessage(&this->msg);
        DispatchMessage(&this->msg);
    }
}


void 任务栏歌词类::网易云进程结束(
    PVOID lpParameter,
    BOOLEAN TimerOrWaitFired
) {
    任务栏歌词类* _this = 任务栏歌词类::_this;
    PostMessage(_this->hwnd, WM_CLOSE, NULL, NULL);
    _this = nullptr;
}


LRESULT CALLBACK 任务栏歌词类::窗口过程(
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
) {
    任务栏歌词类* _this = 任务栏歌词类::_this;

    switch (message) {
        case WM_PAINT:          _this->OnPaint();           break;
        case WM_ERASEBKGND:     _this->OnEraseBkgnd();      break;
        case WM_SETTINGCHANGE:  _this->OnSettingChange();   break;
        case WM_CLOSE:          _this->OnClose();           break;
        case WM_DESTROY:        _this->OnDestroy();         break;
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }

    _this = nullptr;
    return 0;
}


void 任务栏歌词类::OnPaint()
{
    PAINTSTRUCT ps;
    RECT rect;
    HDC hdc = BeginPaint(this->hwnd, &ps);
    GetClientRect(this->hwnd, &rect);

    auto 宽 = rect.right - rect.left;
    auto 高 = rect.bottom - rect.top;

    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, 宽, 高);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBitmap = (HBITMAP) SelectObject(memDC, memBitmap);


    Graphics graphics(memDC);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    //Pen myPen(Color(255, 255, 255), this->DPI(1));
    //Rect myRect(rect.left, rect.top, rect.right, rect.bottom);
    //graphics.DrawRectangle(&myPen, myRect);

    FontFamily fontFamily(this->字体名称.c_str());
    StringFormat stringFormat;
    stringFormat.SetLineAlignment(StringAlignmentCenter);

    SolidBrush 画笔_基本歌词(this->深浅模式 ? this->字体颜色_浅色_基本歌词 : this->字体颜色_深色_基本歌词);
    SolidBrush 画笔_扩展歌词(this->深浅模式 ? this->字体颜色_浅色_扩展歌词 : this->字体颜色_深色_扩展歌词);

    if (this->扩展歌词.empty())
    {
        Font font(&fontFamily, this->DPI(20), FontStyleRegular, UnitPixel);
        RectF 基本歌词_矩形((REAL) this->DPI(10), (REAL) this->DPI(10), rect.right - this->DPI(20), rect.bottom - this->DPI(20));
        stringFormat.SetAlignment(this->对齐方式_基本歌词);
        graphics.DrawString(this->基本歌词.c_str(), this->基本歌词.size(), &font, 基本歌词_矩形, &stringFormat, &画笔_基本歌词);
    }
    else
    {
        Font font(&fontFamily, this->DPI(16), FontStyleRegular, UnitPixel);
        RectF 基本歌词_矩形((REAL) this->DPI(3), (REAL) this->DPI(3), rect.right - this->DPI(6), rect.bottom / 2 - this->DPI(3));
        RectF 扩展歌词_矩形((REAL) this->DPI(3), rect.bottom / 2, rect.right - this->DPI(6), rect.bottom / 2 - this->DPI(3));
        stringFormat.SetAlignment(this->对齐方式_基本歌词);
        graphics.DrawString(this->基本歌词.c_str(), this->基本歌词.size(), &font, 基本歌词_矩形, &stringFormat, &画笔_基本歌词);
        stringFormat.SetAlignment(this->对齐方式_扩展歌词);
        graphics.DrawString(this->扩展歌词.c_str(), this->扩展歌词.size(), &font, 扩展歌词_矩形, &stringFormat, &画笔_扩展歌词);
    }


    BitBlt(hdc, 0, 0, 宽, 高, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    EndPaint(this->hwnd, &ps);
}


void 任务栏歌词类::OnEraseBkgnd()
{
    return;
}


void 任务栏歌词类::OnSettingChange()
{
    [&]() {
        DWORD value;
        std::wstring path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        std::wstring key = L"SystemUsesLightTheme";
        if (!this->读取注册表(path, key, value))
        {
            this->深浅模式 = (bool) value;
        }
    } ();

    [&] () {
        DWORD value;
        std::wstring path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
        std::wstring key = L"TaskbarAl";
        if (!this->读取注册表(path, key, value))
        {
            if (!this->强制使用设置位置选项)
            {
                this->居中对齐 = (bool) value;
            }
        }
    } ();

    [&] () {
        DWORD value;
        std::wstring path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
        std::wstring key = L"TaskbarDa";
        if (!this->读取注册表(path, key, value))
        {
            this->组件按钮 = (bool) value;
        }
    } ();

    this->更新窗口();
}


void 任务栏歌词类::OnClose()
{
    DestroyWindow(this->hwnd);
}


void 任务栏歌词类::OnDestroy()
{
    PostQuitMessage(0);
}


UINT 任务栏歌词类::DPI(
    UINT pixel
) {
    UINT dpi = GetDpiForWindow(this->任务栏_句柄);
    return pixel * dpi / 96;
}


void 任务栏歌词类::更新窗口()
{
    GetWindowRect(this->任务栏_句柄, &this->任务栏_矩形);
    GetWindowRect(this->开始按钮_句柄, &this->开始按钮_矩形);
    GetWindowRect(this->活动区域_句柄, &this->活动区域_矩形);
    GetWindowRect(this->通知区域_句柄, &this->通知区域_矩形);

    UINT 左;
    UINT 上;
    UINT 宽;
    UINT 高;

    if (this->居中对齐)
    {
        左 = (this->组件按钮 ? this->DPI(160) : 0);
        上 = 0;
        宽 = this->开始按钮_矩形.left - (this->组件按钮 ? this->DPI(160) : 0);
        高 = this->任务栏_矩形.bottom - this->任务栏_矩形.top;
    }
    else
    {
        左 = this->活动区域_矩形.right;
        上 = 0;
        宽 = this->通知区域_矩形.left - this->活动区域_矩形.right - (this->组件按钮 ? this->DPI(160) : 0);
        高 = this->任务栏_矩形.bottom - this->任务栏_矩形.top;
    }


    MoveWindow(this->hwnd, 左, 上, 宽, 高, true);
    InvalidateRect(this->hwnd, nullptr, true);
}


bool 任务栏歌词类::读取注册表(
    std::wstring path,
    std::wstring keyName,
    DWORD& value
) {
    HKEY key;
    DWORD bufferSize;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, path.c_str(), NULL, KEY_READ, &key)) return true;
    if (RegQueryValueEx(key, keyName.c_str(), NULL, NULL, (LPBYTE) &value, &bufferSize)) return true;
    RegCloseKey(key);
    return false;
}


void 任务栏歌词类::剩余宽度检测()
{
    auto 线程函数 = [&] () {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            RECT 任务栏_矩形;
            RECT 开始按钮_矩形;
            RECT 活动区域_矩形;
            RECT 通知区域_矩形;

            GetWindowRect(this->任务栏_句柄, &任务栏_矩形);
            GetWindowRect(this->开始按钮_句柄, &开始按钮_矩形);
            GetWindowRect(this->活动区域_句柄, &活动区域_矩形);
            GetWindowRect(this->通知区域_句柄, &通知区域_矩形);

            if (std::memcmp(&this->任务栏_矩形, &任务栏_矩形, sizeof(RECT)))
            {
                this->任务栏_矩形 = 任务栏_矩形;
                this->更新窗口();
            }

            if (this->居中对齐)
            {
                if (std::memcmp(&this->开始按钮_矩形, &开始按钮_矩形, sizeof(RECT)))
                {
                    this->开始按钮_矩形 = 开始按钮_矩形;
                    this->更新窗口();
                }
            }
            else
            {
                if (std::memcmp(&this->活动区域_矩形, &活动区域_矩形, sizeof(RECT)))
                {
                    this->活动区域_矩形 = 活动区域_矩形;
                    this->更新窗口();
                }
                if (std::memcmp(&this->通知区域_矩形, &通知区域_矩形, sizeof(RECT))) {
                    this->通知区域_矩形 = 通知区域_矩形;
                    this->更新窗口();
                }
            }

        }
    };

    this->剩余宽度检测_线程 = new std::thread(线程函数);
}
