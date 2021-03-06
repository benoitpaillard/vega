/*
 * Copyright (C) Alneos, s. a r. l. (contact@alneos.fr)
 * Released under the GNU General Public License
 *
 * Mesh.cpp
 *
 *  Created on: 26 févr. 2013
 *      Author: dallolio
 */

#include "Mesh.h"

#if VALGRIND_FOUND && defined VDEBUG && defined __GNUC__  && !defined(_WIN32)
#include <valgrind/memcheck.h>
#endif
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/comparable_distance.hpp>
#include "Model.h"
#include <algorithm>
#include <cstddef>
#include <cfloat>
#include <utility>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace vega {

using namespace std;
namespace fs = boost::filesystem;

const double NodeStorage::RESERVED_POSITION = -DBL_MAX;

NodeData::NodeData(int id, const DOFS& dofs, double x, double y, double z, pos_t cpPos, pos_t cdPos, int nodePart) :
    id(id), dofs(dofs), x(x), y(y), z(z), cpPos(cpPos), cdPos(cdPos), nodePart(nodePart) {
}

/**
 * Node Container class
 */

int NodeStorage::lastNodePart = 0;

NodeStorage::NodeStorage(Mesh& mesh, LogLevel logLevel) :
		logLevel(logLevel), mesh(mesh) {
	nodeDatas.reserve(4096);
}

NodeStorage::NodeIterator NodeStorage::begin() const {
	return NodeStorage::NodeIterator(*this, nodepositionById.begin());
}

NodeStorage::NodeIterator NodeStorage::end() const {
	return NodeStorage::NodeIterator(*this, nodepositionById.end());
}

NodeStorage::NodeIterator::NodeIterator(const NodeStorage& nodeStorage, mapid_iterator currentIdIterator) :
		nodeStorage(nodeStorage), currentIdIterator(currentIdIterator) {
}

void NodeStorage::NodeIterator::increment() {
	currentIdIterator++;
}

bool NodeStorage::NodeIterator::hasNext() const {
	return currentIdIterator != nodeStorage.nodepositionById.end();
}

bool NodeStorage::NodeIterator::equal(NodeStorage::NodeIterator const& other) const {
	//this.mesh == other.mesh
	return this->currentIdIterator == other.currentIdIterator;
}

NodeStorage::NodeIterator& NodeStorage::NodeIterator::operator ++() {
	increment();
	return *this;
}

NodeStorage::NodeIterator NodeStorage::NodeIterator::operator ++(int) {
	throw logic_error("not yet implemented");
}

bool NodeStorage::NodeIterator::operator ==(const NodeStorage::NodeIterator& rhs) const {
	return this->currentIdIterator == rhs.currentIdIterator;
}

bool NodeStorage::NodeIterator::operator !=(const NodeStorage::NodeIterator& rhs) const {
	return this->currentIdIterator != rhs.currentIdIterator;
}

Node NodeStorage::NodeIterator::operator *() {
	return nodeStorage.mesh.findNode(this->currentIdIterator->second);
}

Node NodeStorage::NodeIterator::next() {
	const Node& result(nodeStorage.mesh.findNode(this->currentIdIterator->second));
	this->increment();
	return result;
}

bool NodeStorage::validate() const {
	bool validNodes = true;
	for (size_t i = 0; i < nodeDatas.size(); ++i) {
		const NodeData &nodeData = nodeDatas[i];
		if (nodeData.id == Node::UNAVAILABLE_NODE) {
			validNodes = false;
			cerr << "Node in position " << i << " has been reserved, but never defined" << endl;
		}
	}
	if (validNodes && this->logLevel >= LogLevel::DEBUG) {
		cout << "All the reserved nodes have been defined." << endl;
	}
	return validNodes;
}

CellData::CellData(int id, const CellType& type, bool isvirtual, int elementId, pos_t cellTypePosition) :
		id(id), typeCode(type.code), isvirtual(isvirtual), elementId(
				elementId), cellTypePosition(cellTypePosition) {
}

/******************************************************************************
 * CellStorage class
 ******************************************************************************/

CellStorage::CellStorage(Mesh& mesh, LogLevel logLevel) :
		logLevel(logLevel), mesh(mesh) {
}

CellIterator CellStorage::cells_begin(const CellType &type) const {
	if (type.numNodes == 0) {
		throw logic_error(
				"Iteration on " + type.description + " not yet implemented");
	}

	return CellIterator(this, type, CellIterator::POSITION_BEGIN);
}

CellIterator CellStorage::cells_end(const CellType &type) const {
	if (type.numNodes == 0) {
		throw logic_error(
				"Iteration on " + type.description + " not yet implemented");
	}
	return CellIterator(this, type, CellIterator::POSITION_END);
}

