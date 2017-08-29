/*
 * Copyright 2017 André Hentschel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/* NOTE: The guest side uses mingw's headers. The host side uses Wine's headers. */

#include <windows.h>
#include <stdio.h>

#include "windows-user-services.h"
#include "dll_list.h"
#include "user32.h"

struct qemu_set_callbacks
{
    struct qemu_syscall super;
    uint64_t rev_wndproc_wrapper;
    uint64_t wndproc_wrapper;
    uint64_t guest_mod;
    uint64_t guest_win_event_wrapper;
};

struct wndproc_call
{
    uint64_t wndproc;
    uint64_t win, msg, wparam, lparam;
};

#ifdef QEMU_DLL_GUEST

static LRESULT wndproc_wrapper(const struct wndproc_call *call)
{
    WNDPROC proc = (WNDPROC)call->wndproc;

    return proc((HWND)call->win, call->msg, call->wparam, call->lparam);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *reserved)
{
    struct qemu_set_callbacks call;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            call.super.id = QEMU_SYSCALL_ID(CALL_SET_CALLBACKS);
            call.rev_wndproc_wrapper = (uint64_t)reverse_wndproc_func;
            call.wndproc_wrapper = (uint64_t)wndproc_wrapper;
            call.guest_mod = (uint64_t)mod;
            call.guest_win_event_wrapper = (uint64_t)guest_win_event_wrapper;
            qemu_syscall(&call.super);
            break;
    }
    return TRUE;
}

#else

#include <wine/debug.h>
#include <assert.h>

WINE_DEFAULT_DEBUG_CHANNEL(qemu_user32);

uint64_t reverse_wndproc_func;

static void qemu_set_callbacks(struct qemu_syscall *call)
{
    struct qemu_set_callbacks *c = (struct qemu_set_callbacks *)call;
    reverse_wndproc_func = c->rev_wndproc_wrapper;
    guest_wndproc_wrapper = c->wndproc_wrapper;
    guest_mod = (HMODULE)c->guest_mod;
    guest_win_event_wrapper = c->guest_win_event_wrapper;
}

const struct qemu_ops *qemu_ops;

