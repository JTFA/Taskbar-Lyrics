﻿#include "DrawWindow.hpp"

#pragma comment (lib, "Gdiplus.lib")


绘制窗口类::绘制窗口类(
    HWND* 窗口句柄
) {
    this->窗口句柄 = 窗口句柄;

    this->任务栏_句柄 = FindWindow(L"Shell_TrayWnd", NULL);
    this->通知区域_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"TrayNotifyWnd", NULL);
    this->开始按钮_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"Start", NULL);
    HWND 最小化区域_句柄 = FindWindowEx(this->任务栏_句柄, NULL, L"ReBarWindow32", NULL);
    this->活动区域_句柄 = FindWindowEx(最小化区域_句柄, NULL, L"MSTaskSwWClass", NULL);
}


void 绘制窗口类::更新窗口()
{
    GetWindowRect(this->任务栏_句柄, &this->任务栏_矩形);
    GetWindowRect(this->通知区域_句柄, &this->通知区域_矩形);
    GetWindowRect(this->开始按钮_句柄, &this->开始按钮_矩形);
    GetWindowRect(this->活动区域_句柄, &this->活动区域_矩形);

    UINT 左, 上, 宽, 高;

    if (this->居中对齐)
    {
        左 = static_cast<UINT>(this->组件按钮 ? this->DPI(160) : 0);
        上 = 0;
        宽 = this->开始按钮_矩形.left - static_cast<UINT>(this->组件按钮 ? this->DPI(160) : 0);
        高 = this->任务栏_矩形.bottom - this->任务栏_矩形.top;
    }
    else
    {
        左 = this->活动区域_矩形.right;
        上 = 0;
        宽 = this->通知区域_矩形.left - this->活动区域_矩形.right;
        高 = this->任务栏_矩形.bottom - this->任务栏_矩形.top;
    }

    MoveWindow(*this->窗口句柄, 左, 上, 宽, 高, false);
    this->绘制窗口(左, 上, 宽, 高);
}


void 绘制窗口类::绘制窗口(
    UINT 左,
    UINT 上,
    UINT 宽,
    UINT 高
)
{
    HDC hdc = GetDC(*this->窗口句柄);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, 宽, 高);
    HBITMAP oldBitmap = HBITMAP(SelectObject(memDC, memBitmap));

    this->绘制歌词(memDC);

    BLENDFUNCTION blend;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;

    POINT 目标位置 = { 左, 上 };
    SIZE 大小 = { 宽, 高 };
    POINT 来源位置 = { 0, 0 };

    UpdateLayeredWindow(*this->窗口句柄, hdc, &目标位置, &大小, memDC, &来源位置, 0, &blend, ULW_ALPHA);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    ReleaseDC(*this->窗口句柄, hdc);
}


void 绘制窗口类::绘制歌词(
    HDC &hdc
) {
    RECT rect;
    GetClientRect(*this->窗口句柄, &rect);

    Graphics graphics(hdc);
    graphics.Clear(Color(0, 0, 0, 0));
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    #ifdef _DEBUG
    Pen myPen(Color(255, 255, 255), 1);
    Rect myRect(rect.left, rect.top, rect.right - 1, rect.bottom - 1);
    graphics.DrawRectangle(&myPen, myRect);
    #endif

    FontFamily fontFamily(this->字体名称.c_str());
    StringFormat stringFormat;
    stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);

    SolidBrush 画笔_基本歌词(this->深浅模式 ? this->字体颜色_浅色_基本歌词 : this->字体颜色_深色_基本歌词);
    SolidBrush 画笔_扩展歌词(this->深浅模式 ? this->字体颜色_浅色_扩展歌词 : this->字体颜色_深色_扩展歌词);

    if (this->扩展歌词.empty())
    {
        Font font(&fontFamily, this->DPI(20), FontStyleRegular, UnitPixel);
        RectF 基本歌词_矩形(this->DPI(10), this->DPI(10), rect.right - this->DPI(20), rect.bottom - this->DPI(20));
        stringFormat.SetLineAlignment(StringAlignmentCenter);
        stringFormat.SetAlignment(this->对齐方式_基本歌词);
        graphics.DrawString(this->基本歌词.c_str(), this->基本歌词.size(), &font, 基本歌词_矩形, &stringFormat, &画笔_基本歌词);

        #ifdef _DEBUG
        graphics.DrawRectangle(&myPen, 基本歌词_矩形);
        #endif
    }
    else
    {
        Font font(&fontFamily, this->DPI(16), FontStyleRegular, UnitPixel);

        stringFormat.SetLineAlignment(StringAlignmentNear);
        stringFormat.SetAlignment(this->对齐方式_基本歌词);
        RectF 基本歌词_矩形(this->DPI(3), this->DPI(3), rect.right - this->DPI(6), rect.bottom / 2 - this->DPI(3));
        graphics.DrawString(this->基本歌词.c_str(), this->基本歌词.size(), &font, 基本歌词_矩形, &stringFormat, &画笔_基本歌词);

        stringFormat.SetLineAlignment(StringAlignmentFar);
        stringFormat.SetAlignment(this->对齐方式_扩展歌词);
        RectF 扩展歌词_矩形(this->DPI(3), rect.bottom / 2, rect.right - this->DPI(6), rect.bottom / 2 - this->DPI(3));
        graphics.DrawString(this->扩展歌词.c_str(), this->扩展歌词.size(), &font, 扩展歌词_矩形, &stringFormat, &画笔_扩展歌词);

        #ifdef _DEBUG
        graphics.DrawRectangle(&myPen, 基本歌词_矩形);
        graphics.DrawRectangle(&myPen, 扩展歌词_矩形);
        #endif
    }
}


REAL 绘制窗口类::DPI(
    UINT 像素大小
) {
    auto 屏幕DPI = GetDpiForWindow(*this->窗口句柄);
    auto 新像素大小 = static_cast<REAL>(像素大小 * 屏幕DPI / 96);
    return 新像素大小;
}