vector<CellType> CellStorage::cellTypes() const {
    vector<CellType> keys;
    keys.reserve(nodepositionsByCelltype.size());
    for (const auto kv : nodepositionsByCelltype) {
        keys.push_back(kv.first);
    }
	return keys;
}

/******************************************************************************
 * Mesh class
 ******************************************************************************/

Mesh::Mesh(LogLevel logLevel, const string& modelName) :
		logLevel(logLevel), name(modelName), //
		nodes(NodeStorage(*this, this->logLevel)),
				cells(CellStorage(*this, this->logLevel)),
				coordinateSystemStorage(*this, this->logLevel), allCells{*this} {

	finished = false;
	for (const auto& cellTypePair : CellType::typeByCode) {
		cellPositionsByType[*(cellTypePair.second)] = {};
	}
}

pos_t Mesh::addNode(const int id, const double x, const double y, const double z, const pos_t cpPos, const pos_t cdPos, const int nodePart) noexcept {
	pos_t nodePosition;

	// In auto mode, we assign the first free node, starting from the biggest possible number
	int assignId;
	if (id == Node::AUTO_ID){
		assignId = Node::auto_node_id--;
		while (findNodePosition(assignId) != Node::UNAVAILABLE_NODE){
			assignId = Node::auto_node_id--;
		}
	} else {
        assignId = id;
	}
	auto positionIterator = nodes.nodepositionById.find(assignId);
	if (positionIterator == nodes.nodepositionById.end()) {
		nodePosition = static_cast<pos_t>(nodes.nodeDatas.size());
		NodeData nodeData(assignId, DOFS::NO_DOFS, x, y, z, cpPos, cdPos, nodePart);
		nodes.nodeDatas.push_back(nodeData);
		nodes.nodepositionById[id] = nodePosition;
	} else {
		nodePosition = positionIterator->second;
		NodeData& nodeData = nodes.nodeDatas[nodePosition];
		nodeData.x = x;
		nodeData.y = y;
		nodeData.z = z;
        nodeData.cpPos = cpPos;
        nodeData.cdPos = cdPos;
	}

	return nodePosition;
}

size_t Mesh::countNodes() const noexcept {
	return nodes.nodeDatas.size();
}

Node Mesh::findNode(const pos_t nodePosition) const {
	if (nodePosition == Node::UNAVAILABLE_NODE) {
		throw invalid_argument(
				"Node position " + to_string(nodePosition) + " not found.");
	}
	const NodeData &nodeData = nodes.nodeDatas[nodePosition];
	if (nodeData.cpPos == CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID) {
      // Should always be an "unnamed return" to avoid useless copies
      return Node(nodeData.id, nodeData.x, nodeData.y, nodeData.z, nodePosition, nodeData.dofs,
          nodeData.x, nodeData.y, nodeData.z, nodeData.cpPos, nodeData.cdPos, nodeData.nodePart);
	} else {
      shared_ptr<CoordinateSystem> coordSystem = this->getCoordinateSystemByPosition(nodeData.cpPos);
      if (coordSystem == nullptr) {
          ostringstream oss;
          oss << "ERROR: Coordinate System of position " << nodeData.cpPos << " for Node "<<nodeData.id<<" not found.";
          // We should throw an error... but only in "strict mode".
          // For other, it's too harsh for a regular translation.
          //throw logic_error(oss.str());
          oss <<  " Global Coordinate System used instead."<< endl;
          cerr<< oss.str();
          return Node(nodeData.id, nodeData.x, nodeData.y, nodeData.z, nodePosition, nodeData.dofs,
              nodeData.x, nodeData.y, nodeData.z, nodeData.cpPos, nodeData.cdPos);
      }
      const VectorialValue& gCoord = coordSystem->positionToGlobal(VectorialValue(nodeData.x,nodeData.y,nodeData.z));
      return Node(nodeData.id, nodeData.x, nodeData.y, nodeData.z, nodePosition, nodeData.dofs,
          gCoord.x(), gCoord.y(), gCoord.z(), nodeData.cpPos, nodeData.cdPos);
	}
}