static const syscall_handler dll_functions[] =
{
    qemu_ActivateKeyboardLayout,
    qemu_AddClipboardFormatListener,
    qemu_AdjustWindowRect,
    qemu_AdjustWindowRectEx,
    qemu_AlignRects,
    qemu_AllowSetForegroundWindow,
    qemu_AnimateWindow,
    qemu_AnyPopup,
    qemu_AppendMenuA,
    qemu_AppendMenuW,
    qemu_ArrangeIconicWindows,
    qemu_AttachThreadInput,
    qemu_BeginDeferWindowPos,
    qemu_BeginPaint,
    qemu_BlockInput,
    qemu_BringWindowToTop,
    qemu_BroadcastSystemMessageA,
    qemu_BroadcastSystemMessageExA,
    qemu_BroadcastSystemMessageExW,
    qemu_BroadcastSystemMessageW,
    qemu_CalcChildScroll,
    qemu_call_wndproc,
    qemu_CallMsgFilterA,
    qemu_CallMsgFilterW,
    qemu_CallNextHookEx,
    qemu_CallWindowProcA,
    qemu_CallWindowProcW,
    qemu_CascadeChildWindows,
    qemu_CascadeWindows,
    qemu_ChangeClipboardChain,
    qemu_ChangeDisplaySettingsA,
    qemu_ChangeDisplaySettingsExA,
    qemu_ChangeDisplaySettingsExW,
    qemu_ChangeDisplaySettingsW,
    qemu_ChangeMenuA,
    qemu_ChangeMenuW,
    qemu_ChangeWindowMessageFilter,
    qemu_CharLowerA,
    qemu_CharLowerBuffA,
    qemu_CharLowerBuffW,
    qemu_CharLowerW,
    qemu_CharNextA,
    qemu_CharNextExA,
    qemu_CharNextExW,
    qemu_CharNextW,
    qemu_CharPrevA,
    qemu_CharPrevExA,
    qemu_CharPrevExW,
    qemu_CharPrevW,
    qemu_CharToOemA,
    qemu_CharToOemBuffA,
    qemu_CharToOemBuffW,
    qemu_CharToOemW,
    qemu_CharUpperA,
    qemu_CharUpperBuffA,
    qemu_CharUpperBuffW,
    qemu_CharUpperW,
    qemu_CheckDlgButton,
    qemu_CheckMenuItem,
    qemu_CheckMenuRadioItem,
    qemu_CheckRadioButton,
    qemu_ChildWindowFromPoint,
    qemu_ChildWindowFromPointEx,
    qemu_ClientToScreen,
    qemu_ClipCursor,
    qemu_CloseClipboard,
    qemu_CloseDesktop,
    qemu_CloseWindow,
    qemu_CloseWindowStation,
    qemu_CopyAcceleratorTableA,
    qemu_CopyAcceleratorTableW,
    qemu_CopyIcon,
    qemu_CopyImage,
    qemu_CopyRect,
    qemu_CountClipboardFormats,
    qemu_CreateAcceleratorTableA,
    qemu_CreateAcceleratorTableW,
    qemu_CreateCaret,
    qemu_CreateCursor,
    qemu_CreateDesktopA,
    qemu_CreateDesktopW,
    qemu_CreateDialogIndirectParamA,
    qemu_CreateDialogIndirectParamAorW,
    qemu_CreateDialogIndirectParamW,
    qemu_CreateDialogParamA,
    qemu_CreateDialogParamW,
    qemu_CreateIcon,
    qemu_CreateIconFromResource,
    qemu_CreateIconFromResourceEx,
    qemu_CreateIconIndirect,
    qemu_CreateMDIWindowA,
    qemu_CreateMDIWindowW,
    qemu_CreateMenu,
    qemu_CreatePopupMenu,
    qemu_CreateWindowExA,
    qemu_CreateWindowExW,
    qemu_CreateWindowStationA,
    qemu_CreateWindowStationW,
    qemu_DdeAbandonTransaction,
    qemu_DdeAccessData,
    qemu_DdeAddData,
    qemu_DdeClientTransaction,
    qemu_DdeCmpStringHandles,
    qemu_DdeConnect,
    qemu_DdeConnectList,
    qemu_DdeCreateDataHandle,
    qemu_DdeCreateStringHandleA,
    qemu_DdeCreateStringHandleW,
    qemu_DdeDisconnect,
    qemu_DdeDisconnectList,
    qemu_DdeEnableCallback,
    qemu_DdeFreeDataHandle,
    qemu_DdeFreeStringHandle,
    qemu_DdeGetData,
    qemu_DdeGetLastError,
    qemu_DdeImpersonateClient,
    qemu_DdeInitializeA,
    qemu_DdeInitializeW,
    qemu_DdeKeepStringHandle,
    qemu_DdeNameService,
    qemu_DdePostAdvise,
    qemu_DdeQueryConvInfo,
    qemu_DdeQueryNextServer,
    qemu_DdeQueryStringA,
    qemu_DdeQueryStringW,
    qemu_DdeReconnect,
    qemu_DdeSetQualityOfService,
    qemu_DdeUnaccessData,
    qemu_DdeUninitialize,
    qemu_DefDlgProcA,
    qemu_DefDlgProcW,
    qemu_DeferWindowPos,
    qemu_DefFrameProcA,
    qemu_DefFrameProcW,
    qemu_DefMDIChildProcA,
    qemu_DefMDIChildProcW,
    qemu_DefRawInputProc,
    qemu_DefWindowProcA,
    qemu_DefWindowProcW,
    qemu_DeleteMenu,
    qemu_DeregisterShellHookWindow,
    qemu_DestroyAcceleratorTable,
    qemu_DestroyCaret,
    qemu_DestroyCursor,
    qemu_DestroyIcon,
    qemu_DestroyMenu,
    qemu_DestroyWindow,
    qemu_DialogBoxIndirectParam,
    qemu_DialogBoxIndirectParamAorW,
    qemu_DialogBoxIndirectParam,
    qemu_DialogBoxParam,
    qemu_DialogBoxParam,
    qemu_DisableProcessWindowsGhosting,
    qemu_DispatchMessageA,
    qemu_DispatchMessageW,
    qemu_DlgDirListA,
    qemu_DlgDirListComboBoxA,
    qemu_DlgDirListComboBoxW,
    qemu_DlgDirListW,
    qemu_DlgDirSelectComboBoxExA,
    qemu_DlgDirSelectComboBoxExW,
    qemu_DlgDirSelectExA,
    qemu_DlgDirSelectExW,
    qemu_DrawAnimatedRects,
    qemu_DrawCaption,
    qemu_DrawCaptionTempA,
    qemu_DrawCaptionTempW,
    qemu_DrawEdge,
    qemu_DrawFocusRect,
    qemu_DrawFrameControl,
    qemu_DrawIcon,
    qemu_DrawIconEx,
    qemu_DrawMenuBar,
    qemu_DrawMenuBarTemp,
    qemu_DrawStateA,
    qemu_DrawStateW,
    qemu_DrawTextA,
    qemu_DrawTextExA,
    qemu_DrawTextExW,
    qemu_DrawTextW,
    qemu_EmptyClipboard,
    qemu_EnableMenuItem,
    qemu_EnableScrollBar,
    qemu_EnableWindow,
    qemu_EndDeferWindowPos,
    qemu_EndDialog,
    qemu_EndMenu,
    qemu_EndPaint,
    qemu_EnumChildWindows,
    qemu_EnumClipboardFormats,
    qemu_EnumDesktopsA,
    qemu_EnumDesktopsW,
    qemu_EnumDesktopWindows,
    qemu_EnumDisplayDevicesA,
    qemu_EnumDisplayDevicesW,
    qemu_EnumDisplayMonitors,
    qemu_EnumDisplaySettingsA,
    qemu_EnumDisplaySettingsExA,
    qemu_EnumDisplaySettingsExW,
    qemu_EnumDisplaySettingsW,
    qemu_EnumPropsA,
    qemu_EnumPropsExA,
    qemu_EnumPropsExW,
    qemu_EnumPropsW,
    qemu_EnumThreadWindows,
    qemu_EnumWindows,
    qemu_EnumWindowStationsA,
    qemu_EnumWindowStationsW,
    qemu_EqualRect,
    qemu_ExcludeUpdateRgn,
    qemu_ExitWindowsEx,
    qemu_FillRect,
    qemu_FindWindowA,
    qemu_FindWindowExA,
    qemu_FindWindowExW,
    qemu_FindWindowW,
    qemu_FlashWindow,
    qemu_FlashWindowEx,
    qemu_FrameRect,
    qemu_FreeDDElParam,
    qemu_GetActiveWindow,
    qemu_GetAltTabInfoA,
    qemu_GetAltTabInfoW,
    qemu_GetAncestor,
    qemu_GetAppCompatFlags,
    qemu_GetAppCompatFlags2,
    qemu_GetAsyncKeyState,
    qemu_GetCapture,
    qemu_GetCaretBlinkTime,
    qemu_GetCaretPos,
    qemu_GetClassInfoA,
    qemu_GetClassInfoExA,
    qemu_GetClassInfoExW,
    qemu_GetClassInfoW,
    qemu_GetClassLongA,
    qemu_GetClassLongPtrA,
    qemu_GetClassLongPtrW,
    qemu_GetClassLongW,
    qemu_GetClassNameA,
    qemu_GetClassNameW,
    qemu_GetClassWord,
    qemu_GetClientRect,
    qemu_GetClipboardData,
    qemu_GetClipboardFormatNameA,
    qemu_GetClipboardFormatNameW,
    qemu_GetClipboardOwner,
    qemu_GetClipboardSequenceNumber,
    qemu_GetClipboardViewer,
    qemu_GetClipCursor,
    qemu_GetComboBoxInfo,
    qemu_GetCursor,
    qemu_GetCursorFrameInfo,
    qemu_GetCursorInfo,
    qemu_GetCursorPos,
    qemu_GetDC,
    qemu_GetDCEx,
    qemu_GetDesktopWindow,
    qemu_GetDialogBaseUnits,
    qemu_GetDisplayConfigBufferSizes,
    qemu_GetDlgCtrlID,
    qemu_GetDlgItem,
    qemu_GetDlgItemInt,
    qemu_GetDlgItemTextA,
    qemu_GetDlgItemTextW,
    qemu_GetDoubleClickTime,
    qemu_GetFocus,
    qemu_GetForegroundWindow,
    qemu_GetGestureConfig,
    qemu_GetGuiResources,
    qemu_GetGUIThreadInfo,
    qemu_GetIconInfo,
    qemu_GetIconInfoExA,
    qemu_GetIconInfoExW,
    qemu_GetInputState,
    qemu_GetInternalWindowPos,
    qemu_GetKBCodePage,
    qemu_GetKeyboardLayout,
    qemu_GetKeyboardLayoutList,
    qemu_GetKeyboardLayoutNameA,
    qemu_GetKeyboardLayoutNameW,
    qemu_GetKeyboardState,
    qemu_GetKeyboardType,
    qemu_GetKeyNameTextA,
    qemu_GetKeyNameTextW,
    qemu_GetKeyState,
    qemu_GetLastActivePopup,
    qemu_GetLastInputInfo,
    qemu_GetLayeredWindowAttributes,
    qemu_GetListBoxInfo,
    qemu_GetMenu,
    qemu_GetMenuBarInfo,
    qemu_GetMenuContextHelpId,
    qemu_GetMenuDefaultItem,
    qemu_GetMenuInfo,
    qemu_GetMenuItemCount,
    qemu_GetMenuItemID,
    qemu_GetMenuItemInfoA,
    qemu_GetMenuItemInfoW,
    qemu_GetMenuItemRect,
    qemu_GetMenuState,
    qemu_GetMenuStringA,
    qemu_GetMenuStringW,
    qemu_GetMessageA,
    qemu_GetMessageExtraInfo,
    qemu_GetMessagePos,
    qemu_GetMessageTime,
    qemu_GetMessageW,
    qemu_GetMonitorInfoA,
    qemu_GetMonitorInfoW,
    qemu_GetMouseMovePointsEx,
    qemu_GetNextDlgGroupItem,
    qemu_GetNextDlgTabItem,
    qemu_GetOpenClipboardWindow,
    qemu_GetParent,
    qemu_GetPhysicalCursorPos,
    qemu_GetPriorityClipboardFormat,
    qemu_GetProcessDefaultLayout,
    qemu_GetProcessWindowStation,
    qemu_GetProgmanWindow,
    qemu_GetPropA,
    qemu_GetPropW,
    qemu_GetQueueStatus,
    qemu_GetRawInputBuffer,
    qemu_GetRawInputData,
    qemu_GetRawInputDeviceInfoA,
    qemu_GetRawInputDeviceInfoW,
    qemu_GetRawInputDeviceList,
    qemu_GetRegisteredRawInputDevices,
    qemu_GetScrollBarInfo,
    qemu_GetScrollInfo,
    qemu_GetScrollPos,
    qemu_GetScrollRange,
    qemu_GetShellWindow,
    qemu_GetSubMenu,
    qemu_GetSysColor,
    qemu_GetSysColorBrush,
    qemu_GetSystemMenu,
    qemu_GetSystemMetrics,
    qemu_GetTabbedTextExtentA,
    qemu_GetTabbedTextExtentW,
    qemu_GetTaskmanWindow,
    qemu_GetThreadDesktop,
    qemu_GetTitleBarInfo,
    qemu_GetTopWindow,
    qemu_GetUpdatedClipboardFormats,
    qemu_GetUpdateRect,
    qemu_GetUpdateRgn,
    qemu_GetUserObjectInformationA,
    qemu_GetUserObjectInformationW,
    qemu_GetUserObjectSecurity,
    qemu_GetWindow,
    qemu_GetWindowContextHelpId,
    qemu_GetWindowDC,
    qemu_GetWindowInfo,
    qemu_GetWindowLongA,
    qemu_GetWindowLongPtrA,
    qemu_GetWindowLongPtrW,
    qemu_GetWindowLongW,
    qemu_GetWindowModuleFileNameA,
    qemu_GetWindowModuleFileNameW,
    qemu_GetWindowPlacement,
    qemu_GetWindowRect,
    qemu_GetWindowRgn,
    qemu_GetWindowRgnBox,
    qemu_GetWindowTextA,
    qemu_GetWindowTextLengthA,
    qemu_GetWindowTextLengthW,
    qemu_GetWindowTextW,
    qemu_GetWindowThreadProcessId,
    qemu_GetWindowWord,
    qemu_GrayStringA,
    qemu_GrayStringW,
    qemu_HideCaret,
    qemu_HiliteMenuItem,
    qemu_ImpersonateDdeClientWindow,
    qemu_InflateRect,
    qemu_InSendMessage,
    qemu_InSendMessageEx,
    qemu_InsertMenuA,
    qemu_InsertMenuItemA,
    qemu_InsertMenuItemW,
    qemu_InsertMenuW,
    qemu_InternalGetWindowText,
    qemu_IntersectRect,
    qemu_InvalidateRect,
    qemu_InvalidateRgn,
    qemu_InvertRect,
    qemu_IsCharAlphaA,
    qemu_IsCharAlphaNumericA,
    qemu_IsCharAlphaNumericW,
    qemu_IsCharAlphaW,
    qemu_IsCharLowerA,
    qemu_IsCharLowerW,
    qemu_IsCharUpperA,
    qemu_IsCharUpperW,
    qemu_IsChild,
    qemu_IsClipboardFormatAvailable,
    qemu_IsDialogMessageA,
    qemu_IsDialogMessageW,
    qemu_IsDlgButtonChecked,
    qemu_IsGUIThread,
    qemu_IsHungAppWindow,
    qemu_IsIconic,
    qemu_IsMenu,
    qemu_IsProcessDPIAware,
    qemu_IsRectEmpty,
    qemu_IsTouchWindow,
    qemu_IsWindow,
    qemu_IsWindowEnabled,
    qemu_IsWindowRedirectedForPrint,
    qemu_IsWindowUnicode,
    qemu_IsWindowVisible,
    qemu_IsWinEventHookInstalled,
    qemu_IsZoomed,
    qemu_keybd_event,
    qemu_KillSystemTimer,
    qemu_KillTimer,
    qemu_LoadAcceleratorsA,
    qemu_LoadAcceleratorsW,
    qemu_LoadBitmapA,
    qemu_LoadBitmapW,
    qemu_LoadCursorA,
    qemu_LoadCursorFromFileA,
    qemu_LoadCursorFromFileW,
    qemu_LoadCursorW,
    qemu_LoadIconA,
    qemu_LoadIconW,
    qemu_LoadImageA,
    qemu_LoadImageW,
    qemu_LoadKeyboardLayoutA,
    qemu_LoadKeyboardLayoutW,
    qemu_LoadLocalFonts,
    qemu_LoadMenuA,
    qemu_LoadMenuIndirectA,
    qemu_LoadMenuIndirectW,
    qemu_LoadMenuW,
    qemu_LoadStringA,
    qemu_LoadStringW,
    qemu_LockSetForegroundWindow,
    qemu_LockWindowUpdate,
    qemu_LockWorkStation,
    qemu_LogicalToPhysicalPoint,
    qemu_LookupIconIdFromDirectory,
    qemu_LookupIconIdFromDirectoryEx,
    qemu_MapDialogRect,
    qemu_MapVirtualKeyA,
    qemu_MapVirtualKeyExA,
    qemu_MapVirtualKeyExW,
    qemu_MapVirtualKeyW,
    qemu_MapWindowPoints,
    qemu_MessageBeep,
    qemu_MessageBoxA,
    qemu_MessageBoxExA,
    qemu_MessageBoxExW,
    qemu_MessageBoxIndirectA,
    qemu_MessageBoxIndirectW,
    qemu_MessageBoxTimeoutA,
    qemu_MessageBoxTimeoutW,
    qemu_MessageBoxW,
    qemu_ModifyMenuA,
    qemu_ModifyMenuW,
    qemu_MonitorFromPoint,
    qemu_MonitorFromRect,
    qemu_MonitorFromWindow,
    qemu_mouse_event,
    qemu_MoveWindow,
    qemu_MsgWaitForMultipleObjects,
    qemu_MsgWaitForMultipleObjectsEx,
    qemu_NotifyWinEvent,
    qemu_OemKeyScan,
    qemu_OemToCharA,
    qemu_OemToCharBuffA,
    qemu_OemToCharBuffW,
    qemu_OemToCharW,
    qemu_OffsetRect,
    qemu_OpenClipboard,
    qemu_OpenDesktopA,
    qemu_OpenDesktopW,
    qemu_OpenIcon,
    qemu_OpenInputDesktop,
    qemu_OpenWindowStationA,
    qemu_OpenWindowStationW,
    qemu_PackDDElParam,
    qemu_PaintDesktop,
    qemu_PeekMessageA,
    qemu_PeekMessageW,
    qemu_PhysicalToLogicalPoint,
    qemu_PostMessageA,
    qemu_PostMessageW,
    qemu_PostQuitMessage,
    qemu_PostThreadMessageA,
    qemu_PostThreadMessageW,
    qemu_PrintWindow,
    qemu_PrivateExtractIconExA,
    qemu_PrivateExtractIconExW,
    qemu_PrivateExtractIconsA,
    qemu_PrivateExtractIconsW,
    qemu_PtInRect,
    qemu_QueryDisplayConfig,
    qemu_RealChildWindowFromPoint,
    qemu_RealGetWindowClassA,
    qemu_RealGetWindowClassW,
    qemu_RedrawWindow,
    qemu_RegisterClassEx,
    qemu_RegisterClassEx,
    qemu_RegisterClipboardFormatA,
    qemu_RegisterClipboardFormatW,
    qemu_RegisterDeviceNotificationA,
    qemu_RegisterDeviceNotificationW,
    qemu_RegisterHotKey,
    qemu_RegisterLogonProcess,
    qemu_RegisterPowerSettingNotification,
    qemu_RegisterRawInputDevices,
    qemu_RegisterServicesProcess,
    qemu_RegisterShellHookWindow,
    qemu_RegisterSystemThread,
    qemu_RegisterTasklist,
    qemu_RegisterTouchWindow,
    qemu_RegisterWindowMessageA,
    qemu_RegisterWindowMessageW,
    qemu_ReleaseCapture,
    qemu_ReleaseDC,
    qemu_RemoveClipboardFormatListener,
    qemu_RemoveMenu,
    qemu_RemovePropA,
    qemu_RemovePropW,
    qemu_ReplyMessage,
    qemu_ReuseDDElParam,
    qemu_ScreenToClient,
    qemu_ScrollChildren,
    qemu_ScrollDC,
    qemu_ScrollWindow,
    qemu_ScrollWindowEx,
    qemu_SendDlgItemMessageA,
    qemu_SendDlgItemMessageW,
    qemu_SendIMEMessageExA,
    qemu_SendIMEMessageExW,
    qemu_SendInput,
    qemu_SendMessageA,
    qemu_SendMessageCallbackA,
    qemu_SendMessageCallbackW,
    qemu_SendMessageTimeoutA,
    qemu_SendMessageTimeoutW,
    qemu_SendMessageW,
    qemu_SendNotifyMessageA,
    qemu_SendNotifyMessageW,
    qemu_set_callbacks,
    qemu_SetActiveWindow,
    qemu_SetCapture,
    qemu_SetCaretBlinkTime,
    qemu_SetCaretPos,
    qemu_SetClassLongA,
    qemu_SetClassLongPtrA,
    qemu_SetClassLongPtrW,
    qemu_SetClassLongW,
    qemu_SetClassWord,
    qemu_SetClipboardData,
    qemu_SetClipboardViewer,
    qemu_SetCoalescableTimer,
    qemu_SetCursor,
    qemu_SetCursorPos,
    qemu_SetDebugErrorLevel,
    qemu_SetDeskWallPaper,
    qemu_SetDlgItemInt,
    qemu_SetDlgItemTextA,
    qemu_SetDlgItemTextW,
    qemu_SetDoubleClickTime,
    qemu_SetFocus,
    qemu_SetForegroundWindow,
    qemu_SetGestureConfig,
    qemu_SetInternalWindowPos,
    qemu_SetKeyboardState,
    qemu_SetLastErrorEx,
    qemu_SetLayeredWindowAttributes,
    qemu_SetLogonNotifyWindow,
    qemu_SetMenu,
    qemu_SetMenuContextHelpId,
    qemu_SetMenuDefaultItem,
    qemu_SetMenuInfo,
    qemu_SetMenuItemBitmaps,
    qemu_SetMenuItemInfoA,
    qemu_SetMenuItemInfoW,
    qemu_SetMessageExtraInfo,
    qemu_SetMessageQueue,
    qemu_SetParent,
    qemu_SetPhysicalCursorPos,
    qemu_SetProcessDefaultLayout,
    qemu_SetProcessDPIAware,
    qemu_SetProcessWindowStation,
    qemu_SetProgmanWindow,
    qemu_SetPropA,
    qemu_SetPropW,
    qemu_SetRect,
    qemu_SetRectEmpty,
    qemu_SetScrollInfo,
    qemu_SetScrollPos,
    qemu_SetScrollRange,
    qemu_SetShellWindow,
    qemu_SetShellWindowEx,
    qemu_SetSysColors,
    qemu_SetSysColorsTemp,
    qemu_SetSystemCursor,
    qemu_SetSystemMenu,
    qemu_SetSystemTimer,
    qemu_SetTaskmanWindow,
    qemu_SetThreadDesktop,
    qemu_SetTimer,
    qemu_SetUserObjectInformationA,
    qemu_SetUserObjectInformationW,
    qemu_SetUserObjectSecurity,
    qemu_SetWindowContextHelpId,
    qemu_SetWindowLongA,
    qemu_SetWindowLongPtrA,
    qemu_SetWindowLongPtrW,
    qemu_SetWindowLongW,
    qemu_SetWindowPlacement,
    qemu_SetWindowPos,
    qemu_SetWindowRgn,
    qemu_SetWindowsHookA,
    qemu_SetWindowsHookExA,
    qemu_SetWindowsHookExW,
    qemu_SetWindowsHookW,
    qemu_SetWindowStationUser,
    qemu_SetWindowTextA,
    qemu_SetWindowTextW,
    qemu_SetWindowWord,
    qemu_SetWinEventHook,
    qemu_ShowCaret,
    qemu_ShowCursor,
    qemu_ShowOwnedPopups,
    qemu_ShowScrollBar,
    qemu_ShowWindow,
    qemu_ShowWindowAsync,
    qemu_ShutdownBlockReasonCreate,
    qemu_ShutdownBlockReasonDestroy,
    qemu_SubtractRect,
    qemu_SwapMouseButton,
    qemu_SwitchDesktop,
    qemu_SwitchToThisWindow,
    qemu_SystemParametersInfoA,
    qemu_SystemParametersInfoW,
    qemu_TabbedTextOutA,
    qemu_TabbedTextOutW,
    qemu_TileChildWindows,
    qemu_TileWindows,
    qemu_ToAscii,
    qemu_ToAsciiEx,
    qemu_ToUnicode,
    qemu_ToUnicodeEx,
    qemu_TrackMouseEvent,
    qemu_TrackPopupMenu,
    qemu_TrackPopupMenuEx,
    qemu_TranslateAcceleratorA,
    qemu_TranslateAcceleratorW,
    qemu_TranslateMDISysAccel,
    qemu_TranslateMessage,
    qemu_UnhookWindowsHook,
    qemu_UnhookWindowsHookEx,
    qemu_UnhookWinEvent,
    qemu_UnionRect,
    qemu_UnloadKeyboardLayout,
    qemu_UnpackDDElParam,
    qemu_UnregisterClass,
    qemu_UnregisterClass,
    qemu_UnregisterDeviceNotification,
    qemu_UnregisterHotKey,
    qemu_UnregisterPowerSettingNotification,
    qemu_UpdateLayeredWindow,
    qemu_UpdateLayeredWindowIndirect,
    qemu_UpdateWindow,
    qemu_User32InitializeImmEntryTable,
    qemu_UserHandleGrantAccess,
    qemu_UserRealizePalette,
    qemu_UserSignalProc,
    qemu_ValidateRect,
    qemu_ValidateRgn,
    qemu_VkKeyScanA,
    qemu_VkKeyScanExA,
    qemu_VkKeyScanExW,
    qemu_VkKeyScanW,
    qemu_WaitForInputIdle,
    qemu_WaitMessage,
    qemu_WindowFromDC,
    qemu_WindowFromPoint,
    qemu_WinHelpA,
    qemu_WinHelpW,
    qemu_WINNLSEnableIME,
    qemu_WINNLSGetEnableStatus,
    qemu_WINNLSGetIMEHotkey,
};

