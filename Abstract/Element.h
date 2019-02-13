/*
 * Copyright (C) Alneos, s. a r. l. (contact@alneos.fr)
 * Released under the GNU General Public License
 *
 * Element.h
 *
 *  Created on: Sep 11, 2013
 *      Author: devel
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Material.h"
#include "Mesh.h"
#include "Dof.h"

#include <fstream>

namespace vega {

class Model;

class ModelType final {
private:
	std::string name;
	SpaceDimension dimension;
	ModelType(std::string name, const SpaceDimension dimension);
	public:
	inline bool operator==(const ModelType& rhs) {
		return this->name == rhs.name;
	}

	static const ModelType PLANE_STRESS;
	static const ModelType PLANE_STRAIN;
	static const ModelType AXISYMMETRIC;
	static const ModelType TRIDIMENSIONAL;
	static const ModelType TRIDIMENSIONAL_SI;
};

class ElementSet: public Identifiable<ElementSet> {
    friend std::ostream &operator<<(std::ostream&, const ElementSet&);    //output
public:
    enum class Type {
        DISCRETE_0D,
        DISCRETE_1D,
        NODAL_MASS,
        CIRCULAR_SECTION_BEAM,
        TUBE_SECTION_BEAM,
        RECTANGULAR_SECTION_BEAM,
        I_SECTION_BEAM,
        GENERIC_SECTION_BEAM,
        STRUCTURAL_SEGMENT,
        SHELL,
        CONTINUUM,
        STIFFNESS_MATRIX,
        MASS_MATRIX,
        DAMPING_MATRIX,
        RIGIDSET,
        RBAR,
        RBE3,
        LMPC,
        SCALAR_SPRING,
        COMPOSITE,
        SURFACE_SLIDE_CONTACT,
        UNKNOWN,
    };
protected:
    Model& model;
    ElementSet(Model&, Type, const ModelType& modelType = ModelType::TRIDIMENSIONAL,
            int original_id = NO_ORIGINAL_ID);
public:
    virtual ~ElementSet() = default;
    static const std::string name;
    static const std::map<Type, std::string> stringByType;
    const Type type;
    const ModelType& modelType;
    std::shared_ptr<CellGroup> cellGroup;
    std::shared_ptr<Material> material;
    void assignMaterial(int materialId);
    void assignMaterial(std::shared_ptr<Material> material) {
        this->material = std::move(material);
    }
    void assignCellGroup(std::shared_ptr<CellGroup>);
    const ModelType getModelType() const;
    virtual bool validate() const override;
    virtual std::shared_ptr<ElementSet> clone() const = 0;
    virtual const std::set<int> nodePositions() const {
        assert(cellGroup != nullptr);
        return cellGroup->nodePositions();
    }
    virtual const DOFS getDOFSForNode(const int nodePosition) const = 0;
    virtual double getAdditionalRho() const {
        return 0;
    }
    virtual bool isBeam() const {
        return false;
    }
    virtual bool isTruss() const {
        return false;
    }
    virtual bool isShell() const {
        return false;
    }
    virtual bool isComposite() const {
        return false;
    }
    virtual bool isDiscrete() const {
        return false;
    }
    virtual bool isMatrixElement() const {
        return false;
    }
};

class RecoveryPoint {
private:
    const Model& model;
    const VectorialValue localCoords;
public:
    RecoveryPoint(const Model& model, const double lx, const double ly, const double lz);
    const VectorialValue getLocalCoords() const;
    const VectorialValue getGlobalCoords(const int cellId) const;
};

class Beam: public ElementSet {
public:
	enum class BeamModel {
		EULER,
		TIMOSHENKO,
		TRUSS /**< Element with no rotational stiffness in linear static analysis, also called PROD (Nastran) or BARRE (Aster) */
	};
	BeamModel beamModel;
protected:
	double additional_mass;
	Beam(Model&, Type type, const ModelType& modelType = ModelType::TRIDIMENSIONAL, BeamModel beamModel = BeamModel::EULER,
			double additionalMass = 0.0, int original_id = NO_ORIGINAL_ID);
