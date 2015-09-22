#include "stdafx.h"

#pragma once

#include "libs.h"

#ifdef DEBUG
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "gtest.lib")
#endif

int _tmain(int argc, _TCHAR *argv[]) {
  touchmind::logging::InitLogger(L"test.log");
  LOG(SEVERITY_LEVEL_DEBUG) << L"logging start";
  testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  getchar();
  return 0;
}
