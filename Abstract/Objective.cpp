/*
 * Copyright (C) Alneos, s. a r. l. (contact@alneos.fr)
 * This file is part of Vega.
 *
 *   Vega is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Vega is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Vega.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Objective.cpp
 *
 *  Created on: 6 mars 2014
 *      Author: siavelis
 */

#include "Objective.h"
#include "Model.h"

using namespace std;

namespace vega {

Objective::Objective(Model& model, Objective::Type type, int original_id) :
        Identifiable(original_id), model(model), type(type) {
}

const string Objective::name = "Objective";

const map<Objective::Type, string> Objective::stringByType = {
        { Objective::Type::NODAL_DISPLACEMENT_ASSERTION, "NODAL_DISPLACEMENT_ASSERTION" },
        { Objective::Type::FREQUENCY_ASSERTION, "FREQUENCY_ASSERTION" },
        { Objective::Type::FREQUENCY_SEARCH, "FREQUENCY_SEARCH" },
        { Objective::Type::FREQUENCY_EXCIT, "FREQUENCY_EXCIT" },
        { Objective::Type::MODAL_DAMPING, "MODAL_DAMPING" },
        { Objective::Type::NONLINEAR_STRATEGY, "NONLINEAR_STRATEGY" }
};

ostream &operator<<(ostream &out, const Objective& objective) {
    out << to_str(objective);
    return out;
}

ObjectiveSet::ObjectiveSet(Model& model, Type type, int original_id) :
        Identifiable(original_id), model(model), type(type) {
}

void ObjectiveSet::add(const Reference<ObjectiveSet>& objectiveSetReference) {
    objectiveSetReferences.push_back(objectiveSetReference);
}

const set<shared_ptr<Objective>, ptrLess<Objective> > ObjectiveSet::getObjectives() const {
    auto result = model.getObjectivesByObjectiveSet(this->getReference());
    for (const auto& objectiveSetReference : objectiveSetReferences) {
        const auto& setToInsert = model.getObjectivesByObjectiveSet(
                objectiveSetReference);
        result.insert(setToInsert.begin(), setToInsert.end());
    }
    return result;
}

const set<shared_ptr<Objective>, ptrLess<Objective> > ObjectiveSet::getObjectivesByType(
        Objective::Type type) const {
    set<shared_ptr<Objective>, ptrLess<Objective> > result;
    for (const auto& objective : getObjectives()) {
        if (objective->type == type) {
            result.insert(objective);
        }
    }
    return result;
}

int ObjectiveSet::size() const {
    return static_cast<int>(getObjectives().size());
}

const string ObjectiveSet::name = "ObjectiveSet";

const map<ObjectiveSet::Type, string> ObjectiveSet::stringByType = {
    { ObjectiveSet::Type::FREQ, "FREQ" },
    };

ostream &operator<<(ostream &out, const ObjectiveSet& objectiveSet) {
    out << to_str(objectiveSet);
    return out;
}

Assertion::Assertion(Model& model, Type type, double tolerance, int original_id) :
        Objective(model, type, original_id), tolerance(tolerance) {
}

NodalAssertion::NodalAssertion(Model& model, Type type, double tolerance, int nodeId,
        DOF dof, int original_id) :
        Assertion(model, type, tolerance, original_id), nodePosition(model.mesh.findOrReserveNode(nodeId)), nodeId(nodeId), dof(dof) {
}

const DOFS NodalAssertion::getDOFSForNode(const int nodePosition) const {
    UNUSEDV(nodePosition);
    return dof;
}
set<int> NodalAssertion::nodePositions() const {
    set<int> result;
    result.insert(nodePosition);
    return result;
}

NodalDisplacementAssertion::NodalDisplacementAssertion(Model& model, double tolerance,
        int nodeId, DOF dof, double value, double instant, int original_id) :
        NodalAssertion(model, Objective::Type::NODAL_DISPLACEMENT_ASSERTION, tolerance, nodeId, dof,
                original_id), value(value), instant(instant) {
}

ostream &operator<<(ostream &out, const NodalDisplacementAssertion& objective) {
    out << to_str(objective) << "Node Pos " << objective.nodePosition << " DOF " << objective.dof << " "
            << objective.value;
    return out;
}

NodalComplexDisplacementAssertion::NodalComplexDisplacementAssertion(Model& model,
        double tolerance, int nodeId, DOF dof, complex<double> value, double frequency,
        int original_id) :
        NodalAssertion(model, Objective::Type::NODAL_COMPLEX_DISPLACEMENT_ASSERTION, tolerance, nodeId, dof,
                original_id), value(value), frequency(frequency) {
}

ostream &operator<<(ostream &out, const NodalComplexDisplacementAssertion& objective) {
    out << to_str(objective) << "Node Pos " << objective.nodePosition << " DOF " << objective.dof << " "
            << objective.value;
    return out;
}

FrequencyAssertion::FrequencyAssertion(Model& model, int number, double cycles, double eigenValue, double generalizedMass, double generalizedStiffness,
        double tolerance, int original_id) :
        Assertion(model, Objective::Type::FREQUENCY_ASSERTION, tolerance, original_id), number(number), cycles(cycles), eigenValue(eigenValue), generalizedMass(generalizedMass), generalizedStiffness(generalizedStiffness) {
}

const DOFS FrequencyAssertion::getDOFSForNode(const int nodePosition) const {
    UNUSEDV(nodePosition);
    return DOFS::NO_DOFS;
}
set<int> FrequencyAssertion::nodePositions() const {
    return set<int>();
}

AnalysisParameter::AnalysisParameter(Model& model, Type type, int original_id) :
        Objective(model, type, original_id) {
}

FrequencySearch::FrequencySearch(Model& model, const FrequencyType frequencyType, const NamedValue& namedValue, NormType norm, int original_id) :
        AnalysisParameter(model, Objective::Type::FREQUENCY_SEARCH, original_id), namedValue(namedValue), frequencyType(frequencyType), norm(norm) {
}

const shared_ptr<NamedValue> FrequencySearch::getValue() const {
    return dynamic_pointer_cast<NamedValue>(model.find(namedValue));
}

const FunctionPlaceHolder FrequencySearch::getValueRangePlaceHolder() const {
    return FunctionPlaceHolder(model, namedValue.type, namedValue.original_id, Function::ParaName::FREQ);
}

FrequencyExcit::FrequencyExcit(Model& model, const FrequencyType frequencyType, const NamedValue& namedValue, NormType norm, int original_id) :
        AnalysisParameter(model, Objective::Type::FREQUENCY_EXCIT, original_id), namedValue(namedValue), frequencyType(frequencyType), norm(norm) {
}

const shared_ptr<NamedValue> FrequencyExcit::getValue() const {
    return dynamic_pointer_cast<NamedValue>(model.find(namedValue));
}

const FunctionPlaceHolder FrequencyExcit::getValueRangePlaceHolder() const {
    return FunctionPlaceHolder(model, namedValue.type, namedValue.original_id, Function::ParaName::FREQ);
}

ModalDamping::ModalDamping(Model& model, const FunctionTable& function_table, int original_id) :
        AnalysisParameter(model, Objective::Type::MODAL_DAMPING, original_id), function_table(Value::Type::FUNCTION_TABLE,
                Reference<NamedValue>::NO_ID, function_table.getId()) {
}

ModalDamping::ModalDamping(Model& model, int function_table_original_id, int original_id) :
        AnalysisParameter(model, Objective::Type::MODAL_DAMPING, original_id), function_table(Value::Type::FUNCTION_TABLE,
                function_table_original_id) {
}

const shared_ptr<FunctionTable> ModalDamping::getFunctionTable() const {
    return dynamic_pointer_cast<FunctionTable>(model.find(function_table));
}

const FunctionPlaceHolder ModalDamping::getFunctionTablePlaceHolder() const {
    return FunctionPlaceHolder(model, function_table.type, function_table.original_id, Function::ParaName::FREQ);
}

NonLinearStrategy::NonLinearStrategy(Model& model, int number_of_increments, int original_id) :
        AnalysisParameter(model, Objective::Type::NONLINEAR_STRATEGY, original_id), number_of_increments(
                number_of_increments) {
}

ArcLengthMethod::ArcLengthMethod(Model& model, const Reference<Objective>& strategy_reference, int original_id) :
        AnalysisParameter(model, Objective::Type::ARC_LENGTH_METHOD, original_id), strategy_reference(strategy_reference) {
}

Output::Output(Model& model, Type type, int original_id) :
        Objective(model, type, original_id) {
}

NodalDisplacementOutput::NodalDisplacementOutput(Model& model, shared_ptr<Reference<NamedValue>> collection, int original_id) :
        Output(model, Objective::Type::NODAL_DISPLACEMENT_OUTPUT, original_id), NodeContainer(model.mesh), collection(collection) {
}

vector<shared_ptr<NodeGroup>> NodalDisplacementOutput::getNodeGroups() const {
    vector<shared_ptr<NodeGroup>>&& nodeGroups = NodeContainer::getNodeGroups();
    if (collection != nullptr) {
        const auto& setValue = dynamic_pointer_cast<SetValue<int>>(model.find(*collection));
        if (setValue == nullptr)
            throw logic_error("Cannot find set of displacement output nodes");

        shared_ptr<NodeGroup> nodeGroup = model.mesh.findOrCreateNodeGroup("SET_" + to_string(setValue->getOriginalId()));
        for (const int nodeId : setValue->getSet()) {
            nodeGroup->addNodeId(nodeId);
        }
        setValue->markAsWritten();
        nodeGroups.push_back(nodeGroup);
    }
    return nodeGroups;
}

bool NodalDisplacementOutput::hasNodeGroups() const {
    return collection != nullptr or NodeContainer::hasNodeGroups();
}

VonMisesStressOutput::VonMisesStressOutput(Model& model, shared_ptr<Reference<NamedValue>> collection, int original_id) :
        Output(model, Objective::Type::VONMISES_STRESS_OUTPUT, original_id), CellContainer(model.mesh), collection(collection) {
}

vector<shared_ptr<CellGroup>> VonMisesStressOutput::getCellGroups() const {
    vector<shared_ptr<CellGroup>>&& cellGroups = CellContainer::getCellGroups();
    if (collection != nullptr) {
        const auto& setValue = dynamic_pointer_cast<SetValue<int>>(model.find(*collection));
        if (setValue == nullptr)
            throw logic_error("Cannot find set of displacement output nodes");

        shared_ptr<CellGroup> cellGroup = model.mesh.createCellGroup("SET_" + to_string(setValue->getOriginalId()));
        for (const int cellId : setValue->getSet()) {
            cellGroup->addCellId(cellId);
        }
        setValue->markAsWritten();
        cellGroups.push_back(cellGroup);
    }
    return cellGroups;
}

bool VonMisesStressOutput::hasCellGroups() const {
    return collection != nullptr or CellContainer::hasCellGroups();
}

} /* namespace vega */

