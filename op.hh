#ifndef _OP_H_
#define _OP_H_

#include <memory>
#include <cassert>

//#include "value.hh"
#include "make_unique.hh"
#include "dwgrep.hh"
#include "valfile.hh"
#include "pred_result.hh"

// Subclasses of class op represent computations.  An op node is
// typically constructed such that it directly feeds from another op
// node, called upstream (see tree::build_exec).
class op
{
public:
  virtual ~op () {}

  // Produce next value.
  virtual valfile::uptr next () = 0;
  virtual std::string name () const = 0;
};

// Class pred is for holding predicates.  These don't alter the
// computations at all.
class pred;

class op_origin
  : public op
{
  bool m_done;

public:
  op_origin ();

  valfile::uptr next () override;
  std::string name () const override;
};

class op_sel_universe
  : public op
{
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

public:
  op_sel_universe (std::shared_ptr <op> upstream,
		   dwgrep_graph::ptr q,
		   size_t osz, size_t nsz, slot_idx dst);
  ~op_sel_universe ();
  valfile::uptr next () override;
  std::string name () const override;
};

class op_f_child
  : public op
{
  class pimpl;
  std::unique_ptr <pimpl> m_pimpl;

public:
  op_f_child (std::shared_ptr <op> upstream,
	      dwgrep_graph::ptr q,
	      size_t osz, size_t nsz, slot_idx src, slot_idx dst);
  ~op_f_child ();
  valfile::uptr next () override;
  std::string name () const override;
};

class op_nop
  : public op
{
  std::shared_ptr <op> m_upstream;

public:
  explicit op_nop (std::shared_ptr <op> upstream) : m_upstream (upstream) {}
  valfile::uptr next () override;
  std::string name () const override;
};

class op_assert
  : public op
{
  std::shared_ptr <op> m_upstream;
  std::unique_ptr <pred> m_pred;

public:
  op_assert (std::shared_ptr <op> upstream, std::unique_ptr <pred> p)
    : m_upstream (upstream)
    , m_pred (std::move (p))
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_f_atval
  : public op
{
  std::shared_ptr <op> m_upstream;
  int m_name;
  slot_idx m_idx;

public:
  op_f_atval (std::shared_ptr <op> upstream, int name, slot_idx idx)
    : m_upstream (upstream)
    , m_name (name)
    , m_idx (idx)
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_f_offset
  : public op
{
  std::shared_ptr <op> m_upstream;
  slot_idx m_src;
  slot_idx m_dst;

public:
  op_f_offset (std::shared_ptr <op> upstream, slot_idx src, slot_idx dst)
    : m_upstream (upstream)
    , m_src (src)
    , m_dst (dst)
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_format
  : public op
{
  std::string m_str;
  size_t m_idx;

public:
  op_format (std::string lit, size_t idx)
    : m_str (lit)
    , m_idx (idx)
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_drop
  : public op
{
  size_t m_idx;

public:
  explicit op_drop (size_t idx)
    : m_idx (idx)
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_const
  : public op
{
  std::shared_ptr <op> m_upstream;
  constant m_cst;
  slot_idx m_dst;

public:
  op_const (std::shared_ptr <op> upstream,
	    constant cst, slot_idx dst)
    : m_upstream (upstream)
    , m_cst (cst)
    , m_dst (dst)
  {}

  valfile::uptr next () override;
  std::string name () const override;
};

class op_alt;
class op_capture;
class op_transform;
class op_protect;
class op_close; //+, *, ?
class op_str;
class op_atval;
class op_f_add;
class op_f_sub;
class op_f_mul;
class op_f_div;
class op_f_mod;
class op_f_parent;
class op_f_prev;
class op_f_next;
class op_f_type;
class op_f_name;
class op_f_tag;
class op_f_form;
class op_f_value;
class op_f_pos;
class op_f_count;
class op_f_each;
class op_sel_section;
class op_sel_unit;

class pred
{
public:
  virtual pred_result result (valfile &vf) const = 0;
  virtual std::string name () const = 0;
};

class pred_not
  : public pred
{
  std::unique_ptr <pred> m_a;

public:
  explicit pred_not (std::unique_ptr <pred> a) : m_a { std::move (a) } {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_and
  : public pred
{
  std::unique_ptr <pred> m_a;
  std::unique_ptr <pred> m_b;

public:
  pred_and (std::unique_ptr <pred> a, std::unique_ptr <pred> b)
    : m_a { std::move (a) }
    , m_b { std::move (b) }
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_or
  : public pred
{
  std::unique_ptr <pred> m_a;
  std::unique_ptr <pred> m_b;

public:
  pred_or (std::unique_ptr <pred> a, std::unique_ptr <pred> b)
    : m_a { std::move (a) }
    , m_b { std::move (b) }
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_at
  : public pred
{
  unsigned m_atname;
  slot_idx m_idx;

public:
  pred_at (unsigned atname, slot_idx idx)
    : m_atname (atname)
    , m_idx (idx)
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_tag
  : public pred
{
  int m_tag;
  slot_idx m_idx;

public:
  pred_tag (int tag, slot_idx idx)
    : m_tag (tag)
    , m_idx (idx)
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_binary
  : public pred
{
protected:
  slot_idx m_idx_a;
  slot_idx m_idx_b;

public:
  pred_binary (slot_idx idx_a, slot_idx idx_b)
    : m_idx_a (idx_a)
    , m_idx_b (idx_b)
  {}
};

class pred_eq
  : public pred_binary
{
public:
  using pred_binary::pred_binary;

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_lt
  : public pred_binary
{
public:
  using pred_binary::pred_binary;

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_gt
  : public pred_binary
{
public:
  using pred_binary::pred_binary;

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_root
  : public pred
{
  dwgrep_graph::ptr m_q;
  slot_idx m_idx_a;

public:
  pred_root (dwgrep_graph::ptr q, slot_idx idx_a)
    : m_q (q)
    , m_idx_a (idx_a)
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_subx_any
  : public pred
{
  std::unique_ptr <op> m_op;

public:
  explicit pred_subx_any (std::unique_ptr <op> op)
    : m_op (std::move (op))
  {}

  pred_result result (valfile &vf) const override;
  std::string name () const override;
};

class pred_find;
class pred_match;
class pred_empty;

#endif /* _OP_H_ */
