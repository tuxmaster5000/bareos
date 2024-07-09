/*
   Copyright (C) 2024 tuxmaster5000 tuxmaster5000@googlemail.com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Tuxmaster5000, Jun 2024


#ifndef ZFS_FD_CONFIG_H
#define ZFS_FD_CONFIG_H

#include <string>

class ZFSfdConfig {
	public:
		ZFSfdConfig();
		void setSnapshotPrefix(std::string prefix);
		void addTank(std::string tank);
		void addTanks(std::list<std::string> tanks);
		const std::string& getSnapshotPrefix() const;
		const std::list<std::string>& getTanks() const;
	private:
		std::string m_snapshot_prefix;
		std::list<std::string> m_tanks;
		
};
#endif // ZFS_FD_CONFIG_H