struct wndproc_wrapper *wndproc_wrappers;
unsigned int wndproc_wrapper_count;
uint64_t guest_wndproc_wrapper;

LRESULT WINAPI wndproc_wrapper(HWND win, UINT msg, WPARAM wparam, LPARAM lparam, struct wndproc_wrapper *wrapper)
{
    struct wndproc_call call;
    MSG msg_struct = {win, msg, wparam, lparam};

    msg_host_to_guest(&msg_struct, &msg_struct);

    call.wndproc = wrapper->guest_proc;
    call.win = (uint64_t)msg_struct.hwnd;
    call.msg = msg_struct.message;
    call.wparam = msg_struct.wParam;
    call.lparam = msg_struct.lParam;

    WINE_TRACE("Calling guest wndproc 0x%lx(%p, %x, %lx, %lx)\n", wrapper->guest_proc, win, msg, wparam, lparam);
    WINE_TRACE("wrapper at %p\n", wrapper);
    return qemu_ops->qemu_execute(QEMU_G2H(guest_wndproc_wrapper), QEMU_H2G(&call));
}

void init_wndproc(struct wndproc_wrapper *wrapper)
{
    size_t offset;

    offset = offsetof(struct wndproc_wrapper, selfptr) - offsetof(struct wndproc_wrapper, ldrx4);
    /* The load offset is stored in bits 5-24. The stored offset is left-shifted by 2 to generate the 21
     * bit real offset. So to place it in the right place we need our offset (multiple of 4, unless the
     * compiler screwed up terribly) shifted by another 3 bits. */
    wrapper->ldrx4 = 0x58000004 | (offset << 3); /* ldr x4, offset */

    offset = offsetof(struct wndproc_wrapper, host_proc) - offsetof(struct wndproc_wrapper, ldrx5);
    wrapper->ldrx5 = 0x58000005 | (offset << 3);   /* ldr x5, offset */

    wrapper->br = 0xd61f00a0; /* br x5 */

    wrapper->selfptr = wrapper;
    wrapper->host_proc = wndproc_wrapper;
    wrapper->guest_proc = 0;

    __clear_cache(&wrapper->ldrx4, &wrapper->br + 1);
}