public:
    virtual ~Beam() = default;
    std::vector<RecoveryPoint> recoveryPoints{};
	double getAdditionalRho() const override {
		return additional_mass / std::max(getAreaCrossSection(), DBL_MIN);
	}
	/**
	 * True only for elements having rotational stiffness in linear static analysis, false for Truss elements (also see isTruss() method )
	 */
	bool isBeam() const override final {
		return beamModel != BeamModel::TRUSS;
	}
	/**
	 * True only for elements with no rotational stiffness in linear static analysis, also called PROD (Nastran) or BARRE (Aster)
	 */
    bool isTruss() const override final {
		return beamModel == BeamModel::TRUSS;
	}
	virtual double getAreaCrossSection() const = 0;
	virtual double getMomentOfInertiaY() const = 0;
	virtual double getMomentOfInertiaZ() const = 0;
	virtual double getTorsionalConstant() const = 0;
	virtual double getShearAreaFactorY() const = 0;
	virtual double getShearAreaFactorZ() const = 0;
	const DOFS getDOFSForNode(const int nodePosition) const override final;
};

class CircularSectionBeam: public Beam {

public:
	const double radius;
	CircularSectionBeam(Model&, double _radius, BeamModel beamModel = BeamModel::EULER,
			double additional_mass = 0, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override;
	double getAreaCrossSection() const override;
	double getMomentOfInertiaY() const override;
	double getMomentOfInertiaZ() const override;
	double getTorsionalConstant() const override;
	double getShearAreaFactorY() const override;
	double getShearAreaFactorZ() const override;
};

class TubeSectionBeam: public Beam {

public:
	const double radius;
	const double thickness;
	TubeSectionBeam(Model&, double _radius, double _thickness, BeamModel beamModel = BeamModel::EULER,
			double additional_mass = 0, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override;
	double getAreaCrossSection() const override;
	double getMomentOfInertiaY() const override;
	double getMomentOfInertiaZ() const override;
	double getTorsionalConstant() const override;
	double getShearAreaFactorY() const override;
	double getShearAreaFactorZ() const override;
};


class GenericSectionBeam: public Beam {
private:
	const double area_cross_section;
	const double moment_of_inertia_Y;
	const double moment_of_inertia_Z;
	const double torsional_constant;
	const double shear_area_factor_Y;
	const double shear_area_factor_Z;
public:
	GenericSectionBeam(Model& model, double area_cross_section, double moment_of_inertia_Y,
			double moment_of_inertia_Z, double torsional_constant, double shear_area_factor_Y,
			double shear_area_factor_Z, BeamModel beamModel = BeamModel::EULER, double additional_mass = 0,
			int original_id = NO_ORIGINAL_ID);
	/* dans AFFE_CARA_ELEM les coefficients de cisaillement
	 AY et AZ ne doivent pas etre entres comme le font
	 tous les autres codes (i.e. un coefficient <=1.) mais a l'inverse (i.e. >=1.)*/
	double getAreaCrossSection() const override;
	double getMomentOfInertiaY() const override;
	double getMomentOfInertiaZ() const override;
	double getTorsionalConstant() const override;
	double getShearAreaFactorY() const override;
	double getShearAreaFactorZ() const override;
	double getInvShearAreaFactorY() const;
	double getInvShearAreaFactorZ() const;
	std::shared_ptr<ElementSet> clone() const override;
};

class RectangularSectionBeam: public Beam {

public:
	const double width;
	const double height;
	RectangularSectionBeam(Model&, double width, double height, BeamModel beamModel = BeamModel::EULER,
			double additional_mass = 0, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override;
	double getAreaCrossSection() const override;
	double getMomentOfInertiaY() const override;
	double getMomentOfInertiaZ() const override;
	double getTorsionalConstant() const override;
	double getShearAreaFactorY() const override;
	double getShearAreaFactorZ() const override;
};

/**
 *     '''
 *   see http://en.wikipedia.org/wiki/I-beam for attribute terms
 *   see http://en.wikipedia.org/wiki/Second_moment_of_area
 *   '''
 */
class ISectionBeam: public Beam {

public:
	const double upper_flange_width;
	const double lower_flange_width;
	const double upper_flange_thickness;
	const double lower_flange_thickness;
	const double beam_height;
	const double web_thickness;

