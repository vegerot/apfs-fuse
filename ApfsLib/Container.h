/*
	This file is part of apfs-fuse, a read-only implementation of APFS
	(Apple File System) for FUSE.
	Copyright (C) 2017 Simon Gander

	Apfs-fuse is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	Apfs-fuse is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with apfs-fuse.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Global.h"
#include "BTree.h"
#include "DiskStruct.h"
#include "Device.h"
#include "CheckPointMap.h"
#include "ApfsNodeMapperBTree.h"
#include "KeyMgmt.h"
#include "Object.h"

#include <cstdint>
#include <vector>

class Volume;
class BlockDumper;
class ObjCache;
class OMap;
class Spaceman;

class Container : public Object
{
public:
	Container();
	~Container();

	int init(const void* params) override;

	static int Mount(ObjPtr<Container>& ptr, Device *disk_main, uint64_t main_start, uint64_t main_len, Device *disk_tier2 = 0, uint64_t tier2_start = 0, uint64_t tier2_len = 0, xid_t req_xid = 0);
	int FinishMount();
	static int Unmount(ObjPtr<Container>& ptr);

	int MountVolume(ObjPtr<Volume>& ptr, unsigned int fsid, const std::string &passphrase = std::string(), xid_t snap_xid = 0);
	int GetVolumeInfo(unsigned int fsid, apfs_superblock_t &apsb);

	int ReadBlocks(uint8_t *data, paddr_t paddr, uint64_t blkcnt = 1) const;
	int ReadAndVerifyHeaderBlock(uint8_t *data, paddr_t paddr) const;

	int getOMap(ObjPtr<OMap>& ptr);

	uint32_t GetBlocksize() const { return m_nxsb->nx_block_size; }
	uint64_t GetBlockCount() const { return m_nxsb->nx_block_count; }
	uint64_t GetFreeBlocks();

	int GetVolumeKey(uint8_t *key, const apfs_uuid_t &vol_uuid, const char *password = nullptr);
	bool GetPasswordHint(std::string &hint, const apfs_uuid_t &vol_uuid);
	bool IsUnencrypted();

	void dump(BlockDumper& bd);

private:
	int getSpaceman(ObjPtr<Spaceman> &sm);
	int loadKeybag();

	const nx_superblock_t* m_nxsb;

	Device *m_main_disk;
	uint64_t m_main_part_start;
	uint64_t m_main_part_len;

	Device *m_tier2_disk;
	uint64_t m_tier2_part_start;
	uint64_t m_tier2_part_len;

	std::string m_passphrase;

	ObjPtr<OMap> m_omap;
	ObjPtr<Spaceman> m_sm;

	// CheckPointMap m_cpm;

	// Block_8_11 -> omap

	BTree m_fq_tree_mgr; // Don't need those for ro ...
	BTree m_fq_tree_vol; // Don't need those for ro ...

	KeyManager m_keymgr;
};