/* This is most likely dead code. It will only be triggered if Wine runs out of WNDPROC handles, 
 * then a Wine module uses a not-yet-used WNDPROC and the application retrieves this WNDPROC with
 * GetWindowLongPtr or friends. The code has been used previously when WNDPROCs were handled in a
 * different way, so it received some testing before becoming a really rare corner case. */
struct reverse_wndproc_wrapper
        reverse_wndproc_wrappers[REVERSE_WNDPROC_WRAPPER_COUNT];

void init_reverse_wndproc(struct reverse_wndproc_wrapper *wrapper)
{
    /* This is a bit more complicated than the ARM counterpart because we do not
     * have a spare argument in register to put our self pointer in. This means we
     * have to shuffle push the argument on the stack and do an actual call on top
     * of it, and then clean up the stack on return.
     *
     * The code is generated from this C code:
     * LRESULT WINAPI wndproc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
     * {
     *     LRESULT (* WINAPI wndproc2)(HWND wnd, UINT msg, WPARAM wp, LPARAM lp, void *extra);
     *     wndproc2 = *(void **)((ULONG_PTR)wndproc + 0x28);
     *     return wndproc2(wnd, msg, wp, lp, wndproc);
     * }
     **/
    static const char wrapper_code[] =
    {
        0x48, 0x83, 0xec, 0x38,                     /* sub    $0x38,%rsp        - Reserve stack space   */
        0x48, 0x8d, 0x05, 0xf5, 0xff, 0xff, 0xff,   /* lea    -0xb(%rip),%rax   - Get self ptr          */
        0x48, 0x89, 0x44, 0x24, 0x20,               /* mov    %rax,0x20(%rsp)   - push self ptr         */
        0xff, 0x15, 0x0a, 0x00, 0x00, 0x00,         /* callq  *0xa(%rip)        - Call guest func       */
        0x48, 0x83, 0xc4, 0x38,                     /* add    $0x38,%rsp        - Clean up stack        */
        0xc3,                                       /* retq                     - return                */
    };

    memset(wrapper->code, 0xcc, sizeof(wrapper->code));
    memcpy(wrapper->code, wrapper_code, sizeof(wrapper_code));
}