	ISectionBeam(Model& model, double upper_flange_width, double lower_flange_width,
			double upper_flange_thickness, double lower_flange_thickness, double beam_height,
			double web_thickness, BeamModel beamModel = BeamModel::EULER, double additional_mass = 0,
			int original_id = NO_ORIGINAL_ID);

	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<ISectionBeam>(*this);
	}

	double getAreaCrossSection() const override;
	double getMomentOfInertiaY() const override;
	double getMomentOfInertiaZ() const override;
	double getTorsionalConstant() const override;
	double getShearAreaFactorY() const override;
	double getShearAreaFactorZ() const override;
};

class Shell: public ElementSet {

public:
	const double thickness;
	protected:
	double additional_mass;
	public:
	Shell(Model&, double thickness, double additional_mass = 0, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<Shell>(*this);
	}
	double getAdditionalRho() const override {
		return additional_mass / std::max(thickness, DBL_MIN);
	}
	bool isShell() const override final {
		return true;
	}
	const DOFS getDOFSForNode(const int nodePosition) const override final;
};

class Composite;
class CompositeLayer {
    friend Composite;
    int _materialId;
    double _thickness;
    double _orientation;
	CompositeLayer(int materialId, double thickness, double orientation = 0);
public:
	inline int getMaterialId() const {
	    return _materialId;
	}
	inline double getThickness() const {
	    return _thickness;
	}
    inline double getOrientation() const {
	    return _orientation;
	}
};

class Composite: public ElementSet {
    std::vector<CompositeLayer> layers{};
	public:
	Composite(Model&, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<Composite>(*this);
	}
	void addLayer(int materialId, double thickness, double orientation = 0);
	double getTotalThickness();
	inline const std::vector<CompositeLayer>& getLayers() const {
	    return layers;
	}
	bool isComposite() const override final {
		return true;
	}
	const DOFS getDOFSForNode(const int nodePosition) const override final;
};

class Continuum: public ElementSet {

public:
	Continuum(Model&, const ModelType& modelType, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<Continuum>(*this);
	}
	const DOFS getDOFSForNode(const int nodePosition) const override final;
};

class Discrete: public ElementSet {
public:
	static const double NOT_BOUNDED;
	const bool symmetric = false;
	Discrete(Model&, ElementSet::Type type, bool symmetric = false, int original_id = NO_ORIGINAL_ID);
	bool isDiscrete() const override final {
		return true;
	}
	virtual bool hasTranslations() const = 0;
	virtual bool hasRotations() const = 0;
	const DOFS getDOFSForNode(const int nodePosition) const override final;
	virtual const std::vector<double> asStiffnessVector(bool) const = 0;
};

class DiscretePoint final: public Discrete {
private:
	DOFMatrix stiffness;
	DOFMatrix mass;
	DOFMatrix damping;
public:
	DiscretePoint(Model&, double x, double y, double z, double rx = NOT_BOUNDED, double ry =
			NOT_BOUNDED, double rz = NOT_BOUNDED, bool symmetric = true, int original_id = NO_ORIGINAL_ID);
	DiscretePoint(Model&, std::vector<double> components = {}, bool symmetric = true, int original_id =
			NO_ORIGINAL_ID);
	bool hasTranslations() const override;
	bool hasRotations() const override;
	void addStiffness(DOF rowcode, DOF colcode,	double value);
	double findStiffness(DOF rowcode, DOF colcode) const;
	void addComponent(DOF, double value);
	const std::vector<double> asStiffnessVector(bool addRotationsIfNotPresent = false) const override final;
	std::shared_ptr<ElementSet> clone() const override;
};

class DiscreteSegment final : public Discrete {
private:
	DOFMatrix stiffness[2][2];
	DOFMatrix mass[2][2];
	DOFMatrix damping[2][2];
public:
	DiscreteSegment(Model&, bool symmetric = true, int original_id = NO_ORIGINAL_ID);
	bool hasTranslations() const override;
	bool hasRotations() const override;
	bool isDiagonalRigid() const;
	void addStiffness(int rowindex, int colindex, DOF rowdof, DOF coldof,
			double value);
	double findStiffness(int rowindex, int colindex, DOF rowdof, DOF coldof) const;
	const std::vector<double> asStiffnessVector(bool addRotationsIfNotPresent = false) const override final;
	std::shared_ptr<ElementSet> clone() const override;
};


/** Generalized Structural Two points elements : used for spring, damper elements.
 *  It may overlapped some functionnalities of DiscreteSegment.
 TODO LD : is this really needed or we could use DiscreteSegment ? */
class StructuralSegment final : public Discrete {
private:
	DOFMatrix stiffness{};
	DOFMatrix mass{};
	DOFMatrix damping{};
public:
	StructuralSegment(Model&, bool symmetric = true, int original_id = NO_ORIGINAL_ID);
	bool hasTranslations() const override;
	bool hasRotations() const override;
	bool isDiagonalRigid() const;
	bool hasStiffness() const;
	bool hasMass() const;
	bool hasDamping() const;
	void addStiffness(DOF rowdof, DOF coldof, double value);
	void addMass(DOF rowdof, DOF coldof, double value);
	void addDamping(DOF rowdof, DOF coldof, double value);
	void setAllZero();
	double findStiffness(DOF rowdof, DOF coldof) const;
	double findDamping(DOF rowdof, DOF coldof) const;
	const std::vector<double> asStiffnessVector(bool addRotationsIfNotPresent = false) const override final;
	const std::vector<double> asDampingVector(bool addRotationsIfNotPresent = false) const;
	std::shared_ptr<ElementSet> clone() const override;
};


class NodalMass: public ElementSet {
	const double m;
	public:
	const double ixx;
	const double iyy;
	const double izz;
	const double ixy;
	const double iyz;
	const double ixz;
	const double ex;
	const double ey;
	const double ez;