pos_t Mesh::findOrReserveNode(int nodeId, int cellPartId) noexcept {

    int mainNodePart = 0;
    if (cellPartId != Globals::UNAVAILABLE_INT) {
        if (nodes.mainNodePartByCellPart.find(cellPartId) == nodes.mainNodePartByCellPart.end()) {
            // Never seen this cellPart, registering a new main node part
            mainNodePart = ++(nodes.lastNodePart);
            nodes.mainNodePartByCellPart[cellPartId] = mainNodePart;
            nodes.cellPartsByNodePart[mainNodePart] = {cellPartId};
        } else {
            mainNodePart = nodes.mainNodePartByCellPart[cellPartId];
        }
    }

	auto nodePosition = findNodePosition(nodeId);
	if (nodePosition == Node::UNAVAILABLE_NODE) {

        nodePosition = addNode(nodeId, NodeStorage::RESERVED_POSITION, NodeStorage::RESERVED_POSITION,
                NodeStorage::RESERVED_POSITION, CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID, CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID, mainNodePart);
        nodes.nodepositionById[nodeId] = nodePosition;
        nodes.reservedButUnusedNodePositions.insert(nodePosition);
        if (this->logLevel >= LogLevel::TRACE) {
            cout << "Reserve node id:" << nodeId << " position:" << nodePosition << endl;
        }
	} else if (cellPartId != Globals::UNAVAILABLE_INT) {
        auto& nodeData = nodes.nodeDatas[nodePosition];
        auto unusedNodeIt = nodes.reservedButUnusedNodePositions.find(nodePosition);
        if (unusedNodeIt != nodes.reservedButUnusedNodePositions.end()) {
            nodes.reservedButUnusedNodePositions.erase(unusedNodeIt);
        }
        if (nodeData.nodePart == 0) {
            // Node exists but this is the first cell containing it
            nodeData.nodePart = mainNodePart;
        } else if (nodeData.nodePart != mainNodePart) {
            // Node exists and already has a nodePart which is from another cell part
            auto& currentCellParts = nodes.cellPartsByNodePart[nodeData.nodePart];
            if (currentCellParts.find(cellPartId) == currentCellParts.end()) {
                // Current nodePart did not include this cellPart : interface node
                set<int> newCellParts{currentCellParts};
                newCellParts.insert(cellPartId);
                const auto& interfaceEntry = nodes.interfaceNodePartByCellParts.find(newCellParts);
                if (interfaceEntry == nodes.interfaceNodePartByCellParts.end()) {
                    // Never seen this cellpart set, registering a new interface node part
                    int interfaceNodePart = ++(nodes.lastNodePart);
                    nodes.interfaceNodePartByCellParts[newCellParts] = interfaceNodePart;
                    // Changing node part for this node
                    nodeData.nodePart = interfaceNodePart;
                    nodes.cellPartsByNodePart[interfaceNodePart] = newCellParts;
                } else {
                    // Updating nodepart to the interface
                    nodeData.nodePart = interfaceEntry->second;
                }
            }
        }
    }
	return nodePosition;

}

set<pos_t> Mesh::findOrReserveNodes(const set<int>& nodeIds) noexcept {
	set<pos_t> nodePositions;

	for (int nodeId : nodeIds) {
		nodePositions.insert(this->findOrReserveNode(nodeId));
	}
	return nodePositions;
}

int Mesh::findNodeId(const pos_t nodePosition) const noexcept {
	return nodePosition == Node::UNAVAILABLE_NODE ? Node::UNAVAILABLE_NODE : nodes.nodeDatas[nodePosition].id;
}

int Mesh::findNodePartId(const pos_t nodePosition) const noexcept {
	return nodePosition == Node::UNAVAILABLE_NODE ? Node::UNAVAILABLE_NODE : nodes.nodeDatas[nodePosition].nodePart;
}

pos_t Mesh::findNodePosition(const int nodeId) const noexcept {
	auto positionIterator = this->nodes.nodepositionById.find(nodeId);
	if (positionIterator == this->nodes.nodepositionById.end()) {
		return Node::UNAVAILABLE_NODE;
	}
	return positionIterator->second;
}

void Mesh::allowDOFS(const pos_t nodePosition, const DOFS& allowed) noexcept {
	nodes.nodeDatas[nodePosition].dofs = static_cast<char>(nodes.nodeDatas[nodePosition].dofs
			| allowed);
}

