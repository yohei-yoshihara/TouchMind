#include "stdafx.h"
#include "touchmind/touch/AbstractManipulationHelper.h"
#include "touchmind/touch/TouchUtil.h"

void touchmind::touch::TouchUtil::DumpGestureConfig(HWND hWnd) {
  const static size_t GC_SIZE = 5;
  GESTURECONFIG gc[GC_SIZE];
  UINT uiGcs = GC_SIZE;
  ZeroMemory(&gc, sizeof(gc));
  gc[0].dwID = GID_ZOOM;
  gc[1].dwID = GID_PAN;
  gc[2].dwID = GID_ROTATE;
  gc[3].dwID = GID_TWOFINGERTAP;
  gc[4].dwID = GID_PRESSANDTAP;
  BOOL bResult = GetGestureConfig(hWnd, 0, 0, &uiGcs, gc, sizeof(GESTURECONFIG));
  if (!bResult) {
    LOG(SEVERITY_LEVEL_ERROR) << L"GetGestureConfig error = " << GetLastError();
  } else {
    for (int i = 0; i < GC_SIZE; ++i) {
      LOG(SEVERITY_LEVEL_ERROR) << gc[i];
    }
  }
}
