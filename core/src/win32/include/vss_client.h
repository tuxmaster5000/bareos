/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2006-2012 Free Software Foundation Europe e.V.
   Copyright (C) 2019-2024 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#ifndef BAREOS_WIN32_INCLUDE_VSS_CLIENT_H_
#define BAREOS_WIN32_INCLUDE_VSS_CLIENT_H_

/*                               -*- Mode: C -*-
 * vss.h --
 */
//
// Copyright transferred from MATRIX-Computer GmbH to
//   Kern Sibbald by express permission.
/*
 *
 * Author          : Thorsten Engel
 * Created On      : Fri May 06 21:44:00 2006
 */

#include <unordered_set>

#ifndef b_errno_win32
#  define b_errno_win32 (1 << 29)
#endif

#ifdef WIN32_VSS

void VSSInit(JobControlRecord* jcr);

#  define VSS_INIT_RESTORE_AFTER_INIT 1
#  define VSS_INIT_RESTORE_AFTER_GATHER 2

// some forward declarations
struct IVssAsync;
struct IVssBackupComponents;
struct _VSS_SNAPSHOT_PROP;

typedef HRESULT(STDAPICALLTYPE* t_CreateVssBackupComponents)(
    OUT IVssBackupComponents**);
typedef void(APIENTRY* t_VssFreeSnapshotProperties)(IN _VSS_SNAPSHOT_PROP*);

class VSSClient {
 public:
  VSSClient() = default;
  VSSClient(const VSSClient&) = delete;
  VSSClient& operator=(const VSSClient&) = delete;
  VSSClient(const VSSClient&&) = delete;
  VSSClient& operator=(VSSClient&&) = delete;
  virtual ~VSSClient();

  // Backup Process
  bool InitializeForBackup(JobControlRecord* jcr);
  bool InitializeForRestore(JobControlRecord* jcr);
  virtual void AddVolumeSnapshots(IVssBackupComponents* pVssObj,
                                  const std::vector<std::wstring>& volumes,
                                  bool onefs_disabled)
      = 0;
  virtual void AddVolumeMountPointSnapshots(
      IVssBackupComponents* pVssObj,
      const std::wstring& volume,
      std::unordered_set<std::wstring>& snapshoted_volumes)
      = 0;
  virtual void ShowVolumeMountPointStats(JobControlRecord* jcr) = 0;

  virtual bool CreateSnapshots(const std::vector<std::wstring>& volumes,
                               bool onefs_disabled)
      = 0;
  virtual bool CloseBackup() = 0;
  virtual bool CloseRestore() = 0;
  virtual WCHAR* GetMetadata() = 0;
  virtual const char* GetDriverName() = 0;
  char* GetShadowPath(const char* szFilePath);
  wchar_t* GetShadowPathW(const wchar_t* szFilePath);

  size_t GetWriterCount() const;
  const char* GetWriterInfo(size_t nIndex) const;
  int GetWriterState(size_t nIndex) const;
  void DestroyWriterInfo();
  void AppendWriterInfo(int nState, const char* pszInfo);
  const bool IsInitialized() { return bBackupIsInitialized_; };
  HMODULE GetVssDllHandle() { return hLib_; };
  IUnknown* GetVssObject() { return pVssObject_; };

  // sadly we need to construct a string here for lookups
  // if we switch to c++20 we can use lookups without constructing strings!
  // check: https://stackoverflow.com/a/71258936
  std::unordered_map<std::string, std::string> mount_to_vol{};
  std::unordered_map<std::string, std::string> vol_to_vss{};
  std::unordered_map<std::wstring, std::wstring> mount_to_vol_w{};
  std::unordered_map<std::wstring, std::wstring> vol_to_vss_w{};

 private:
  virtual bool Initialize(DWORD dwContext, bool bDuringRestore = FALSE) = 0;
  virtual bool WaitAndCheckForAsyncOperation(IVssAsync* pAsync) = 0;
  virtual void QuerySnapshotSet(GUID snapshotSetID) = 0;

 protected:
  HMODULE hLib_ = nullptr;
  JobControlRecord* jcr_ = nullptr;

  t_CreateVssBackupComponents CreateVssBackupComponents_;
  t_VssFreeSnapshotProperties VssFreeSnapshotProperties_;

  DWORD dwContext_ = 0;

  IUnknown* pVssObject_ = nullptr;
  GUID uidCurrentSnapshotSet_ = GUID_NULL;

  wchar_t* metadata_ = nullptr;

  struct WriterInfo {
    int state_ = 0;
    std::string info_text_;
  };
  std::vector<WriterInfo> writer_info_;

  bool bCoInitializeCalled_ = false;
  bool bDuringRestore_ = false; /* true if we are doing a restore */
  bool bBackupIsInitialized_ = false;
  bool bWriterStatusCurrent_ = false;

  int VMPs = 0;          /* volume mount points */
  int VMP_snapshots = 0; /* volume mount points that are snapshotted */
};

class VSSClientVista : public VSSClient {
 public:
  VSSClientVista();
  virtual ~VSSClientVista() override;
  virtual void AddVolumeSnapshots(IVssBackupComponents* pVssObj,
                                  const std::vector<std::wstring>& volumes,
                                  bool onefs_disabled) override;
  virtual void AddVolumeMountPointSnapshots(
      IVssBackupComponents* pVssObj,
      const std::wstring& volume,
      std::unordered_set<std::wstring>& snapshoted_volumes) override;
  virtual void ShowVolumeMountPointStats(JobControlRecord* jcr) override;
  virtual bool CreateSnapshots(const std::vector<std::wstring>& volumes,
                               bool onefs_disabled) override;
  virtual bool CloseBackup() override;
  virtual bool CloseRestore() override;
  virtual WCHAR* GetMetadata() override;
#  ifdef _WIN64
  virtual const char* GetDriverName() override { return "Win64 VSS"; };
#  else
  virtual const char* GetDriverName() override { return "Win32 VSS"; };
#  endif

 private:
  virtual bool Initialize(DWORD dwContext, bool bDuringRestore) override;
  virtual bool WaitAndCheckForAsyncOperation(IVssAsync* pAsync) override;
  virtual void QuerySnapshotSet(GUID snapshotSetID) override;
  bool CheckWriterStatus();
};

#endif /* WIN32_VSS */

#endif  // BAREOS_WIN32_INCLUDE_VSS_CLIENT_H_
