#include "stdafx.h"
#include "touchmind/print/PrintTicketHelper.h"

void touchmind::print::PrintTicketHelper::CreatePrintTicket(const std::wstring &deviceName, LPPRINTDLGEX pPDX,
                                                            IXpsPrintJobStream *printTicketStream) {
  HRESULT hr = S_OK;
  HPTPROVIDER phProvider = nullptr;
  if (SUCCEEDED(hr)) {
    hr = PTOpenProvider(deviceName.c_str(), 1, &phProvider);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"PTOpenProvider failed, hr = {:x}", hr);
    }
  }

  IStream *pPrintTicketStream = nullptr;
  if (SUCCEEDED(hr)) {
    hr = CreateStreamOnHGlobal(nullptr, TRUE, &pPrintTicketStream);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"CreateStreamOnHGlobal failed, hr = {:x}", hr);
    }
  }

  LPDEVMODE pDevmode = (LPDEVMODE)GlobalLock(pPDX->hDevMode);
  ULONG cbDevmode = sizeof(DEVMODE);
  if (SUCCEEDED(hr)) {
    hr = PTConvertDevModeToPrintTicket(phProvider, cbDevmode, pDevmode, kPTJobScope, pPrintTicketStream);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"PTConvertDevModeToPrintTicket failed, hr = {:x}", hr);
    }
  }
  GlobalUnlock(pDevmode);

  LARGE_INTEGER liZero = {0};
  if (SUCCEEDED(hr)) {
    hr = pPrintTicketStream->Seek(liZero, STREAM_SEEK_SET, nullptr);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"IStream::Seek failed, hr = {:x}", hr);
    }
  }

  if (SUCCEEDED(hr)) {
    WritePrintTicket(pPrintTicketStream, printTicketStream);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"WritePrintTicket failed, hr = {:x}", hr);
    }
  }
  SafeRelease(&pPrintTicketStream);
  PTCloseProvider(phProvider);
}

void touchmind::print::PrintTicketHelper::WritePrintTicket(IStream *pStream, IXpsPrintJobStream *pPrintStream) {
  char buf[1024];
  HRESULT hr = S_OK;

  ULONG cbRead;
  ULONG cbWritten;
  while (true) {
    hr = pStream->Read(buf, 1024, &cbRead);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"IStream::Read failed, hr = {:x}", hr);
      break;
    }
    if (cbRead == 0) {
      hr = S_OK;
      break;
    }
    hr = pPrintStream->Write(buf, cbRead, &cbWritten);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"IXpsPrintJobStream::Write failed, hr = {:x}", hr);
      break;
    }
    if (cbRead != cbWritten) {
      SPDLOG_ERROR(L"IXpsPrintJobStream::Write failed, could not write all the data, read bytes = {}, write bytes = {}",
                                cbRead, cbWritten);
      hr = E_FAIL;
      break;
    }
  }
}