pos_t Mesh::addCell(const int id, const CellType& cellType, const std::vector<int>& nodeIds,
		bool virtualCell, const pos_t cpos, int elementId, double offset) {
	int cellId;
	const pos_t cellPosition = static_cast<pos_t>(cells.cellDatas.size());

	// In "auto" mode, we choose the first available Id, starting from the maximum authorized number
	if (id == Cell::AUTO_ID) {
		cellId = Cell::auto_cell_id--;
		while (findCellPosition(cellId)!= Cell::UNAVAILABLE_CELL){
			cellId = Cell::auto_cell_id--;
		}
	} else {
		cellId = id;
	}

    if (cellType.numNodes == 0) {
        throw logic_error("Cell without nodes");
    }

	if (this->logLevel >= LogLevel::TRACE) {

		//check connectivity, it causes strange errors in med
		set<int> coord_set(nodeIds.begin(), nodeIds.end());
		if (coord_set.size() != nodeIds.size()) {
			cerr << "Cell ID:" << cellId << " has duplicate nodes in connectivity. ";
			copy(nodeIds.begin(), nodeIds.end(),
					ostream_iterator<int>(cerr, " "));
			cerr << endl;
			throw logic_error(
					"Duplicate node in connectivity cellId:"
							+ to_string(cellId));
		}
		if (cells.cellpositionById.find(cellId) != cells.cellpositionById.end()) {
			throw logic_error(
					"CellId: " + to_string(cellId) + " Already used.");
		}

        if ((cellType.specificSize) && (cellType.numNodes != nodeIds.size())) {
            cerr << "Cell " << cellId << " not added because connectivity array differs from expected "
                    "length";
            throw logic_error("Invalid cell");
        }
	}

	cells.cellpositionById[cellId] = cellPosition;
	const auto cellTypePosition = static_cast<pos_t>(cellPositionsByType.find(cellType)->second.size());
	cellPositionsByType.find(cellType)->second.push_back(cellPosition);
	CellData cellData(cellId, cellType, virtualCell, elementId, cellTypePosition);

	if (cells.nodepositionsByCelltype.find(cellType) == cells.nodepositionsByCelltype.end()) {
		cells.nodepositionsByCelltype[cellType] = make_shared<deque<pos_t>>(deque<pos_t>());
	}
	const auto& nodePositionsPtr = cells.nodepositionsByCelltype[cellType];
	for (const auto& nodeId : nodeIds) {
		nodePositionsPtr->push_back(findOrReserveNode(nodeId, elementId));
	}

    switch (cellType.dimension.code) {
    case SpaceDimension::Code::DIMENSION0D_CODE: {
        if (cells.additional0DdataByCelltype.find(cellType) == cells.additional0DdataByCelltype.end())
            cells.additional0DdataByCelltype[cellType] = {};
        cells.additional0DdataByCelltype[cellType].push_back(DimensionData0D{});
        break;
    }
    case SpaceDimension::Code::DIMENSION1D_CODE: {
        if (cells.additional1DdataByCelltype.find(cellType) == cells.additional1DdataByCelltype.end())
            cells.additional1DdataByCelltype[cellType] = {};
        cells.additional1DdataByCelltype[cellType].push_back(DimensionData1D{});
        break;
    }
    case SpaceDimension::Code::DIMENSION2D_CODE: {
        if (cells.additional2DdataByCelltype.find(cellType) == cells.additional2DdataByCelltype.end())
            cells.additional2DdataByCelltype[cellType] = {};
        cells.additional2DdataByCelltype[cellType].push_back(DimensionData2D{offset});
        if (not is_zero(offset)) {
            this->withNonZeroOffsets = true;
        }
        break;
    }
    case SpaceDimension::Code::DIMENSION3D_CODE: {
/*        if (cells.additional3DdataByCelltype.find(cellType) == cells.additional3DdataByCelltype.end())
            cells.additional3DdataByCelltype[cellType] = {};
        cells.additional3DdataByCelltype[cellType].push_back(DimensionData3D{});*/
        break;
    }
    default: {
        throw logic_error("Not yet implemented");
    }
    }

	if (cpos != CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID) {
		this->getOrCreateCellGroupForCS(cpos)->addCellId(cellId);
		cellData.csPos = cpos;
    }
	cells.cellDatas.push_back(cellData);

	return cellPosition;
}

