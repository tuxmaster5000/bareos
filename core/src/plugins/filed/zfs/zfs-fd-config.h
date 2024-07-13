/*
   Copyright (C) 2024 tuxmaster5000 tuxmaster5000@googlemail.com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Tuxmaster5000, Jun 2024


#ifndef ZFS_FD_CONFIG_H
#define ZFS_FD_CONFIG_H

#include <string>
#include <set>

class ZFSfdConfig {
	public:
		ZFSfdConfig();
		void setSnapshotPrefix(std::string prefix);
		void addTank(std::string tank);
		void addTanks(std::set<std::string> tanks);
		void addExcludeDataset(std::string dataset);
		void addExcludeDatasets(std::set<std::string> datasets);
		void addExcludeVolume(std::string volume);
		void addExcludeVolumes(std::set<std::string> volumes);
		void addDataset(std::string dataset);
		void addDatasets(std::set<std::string> datasets);
		void addVolume(std::string volume);
		void addVolumes(std::set<std::string> volumes);
		void verifyConfig();
		const std::string& getSnapshotPrefix() const;
		const std::set<std::string>& getTanks() const;
		const std::set<std::string>& getExcludeDatasets() const;
		const std::set<std::string>& getExcludeVolumes() const;
		const std::set<std::string>& getDatasets() const;
		const std::set<std::string>& getVolumes() const;
	private:
		std::string m_snapshot_prefix;
		std::set<std::string> m_tanks;
		std::set<std::string> m_exclude_datasets;
		std::set<std::string> m_exclude_volumes;
		std::set<std::string> m_datasets;
		std::set<std::string> m_volumes;
};
 #endif // ZFS_FD_CONFIG_H
