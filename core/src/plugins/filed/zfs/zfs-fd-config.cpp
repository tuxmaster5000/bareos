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

#include "zfs-fd-config.h"
#include <stdexcept>

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
	m_tanks.insert(tank);
}
void ZFSfdConfig::addTanks(std::set<std::string> tanks) {
	m_tanks.insert(tanks.begin(), tanks.end());
}
const std::set<std::string>& ZFSfdConfig::getTanks() const {
	return m_tanks;
}
void ZFSfdConfig::addExcludeDataset(std::string dataset) {
	m_exclude_datasets.insert(dataset);
}
void ZFSfdConfig::addExcludeDatasets(std::set<std::string> datasets) {
	m_exclude_datasets.insert(datasets.begin(), datasets.end());
}
const std::set<std::string>& ZFSfdConfig::getExcludeDatasets() const {
	return m_exclude_datasets;
}
void ZFSfdConfig::addExcludeVolume(std::string volume) {
	m_exclude_volumes.insert(volume);
}
void ZFSfdConfig::addExcludeVolumes(std::set<std::string> volumes) {
	m_exclude_volumes.insert(volumes.begin(), volumes.end());
}
const std::set<std::string>& ZFSfdConfig::getExcludeVolumes() const {
	return m_exclude_volumes;
}
void ZFSfdConfig::addDataset(std::string dataset) {
	m_datasets.insert(dataset);
}
void ZFSfdConfig::addDatasets(std::set<std::string> datasets) {
	m_datasets.insert(datasets.begin(), datasets.end());
}
const std::set<std::string>& ZFSfdConfig::getDatasets() const {
	return m_datasets;
}
void ZFSfdConfig::addVolume(std::string volume) {
	m_volumes.insert(volume);
}
void ZFSfdConfig::addVolumes(std::set<std::string> volumes) {
	m_volumes.insert(volumes.begin(), volumes.end());
}
const std::set<std::string>&  ZFSfdConfig::getVolumes() const {
	return m_volumes;
}
void ZFSfdConfig::verifyConfig() {
	if (m_snapshot_prefix.empty())
		throw std::invalid_argument("The snapshot prefix can't be empty.");
	// When all tanks are selected, then the includes must be empty
	if (m_tanks.empty() and (!m_datasets.empty() or !m_volumes.empty()))
		throw std::invalid_argument("When all tanks are selected, include of volumes or data sets makes no sense.");
	// Check when 'none' is set, that it will be the only tank
	if (m_tanks.contains("none") and m_tanks.size() > 1)
		throw std::invalid_argument("When tanks is set to none, then it can be the only tank.");
        // When the tanks are set to 'none' then includes must be set.
	if (*m_tanks.cbegin() == "none" and (m_datasets.empty() and m_volumes.empty()))
		throw std::invalid_argument("When tanks set to none, the datasets/volumes must be set.");
	// When datasets/volumes are set, then exclude settings makes no sense.
	if (*m_tanks.cbegin() == "none" and (!m_exclude_datasets.empty() or !m_exclude_volumes.empty()))
		throw std::invalid_argument("When settings the datasets/volumes by hand, the exclude settings make no sense.");
}