pos_t Mesh::updateCell(int id, const CellType &cellType, const std::vector<int> &nodeIds,
        bool virtualCell, const int cpos, int elementId) {
    const auto oldCellPosition = findCellPosition(id);
    if (id == Cell::AUTO_ID) {
        throw invalid_argument("Can't update a cell with AUTO_ID.");
    }
    if (oldCellPosition == Cell::UNAVAILABLE_CELL){
        throw invalid_argument("Can't update a cell which does not exist yet.");
    }
    if (cellType.numNodes == 0) {
        cerr << "Unsupported cell type" << cellType << endl;
    }

    // Copy/paste from addCell : don't know why it's needed ?
    if (this->logLevel >= LogLevel::TRACE) {
        //check connectivity, it causes strange errors in med
        set<int> coord_set(nodeIds.begin(), nodeIds.end());
        if (coord_set.size() != nodeIds.size()) {
            cerr << "Cell ID:" << id << " has duplicate nodes in connectivity. ";
            copy(nodeIds.begin(), nodeIds.end(), ostream_iterator<int>(cerr, " "));
            cerr << endl;
            throw logic_error(
                    "Duplicate node in connectivity cellId:"
                            + to_string(id));
        }

        if ((cellType.specificSize) && (cellType.numNodes != nodeIds.size())) {
            cerr << "Cell " << id << " not updated because connectivity array differs from expected "
                    "length";
            throw logic_error("Invalid cell");
        }
    }

    // We don't update the old data, which is too complicated
    // We build another CellData, with an other cellPosition, and hope
    // for the best
    const pos_t cellPosition = static_cast<pos_t>(cells.cellDatas.size());
    cells.cellpositionById[id] = cellPosition;

    const pos_t cellTypePosition = static_cast<pos_t>(cellPositionsByType.find(cellType)->second.size());
    cellPositionsByType.find(cellType)->second.push_back(cellPosition);
    CellData cellData(id, cellType, virtualCell, elementId, cellTypePosition);

    if (cells.nodepositionsByCelltype.find(cellType) == cells.nodepositionsByCelltype.end()) {
        cells.nodepositionsByCelltype[cellType] = make_shared<deque<pos_t>>(deque<pos_t>());
    }
    const auto& nodePositionsPtr = cells.nodepositionsByCelltype[cellType];
	for (const auto& nodeId : nodeIds) {
		nodePositionsPtr->push_back(findOrReserveNode(nodeId));
	}
    if (cpos != CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID) {
        this->getOrCreateCellGroupForCS(cpos)->addCellId(id);
        cellData.csPos = cpos;
    }
    cells.cellDatas.push_back(cellData);

    // Update cell groups
    for (auto& kv : groupByName) {
        const auto& cellGroup = dynamic_pointer_cast<CellGroup>(kv.second);
        if (cellGroup != nullptr and cellGroup->containsCellPosition(oldCellPosition)) {
            cellGroup->removeCellPosition(oldCellPosition);
            cellGroup->addCellPosition(cellPosition);
        }
    }

    return cellPosition;
}


Cell Mesh::findCell(const pos_t cellPosition) const {
	if (cellPosition == Cell::UNAVAILABLE_CELL) {
		throw logic_error("Unavailable cell requested.");
	}
	const auto& cellData = cells.cellDatas[cellPosition];
	const CellType* cellType = CellType::findByCode(cellData.typeCode);
	const unsigned int numNodes = cellType->numNodes;
	vector<int> nodeIds;
	nodeIds.resize(numNodes);
	auto it = cells.nodepositionsByCelltype.find(*cellType);
	const auto& globalNodePositions = *(it->second);
	const auto start = cellData.cellTypePosition * numNodes;
	vector<pos_t> nodePositions(globalNodePositions.begin() + start,
			globalNodePositions.begin() + start + numNodes);
	for (unsigned int i = 0; i < numNodes; i++) {
		const NodeData &nodeData = nodes.nodeDatas[nodePositions[i]];
		nodeIds[i] = nodeData.id;
	}
	shared_ptr<OrientationCoordinateSystem> ocs = nullptr;
	if (cellData.csPos!=CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID)
        ocs = static_pointer_cast<OrientationCoordinateSystem>(coordinateSystemStorage.findByPosition(cellData.csPos));

    double offset;
    switch (cellType->dimension.code) {
    case SpaceDimension::Code::DIMENSION0D_CODE: {
        const auto& additional0Ddata = cells.additional0DdataByCelltype.at(*cellType)[cellData.cellTypePosition];
        UNUSEDV(additional0Ddata);
        break;
    }
    case SpaceDimension::Code::DIMENSION1D_CODE: {
        const auto& additional1Ddata = cells.additional1DdataByCelltype.at(*cellType)[cellData.cellTypePosition];
        UNUSEDV(additional1Ddata);
        break;
    }
    case SpaceDimension::Code::DIMENSION2D_CODE: {
        const auto& additional2Ddata = cells.additional2DdataByCelltype.at(*cellType)[cellData.cellTypePosition];
        offset = additional2Ddata.offset;
        break;
    }
    case SpaceDimension::Code::DIMENSION3D_CODE: {
        //const auto& additional3Ddata = cells.additional3DdataByCelltype.at(*cellType)[cellData.cellTypePosition];
        break;
    }
    default: {
        throw logic_error("Not yet implemented");
    }
    }
	// Should stay as a "return unnamed" so that compiler can avoid rvalue copy
	return Cell(cellData.id, *cellType, nodeIds, cellPosition, nodePositions, false, cellData.csPos, cellData.elementId, cellData.cellTypePosition, ocs, offset);
}

