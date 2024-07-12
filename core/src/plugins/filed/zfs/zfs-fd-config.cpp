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
	m_tanks.push_back(tank);
}
void ZFSfdConfig::addTanks(std::list<std::string> tanks) {
	mergeLists(tanks, m_tanks);
}
const std::list<std::string>& ZFSfdConfig::getTanks() const {
	return m_tanks;
}
void ZFSfdConfig::addExcludeDataset(std::string dataset) {
	m_exclude_datasets.push_back(dataset);
}
void ZFSfdConfig::addExcludeDatasets(std::list<std::string> datasets) {
	mergeLists(datasets, m_exclude_datasets);
}
const std::list<std::string>& ZFSfdConfig::getExcludeDatasets() const {
	return m_exclude_datasets;
}
void ZFSfdConfig::addExcludeVolume(std::string volume) {
	m_exclude_volumes.push_back(volume);
}
void ZFSfdConfig::addExcludeVolumes(std::list<std::string> volumes) {
	mergeLists(volumes, m_exclude_volumes);
}
const std::list<std::string>& ZFSfdConfig::getExcludeVolumes() const {
	return m_exclude_volumes;
}
void ZFSfdConfig::addDataset(std::string dataset) {
	m_datasets.push_back(dataset);
}
void ZFSfdConfig::addDatasets(std::list<std::string> datasets) {
	mergeLists(datasets, m_datasets);
}
const std::list<std::string>& ZFSfdConfig::getDatasets() const {
	return m_datasets;
}
void ZFSfdConfig::addVolume(std::string volume) {
	m_volumes.push_back(volume);
}
void ZFSfdConfig::addVolumes(std::list<std::string> volumes) {
	 mergeLists(volumes, m_volumes);
}
const std::list<std::string>&  ZFSfdConfig::getVolumes() const {
	return m_volumes;
}
void ZFSfdConfig::mergeLists(const std::list<std::string> &source, std::list<std::string> &target) {
	target.sort();
	target.merge(&source);
	target.unique();
}