	NodalMass(Model&, double m = 0, double ixx = 0, double iyy = 0, double izz = 0, double ixy = 0,
			double iyz = 0, double ixz = 0, double ex = 0, double ey = 0, double ez = 0,
			int original_id = NO_ORIGINAL_ID);
	/**
	 * Get mass (in kg)
	 */
	double getMass() const;

	/**
	 * Get mass as equivalent force (in N)
	 */
	double getMassAsForce() const;

	const DOFS getDOFSForNode(const int nodePosition) const override final;
	bool hasTranslations() const;
	bool hasRotations() const;

	inline std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<NodalMass>(*this);
	}
};

/* Matrix for a group nodes.*/
class MatrixElement : public ElementSet {
private:
	std::map<std::pair<int, int>, std::shared_ptr<DOFMatrix>> submatrixByNodes{};
	bool symmetric = false;
public:
	MatrixElement(Model&, Type type, bool symmetric = false, int original_id = NO_ORIGINAL_ID);
	void addComponent(const int nodeid1, const DOF dof1, const int nodeid2, const DOF dof2, const double value);
	/**
	 * Clear all nodes and submatrices of the Matrix.
	 */
	void clear();
	const std::shared_ptr<DOFMatrix> findSubmatrix(const int nodePosition1, const int nodePosition2) const;
	const std::set<int> nodePositions() const override;
	const std::set<std::pair<int, int>> nodePairs() const;
	const std::set<std::pair<int, int>> findInPairs(int nodePosition) const;
	const DOFS getDOFSForNode(const int nodePosition) const override final;
	bool isMatrixElement() const override final {
		return true;
	}
	virtual bool validate() const override {
		return true;
	}
};

class StiffnessMatrix : public MatrixElement {
public:
	StiffnessMatrix(Model&, int original_id = NO_ORIGINAL_ID);
	void addStiffness(const int nodeid1, const DOF dof1, const int nodeid2, const DOF dof2, const double stiffness);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<StiffnessMatrix>(*this);
	}
};