pos_t Mesh::generateSkinCell(const vector<int>& faceIds, const SpaceDimension& dimension) {
    CellType* cellTypeFound = nullptr;
    for (const auto& typeAndCodePair : CellType::typeByCode) {
        CellType * typeToTest = typeAndCodePair.second;
        if (typeToTest->dimension == dimension && faceIds.size() == typeToTest->numNodes) {
            cellTypeFound = typeToTest;
            break;
        }
    }
    if (cellTypeFound == nullptr) {
        throw logic_error(
                "CellType not found connections:"
                        + to_string(faceIds.size()));
    }
    return addCell(Cell::AUTO_ID, *cellTypeFound, faceIds, true);
}

pair<Cell, int> Mesh::volcellAndFaceNum_from_skincell(const Cell& skinCell) const {
    const set<int> surfOrderedNodeIds(skinCell.nodeIds.begin(), skinCell.nodeIds.end());
    for (const auto& cellEntry : this->cellPositionsByType) {
        if (cellEntry.first.dimension != SpaceDimension::DIMENSION_3D)
            continue;
        for (const pos_t cellPosition : cellEntry.second) {
            const Cell& volCell = this->findCell(cellPosition);
            for (const auto& faceEntry : volCell.nodeIdsByFaceNum()) {
                const vector<int>& faceNodeIds = faceEntry.second;
                if (faceNodeIds.size() != surfOrderedNodeIds.size())
                    continue;
                const set<int> faceOrderedNodeIds(faceNodeIds.begin(), faceNodeIds.end());
                if (faceOrderedNodeIds == surfOrderedNodeIds) //< Must be ordered for set comparison
                    return {volCell, faceEntry.first};
            }
        }
    }
    throw logic_error("Cannot find volume cell corresponding to surface cell id : " + to_string(skinCell.id));
}

shared_ptr<CellGroup> Mesh::getOrCreateCellGroupForCS(pos_t cspos){
	shared_ptr<CellGroup> result;
	auto cellGroupNameIter = cellGroupNameByCspos.find(cspos);
	if (cellGroupNameIter != cellGroupNameByCspos.end()) {
		const auto& cellGroupName = cellGroupNameIter->second;
		result = static_pointer_cast<CellGroup>(findGroup(cellGroupName));
	} else {
		string gmaName;
		const auto& id = to_string(cellGroupNameByCspos.size() + 1);
		if (id.length() > 7) {
			gmaName = "C" + id.substr(id.length() - 7, 7);
		} else {
			gmaName = "C" + id;
		}
		cellGroupNameByCspos[cspos] = gmaName;
		result = createCellGroup(gmaName, CellGroup::NO_ORIGINAL_ID, "Orientation of coordinate system: " + to_string(cspos));
	}
	return result;
}

void Mesh::add(const CoordinateSystem& coordinateSystem) {
  if (this->logLevel >= LogLevel::TRACE) {
      cout << "Adding " << coordinateSystem << endl;
  }
  coordinateSystemStorage.add(coordinateSystem);
}

shared_ptr<CoordinateSystem> Mesh::findCoordinateSystem(const Reference<CoordinateSystem> csref) const {
  return coordinateSystemStorage.find(csref);
}

pos_t Mesh::findOrReserveCoordinateSystem(const Reference<CoordinateSystem> csref){
	if (csref == CoordinateSystem::GLOBAL_COORDINATE_SYSTEM)
		return CoordinateSystem::GLOBAL_COORDINATE_SYSTEM_ID;
	auto cpos = coordinateSystemStorage.findPosition(csref);
	if (cpos == CoordinateSystemStorage::UNAVAILABLE_POSITION)
		cpos = coordinateSystemStorage.reserve(csref);
	return cpos;
}

pos_t Mesh::addOrFindOrientation(const OrientationCoordinateSystem & ocs){

	auto posOrientation = findOrientation(ocs);
	if (posOrientation == 0){
		this->add(ocs);
		posOrientation = coordinateSystemStorage.findPosition(ocs);
	}
	return posOrientation;
}

pos_t Mesh::findOrientation(const OrientationCoordinateSystem & ocs) const{
	pos_t posOrientation=0;
	for (const auto& coordinateSystemEntry : this->coordinateSystemStorage.coordinateSystemByRef) {
        const auto& coordinateSystem = coordinateSystemEntry.second;
		if (coordinateSystem->type==CoordinateSystem::Type::RELATIVE){
			const auto& mocs = static_pointer_cast<OrientationCoordinateSystem>(coordinateSystem);
			if (ocs == *mocs){
				posOrientation = coordinateSystemStorage.findPosition(mocs->getReference());
				break;
			}
		}
	}
	return posOrientation;
}