const WINAPI syscall_handler *qemu_dll_register(const struct qemu_ops *ops, uint32_t *dll_num)
{
    unsigned int i;
    LRESULT ret;

    WINE_TRACE("Loading host-side user32 wrapper.\n");

    qemu_ops = ops;
    *dll_num = QEMU_CURRENT_DLL;

    /* Wine has 16384 WNDPROC slots and if those are full it falls back to calling the
     * function pointer directly. The tests exercise the fallback code, so allocate more
     * slots than Wine has handles.
     *
     * WNDPROC wrappers are never freed, just as WNDPROC handles are never freed. The
     * basic idea is that an application may create and destroy many Windows, but it won't
     * have an infinite supply of code to use as possible WNDPROCs. WNDPROC handles are
     * expected to be valid after the Window that 'created' them was destroyed.
     *
     * This array uses 800 kilobytes of memory :-\ . */
    wndproc_wrapper_count = 20 * 1024;
    wndproc_wrappers = VirtualAlloc(NULL, sizeof(*wndproc_wrappers) * wndproc_wrapper_count,
            MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!wndproc_wrappers)
    {
        WINE_ERR("Failed to allocate memory for class wndproc wrappers.\n");
        return NULL;
    }

    for (i = 0; i < wndproc_wrapper_count; ++i)
        init_wndproc(&wndproc_wrappers[i]);

    for (i = 0; i < REVERSE_WNDPROC_WRAPPER_COUNT; ++i)
        init_reverse_wndproc(&reverse_wndproc_wrappers[i]);

    user32_tls = TlsAlloc();
    if (user32_tls == TLS_OUT_OF_INDEXES)
        WINE_ERR("Out of TLS indices\n");

    return dll_functions;
}

