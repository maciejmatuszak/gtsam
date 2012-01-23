/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    Conditional.h
 * @brief   Base class for conditional densities
 * @author  Frank Dellaert
 */

// \callgraph

#pragma once

#include <iostream>
#include <boost/utility.hpp> // for noncopyable
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/serialization/nvp.hpp>
#include <gtsam/inference/Factor.h>

namespace gtsam {

/**
 * Base class for conditional densities, templated on KEY type.  This class
 * provides storage for the keys involved in a conditional, and iterators and
 * access to the frontal and separator keys.
 *
 * Derived classes *must* redefine the Factor and shared_ptr typedefs to refer
 * to the associated factor type and shared_ptr type of the derived class.  See
 * IndexConditional and GaussianConditional for examples.
 * \nosubgrouping
 */
template<typename KEY>
class Conditional: public gtsam::Factor<KEY> {

private:

	/** Create keys by adding key in front */
	template<typename ITERATOR>
	static std::vector<KEY> MakeKeys(KEY key, ITERATOR firstParent, ITERATOR lastParent) {
		std::vector<Key> keys((lastParent - firstParent) + 1);
		std::copy(firstParent, lastParent, keys.begin() + 1);
		keys[0] = key;
		return keys;
	}

protected:

  /** The first nFrontal variables are frontal and the rest are parents. */
  size_t nrFrontals_;

  // Calls the base class assertInvariants, which checks for unique keys
  void assertInvariants() const { Factor<KEY>::assertInvariants(); }

public:

  typedef KEY Key;
  typedef Conditional<Key> This;
  typedef Factor<Key> Base;

  /**
   * Typedef to the factor type that produces this conditional and that this
   * conditional can be converted to using a factor constructor. Derived
   * classes must redefine this.
   */
  typedef gtsam::Factor<Key> FactorType;

  /** A shared_ptr to this class.  Derived classes must redefine this. */
  typedef boost::shared_ptr<This> shared_ptr;

  /** Iterator over keys */
  typedef typename FactorType::iterator iterator;

  /** Const iterator over keys */
  typedef typename FactorType::const_iterator const_iterator;

  /** View of the frontal keys (call frontals()) */
  typedef boost::iterator_range<const_iterator> Frontals;

  /** View of the separator keys (call parents()) */
  typedef boost::iterator_range<const_iterator> Parents;

	/// @name Standard Constructors
	/// @{

  /** Empty Constructor to make serialization possible */
  Conditional() : nrFrontals_(0) { assertInvariants(); }

  /** No parents */
  Conditional(Key key) : FactorType(key), nrFrontals_(1) { assertInvariants(); }

  /** Single parent */
  Conditional(Key key, Key parent) : FactorType(key, parent), nrFrontals_(1) { assertInvariants(); }

  /** Two parents */
  Conditional(Key key, Key parent1, Key parent2) : FactorType(key, parent1, parent2), nrFrontals_(1) { assertInvariants(); }

  /** Three parents */
  Conditional(Key key, Key parent1, Key parent2, Key parent3) : FactorType(key, parent1, parent2, parent3), nrFrontals_(1) { assertInvariants(); }

	/// @}
	/// @name Advanced Constructors
	/// @{

  /** Constructor from a frontal variable and a vector of parents */
	Conditional(Key key, const std::vector<Key>& parents) :
		FactorType(MakeKeys(key, parents.begin(), parents.end())), nrFrontals_(1) {
		assertInvariants();
	}

  /** Constructor from keys and nr of frontal variables */
	Conditional(const std::vector<Index>& keys, size_t nrFrontals) :
		FactorType(keys), nrFrontals_(nrFrontals) {
		assertInvariants();
	}

	/// @}
	/// @name Testable
	/// @{

  /** print */
  void print(const std::string& s = "Conditional") const;

  /** check equality */
  template<class DERIVED>
  bool equals(const DERIVED& c, double tol = 1e-9) const {
    return nrFrontals_ == c.nrFrontals_ && FactorType::equals(c, tol); }

	/// @}
	/// @name Standard Interface
	/// @{

	/** return the number of frontals */
	size_t nrFrontals() const { return nrFrontals_; }

	/** return the number of parents */
	size_t nrParents() const { return FactorType::size() - nrFrontals_; }

	/** Special accessor when there is only one frontal variable. */
	Key firstFrontalKey() const { assert(nrFrontals_>0); return FactorType::front(); }
	Key lastFrontalKey() const { assert(nrFrontals_>0); return *(endFrontals()-1); }

  /** return a view of the frontal keys */
  Frontals frontals() const {
    return boost::make_iterator_range(beginFrontals(), endFrontals()); }

	/** return a view of the parent keys */
	Parents parents() const {
	  return boost::make_iterator_range(beginParents(), endParents()); }

	 /** Iterators over frontal and parent variables. */
	  const_iterator beginFrontals() const { return FactorType::begin(); }						///<TODO: comment
	  const_iterator endFrontals() const { return FactorType::begin()+nrFrontals_; }	///<TODO: comment
	  const_iterator beginParents() const { return FactorType::begin()+nrFrontals_; }	///<TODO: comment
	  const_iterator endParents() const { return FactorType::end(); }									///<TODO: comment

	/// @}
	/// @name Advanced Interface
	/// @{

	  /** Mutable iterators and accessors */
	  iterator beginFrontals() { return FactorType::begin(); }						///<TODO: comment
	  iterator endFrontals() { return FactorType::begin()+nrFrontals_; }	///<TODO: comment
	  iterator beginParents() { return FactorType::begin()+nrFrontals_; }	///<TODO: comment
	  iterator endParents() { return FactorType::end(); }									///<TODO: comment

	  ///TODO: comment
	  boost::iterator_range<iterator> frontals() {
	  	return boost::make_iterator_range(beginFrontals(), endFrontals()); }

	  ///TODO: comment
	  boost::iterator_range<iterator> parents() {
	  	return boost::make_iterator_range(beginParents(), endParents()); }

private:
  /** Serialization function */
  friend class boost::serialization::access;
  template<class ARCHIVE>
  void serialize(ARCHIVE & ar, const unsigned int version) {
  	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);
    ar & BOOST_SERIALIZATION_NVP(nrFrontals_);
  }

	/// @}

};


/* ************************************************************************* */
template<typename KEY>
void Conditional<KEY>::print(const std::string& s) const {
  std::cout << s << " P(";
  BOOST_FOREACH(Key key, frontals()) std::cout << " " << key;
  if (nrParents()>0) std::cout << " |";
  BOOST_FOREACH(Key parent, parents()) std::cout << " " << parent;
  std::cout << ")" << std::endl;
}

} // gtsam