shared_ptr<vega::CoordinateSystem> Mesh::getCoordinateSystemByPosition(const pos_t pos) const{
	return coordinateSystemStorage.findByPosition(pos);
}

string Mesh::getName() const noexcept {
    return name;
}

size_t Mesh::countCells() const noexcept {
	return cells.cellpositionById.size();
}

void Mesh::finish() noexcept {
	finished = true;

}

shared_ptr<NodeGroup> Mesh::createNodeGroup(const string& name, int group_id, const string & comment) {
	if (name.empty()) {
		throw invalid_argument("Can't create a nodeGroup with empty name ");
	}
	if (this->groupByName.find(name) != this->groupByName.end()) {
		throw invalid_argument("Another group exists with same name : " + name);
	}
	if (group_id != NodeGroup::NO_ORIGINAL_ID
			&& this->groupById.find(group_id) != this->groupById.end()) {
		string errorMessage = "Another group exists with same id : " + to_string(group_id);
        if (logLevel >= LogLevel::DEBUG) {
            stacktrace();
        }
		throw invalid_argument(errorMessage);
	}
	shared_ptr<NodeGroup> group = shared_ptr<NodeGroup>(new NodeGroup(*this, name, group_id, comment));
	this->groupByName[name] = group;
	if (group_id != NodeGroup::NO_ORIGINAL_ID) {
		this->groupById[group_id] = group;
	}

	if (this->logLevel >= LogLevel::TRACE) {
		cout << "Created Node Group:" << name;
		if (not comment.empty())
            cout <<" with comment: "<<comment;
        cout << endl;
	}
	return group;
}

shared_ptr<NodeGroup> Mesh::findOrCreateNodeGroup(const string& name, int group_id, const string & comment) {
	if (name.empty()) {
		throw invalid_argument("Can't find or create a nodeGroup with empty name ");
	}
	const auto& group = dynamic_pointer_cast<NodeGroup>(this->findGroup(name));
	if (group == nullptr){
		return this->createNodeGroup(name, group_id, comment);
	} else {
		if (group->type != Group::Type::NODEGROUP) {
			throw invalid_argument("Group " + name + " is not a nodeGroup.");
		}
		return group;
	}
}

bool Mesh::hasGroup(const int groupId) const noexcept {
    return groupById.find(groupId) != groupById.end();
}

bool Mesh::hasGroup(const string& name) const noexcept {
    return groupByName.find(name) != groupByName.end();
}

shared_ptr<CellGroup> Mesh::createCellGroup(const string& name, int groupId, const string & comment) {
	if (name.empty()) {
		throw invalid_argument("Can't create a cellGroup with empty name.");
	}
	if (this->groupByName.find(name) != this->groupByName.end()) {
		throw invalid_argument("Another group exists with same name: " + name);
	}
	if (groupId != CellGroup::NO_ORIGINAL_ID
			&& this->groupById.find(groupId) != this->groupById.end()) {
		string errorMessage = "Another group exists with same id: " + to_string(groupId);
		if (logLevel >= LogLevel::DEBUG) {
            stacktrace();
        }
		throw invalid_argument(errorMessage);
	}
	shared_ptr<CellGroup> group = shared_ptr<CellGroup>(new CellGroup(*this, name, groupId, comment));
	this->groupByName[name] = group;
	if (groupId != CellGroup::NO_ORIGINAL_ID) {
		this->groupById[groupId] = group;
	}

	if (this->logLevel >= LogLevel::TRACE) {
		cout << "Created Cell Group: " << name;
        if (not comment.empty())
            cout <<" with comment: "<<comment;
        cout << endl;
	}
	return group;
}

void Mesh::renameGroup(const string& oldname, const string& newname, const string& comment) {
    if (oldname.empty()) {
        throw invalid_argument("Can't rename a group with empty oldname.");
    }
    if (newname.empty()) {
        throw invalid_argument("Can't rename a group with empty newname.");
    }
    const auto & it = this->groupByName.find(oldname);
    if (it == this->groupByName.end()) {
        throw invalid_argument("No group exists with this name: " + oldname);
    }

    shared_ptr<Group> group = it->second;
    group->name = newname;
    group->comment = comment;
    this->groupByName.erase(it);
    this->groupByName[newname] = group;


    if (this->logLevel >= LogLevel::DEBUG) {
        cout << "Renamed Cell Group: " << newname << endl;
    }
}

void Mesh::removeGroup(const string& name) noexcept {

    auto it = this->groupByName.find(name);
    if (it != this->groupByName.end()) {
        std::shared_ptr<Group> group = it->second;
        this->groupByName.erase(name);
        const int gId= group->getId();
        if (gId!= Group::NO_ORIGINAL_ID){
            this->groupById.erase(gId);
        }
        if (this->logLevel >= LogLevel::DEBUG) {
            cout << "Remove group: " << name << endl;
        }
    }
}

