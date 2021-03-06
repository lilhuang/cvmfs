/**
 * This file is part of the CernVM File System.
 */

#ifndef CVMFS_RECEIVER_CATALOG_MERGE_TOOL_H_
#define CVMFS_RECEIVER_CATALOG_MERGE_TOOL_H_

#include <string>

#include "catalog_diff_tool.h"
#include "params.h"
#include "util/pointer.h"

namespace catalog {
class WritableCatalogManager;
}

namespace download {
class DownloadManager;
}

namespace manifest {
class Manifest;
}

namespace shash {
struct Any;
}

namespace receiver {

template <typename RwCatalogMgr, typename RoCatalogMgr>
class CatalogMergeTool : public CatalogDiffTool<RoCatalogMgr> {
 public:
  CatalogMergeTool(RoCatalogMgr* old_catalog_mgr, RoCatalogMgr* new_catalog_mgr,
                   RwCatalogMgr* output_catalog_mgr,
                   const std::string& temp_dir_prefix,
                   manifest::Manifest* manifest)
      : CatalogDiffTool<RoCatalogMgr>(old_catalog_mgr, new_catalog_mgr),
        repo_path_(""),
        temp_dir_prefix_(temp_dir_prefix),
        download_manager_(NULL),
        manifest_(manifest),
        output_catalog_mgr_(output_catalog_mgr),
        needs_setup_(false) {}

  CatalogMergeTool(const std::string& repo_path,
                   const shash::Any& old_root_hash,
                   const shash::Any& new_root_hash,
                   const std::string& temp_dir_prefix,
                   download::DownloadManager* download_manager,
                   manifest::Manifest* manifest)
      : CatalogDiffTool<RoCatalogMgr>(repo_path, old_root_hash, new_root_hash,
                                      temp_dir_prefix, download_manager),
        repo_path_(repo_path),
        temp_dir_prefix_(temp_dir_prefix),
        download_manager_(download_manager),
        manifest_(manifest),
        needs_setup_(true) {}

  virtual ~CatalogMergeTool() {}

  bool Run(const Params& params, std::string* new_manifest_path);

 protected:
  virtual void ReportAddition(const PathString& path,
                              const catalog::DirectoryEntry& entry,
                              const XattrList& xattrs);
  virtual void ReportRemoval(const PathString& path,
                             const catalog::DirectoryEntry& entry);
  virtual void ReportModification(const PathString& path,
                                  const catalog::DirectoryEntry& old_entry,
                                  const catalog::DirectoryEntry& new_entry,
                                  const XattrList& xattrs);

 private:
  bool CreateNewManifest(std::string* new_manifest_path);

  std::string repo_path_;
  std::string temp_dir_prefix_;

  download::DownloadManager* download_manager_;

  manifest::Manifest* manifest_;

  UniquePtr<RwCatalogMgr> output_catalog_mgr_;

  const bool needs_setup_;
};

}  // namespace receiver

#include "catalog_merge_tool_impl.h"

#endif  // CVMFS_RECEIVER_CATALOG_MERGE_TOOL_H_
