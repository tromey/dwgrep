/*
   Copyright (C) 2014 Red Hat, Inc.
   This file is part of dwgrep.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   dwgrep is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifndef _VALUE_DW_H_
#define _VALUE_DW_H_

#include <elfutils/libdwfl.h>
#include "value.hh"
#include "dwfl_context.hh"

class value_dwarf
  : public value
{
  std::string m_fn;
  std::shared_ptr <dwfl_context> m_dwctx;

public:
  static value_type const vtype;

  value_dwarf (std::string const &fn, size_t pos);
  value_dwarf (value_dwarf const &that) = default;

  std::string &get_fn ()
  { return m_fn; }

  std::shared_ptr <dwfl_context> get_dwctx ()
  { return m_dwctx; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

class value_die
  : public value
{
  std::shared_ptr <dwfl_context> m_dwctx;
  Dwarf_Die m_die;

public:
  static value_type const vtype;

  value_die (std::shared_ptr <dwfl_context> dwctx, Dwarf_Die die, size_t pos)
    : value {vtype, pos}
    , m_dwctx {(assert (dwctx != nullptr), dwctx)}
    , m_die (die)
  {}

  value_die (value_die const &that) = default;

  Dwarf_Die &get_die ()
  { return m_die; }

  std::shared_ptr <dwfl_context> get_dwctx ()
  { return m_dwctx; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

class value_attr
  : public value
{
  std::shared_ptr <dwfl_context> m_dwctx;
  Dwarf_Die m_die;
  Dwarf_Attribute m_attr;

public:
  static value_type const vtype;

  value_attr (std::shared_ptr <dwfl_context> dwctx,
	      Dwarf_Attribute attr, Dwarf_Die die, size_t pos)
    : value {vtype, pos}
    , m_dwctx {dwctx}
    , m_die (die)
    , m_attr (attr)
  {}

  value_attr (value_attr const &that) = default;

  std::shared_ptr <dwfl_context> get_dwctx ()
  { return m_dwctx; }

  Dwarf_Die &get_die ()
  { return m_die; }

  Dwarf_Attribute &get_attr ()
  { return m_attr; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

class value_loclist_elem
  : public value
{
  std::shared_ptr <dwfl_context> m_dwctx;
  Dwarf_Attribute m_attr;
  Dwarf_Addr m_low;
  Dwarf_Addr m_high;
  Dwarf_Op *m_expr;
  size_t m_exprlen;

public:
  static value_type const vtype;

  value_loclist_elem (std::shared_ptr <dwfl_context> dwctx, Dwarf_Attribute attr,
		      Dwarf_Addr low, Dwarf_Addr high,
		      Dwarf_Op *expr, size_t exprlen, size_t pos)
    : value {vtype, pos}
    , m_dwctx {dwctx}
    , m_attr (attr)
    , m_low {low}
    , m_high {high}
    , m_expr {expr}
    , m_exprlen {exprlen}
  {}

  value_loclist_elem (value_loclist_elem const &that) = default;

  std::shared_ptr <dwfl_context> get_dwctx ()
  { return m_dwctx; }

  Dwarf_Attribute &get_attr ()
  { return m_attr; }

  Dwarf_Addr get_low () const
  { return m_low; }

  Dwarf_Addr get_high () const
  { return m_high; }

  Dwarf_Op *get_expr () const
  { return m_expr; }

  size_t get_exprlen () const
  { return m_exprlen; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

class value_addr_range
  : public value
{
  constant m_low;
  constant m_high;

public:
  static value_type const vtype;

  value_addr_range (constant const &low, constant const &high, size_t pos)
    : value {vtype, pos}
    , m_low {low}
    , m_high {high}
  {}

  value_addr_range (value_addr_range const &that) = default;

  constant const &get_low () const
  { return m_low; }

  constant const &get_high () const
  { return m_high; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

class value_loclist_op
  : public value
{
  // This apparently wild pointer points into libdw-private data.  We
  // actually need to carry a pointer, as some functions require that
  // they be called with the original pointer, not our own copy.
  std::shared_ptr <dwfl_context> m_dwctx;
  Dwarf_Attribute m_attr;
  Dwarf_Op *m_dwop;

public:
  static value_type const vtype;

  value_loclist_op (std::shared_ptr <dwfl_context> dwctx, Dwarf_Attribute attr,
		    Dwarf_Op *dwop, size_t pos)
    : value {vtype, pos}
    , m_dwctx {dwctx}
    , m_attr (attr)
    , m_dwop (dwop)
  {}

  value_loclist_op (value_loclist_op const &that) = default;

  std::shared_ptr <dwfl_context> get_dwctx ()
  { return m_dwctx; }

  Dwarf_Attribute &get_attr ()
  { return m_attr; }

  Dwarf_Op *get_dwop ()
  { return m_dwop; }

  void show (std::ostream &o, brevity brv) const override;
  std::unique_ptr <value> clone () const override;
  cmp_result cmp (value const &that) const override;
};

#endif /* _VALUE_DW_H_ */