WNDPROC wndproc_guest_to_host(uint64_t guest_proc)
{
    unsigned int i;

    if (!guest_proc || wndproc_is_handle(guest_proc))
        return (WNDPROC)guest_proc;

    if (guest_proc >= (ULONG_PTR)&reverse_wndproc_wrappers[0]
            && guest_proc <= (ULONG_PTR)&reverse_wndproc_wrappers[REVERSE_WNDPROC_WRAPPER_COUNT])
    {
        struct reverse_wndproc_wrapper *rev_wrapper = (struct reverse_wndproc_wrapper *)guest_proc;

        WINE_TRACE("Guest passed in reverse wrapper %p, returning host function %p.\n",
                rev_wrapper, rev_wrapper->host_proc);
        return rev_wrapper->host_proc;
    }

    for (i = 0; i < wndproc_wrapper_count; i++)
    {
        if (wndproc_wrappers[i].guest_proc == guest_proc)
            return (WNDPROC)&wndproc_wrappers[i];
        if (!wndproc_wrappers[i].guest_proc)
        {
            WINE_TRACE("Creating host WNDPROC %p for guest func 0x%lx.\n",
                    &wndproc_wrappers[i], guest_proc);
            wndproc_wrappers[i].guest_proc = guest_proc;
            return (WNDPROC)&wndproc_wrappers[i];
        }
    }

    WINE_FIXME("Out of guest -> host WNDPROC wrappers.\n");
    assert(0);
}

