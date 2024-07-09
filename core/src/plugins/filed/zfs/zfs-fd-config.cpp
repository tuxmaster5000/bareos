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

#include "zfs-fd-config.h"

ZFSfdConfig::ZFSfdConfig() {
	setSnapshotPrefix("bareos");
}
void ZFSfdConfig::setSnapshotPrefix(std::string prefix) {
	m_snapshot_prefix = prefix;	
}
const std::string& ZFSfdConfig::getSnapshotPrefix() const {
	return m_snapshot_prefix;
}
void ZFSfdConfig::addTank(std::string tank) {
	m_tanks.push_back(tank)
}
void ZFSfdConfig::addTanks(std::list<std::string> tanks) {
	m_tanks.sort();
	m_tanks.merge(tanks);
        m_tanks.unique();
}
const std::list<std::string>& ZFSfdConfig::getTanks() const {
	return m_tanks;
}
