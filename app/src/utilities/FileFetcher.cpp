#include "utilities/FileFetcher.hpp"
#include "Windows.h"
#include "commdlg.h"

// 文件获取器实现文件
// 提供Windows文件选择对话框功能

namespace NRenderer
{
    using namespace std;

    // 打开文件选择对话框
    // filter: 文件过滤器字符串，如 "All(*.obj *.scn)\0 *.obj;*.scn\0"
    // 返回值: 选中文件的路径，如果取消则返回nullopt
    optional<string> FileFetcher::fetch(const char* filter) const
    {
        OPENFILENAME ofn;              // 通用对话框结构体
        TCHAR szFile[260];             // 文件名缓冲区
        HWND hwnd = GetActiveWindow(); // 所有者窗口句柄
        HANDLE hf;                     // 文件句柄

        // 初始化OPENFILENAME结构体
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        
        // 将lpstrFile[0]设置为'\0'，这样GetOpenFileName不会使用szFile的内容进行初始化
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        
        // 设置文件过滤器
        // 示例: "All(*.obj *.scn)\0 *.obj;*.scn\0.obj file\0*.obj\0custom scene file\0*.scn\0"
        ofn.lpstrFilter = (filter);
        ofn.nFilterIndex = 1;          // 默认选择第一个过滤器
        
        // 其他设置
        ofn.lpstrFileTitle = NULL;     // 不需要文件标题
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;    // 使用默认目录
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  // 文件和路径必须存在

        // 显示对话框并处理结果
        if (GetOpenFileName(&ofn) == TRUE)
        {
            string filePath{(char *)ofn.lpstrFile};  // 获取选中的文件路径
            return filePath;                         // 返回文件路径
        }
        else {
            return nullopt;                          // 用户取消，返回nullopt
        }
    }
}