vector<shared_ptr<NodeGroup>> Mesh::getNodeGroups() const noexcept {
	vector<shared_ptr<NodeGroup>> groups;
	for (const auto& it : groupByName) {
		const auto& group = it.second;
		if (group->type != Group::Type::NODEGROUP) {
			continue;
		}
		groups.push_back(dynamic_pointer_cast<NodeGroup>(group));
	}
	return groups;
}

vector<shared_ptr<CellGroup>> Mesh::getCellGroups() const noexcept {
	vector<shared_ptr<CellGroup>> groups;
	for (const auto& it : groupByName) {
		const auto& group = it.second;
		if (group->type != Group::Type::CELLGROUP) {
			continue;
		}
		groups.push_back(dynamic_pointer_cast<CellGroup>(group));
	}
	return groups;
}

shared_ptr<Group> Mesh::findGroup(string groupName) const noexcept {
	auto groupIterator = groupByName.find(groupName);
	if (groupIterator == groupByName.end())
		return nullptr;
	return groupIterator->second;
}

shared_ptr<Group> Mesh::findGroup(int originalId) const noexcept {
	auto groupIterator = groupById.find(originalId);
	if (groupIterator == groupById.end())
		return nullptr;
	return groupIterator->second;
}

size_t Mesh::countCells(const CellType& type) const noexcept {

	//if (type.code == CellType::POLYL.code) {
	//	//FIXME polylines not handled
	//	return 0;
	//}
	const auto& positions = cellPositionsByType.find(type)->second;
	return positions.size();
}

bool Mesh::hasCell(int cellId) const noexcept {
	auto positionIterator =
			cells.cellpositionById.find(cellId);
	return positionIterator != cells.cellpositionById.end();
}

pos_t Mesh::findCellPosition(int cellId) const noexcept {
	auto it = this->cells.cellpositionById.find(cellId);
	if (it == this->cells.cellpositionById.end()) {
		return Cell::UNAVAILABLE_CELL;
	}
	return it->second;
}

bool Mesh::validate() const {
	return nodes.validate();
}

MeshStatistics Mesh::calcStats() {
    if (stats != nullptr) {
        return *stats;
    }
    stats = make_unique<MeshStatistics>();
    double minSquareDistance = DBL_MAX;
    double minSquareDistanceNonZero = DBL_MAX;
    double maxSquareDistance = 0;
    double sumSquareDistance = 0;
    size_t countSquareDistance = 0;
    for (const CellType& cellType : cells.cellTypes()) {
        if (cellType.numNodes < 2)
            continue;
        vector<double> squareDistances;
        squareDistances.reserve(cellType.numNodes * (cellType.numNodes - 1) / 2);
        for (auto cellit = cells.cells_begin(cellType); cellit != cells.cells_end(cellType); cellit++) {
            const Cell& cell = *cellit;
            for (unsigned i = 0; i < cellType.numNodes - 1; i++) {
                for (unsigned j = i; j < cellType.numNodes; j++) {
                    const Node& n1 = findNode(cell.nodePositions[i]);
                    const Node& n2 = findNode(cell.nodePositions[j]);
                    squareDistances.push_back(boost::geometry::comparable_distance(n1, n2));
                    //squareDistances.push_back(n1.square_distance(n2));
                }
            }
        }
        const auto mm = minmax_element(squareDistances.begin(), squareDistances.end());
        if (minSquareDistance > 0)
            minSquareDistance = min(minSquareDistance, *(mm.first));
        maxSquareDistance = max(maxSquareDistance, *(mm.second));
        squareDistances.erase(remove_if(squareDistances.begin(), squareDistances.end(), [](double x){return is_zero(x);}), squareDistances.end());
        const auto mmnz = min_element(squareDistances.begin(), squareDistances.end());
        double sum = accumulate( squareDistances.begin(), squareDistances.end(), 0.0);
        minSquareDistanceNonZero = min(minSquareDistanceNonZero, *mmnz);
        sumSquareDistance += sum;
        countSquareDistance += squareDistances.size();
        squareDistances.clear();
    }
    stats->minLength = sqrt(minSquareDistance);
    stats->minNonzeroLength = sqrt(minSquareDistanceNonZero);
    stats->maxLength = sqrt(maxSquareDistance);
    stats->quadraticMeanLength = sqrt(sumSquareDistance / static_cast<double>(countSquareDistance));
    return *stats;
}

} /* namespace vega */