uint64_t wndproc_host_to_guest(WNDPROC host_proc)
{
    unsigned int i;

    if (!host_proc || wndproc_is_handle((LONG_PTR)host_proc))
        return (uint64_t)host_proc;

    if ((ULONG_PTR)host_proc >= (ULONG_PTR)&wndproc_wrappers[0]
            && (ULONG_PTR)host_proc <= (ULONG_PTR)&wndproc_wrappers[wndproc_wrapper_count])
    {
        struct wndproc_wrapper *wrapper = (struct wndproc_wrapper *)host_proc;
        WINE_TRACE("Host wndproc %p is a wrapper function. Returning guest wndproc 0x%lx.\n",
                wrapper, wrapper->guest_proc);
        return wrapper->guest_proc;
    }

    for (i = 0; i < REVERSE_WNDPROC_WRAPPER_COUNT; ++i)
    {
        if (reverse_wndproc_wrappers[i].host_proc == host_proc)
        {
            WINE_TRACE("Allocated reverse WNDPROC wrapper %p for host func %p.\n",
                    &reverse_wndproc_wrappers[i], host_proc);
            return QEMU_H2G(&reverse_wndproc_wrappers[i]);
        }
        if (!reverse_wndproc_wrappers[i].host_proc)
        {
            reverse_wndproc_wrappers[i].host_proc = host_proc;
            reverse_wndproc_wrappers[i].guest_proc = reverse_wndproc_func;
            return QEMU_H2G(&reverse_wndproc_wrappers[i]);
        }
    }

    /* Out of reverse wrappers. */
    assert(0);
}

void msg_guest_to_host(MSG *msg_out, const MSG *msg_in)
{
    memcpy(msg_out, msg_in, sizeof(*msg_out));

    switch (msg_in->message)
    {
        case WM_TIMER:
        case WM_SYSTIMER:
            msg_out->lParam = (LPARAM)wndproc_guest_to_host(msg_in->lParam);
            break;

        default:
            break;
    }
}

void msg_host_to_guest(MSG *msg_out, const MSG *msg_in)
{
    memcpy(msg_out, msg_in, sizeof(*msg_out));

    switch (msg_in->message)
    {
        case WM_TIMER:
        case WM_SYSTIMER:
            msg_out->lParam = wndproc_host_to_guest((WNDPROC)msg_in->lParam);
            break;

        default:
            break;
    }
}

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            wrapper_mod = mod;
            host_mod = GetModuleHandleA("user32.dll");

            wine_rb_init(&win_event_tree, win_event_compare);
            break;

        default:
            break;
    }
}

#endif