class MassMatrix : public MatrixElement {
public:
	MassMatrix(Model&, int original_id = NO_ORIGINAL_ID);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<MassMatrix>(*this);
	}
};

class DampingMatrix : public MatrixElement {
public:
	DampingMatrix(Model&, int original_id = NO_ORIGINAL_ID);
	void addDamping(const int nodeid1, const DOF dof1, const int nodeid2, const DOF dof2, const double damping);
	std::shared_ptr<ElementSet> clone() const override {
		return std::make_shared<DampingMatrix>(*this);
	}
};


/**
 * RigidSet is a the parent virtual class for Rigid ElementSet,
 * like RBAR and RBE3.
 */
class RigidSet: public ElementSet {
protected:
    RigidSet(Model&, Type type, int master_id, int original_id = NO_ORIGINAL_ID);
public:
    virtual ~RigidSet() = default;
    const DOFS getDOFSForNode(const int nodePosition) const override final;
    int masterId;
};

class Rbar: public RigidSet {
public:
    Rbar(Model&, int master_id, int original_id = NO_ORIGINAL_ID);
    std::shared_ptr<ElementSet> clone() const override;
};

class Rbe3: public RigidSet {
public:
    Rbe3(Model&, int master_id, DOFS mdofs, DOFS sdofs, int original_id = NO_ORIGINAL_ID);
    const DOFS mdofs;
    const DOFS sdofs;
    std::shared_ptr<ElementSet> clone() const override;
};

/**
 * A Lmpc elemenset regroup various cells subject to the same LMPC rigid constraints
 **/
class Lmpc: public RigidSet {
public:
    Lmpc(Model&, int analysisId, int original_id = NO_ORIGINAL_ID);
    const int analysisId;
    std::vector<DOFCoefs> dofCoefs{};
    std::shared_ptr<ElementSet> clone() const override;
    void assignDofCoefs(const std::vector<DOFCoefs>);
};

/**
 * A Sliding elemenset regroup various cells subject to the same Sliding surface constraints
 **/
class SurfaceSlideSet: public RigidSet {
public:
    SurfaceSlideSet(Model&, int original_id = NO_ORIGINAL_ID);
    std::shared_ptr<ElementSet> clone() const override;
};

/**
 * ScalarSpring elemenset represent springs between two nodes.
 * All springs in this elementSet have the same stiffness and damping, but various
 * "directions" of spring can be collected.
 * TODO LD : is this really needed or we could use DiscreteSegment ?
 **/
class ScalarSpring : public Discrete {
private:
    std::map<std::pair<DOF, DOF>, std::vector<int>> cellpositionByDOFS{};
    double stiffness;
    double damping;
public:
    ScalarSpring(Model&, int original_id = NO_ORIGINAL_ID, double stiffness = Globals::UNAVAILABLE_DOUBLE, double damping= Globals::UNAVAILABLE_DOUBLE);
    double getStiffness() const;
    double getDamping() const;
    const std::map<std::pair<DOF, DOF>, std::vector<int>> getCellPositionByDOFS() const;
    void setStiffness(const double stiffness);
    void setDamping (const double damping);
    bool hasStiffness() const;
    bool hasDamping() const;
	bool hasTranslations() const override;
	bool hasRotations() const override;
    std::vector<std::pair<DOF, DOF>> getDOFSSpring() const;
    int getNbDOFSSpring() const;

    /**
     *  Add a spring cell to the elementSet, precising its position (vega id),
     *  and the two impacted DOF.
     */
    void addSpring(int cellPosition, DOF dofNodeA, DOF dofNodeB);
    const std::vector<double> asStiffnessVector(bool addRotationsIfNotPresent = false) const override final;
    const std::vector<double> asDampingVector(bool addRotationsIfNotPresent = false);
    bool validate() const override {
        return true;
    }
    std::shared_ptr<ElementSet> clone() const override;
};

} /* namespace vega */
#endif /* ELEMENT_H_ */
